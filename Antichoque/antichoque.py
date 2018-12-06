from flask import Flask, session, redirect, url_for, escape, request, render_template
from hashlib import md5
from flask import jsonify
import MySQLdb
import socket
import math
import datetime
import time

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# Connect the socket to the port where the server is listening
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
    global sock
    if not 'username' in session:
        return render_template('index.html')
    else:
        # Definiciones locales
        username_session = escape(session['username']).capitalize()
        username_session = username_session.lower()
        cur.execute("SELECT COUNT(1) FROM users WHERE state = 'online' && username = %s;", [username_session])
        if not cur.fetchone()[0]:
            error = request.args.get("error") 
            if not error:
                error = "Usted no esta conectado"
            return render_template('index.html', error=error, session_user_name=username_session, state='Desconectado', direction=0, marcha=0 , turn=0, back_distance=0, front_distance=0, right_distance=0, left_distance=0)
        else:
            inform = request.args.get("inform") 
            return render_template('index.html', inform=inform, session_user_name=username_session,  state='Desconectado', direction=0, marcha=0, turn=0, back_distance=0, front_distance=0, right_distance=0, left_distance=0)

@app.route("/access")
def access():
    global sock
    global turn
    global direction
    global speed
    if 'username' in session:
        # Definiciones locales
        username_session = escape(session['username']).capitalize()
        username_session = username_session.lower()
        f_distance = ''
        b_distance = ''
        l_distance = ''
        r_distance = ''
        front_distance = 0
        back_distance = 0
        right_distance = 0
        left_distance = 0
        state = 'Desconectado'
        cur.execute("SELECT COUNT(1) FROM users WHERE state = 'online' && username = %s;", [username_session])
        #Si estoy conectado al servidor del autito puedo utilizarlo sino no
        if cur.fetchone()[0] == 1:
            try:
                sock.settimeout(3)
                sock.sendall('1'); #Envio un pedido de informacion, es decir un solo byte envio
                recibido = sock.recv(1024)
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
                        elif i == 5:  # Analisis de la direccion(si es cero es porque va hacia adelante o hacia atras)
                            turn = int(recibido[j])
                            if recibido[j] == '1':
                                state = 'Izquierda'
                            elif recibido[j] == '2':
                                state = 'Derecha' 
                        elif i == 6:
                            r_distance = r_distance + recibido[j]
                        else:
                            l_distance = l_distance + recibido[j]

                        j=j+1
                    j=j+1
                #Verificamos que la marcha sea 0 para indicar que el vehiculo esta parado
                if speed == 0:
                    state = 'Parado'
                front_distance = int(f_distance)
                back_distance = int(b_distance)
                right_distance = int(r_distance)
                left_distance = int(l_distance)
                #Agregamos los eventos, si son de importancia
                fecha = datetime.date.today()
                hora = str(datetime.datetime.now().time())
                hora = hora.split(".")[0]
                if right_distance < 100:
                    cur.execute('INSERT INTO eventos(username, fecha, hora, dist, type) VALUES (%s,%s,%s,%s,%s)', ([username_session], [fecha], [hora], [right_distance], 'Derecha'))
                    db.commit() 
                if left_distance < 100:
                    cur.execute('INSERT INTO eventos(username, fecha, hora, dist, type) VALUES (%s,%s,%s,%s,%s)', ([username_session], [fecha], [hora], [left_distance], 'Izquierda'))
                    db.commit() 
                if front_distance < 100:
                    cur.execute('INSERT INTO eventos(username, fecha, hora, dist, type) VALUES (%s,%s,%s,%s,%s)', ([username_session], [fecha], [hora], [front_distance], 'Avance'))
                    db.commit()
                if back_distance < 100:
                    cur.execute('INSERT INTO eventos(username, fecha, hora, dist, type) VALUES (%s,%s,%s,%s,%s)', ([username_session], [fecha], [hora], [back_distance], 'Retroceso'))
                    db.commit()
            except socket.error as socketerror:
                sock.close()
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                error = 'Perdida de conexion'
                print(socketerror)
                print(error)
                cur.execute("UPDATE users SET state = 'semi-online' WHERE username = %s;", [username_session])
                if (reconnect()):
                    print('Reconexion exitosa.')
                    inform = 'Reconexion exitosa'
                    return redirect(url_for('index', inform=inform))
                result = {"error": error, "front_distance": 0, "back_distance": 0}
                return jsonify(result)
            result = {"error": "Ninguno", "state": state, "direction": direction, "marcha": speed, "turn": turn, "front_distance": front_distance, "back_distance": back_distance}
            return jsonify(result)
    result = {"error": "Sin conexion"}
    return jsonify(result)


