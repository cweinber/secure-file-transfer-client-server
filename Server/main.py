import server
if __name__ == '__main__':
    PORT_INFO = "port.info"
    server_port = None
    try:
        with open(PORT_INFO) as f:
            server_port = int(f.read().strip())
    except Exception as err:
        print(f"can't init port number error: {err} using default port 8000")
        server_port = 8000

    server = server.Server(server_port)
    server.start_listener()
