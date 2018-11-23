from flask import Flask, session, redirect, url_for, escape, request, render_template
from hashlib import md5
from flask import jsonify
import MySQLdb
import socket
import math
import datetime

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#Direccion donde se conectara el socket con el ESP8266 (tupla IP-PORT).
server_address = ('192.168.4.1', 666)
#error = ''
#inform = ''
direction = 0
turn = 0
speed = 0

app = Flask(__name__)

if __name__ == '__main__':
    db = MySQLdb.connect(host="localhost", user="root", passwd="1234", db="antichoque")
    cur = db.cursor()
    app.secret_key = 'A0Zr98j/3yX R~XHH!jmN]LWX/,?RT'

class ServerError(Exception):pass

@app.errorhandler(404)
def page_not_found(e):
    return redirect(url_for('index'))

@app.route('/')
def index():
    session_user_name = 1;
    inform = "Bienvenido"
    return render_template('index.html', session_user_name=session_user_name, inform=inform, state='Desconectado', direction=0, marcha=0, turn=0, back_distance=0, front_distance=0)

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
        print("\n/access intenta conectarse al ESP8266\n")
        sock.connect(server_address)
        sock.send('1'); #Envio un pedido de informacion, es decir un solo byte envio
        recibido = sock.recv(15)
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
        if front_distance < 100:
            cur.execute('INSERT INTO eventos(username, fecha, hora, dist, type) VALUES (%s,%s,%s,%s,%s)', ('facu2', [fecha], [hora], [front_distance], 'Avance'))
            db.commit()
        if back_distance < 100:
            cur.execute('INSERT INTO eventos(username, fecha, hora, dist, type) VALUES (%s,%s,%s,%s,%s)', ('facu2', [fecha], [hora], [back_distance], 'Retroceso'))
            db.commit()
    except socket.error as socketerror:
        sock.close()
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        error = 'Error al conectar en /access'
        print(error)
        result = {"error": error}
        return jsonify(result)
    result = {"error": "Ninguno", "state": state, "direction": direction, "marcha": speed, "turn": turn, "front_distance": front_distance, "back_distance": back_distance}
    sock.close()
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
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


#@app.route('/login', methods=['GET', 'POST'])
#def login():
#    if request.method == 'POST':
#        error = None
#        try:
#            username  = request.form['username']
#            cur.execute("SELECT COUNT(1) FROM users WHERE username = %s;", [username])
#            # Si no existe ese nombre de usuario, informamos error
#            if not cur.fetchone()[0]:
#                raise ServerError('Invalid username')
#
#            password  = request.form['password']
#            cur.execute("SELECT password FROM users WHERE username = %s;", [username])
#            # Si la contrasena es correcta permitimos el login
#            for row in cur.fetchall():
#                if md5(password).hexdigest() == row[0]:
#                    session['username'] = request.form['username']
#                    cur.execute("UPDATE users SET state = 'online' WHERE username = %s;", [username])
#                   db.commit()
#                    return redirect(url_for('index'))
#
#            raise ServerError('Invalid password')
#        except ServerError as e:
#            error = str(e)
#           return render_template('login.html', error=error)
#    elif 'username' in session:
#        return redirect(url_for('index'))
#    else:
#        return render_template('login.html')


@app.route('/connect')
def connect():
    global sock
    try:   
        sock.connect(server_address)
        print('\n/connect satisfactorio\n')
        sock.close()
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        inform = '/connect satisfactorio'
        return redirect(url_for('index', inform=inform))
    except socket.error as socketerror:
        print("\nError al conectar /connect\n")
        print(socketerror)
        error = 'Error en /connect al intentar conectarse al vehiculo'
        sock.close()
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        return redirect(url_for('index', error=error))


#@app.route('/logout')
#def logout():
#    global sock
#    if not 'username' in session:
#        return render_template('index.html')
#    else:
#        username = escape(session['username']).capitalize()
#        username = username.lower()
#        session.pop('username', None)
#        cur.execute("SELECT COUNT(1) FROM users WHERE state = 'online' && username = %s;", [username])
#        if cur.fetchone()[0]:
#            sock.close()
#            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#        cur.execute("UPDATE users SET state = 'offline' WHERE username = %s;", [username])
#        db.commit()
#        return redirect(url_for('index'))

