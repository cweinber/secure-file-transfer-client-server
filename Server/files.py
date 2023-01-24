import os

class File:
    """ Represents a file entry """
    FILES_FOLDER_PATH = './files_from_clients/'

    def __init__(self, client_id, file_name, path_name=None, verified=False):
        self.id = client_id  # Unique client ID, 16 bytes.
        self.file_name = file_name  # file name, null terminated ascii string, 255 bytes.
        self.path_name = path_name  # path name, null terminated ascii string, 255 bytes.
        self.verified = verified

    def validate(self):
        """ Validate file attributes according to the requirements """
        if not self.id:
            return False
        if not self.file_name:  # or len(self.file_name) >= protocol.FILE_NAME_SIZE:
            return False
        if not self.path_name:  # or len(self.path_name) >= protocol.PUBLIC_KEY_SIZE:
            return False
        if not self.verified:
            return False
        return True

    def create_file_on_server(self, text):
        res = True
        self.path_name = File.FILES_FOLDER_PATH + self.file_name
        try:
            if not os.path.exists(File.FILES_FOLDER_PATH):
                os.makedirs(File.FILES_FOLDER_PATH)
            with open(self.path_name, "a") as file:
                file.write(text)
        except IOError:
            print('failed to save file to server')
            self.path_name = None
            res = False
        return res
