import sqlite3
import pandas
import clients
class Database:
    CLIENTS_TABLE = 'clients'
    FILES_TABLE = 'files'

    def __init__(self, name):
        self.name = name
        self.clients_table = pandas.DataFrame()
        self.files_table = pandas.DataFrame()

    def initialize(self):
        # create clients table
        if not self.is_table_exist(Database.CLIENTS_TABLE):
            self.executescript(f"""
            CREATE TABLE IF NOT EXISTS {Database.CLIENTS_TABLE} (
            ID CHAR(16) PRIMARY KEY,
            Name CHAR(255) UNIQUE,
            PublicKey CHAR(500),
            LastSeen DATE,
            AESKey CHAR(128)
            );
            """)
        else:
            self.clients_table = pandas.read_sql_query(f"SELECT * FROM {Database.CLIENTS_TABLE}", self.create_connection())

        # create files table
        if not self.is_table_exist(Database.FILES_TABLE):
            self.executescript(f"""
                CREATE TABLE IF NOT EXISTS {Database.FILES_TABLE} (
                ID CHAR(16) NOT NULL,
                FileName CHAR(255) NOT NULL,
                PathName CHAR(255) NOT NULL,
                Verified INTEGER NOT NULL CHECK (Verified IN (0, 1)),
                Content BLOB,
                FOREIGN KEY(ID) REFERENCES CLIENTS_TABLE(ID)
                );
                """)
        else:
            self.files_table = pandas.read_sql_query(f"SELECT * FROM {Database.FILES_TABLE}", self.create_connection())

    def create_connection(self):
        """ create a database connection to the SQLite database
            specified by db_file
        :param db_file: database file
        :return: Connection object or None
        """
        conn = None
        try:
            conn = sqlite3.connect(self.name)
            conn.text_factory = bytes
            return conn
        except Exception as e:
            print(e)

        return conn

    def executescript(self, script):
        conn = self.create_connection()
        try:
            conn.executescript(script)
            conn.commit()
        except Exception as err:
            print(err)
        conn.close()

    def insert_data(self, query, args, commit=True):
        results = None
        conn = self.create_connection()
        try:
            cur = conn.cursor()
            cur.execute(query, args)
            if commit:
                conn.commit()
                results = True
        except Exception as e:
            print(f'database execute: {e}')
        conn.close()
        return results

    def get_data(self, query, args=()):
        conn = self.create_connection()
        try:
            results = pandas.read_sql_query(query, conn, params=args)
        except Exception as e:
            print(f'database execute: {e}')
        conn.close()
        return results
    
    def get_client_by_username(self, username):
        results = self.get_data(f"SELECT ID, Name, PublicKey, LastSeen, AESKey FROM {Database.CLIENTS_TABLE} WHERE Name = ?", [username])
        if not bool(results.size):
            return False
        return clients.Client(results.values[0][0], results.values[0][1], results.values[0][2], results.values[0][3], results.values[0][4])

    def is_table_exist(self, table):
        print(f'Checking if {table} table exists in the database...')
        list_of_tables = self.get_data(f"SELECT * FROM sqlite_master WHERE type='table' AND name='{table}';")
        return bool(list_of_tables.size)

    def add_new_client(self, client):
        if not self.is_client_exist(client.id):
            self.executescript(f"INSERT INTO {Database.CLIENTS_TABLE} (ID,Name,LastSeen,AESKey) VALUES ('{client.id}','{client.name}','{client.last_seen}','{client.aes_key}')")
            self.clients_table = pandas.read_sql_query(f"SELECT * FROM {Database.CLIENTS_TABLE}", self.create_connection())
            return True
        return False

    def is_client_exist(self, client_id):
        # check if client exists in the clients table
        clients_list = self.get_data(f"SELECT * FROM {Database.CLIENTS_TABLE} WHERE ID='{client_id}'")
        return bool(clients_list.size)

    def add_new_file(self, file):
        if not self.is_file_exist(file.file_name):
            self.executescript(f"INSERT INTO {Database.FILES_TABLE} (ID,FileName,PathName,Verified) VALUES ('{file.id}','{file.file_name}','{file.path_name}',{file.verified})")
            self.files_table = pandas.read_sql_query(f"SELECT * FROM {Database.FILES_TABLE}", self.create_connection())
            return True
        return False

    def is_file_exist(self, file_name):
        # check if file exists in the files table
        files_list = self.get_data(f"SELECT * FROM {Database.FILES_TABLE} WHERE FileName='{file_name}'")
        return bool(files_list.size)
    
    def set_last_seen(self, client_id, time):
        result = self.insert_data(f"UPDATE {Database.CLIENTS_TABLE} SET LastSeen = ? WHERE ID = ?", [time, client_id], True)
        if not result:
            return False
        self.client_table = pandas.read_sql_query(f"SELECT * FROM {Database.CLIENTS_TABLE}", self.create_connection())
        return True  

    def get_client_public_key(self, client_id):
        results = self.get_data(f"SELECT PublicKey FROM {Database.CLIENTS_TABLE} WHERE ID = ?", [client_id])
        if not bool(results.size):
            return None
        return results.values[0][0]

    def update_client_public_key(self, client_id:str, public_key:str):
        self.executescript(f"UPDATE {Database.CLIENTS_TABLE} SET PublicKey='{public_key}' WHERE ID='{client_id}'")
        self.client_table = pandas.read_sql_query(f"SELECT * FROM {Database.CLIENTS_TABLE}", self.create_connection())