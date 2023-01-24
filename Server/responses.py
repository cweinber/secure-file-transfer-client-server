import base64
import os
import selectors
import struct
import uuid
from enum import Enum
import zlib
import base64
from Crypto.PublicKey import RSA
from Crypto.Cipher import AES
from Crypto.Cipher import PKCS1_OAEP
from Crypto.PublicKey import RSA
from Crypto.Util import Padding

class Version(Enum):
    VERSION = 3

class ResponseCode(Enum):
    RESPONSE_REG_SUCCEEDED = 2100
    RESPONSE_REG_FAILED = 2101
    RESPONSE_PUB_KEY_REC = 2102
    RESPONSE_FILE_REC = 2103
    RESPONSE_CONFIRM_MESSAGE = 2104
    RESPONSE_CONFIRM_RECONNECT = 2105
    RESPONSE_FAIL_RECONNECT = 2106
    RESPONSE_FATAL_ERROR = 2107

class Responses:
    def __init__(self):
        self.max_int64 = 0xFFFFFFFFFFFFFFFF

    def create_header(self, response_code, data_length):
        frame = bytearray()
        frame.extend(struct.pack("<H", Version.VERSION.value))
        frame.extend(struct.pack("<H", response_code))
        frame.extend(struct.pack("<I", data_length))
        return frame

    def create_client_id(self, client):
        uid = uuid.UUID(hex=client.id)
        packed = struct.pack('<QQ', (uid.int >> 64) & self.max_int64, uid.int & self.max_int64)
        return packed

    def create_encrypted_aes_key(self, aes_key, public_key):
        rsa_key = RSA.import_key(base64.b64decode(public_key))
        cipher_rsa = PKCS1_OAEP.new(rsa_key)
        ciphertext = cipher_rsa.encrypt(aes_key)
        return ciphertext

    def init_reg_suc_response(self, client):
        header = self.create_header(ResponseCode.RESPONSE_REG_SUCCEEDED.value, 16)
        client_id = self.create_client_id(client)
        return header + client_id

    def init_reg_failed_response(self):
        return self.create_header(ResponseCode.RESPONSE_REG_FAILED.value, 0)

    def init_fatal_error(self):
        return self.create_header(ResponseCode.RESPONSE_FATAL_ERROR.value, 0)

    def init_fail_reconnect(self):
        return self.create_header(ResponseCode.RESPONSE_FAIL_RECONNECT.value, 0)

    def init_send_aes_key(self, code, aes_key, client, public_key):
        encrypted_aes_key = self.create_encrypted_aes_key(aes_key, public_key)
        header = self.create_header(code, 16 + len(encrypted_aes_key))
        client_id = self.create_client_id(client)
        packed_ciphertext = struct.pack("<%ds" % len(encrypted_aes_key), encrypted_aes_key)
        return header + client_id + packed_ciphertext
    
    def confirm_message(self, client):
        header = self.create_header(ResponseCode.RESPONSE_CONFIRM_MESSAGE.value, 16)
        client_id = self.create_client_id(client)
        return header + client_id

    def send_checksum(self, client, file_size, file_name, checksum):
        header = self.create_header(ResponseCode.RESPONSE_FILE_REC.value, 280)
        client_id = self.create_client_id(client)
        file_size_pack = struct.pack("<I", file_size)
        
        if len(file_name) < 256:
            padding = bytearray(256 - len(file_name))
            encoded_file = file_name.encode() + padding
        file_name_pack = struct.pack("<256s", encoded_file)
        checksum_pack = struct.pack("<I", checksum)
        return header + client_id + file_size_pack + file_name_pack + checksum_pack

    def init_aes_key(self):
        return os.urandom(16)

    def decrypt_file(self, key, iv, ciphertext_file, plaintext_file):
        # Create a new AES cipher
        cipher = AES.new(key, AES.MODE_CBC, iv)

        # Decrypt the ciphertext and write the resulting plaintext to a file
        with open(ciphertext_file, 'rb') as f:
            ciphertext = f.read()
        with open(plaintext_file, 'wb') as f:
            # add padding before decryption + decryption
            padded_ciphertext = Padding.pad(ciphertext, block_size=AES.block_size)
            plaintext = cipher.decrypt(padded_ciphertext)

            # Remove the padding from the decrypted plaintext
            print((len(padded_ciphertext) - len(ciphertext)))
            unpadded_plaintext = plaintext[0:-((len(padded_ciphertext) - len(ciphertext) + 1) * 3 -1)]
            f.write(unpadded_plaintext)

    def calculate_checksum(self, file):
        # Open the file
        with open(file, "rb") as f:
            # Read the contents of the file into a byte array
            contents = f.read()

        # Calculate the CRC checksum
        crc = zlib.crc32(contents) & 0xffffffff
        return crc, contents