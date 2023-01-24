import struct
from enum import Enum

class RequestCode(Enum):
    REQUEST_REGISTERATION = 1100
    REQUEST_SEND_PUB_KEY = 1101
    REQUEST_RECONNECT = 1102
    REQUEST_SEND_FILE = 1103
    REQUEST_CORRECT_CRC = 1104
    REQUEST_WRONG_CRC = 1105
    REQUEST_FATAL_WRONG_CRC = 1106
    
class Requests:
    def extract_request_header(self, data):
        header_data = data[:24]
        header = struct.unpack('<QQBHIB', header_data)
        return header

    def extract_request_data(self, data):
        header = self.extract_request_header(data)
        up_data = str(header[4]) + 's'
        return data[24:], header[4]

    def extract_string_data(self, data):
        data, data_len = self.extract_request_data(data)
        try:
            return ''.join(chr(b) for b in data if 32 <= b < 128)
        except:
            return ''
        
    def extract_registeration_data(self, data):
        return self.extract_string_data(data)
    
    def extract_pub_key_data(self, data):
        data, data_len = self.extract_request_data(data)
        try:
            un = data[:255]
            user_name = ''.join(chr(b) for b in un if 32 <= b < 128)
            if len(user_name) > 255:
                return '', ''
            pub_key = data[255:]
        except:
            return '', ''
        finally:
            return user_name, pub_key

    def extract_reconnect_data(self, data):
        return self.extract_string_data(data)
    
    def extract_send_file_data(self, data):
        data, data_len = self.extract_request_data(data)
        try:
            content_size = struct.unpack('I', data[:4])
            file_name = ''.join(chr(b) for b in data[4:259] if 32 <= b < 128)
            content = data[260:]
            if len(content) > content_size:
                return 0, '', ''
        except:
            return 0, '', ''
        finally:
            return content_size, file_name, content

    def extract_correct_crc_data(self, data):
        return self.extract_string_data(data)

    def extract_wrong_crc_data(self, data):
        return self.extract_string_data(data)

    def extract_fatal_wrong_crc_data(self, data):
        return self.extract_string_data(data)
