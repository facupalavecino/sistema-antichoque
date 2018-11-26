from flask import Flask, session, redirect, url_for, escape, request, render_template
from hashlib import md5
from flask import jsonify
import socket
from flask_socketio import SocketIO, emit
import math
import datetime
import time, threading

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = ('192.168.4.1', 666)
sock.settimeout(5)
connected = False 

try:
    print("\nConectandose con el ESP...\n")
    sock.connect(server_address)
    print("\n[CONECTADO]\n")
    connected = True
except socket.error as e:
    sock.close()
    print("\nERROR...\n", e)



# def testConnection():
#     try:
#         data = sock.recv(128)
#         if not data:
#             connected = False
#         else:
#             connected = True
#     except socket.error as e:
#         print(e)
#         connected = False

# def test_connection_target():
#   while True:
#     testConnection()
#     time.sleep(10)

# t = threading.Thread(target=test_connection_target)
# t.daemon = True
# t.start()


direction = 0
turn = 0
speed = 0

app = Flask(__name__)
app.config['SECRET KEY'] = 'secret!'
socketio = SocketIO(app)

class ServerError(Exception):pass

@app.errorhandler(404)
def page_not_found(e):
    return redirect(url_for('index'))

@app.route('/')
def index():
    return render_template('index.html', connected=connected, state='Desconectado', direction=0, marcha=0, turn=0, back_distance=0, front_distance=0)

@app.route("/access")
def access():
    global sock
    global turn
    global direction
    global speed
    f_distance = ''
    b_distance = ''
    front_distance = 0
    back_distance = 0
    state = 'Desconectado'
    try:
        #print("\n/access intenta conectarse al ESP8266\n")
        #sock.connect(server_address)
        print ("aca")
        sock.sendall('1'); #Envio un pedido de informacion, es decir un solo byte envio
        print ("Envio dato")
        recibido ='0/0/0/0/0/0'#sock.recv(15)
        print ("recv dato")
        j = 0
        for i in range(0, 5):
            while recibido[j] != '/':
                if i == 0:
                    f_distance = f_distance + recibido[j]
                elif i == 1:
                    b_distance = b_distance + recibido[j]
                elif i == 2:  # Analisis del avance
                    direction = int(recibido[j]) 
                    if recibido[j] == '0': 
                        state = 'Parado'
                    elif recibido[j] == '1':
                        state = 'Avanzando'
                    elif recibido[j] == '2':
                       state = 'Retrocediendo'               
                elif i == 3:  # Analisis de la velocidad
                    speed = int(recibido[j]) 
                else:  # Analisis de la direccion(si es cero es porque va hacia adelante o hacia atras)
                    turn = int(recibido[j])
                    if recibido[j] == '1':
                        state = 'Izquierda'
                    elif recibido[j] == '2':
                        state = 'Derecha' 
                j=j+1
            j=j+1
        #Verificamos que la marcha sea 0 para indicar que el vehiculo esta parado
        if speed == 0:
            state = 'Parado'
        front_distance = int(f_distance)
        back_distance = int(b_distance)
        #Agregamos los eventos, si son de importancia
        fecha = datetime.date.today()
        hora = str(datetime.datetime.now().time())
        hora = hora.split(".")[0]
        print ("antes de mysql")
    except socket.error as socketerror:
        print (socketerror)
        #sock.close()
        #sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        error = 'Error al conectar en /access'
        print(error)
        result = {"error": error}
        return jsonify(result)
    result = {"error": "Ninguno", "state": state, "direction": direction, "marcha": speed, "turn": turn, "front_distance": front_distance, "back_distance": back_distance}
    print ("close")
    #sock.close() 
    #sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print("\nAccess finaliza OK\n")
    return jsonify(result)