@app.route('/control', methods=['GET', 'POST'])
def control(): 
    global turn
    global direction
    global speed 
    if request.method == 'POST':
        username_session = escape(session['username']).capitalize()
        username_session = username_session.lower()
        cur.execute("SELECT COUNT(1) FROM users WHERE state = 'online' && username = %s;", [username_session])
        #Si estoy conectado puedo utilizarlo sino no
        if cur.fetchone()[0]:
            action = request.form['action']
            if action == "Frenar":
                direction = 0
                speed = 0
                turn = 0
            elif action == "Marcha A":
                if turn !=0 or direction !=1:
                    speed = 1
                elif speed < 3:
                    speed = speed + 1
            elif action == "Marcha D":
                if turn !=0 or direction !=1:
                    speed = 1
                elif speed>0:
                    speed = speed - 1
            elif action == "Avanzar":
                direction = 1
                speed = 1
                turn = 0
            elif action == "Izquierda": 
                direction = 1
                speed = 1
                turn = 1
            elif action == "Derecha":
                direction = 1
                speed = 1
                turn = 2
            elif action == "Retroceder":
                direction = 2
                speed = 1
                turn = 0
            sock.sendall(str(direction) + str(speed) + str(turn)); #Envio un pedido de comando, tres bytes (VER SI FUNCIONA)
    return redirect(url_for('index'))

def reconnect():
    global sock
    try:   
        print('Reconexion')
        sock.connect(server_address)
        sock.sendall(str(0) + str(0) + str(0));
        username_session = escape(session['username']).capitalize()
        username_session = username_session.lower()
        cur.execute("UPDATE users SET state = 'online' WHERE username = %s;", [username_session])
        return True
    except socket.error as socketerror:
        print(socketerror)
        error = 'Error al conectarse'
        sock.close()
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        return False


@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        error = None
        try:
            username  = request.form['username']
            cur.execute("SELECT COUNT(1) FROM users WHERE username = %s;", [username])
            # Si no existe ese nombre de usuario, informamos error
            if not cur.fetchone()[0]:
                raise ServerError('Invalid username')

            password  = request.form['password']
            cur.execute("SELECT password FROM users WHERE username = %s;", [username])
            # Si la contrasena es correcta permitimos el login
            for row in cur.fetchall():
                if md5(password).hexdigest() == row[0]:
                    session['username'] = request.form['username']
                    cur.execute("UPDATE users SET state = 'semi-online' WHERE username = %s;", [username])
                    db.commit()
                    return redirect(url_for('index'))

            raise ServerError('Invalid password')
        except ServerError as e:
            error = str(e)
            return render_template('login.html', error=error)
    elif 'username' in session:
        return redirect(url_for('index'))
    else:
        return render_template('login.html')


@app.route('/connect')
def connect():
    global sock
    if not 'username' in session:
        return render_template('index.html')
    else:
        username_session = escape(session['username']).capitalize()
        username_session = username_session.lower()
        cur.execute("SELECT COUNT(1) FROM users WHERE state= 'online' && username = %s;", [username_session])
        if cur.fetchone()[0]:
            return redirect(url_for('index'))
        else:
            try:   
                sock.connect(server_address)
                cur.execute("UPDATE users SET state = 'online' WHERE username = %s;", [username_session])
                inform = 'Conexion exitosa'
                return redirect(url_for('index', inform=inform))
            except socket.error as socketerror:
                print(socketerror)
                error = 'Error al conectarse'
                sock.close()
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                return redirect(url_for('index', error=error))


