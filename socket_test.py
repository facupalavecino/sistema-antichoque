import socket
import time

address = ('192.168.4.1', 666)
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.settimeout(10)

sock.connect(address)

a = 1;
while(1):
	try:
		print ("a = ",a,"\n\n")
		sock.sendall(bytes(a))
		print ("Enviado")
		a+= 1
	except sock.error as e:
		print ("\n",err,"\n")
	time.sleep(1)


