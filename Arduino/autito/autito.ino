/*FUNCIONES ORIGINALES*/
#include <Arduino.h>

/*FUNCIONES EDITADAS O DESARROLLADAS*/

#include <HC_SR04X2.h>
#include <ControlMotor.h>
#include "ESP8266.h"

/*DEFINICIONES DE CONFIGURACION*/
#define BAUDRATE            115200

#define HC_SR04FRONT_OUT    8 // Numero de pin por el cual se envia el pulso de START (trig)
#define HC_SR04FRONT_IN     9 // Numero de pin por el cual se recibe informacion de la medioción (echo)

#define HC_SR04RIGHT_OUT    10 // Numero de pin por el cual se envia el pulso de START (trig)
#define HC_SR04RIGHT_IN     11 // Numero de pin por el cual se recibe informacion de la medioción (echo)

#define HC_SR04LEFT_OUT     12 // Numero de pin por el cual se envia el pulso de START (trig)
#define HC_SR04LEFT_IN      13 // Numero de pin por el cual se recibe informacion de la medioción (echo)

#define HC_SR04BACK_OUT     A0 // Numero de pin por el cual se envia el pulso de START (trig)
#define HC_SR04BACK_IN      A1 // Numero de pin por el cual se recibe informacion de la medioción (echo)

#define TAM_BUFFER          128
#define CRITIC_DISTANCE     40

/*DEFINICIONES DE TIEMPO DE EJECUCION*/
#define F_Sensed                1
#define F_Communication   3
#define F_Movement           2

/*DECLARACION DE VARIABLES GLOBALES*/
ControlMotor control(2, 3, 7, 4, 5, 6); // MotorDer1,MotorDer2,MotorIzq1,MotorIzq2,PWM_Derecho,PWM_Izquierdo
HC_SR04X2 sensor_distance;
ESP8266 wifi(Serial);

uint8_t buffer[TAM_BUFFER] = {0}; //buffer para enviar y recibir informacion
uint32_t len = 0; //Tamaño del mensaje recibido o el mensaje a enviar
uint32_t frecuency = 1; //variables que nos permite implementar un porgrmacion de tareas
String data_send;
//variables de almacenamiento de distancias
float distance_front = 0;
float distance_right = 0;
float distance_left = 0;
float distance_back = 0;
//variables de desplazamiento -> el vehiculo lee estas variables y decide su movimiento
uint8_t direction = 0; // 0=stop, 1=front, 2=back //si va para adelante o para atras
uint8_t speed = 0;   // 0=stop, 1=slow, 2=fast ,3=fasta&furious
uint8_t turn = 0;    // 0=forward, 1=left, 2=right

void setup(void)
{

  Serial.begin(BAUDRATE);
  wifi.restart();
  delay(3000);
  wifi.Configuration();
  pinMode(HC_SR04BACK_OUT, OUTPUT);
  pinMode(HC_SR04BACK_OUT, INPUT);
  sensor_distance.FrontSetup(HC_SR04FRONT_OUT, HC_SR04FRONT_IN);
  sensor_distance.RightSetup(HC_SR04RIGHT_OUT, HC_SR04RIGHT_IN);
  sensor_distance.LeftSetup(HC_SR04LEFT_OUT, HC_SR04LEFT_IN);
  sensor_distance.BackSetup(HC_SR04BACK_OUT, HC_SR04BACK_IN);

  Serial.print("\n");
  Serial.print(wifi.getLocalIP());
  Serial.print("\n");
}

