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
#define F_Sensed              53 //revisar valores
#define F_Communication       50
#define F_Movement            97
#define F_Testing             2000

/*DECLARACION DE VARIABLES GLOBALES*/
ControlMotor control(2, 3, 7, 4, 6, 5); // MotorDer1,MotorDer2,MotorIzq1,MotorIzq2,PWM_Derecho,PWM_Izquierdo
HC_SR04X2 sensor_distance;
ESP8266 wifi(Serial);

uint8_t buffer[TAM_BUFFER] = {0}; //buffer para enviar y recibir informacion
uint32_t len = 0; //Tamaño del mensaje recibido o el mensaje a enviar
int frecuency = 1; //variables que nos permite implementar un porgrmacion de tareas
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

bool order = false;
bool needsLessPower = false;
int movimiento = 0;
unsigned long timerTotal;
unsigned long timerTotalAux;
unsigned long timerMedicion;
unsigned long timerMedicionAux;

void setup(void)
{
  timerTotal = millis();
  timerMedicion = millis();
  Serial.begin(BAUDRATE);
  Serial.print("Iniciando...\r\n");
  wifi.restart();
  wifi.Configuration();
  pinMode(HC_SR04BACK_OUT, OUTPUT);
  pinMode(HC_SR04BACK_IN, INPUT);
  sensor_distance.FrontSetup(HC_SR04FRONT_OUT, HC_SR04FRONT_IN);
  sensor_distance.RightSetup(HC_SR04RIGHT_OUT, HC_SR04RIGHT_IN);
  sensor_distance.LeftSetup(HC_SR04LEFT_OUT, HC_SR04LEFT_IN);
  sensor_distance.BackSetup(HC_SR04BACK_OUT, HC_SR04BACK_IN);
  Serial.print("\n");
  Serial.print(wifi.getLocalIP());
  Serial.print("\n");
  Serial.print("Setup finalizado. Espere 5 segundos para empezar...\r\n");
  delay(2);
}

