import socket

SERVER_IP = '0.0.0.0'  # 任意のアドレスからの接続を許可
SERVER_PORT = 3000  # Spresenseと一致させる

def start_server():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((SERVER_IP, SERVER_PORT))
    server_socket.listen(1)
    print("Server listening on {}:{}".format(SERVER_IP, SERVER_PORT))

    while True:
        client_socket, client_address = server_socket.accept()
        print("Connection from:", client_address)
        
        # 受信したデータをファイルに保存
        with open("received.mp3", "wb") as file:
            while True:
                data = client_socket.recv(1024)
                if not data:
                    break
                file.write(data)
        
        print("MP3 file received.")
        client_socket.close()

start_server()