void loop(void)
{

  // MEDICION DE DISTANCIA Y FRENO DE EMEGENCIA
  /*Si es tiempo de medir la distancia*/
  if (frecuency ==  F_Sensed) {
    Serial.print("Check de distancia\n");
    distance_front = sensor_distance.FrontMeasurement();
    Serial.print("front:");
    Serial.print(distance_front);
    Serial.print("\r\n");
    distance_right = sensor_distance.RightMeasurement();
    Serial.print("right:");
    Serial.print(distance_right);
    Serial.print("\r\n");
    distance_left = sensor_distance.LeftMeasurement();
    Serial.print("left:");
    Serial.print(distance_left);
    Serial.print("\r\n");
    distance_back = sensor_distance.BackMeasurement();
    Serial.print("back:");
    Serial.print(distance_back);
    Serial.print("\r\n");

    /*Si voy para adelante y hay poca distancia*/
    if (direction == 1) { //REVISAR SI NO MOLESTA PARA DOBLAR
      if (distance_front <= CRITIC_DISTANCE) {
        control.parar();
        direction = 0;
        speed = 0;
        Serial.print("STOP FRONT\r\n");
      }
      if  (distance_right <= CRITIC_DISTANCE) {
        control.parar();
        direction = 0;
        speed = 0;
        Serial.print("STOP RIGHT\r\n");
      }
      if (distance_left <= CRITIC_DISTANCE) {
        control.parar();
        direction = 0;
        speed = 0;
        Serial.print("STOP LEFT\r\n");
      }
    } else {
      /*Si voy para atrás y queda poca distancia*/
      if (distance_back <= CRITIC_DISTANCE && direction == 2) {
        control.parar();
        direction = 0;
        speed = 0;
        Serial.print("STOP BACK\r\n");
      }

    }
  }

  // FIN MEDICION DE DISTANCIA Y FRENO DE EMEGENCIA


  // DEZPLAZAMIENTO
  if (frecuency == 2) {
    Serial.print("Check de desplazamiento\n\n");

    /*Si debo avanzar*/
    if ( direction == 1 && speed != 0  ) {
      /*Si hay que simplemente ir derecho*/
      if ( turn == 0) {
        control.avanzar(speed);
      }
      /*Si hay que girar a la izquierda*/
      if ( turn == 1) {
        control.girarIzquierda();
      }
      /*Si hay que girar a la derecha*/
      if ( turn == 2) {
        control.girarDerecha();
      }
    }
    /*Si debo ir para atrás*/
    if ( direction == 2 && speed != 0  ) {
      control.retroceder();
    }

    /*Si debo frenar*/
    if (direction == 0 || speed == 0) {
      control.parar();
    }
  }
  // FIN DEZPLAZAMIENTO


  // COMUNICACION
  if (frecuency == 3) { //SI es momento de procesar la comunicacion
    Serial.print("Check de comunicacion\n\n");
    len = wifi.Receive_data(buffer);
    if (len == 3) { //SI SE RECIBE ORDEN
      Serial.print("Se recibe una orden");
      Serial.print("\n");
      //REcibí un mensaje de desplazamiento, almacenamiento de los movimientos que decidió el usuario
      direction = buffer[0] - 48; //se resta 48, pues el valro que se recibe está en ascci
      speed = buffer[1] - 48;
      turn = buffer[2] - 48;
      Serial.print("Direction recibida: ");
      Serial.print(direction);
      Serial.print("\n");
      Serial.print("Speed recibida: ");
      Serial.print(speed);
      Serial.print("\n");
      Serial.print("Turn recibido: ");
      Serial.print(turn);
      Serial.print("\n");

    }
    if (len == 1) { //SI PIDE INFORMACION
      Serial.print("Servidor pide datos");
      Serial.print("\n");
      //data_send=(String(distance_front, 0)+'/'+String(distance_back, 0)+'/'+String(direction,DEC)+'/'+String(speed,DEC)+'/'+String(turn,DEC)+'/');//preparación de la informacio na enviar
      data_send = 'a';
      for (len = 0; len < data_send.length(); len++) { // Se coloca el string en el buffer para su proximo envio
        if (data_send[len] != 32) { //en el caso de que la conversion a string me de un caracater espacio, lo reemplazo por '0'(En la práctica sucede)
          buffer[len] = data_send[len];
        } else {
          buffer[len] = '0';
        }
        //Serial.print((char)buffer[len]);
      }
      //Serial.print("\n");
      //envio del estado
      Serial.print(data_send.length());
      Serial.print("\n");

      wifi.Send_data(buffer, data_send.length());
      // Serial.print("Envio de datos");
      //Serial.print("\r\n");
      for (len = 0; len < data_send.length(); len++) {
        Serial.print((char)buffer[len]);
      }
      Serial.print("\n");
    }
  }
  // FIN COMUNICACION

  // CONFIGURACION DE TIEMPOS

  delay(200); //espera entre iteraciones

  frecuency++;//Aumento de la iteracion realizada
  if (frecuency == 4) {
    frecuency = 1;
  }
  // FIN CONFIGURACION DE TIEMPOS
}
