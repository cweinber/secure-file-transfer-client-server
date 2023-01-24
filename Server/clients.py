class Client:
    """ Represents a client entry """

    def __init__(self, client_id, client_name, public_key=None, last_seen=None, aes_key=None):
        self.id = client_id  # Unique client ID, 16 bytes.
        self.name = client_name  # Client's name, null terminated ascii string, 255 bytes.
        self.public_key = public_key  # Client's public key, 160 bytes.
        self.last_seen = last_seen  # The Date & time of client's last request.
        self.aes_key = aes_key  # AES key, 160 bytes.

    def validate(self):
        """ Validate Client attributes according to the requirements """
        if not self.id:  # or len(self.id) != protocol.CLIENT_ID_SIZE:
            return False
        if not self.name:  # or len(self.name) >= protocol.CLIENT_NAME_SIZE:
            return False
        if not self.public_key:  # or len(self.public_key) != protocol.PUBLIC_KEY_SIZE:
            return False
        if not self.last_seen:
            return False
        if not self.aes_key:  # or len(self.aes_key) != protocol.:
            return False
        return True