@app.route('/logout')
def logout():
    global sock
    if not 'username' in session:
        return render_template('index.html')
    else:
        username = escape(session['username']).capitalize()
        username = username.lower()
        session.pop('username', None)
        cur.execute("SELECT COUNT(1) FROM users WHERE state = 'online' && username = %s;", [username])
        if cur.fetchone()[0]:
            sock.close()
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        cur.execute("UPDATE users SET state = 'offline' WHERE username = %s;", [username])
        db.commit()
        return redirect(url_for('index'))

@app.route('/signup', methods=['GET', 'POST'])
def signup():
    if request.method == 'POST':
        error = None
        inform = 'Usted se ha registrado correctamente'
        try:
            name = request.form['name']
            surname = request.form['surname']
            email = request.form['email']
            username = request.form['username']
            password = request.form['password']
            cur.execute("SELECT COUNT(1) FROM users WHERE name = %s;", [username])
            if cur.fetchone()[0]:
                raise ServerError('Invalid username')
            cur.execute("SELECT COUNT(1) FROM users WHERE email = %s;", [email])
            if cur.fetchone()[0]:
                raise ServerError('Invalid email')
            cur.execute('INSERT INTO users(name,surname,email,username,password,state) VALUES (%s,%s,%s,%s,%s,%s)', ([name], [surname], [email], [username], [md5(password).hexdigest()], 'offline'))
            db.commit()
        except ServerError as e:
            error = str(e)
            return render_template('signup.html', error=error)
        return render_template('login.html', inform=inform)
    elif 'username' in session:
        return redirect(url_for('index'))
    elif request.method == 'GET':
        return render_template('signup.html')


@app.route('/stats')
def stats():
    if not 'username' in session:
        return render_template('index.html')
    else:
        username = escape(session['username']).capitalize()
        username = username.lower()
        cur.execute('SELECT * FROM eventos WHERE username = %s ORDER BY id DESC LIMIT 10;', [username])
        lista=[row for row in cur.fetchall()]
        return render_template('stats.html', eventos=lista)


@app.route('/faqs')
def faqs():
    if not 'username' in session:
        return render_template('faqs.html')
    else:
        username_session = escape(session['username']).capitalize()
        return render_template('faqs.html', session_user_name=username_session)


@app.route('/account', methods=['GET', 'POST'])
def account():
    if not  'username' in session:
        return render_template('index.html')
    else:
        username_session = escape(session['username']).capitalize()
        username_session = username_session.lower()
        if request.method == 'POST':
            error = None
            inform = 'Usted ha modificado sus datos correctamente'
            try:
                name = request.form['name']
                surname = request.form['surname']
                email = request.form['email']
                username = request.form['username']
                password = request.form['password']
                cur.execute("SELECT COUNT(1) FROM users WHERE username = %s;", [username])
                #Si el username fue cambiado y hay otro igual, informamos error
                if username_session != username:
                    if cur.fetchone()[0]:
                        raise ServerError('Invalid username')
                cur.execute("SELECT email FROM users WHERE username = %s;", [username_session])       
                old_email = cur.fetchone()[0]
                cur.execute("SELECT COUNT(1) FROM users WHERE email = %s;", [email])
                #Si el email es distinto del actual, y hay otro igual al nuevo, informamos error
                if old_email != email:
                    if cur.fetchone()[0]:
                        raise ServerError('Invalid email')
                cur.execute('UPDATE users SET name=%s, surname=%s, email=%s, username=%s, password=%s WHERE username = %s', ([name], [surname], [email], [username], [md5(password).hexdigest()], [username_session]))
                db.commit()
            except ServerError as e:
                error = str(e)
                return render_template('account.html', username_sesion=username_session, error=error, name=name, surname=surname, email=email, username=username)
            return render_template('account.html', username_sesion=username_session, inform=inform, name=name, surname=surname, email=email, username=username)
        else:
            cur.execute('SELECT * FROM users WHERE username = %s;', [username_session])
            row = cur.fetchone()
            name = row[1]
            surname = row[2]
            email = row[3]
            username = row[4]
            return render_template('account.html', username_sesion=username_session, name=name, surname=surname, email=email, username=username)


if __name__ == '__main__':
    app.run(host='localhost', port=8000)