@app.route('/control', methods=['GET', 'POST'])
def control(): 
    global turn
    global direction
    global speed 
    global sock
    if request.method == 'POST':
        action = request.form['action']
        if action == "Frenar":
            print("\n/control Comando -----> FRENAR\n")
            direction = 0
            speed = 0
            turn = 0
        elif action == "Marcha A":
            print("\n/control Comando -----> MARCHA+\n")
            if turn !=0 or direction !=1:
                speed = 1
            elif speed < 3:
                speed = speed + 1
        elif action == "Marcha D":
            print("\n/control Comando -----> MARCHA-\n")
            if turn !=0 or direction !=1:
                speed = 1
            elif speed>0:
                speed = speed - 1
        elif action == "Avanzar":
            print("\n/control Comando -----> AVANZAR\n")
            direction = 1
            speed = 1
            turn = 0
        elif action == "Izquierda": 
            print("\n/control Comando -----> IZQUIERDA\n")
            direction = 1
            speed = 1
            turn = 1
        elif action == "Derecha":
            print("\n/control Comando -----> DERECHA\n")
            direction = 1
            speed = 1
            turn = 2
        elif action == "Retroceder":
            print("\n/control Comando -----> RETROCEDER\n")
            direction = 2
            speed = 1
            turn = 0
        try:
            sock.connect(server_address)
            sock.send(str(direction) + str(speed) + str(turn)); #Envio un pedido de comando, tres bytes (VER SI FUNCIONA)
        except socket.error as socketerror:
            print("\nError al conectar POST /control\n")
        sock.close()
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    return redirect(url_for('index'))

@socketio.on('connection', namespace='/sock')
def handle_connection():
    global sock
    try:
        print("\nConectandose con el ESP...\n")
        sock.connect(server_address)
        print("\n[CONECTADO]\n")
        connected = True
    except socket.error as e:
        sock.close()
        print("\nERROR...\n", e)

@socketio.on('control', namespace='/sock')
def handle_controlling(order):
    global sock
    global direction
    global speed
    global turn
    print("\nOrden:",order['data'])
    if (order['data'] == "Avanzar"):
        direction = 1
        speed = 1
        turn = 0
    elif (order['data'] == "Frenar"):
        direction = 0
        speed = 0
        turn = 0
    elif (order['data'] == "Izquierda"):
        direction = 1
        speed = 1
        turn = 1
    elif (order['data'] == "Derecha"):
        direction = 1
        speed = 1
        turn = 2
    elif (order['data'] == "Reversa"):
        direction = 2
        speed = 1
        turn = 0
    elif (order['data'] == "Marcha+"):
        if turn !=0 or direction !=1:
            speed = 1
        elif speed < 3:
            speed = speed + 1
    elif (order['data'] == "Marcha-"):
        if turn !=0 or direction !=1:
            speed = 1
        elif speed>0:
            speed = speed - 1
    print("\nDatos a enviar:",str(direction).encode('utf-8') + str(speed).encode('utf-8') + str(turn).encode('utf-8'))
    if (sock.sendall(str(direction).encode('utf-8') + str(speed).encode('utf-8') + str(turn).encode('utf-8')) is not None):
        print("\nError al enviar el comando\n")
    else:
        print("\nComando enviado\n")




@socketio.on('fetch', namespace='/sock')
def handle_fetching():
    global sock
    global turn
    global direction
    global speed
    f_distance = ''
    b_distance = ''
    front_distance = 0
    back_distance = 0
    state = 'Desconectado'
    c = "c"
    try:
        sock.sendall(c.encode('utf-8')); #Envio un pedido de informacion, es decir un solo byte envio
        print ("\nSe efectuo el pedido")
        respuesta = sock.recv(1)
        # j = 0
        # for i in range(0, 5):
        #     while recibido[j] != '/':
        #         if i == 0:
        #             f_distance = f_distance + recibido[j]
        #         elif i == 1:
        #             b_distance = b_distance + recibido[j]
        #         elif i == 2:  # Analisis del avance
        #             direction = int(recibido[j]) 
        #             if recibido[j] == '0': 
        #                 state = 'Parado'
        #             elif recibido[j] == '1':
        #                 state = 'Avanzando'
        #             elif recibido[j] == '2':
        #                state = 'Retrocediendo'               
        #         elif i == 3:  # Analisis de la velocidad
        #             speed = int(recibido[j]) 
        #         else:  # Analisis de la direccion(si es cero es porque va hacia adelante o hacia atras)
        #             turn = int(recibido[j])
        #             if recibido[j] == '1':
        #                 state = 'Izquierda'
        #             elif recibido[j] == '2':
        #                 state = 'Derecha' 
        #         j=j+1
        #     j=j+1
        # #Verificamos que la marcha sea 0 para indicar que el vehiculo esta parado
        # if speed == 0:
        #     state = 'Parado'
        # front_distance = int(f_distance)
        # back_distance = int(b_distance)
        print('\n[SUCCESS] Fetching data from ESP\n')
        emit('fetch success', {"state": state, "direction": direction, "marcha": speed, "turn": turn, "front_distance": front_distance, "back_distance": back_distance})
    except socket.error as e:
        print('\n[ERROR] Fetching data from ESP\n',e)
        emit('fetch error')


if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0', port=8000)