void loop(void)
{
  // MEDICION DE DISTANCIA Y FRENO DE EMEGENCIA
  if (frecuency % F_Sensed == 0) { //SI es momento de sensar la distancia PROBAMOS CON || (direction ==1)) && (frecuency % F_Movement != 0)

    timerMedicionAux = timerMedicion;
    timerMedicion = millis();
    //      Serial.print("\n\nUna medicion cada ");
    //      Serial.print(timerMedicion - timerMedicionAux);
    //      Serial.print(" milisegundos\n\n");

    distance_front = sensor_distance.FrontMeasurement();
    //      Serial.print("\n");
    //      Serial.print("\n****** Front:");
    //      Serial.print(distance_front);
    distance_right = sensor_distance.RightMeasurement();
    //       Serial.print("\n****** Right:");
    //       Serial.print(distance_right);
    distance_left = sensor_distance.LeftMeasurement();
    //       Serial.print("\n****** Left:");
    //       Serial.print(distance_left);
    distance_back = sensor_distance.BackMeasurement();
    //       Serial.print("\n****** Back:");
    //       Serial.print(distance_back);
    //       Serial.print("\n\n");

    if (direction == 1) { //REVISAR SI NO MOLESTA PARA DOBLAR
      if (distance_front <= CRITIC_DISTANCE) {
        control.parar();
        order = false;
        needsLessPower = false;
        movimiento = 0;
        direction = 0;
        speed = 0;
        Serial.print("STOP FRONT\r\n");

      }
      if  (distance_right <= CRITIC_DISTANCE) {
        control.parar();
        order = false;
        needsLessPower = false;
        movimiento = 0;
        direction = 0;
        speed = 0;
        Serial.print("STOP RIGHT\r\n");
      }
      if (distance_left <= CRITIC_DISTANCE) {
        control.parar();
        order = false;
        needsLessPower = false;
        movimiento = 0;
        direction = 0;
        speed = 0;
        Serial.print("STOP LEFT\r\n");
      }
    } else {
      if (distance_back <= CRITIC_DISTANCE && direction == 2) { //si hay poca distancia hacia atras y voy hacia atras
        control.parar();// no se que es ese 10
        order = false;
        needsLessPower = false;
        movimiento = 0;
        direction = 0;
        speed = 0;
        Serial.print("STOP BACK\r\n");
      }

    }
  }
  // FIN MEDICION DE DISTANCIA Y FRENO DE EMEGENCIA


  // DEZPLAZAMIENTO
  if (frecuency % F_Movement == 0) { //Si es momento de realizar un movimiento
    if (needsLessPower) {
      switch (movimiento) {
        case 1:
          control.avanzar(75, 110);
          movimiento = 0;
          break;
        case 2:
          control.girarIzquierda(90, 90);
          movimiento = 0;
          break;
        case 3:
          control.girarDerecha(75, 75);
          movimiento = 0;
          break;
        case 4:
          movimiento = 0;
          break;
        default:
          break;
      }
      needsLessPower = false;
    }
    if (order) {
      if ( direction == 1 && speed != 0) { // Si voy hacia adelante
        Serial.print("ADELANTE\r\n");
        if ( turn == 0) { // si voy hacia adelante en linea recta
          control.avanzar(85, 125);
          movimiento = 1;
        }
        if ( turn == 1) { // si voy hacia adelante en hacia la izqueirda
          control.girarIzquierda(110, 110);
          movimiento = 2;
        }
        if ( turn == 2) { // si voy hacia adelante en hacia la izqueirda
          control.girarDerecha(90, 90);
          movimiento = 3;
        }
        if (!needsLessPower)
          needsLessPower = true;
      }
      if ( direction == 2 && speed != 0  ) { // Si voy hacia atrás
        Serial.print("ATRAS\r\n");
        control.retroceder();
        movimiento = 4;
      }
      if (direction == 0 || speed == 0) {
        control.parar();
        needsLessPower = false;
        movimiento = 0;
      }
      order = false;
    }
    Serial.print(direction);
    Serial.print(speed);
    Serial.print(turn);
    Serial.print("\r\n");
  }
  // FIN DEZPLAZAMIENTO


  // COMUNICACION
  if (frecuency % F_Communication == 0 ) { //SI es momento de procesar la comunicacion
    len = wifi.Receive_data(buffer);
    if (len == 3) { //SI SE RECIBE ORDEN
      order = true;
      Serial.print("ORDEN");
      Serial.print("\r\n");
      //REcibí un mensaje de desplazamiento, almacenamiento de los movimientos que decidió el usuario
      direction = buffer[0] - 48; //se resta 48, pues el valro que se recibe está en ascci
      speed = buffer[1] - 48;
      turn = buffer[2] - 48;
      Serial.print("\r\n");
      Serial.print(direction);
      Serial.print(speed);
      Serial.print(turn);
      Serial.print("\r\n");
    }
    if (len == 1) { //SI PIDE INFORMACION
      //               Serial.print("Estadisticas");
      //               Serial.print("\r\n");
        data_send=(String(distance_front, 0)+'/'+String(distance_back, 0)+'/'+String(direction,DEC)+'/'+String(speed,DEC)+'/'+String(turn,DEC)+'/'+String(distance_right, 0)+'/'+String(distance_left, 0)+'/');//preparación de la informacio na enviar      for (len = 0; len < data_send.length(); len++) { // Se coloca el string en el buffer para su proximo envio
        for (len = 0; len < data_send.length(); len++) { // Se coloca el string en el buffer para su proximo envio
          if (data_send[len] != 32) { //en el caso de que la conversion a string me de un caracater espacio, lo reemplazo por '0'(En la práctica sucede)
            buffer[len] = data_send[len];
          } else {
            buffer[len] = '0';
          }
      }
      boolean result = wifi.Send_data(buffer, data_send.length());
      if (result) {
        Serial.print("Result = true");
        Serial.print("\r\n");
      } else {
        Serial.print("Result = false");
        Serial.print("\r\n");
      }
      Serial.print("Envio de datos");
      Serial.print("\r\n");
      for (len = 0; len < data_send.length(); len++) {
        Serial.print((char)buffer[len]);
      }
      Serial.print("\r\n");


    }
  }
  // FIN COMUNICACION


  // ESCANEO DEL ESTADO DE LA COMUNICACION DEL VEHICULO
  /*
    if (frecuency % F_Testing == 0 ){
        wifi.Comprobation();
    }
  */
  // FIN ESCANEO DEL ESTADO DE LA COMUNICACION DEL VEHICULO

  // CONFIGURACION DE TIEMPOS
  //delayMicroseconds(1000);
  //delay(28); //espera entre iteraciones
frecuency++;//Aumento de la iteracion realizada
  if (frecuency == 6300) {
    timerTotalAux = timerTotal;
    timerTotal = millis();
    Serial.print("\n\nReset de la frecuencia cada ");
    Serial.print(timerTotal - timerTotalAux);
    Serial.print(" segundos\n\n");
    frecuency = 0;
  }
  // FIN CONFIGURACION DE TIEMPOS
}