#@app.route('/signup', methods=['GET', 'POST'])
#def signup():
#    if request.method == 'POST':
#        error = None
#        inform = 'Usted se ha registrado correctamente'
#        try:
#            name = request.form['name']
#            surname = request.form['surname']
#            email = request.form['email']
#            username = request.form['username']
#            password = request.form['password']
#            cur.execute("SELECT COUNT(1) FROM users WHERE name = %s;", [username])
#            if cur.fetchone()[0]:
#                raise ServerError('Invalid username')
#            cur.execute("SELECT COUNT(1) FROM users WHERE email = %s;", [email])
#            if cur.fetchone()[0]:
#                raise ServerError('Invalid email')
#            cur.execute('INSERT INTO users(name,surname,email,username,password,state) VALUES (%s,%s,%s,%s,%s,%s)', ([name], [surname], [email], [username], [md5(password).hexdigest()], 'offline'))
#            db.commit()
#        except ServerError as e:
#            error = str(e)
#            return render_template('signup.html', error=error)
#        return render_template('login.html', inform=inform)
#    elif 'username' in session:
#        return redirect(url_for('index'))
#    elif request.method == 'GET':
#        return render_template('signup.html')


@app.route('/stats')
def stats():
    cur.execute('SELECT * FROM eventos WHERE username = facu2 ORDER BY id DESC LIMIT 10;')
    lista=[row for row in cur.fetchall()]
    return render_template('stats.html', eventos=lista)


@app.route('/faqs')
def faqs():
    if not 'username' in session:
        return render_template('faqs.html')
    else:
        username_session = escape(session['username']).capitalize()
        return render_template('faqs.html', session_user_name=username_session)


#@app.route('/account', methods=['GET', 'POST'])
#def account():
#    if not  'username' in session:
#        return render_template('index.html')
#    else:
#        username_session = escape(session['username']).capitalize()
#        username_session = username_session.lower()
#        if request.method == 'POST':
#            error = None
#            inform = 'Usted ha modificado sus datos correctamente'
#            try:
#                name = request.form['name']
#                surname = request.form['surname']
#                email = request.form['email']
#                username = request.form['username']
#                password = request.form['password']
#                cur.execute("SELECT COUNT(1) FROM users WHERE username = %s;", [username])
#                #Si el username fue cambiado y hay otro igual, informamos error
#                if username_session != username:
#                    if cur.fetchone()[0]:
#                        raise ServerError('Invalid username')
#                cur.execute("SELECT email FROM users WHERE username = %s;", [username_session])       
#                old_email = cur.fetchone()[0]
#                cur.execute("SELECT COUNT(1) FROM users WHERE email = %s;", [email])
#                #Si el email es distinto del actual, y hay otro igual al nuevo, informamos error
#                if old_email != email:
#                    if cur.fetchone()[0]:
#                        raise ServerError('Invalid email')
#                cur.execute('UPDATE users SET name=%s, surname=%s, email=%s, username=%s, password=%s WHERE username = %s', ([name], [surname], [email], [username], [md5(password).hexdigest()], [username_session]))
#                db.commit()
#            except ServerError as e:
#                error = str(e)
#                return render_template('account.html', username_sesion=username_session, error=error, name=name, surname=surname, email=email, username=username)
#            return render_template('account.html', username_sesion=username_session, inform=inform, name=name, surname=surname, email=email, username=username)
#        else:
#            cur.execute('SELECT * FROM users WHERE username = %s;', [username_session])
#            row = cur.fetchone()
#            name = row[1]
#            surname = row[2]
#            email = row[3]
#            username = row[4]
#            return render_template('account.html', username_sesion=username_session, name=name, surname=surname, email=email, username=username)


if __name__ == '__main__':
    app.run(host='localhost', port=8000)