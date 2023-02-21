import socket
import serial
from cryptography.hazmat.primitives.ciphers.aead import AESGCM
import subprocess
import time


class SocketManager:
    def __init__(self):
        self.serv_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # Get host name
        self.serv_socket.bind(('localhost', 2230))
        self.serv_socket.listen(1)     
    
    def accept_client(self):
        (client, address) = self.serv_socket.accept()
        self.client_socket = client
        self.client_address = address

    def handle_input(self):
        dataIn = self.client_socket.recv(128)
        output = ""
        try:
            output = str(dataIn, "utf-8").strip()
        except UnicodeDecodeError:
            pass
        return output

def start_terminal(port: serial.Serial, key: str):
    server = SocketManager()
    proc = subprocess.Popen(["putty", "-raw", "-P", "2230", "localhost"])
    server.accept_client()
    
    key = AESGCM.generate_key(bit_length=128)
    aesgcm = AESGCM(key)
    nonce = 0

    uart_len = 0
    uart_delta = 10

    while True:
        user = server.handle_input().strip()
        if len(user) < 1:
            continue

        if user.startswith('!!exit'):
            break

       # enc = aesgcm.encrypt(nonce.to_bytes(12,'little'), bytes(user,"utf-8"), None)
        msg_len = port.write(bytes(user,"utf-8"))
        print(f"Wrote {msg_len} bytes.")

        # while port.in_waiting < 1:
        #     time.sleep(0.01)
        # time.sleep(msg_len*0.01)

        # embd = port.read_all()
        # #dec = aesgcm.decrypt(nonce.to_bytes(12,'little'), embd, None)
        # output = str(embd,"utf-8").strip()
        # print(output)
        #server.client_socket.send(bytes(output,'utf-8'))
        #server.client_socket.send(bytes('\r\n', 'utf-8'))
        
        nonce += 1
        uart_delta = 0
        uart_len = 0
    
    proc.kill()
    server.client_socket.close()