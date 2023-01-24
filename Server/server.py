import base64
import os
import selectors
import socket
import uuid
from datetime import datetime
import base64
import db
import clients
import files
import requests
import responses


class Server:
    DATABASE = 'server.db'

    def __init__(self, port, host='127.0.0.1'):
        self.host = host
        self.port = port
        self.database = db.Database(Server.DATABASE)
        self.database.initialize()
        self.response = responses.Responses()
        self.request = requests.Requests()
        self.is_blocking = True
        self.sel = selectors.DefaultSelector()
        
    def accept_connections(self, server_socket, mask):
        client, address = server_socket.accept()
        print('Connected to: ' + address[0] + ':' + str(address[1]))
        client.setblocking(self.is_blocking)
        self.sel.register(client, selectors.EVENT_READ, self.get_request)
        
    def get_request(self, connenction, mask):
        try:
            client = None
            while True:
                data = connenction.recv(2000)
                if not data:
                    connenction.close()
                    self.sel.unregister(connenction)
                    break
                response = responses.ResponseCode.RESPONSE_FATAL_ERROR.value
                header = self.request.extract_request_header(data)
                command = header[3]
                if command == requests.RequestCode.REQUEST_REGISTERATION.value:
                    user_name = self.request.extract_registeration_data(data)
                    user_id = str(uuid.uuid4())
                    client = clients.Client(user_id, user_name)
                    res = self.database.add_new_client(client)
                    if res:
                        response = responses.ResponseCode.RESPONSE_REG_SUCCEEDED.value
                        aesKey = self.response.init_aes_key()
                    else:
                        response = responses.ResponseCode.RESPONSE_REG_FAILED.value
                elif command == requests.RequestCode.REQUEST_SEND_PUB_KEY.value:
                    user_name, pub_key = self.request.extract_pub_key_data(data)
                    self.database.update_client_public_key(client.id, base64.b64encode(pub_key).decode())
                    response = responses.ResponseCode.RESPONSE_PUB_KEY_REC.value
                elif command == requests.RequestCode.REQUEST_RECONNECT.value:
                    user_name = self.request.extract_reconnect_data(data)
                    c = self.database.get_client_by_username(user_name)
                    if not c:
                        response = responses.ResponseCode.RESPONSE_FAIL_RECONNECT.value
                    else:
                        client = clients.Client(str(c.id, 'utf-8'), c.name, c.public_key, c.last_seen)
                        response = responses.ResponseCode.RESPONSE_CONFIRM_RECONNECT.value
                elif command == requests.RequestCode.REQUEST_SEND_FILE.value:
                    content_size, file_name, content = self.request.extract_send_file_data(data)
                    ciphertext_file = open('ciphertext.enc', 'wb')
                    ciphertext_file.write(content)
                    ciphertext_file.close()
                    # Decrypt the file and calculate the checksum
                    self.response.decrypt_file(aesKey, bytearray(len(aesKey)), 'ciphertext.enc', 'plaintext.txt')
                    file_size = os.path.getsize('plaintext.txt')
                    checksum, contents = self.response.calculate_checksum('plaintext.txt')
                    response = responses.ResponseCode.RESPONSE_FILE_REC.value
                    # Save file in DB
                    now = datetime.now()
                    filename_with_datetime = now.strftime("%Y-%m-%d_%H-%M-%S_") + file_name
                    file = files.File(client.id, filename_with_datetime)
                    res = file.create_file_on_server(str(contents, 'utf-8'))
                    self.database.add_new_file(file)
                if command == requests.RequestCode.REQUEST_CORRECT_CRC.value:
                    file_name = self.request.extract_correct_crc_data(data)
                    response = responses.ResponseCode.RESPONSE_CONFIRM_MESSAGE.value
                elif command == requests.RequestCode.REQUEST_WRONG_CRC.value:
                    file_name = self.request.extract_wrong_crc_data(data)
                    checksum, contents = self.response.calculate_checksum('plaintext.txt')
                    response = responses.ResponseCode.RESPONSE_FILE_REC.value
                elif command == requests.RequestCode.REQUEST_FATAL_WRONG_CRC.value:
                    continue

                if client:
                    res = self.database.set_last_seen(client.id, str(datetime.now()))
                    if not res:
                        print(f'fail to set last seen user ID: {client.id}')

                if response == responses.ResponseCode.RESPONSE_REG_SUCCEEDED.value:
                    frame = self.response.init_reg_suc_response(client)
                elif response == responses.ResponseCode.RESPONSE_REG_FAILED.value:
                    frame = self.response.init_reg_failed_response()
                elif response == responses.ResponseCode.RESPONSE_PUB_KEY_REC.value:
                    pubKey = self.database.get_client_public_key(client.id)
                    frame = self.response.init_send_aes_key(responses.ResponseCode.RESPONSE_PUB_KEY_REC.value, aesKey, client, base64.b64decode(pubKey))
                elif response == responses.ResponseCode.RESPONSE_FILE_REC.value:
                    frame = self.response.send_checksum(client, file_size, file_name, checksum)
                elif response == responses.ResponseCode.RESPONSE_CONFIRM_MESSAGE.value:
                    frame = self.response.confirm_message(client)
                elif response == responses.ResponseCode.RESPONSE_CONFIRM_RECONNECT.value:
                    pubKey = client.public_key
                    aesKey = self.response.init_aes_key()
                    frame = self.response.init_send_aes_key(responses.ResponseCode.RESPONSE_CONFIRM_RECONNECT.value, aesKey, client, base64.b64decode(pubKey))
                elif response == responses.ResponseCode.RESPONSE_FAIL_RECONNECT.value:
                    frame = self.response.init_fail_reconnect()
                elif response == responses.ResponseCode.RESPONSE_FATAL_ERROR.value:
                    frame = self.response.init_fatal_error()
                connenction.send(frame)
        except:
            connenction.close()
            self.sel.unregister(connenction)
            
    def start_listener(self):
        server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        try:
            # Tying to bind
            server.bind((self.host, self.port))
            server.listen(3)

            server.setblocking(self.is_blocking)
            self.sel.register(server, selectors.EVENT_READ, self.accept_connections)

            print("[+] Server bind on %s:%d" % (self.host, self.port))
            # print("[+] Accecpted connection from %s:%d" % (addr[0], addr[1]))
        except socket.error as error:
            print("[!] Unable to bind on %s:%d" % (self.host, self.port))
            return
        # Waiting for connections and handle them
        print("Wating for connections...")
        while True:
            try:
                events = self.sel.select()
                for key, mask in events:
                    callback = key.data
                    callback(key.fileobj, mask)
            except Exception as e:
                print(f"Server main loop exception: {e}")