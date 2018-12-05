import socket

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = ('127.0.0.1',65432)
sock.settimeout(5)

try:
    print("\nConectandose con al puerto 65432...\n")
    sock.connect(server_address)
    print("\n[CONECTADO]\n")
    connected = True
    while connected:
    	sock.sendall("Hola")
    	recibido = sock.recv(2048)
    	print("\n")
    	print(recibido)
    	print("\n")
except socket.error as e:
    sock.close()
    print("\n[ERROR]\n", e)