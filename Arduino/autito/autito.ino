/*FUNCIONES ORIGINALES*/
#include <Arduino.h>

/*FUNCIONES EDITADAS O DESARROLLADAS*/

#include <HC_SR04X2.h>
#include <ControlMotor.h>
#include "ESP8266.h"

/*DEFINICIONES DE CONFIGURACION*/
#define BAUDRATE            115200
#define HC_SR04FRONT_OUT    8 // Numero de pin por el cual se envia el pulso de START
#define HC_SR04FRONT_IN     9 // Numero de pin por el cual se recibe informacion de la medioción
#define HC_SR04BACK_OUT     A0 // Numero de pin por el cual se envia el pulso de START
#define HC_SR04BACK_IN      A1 // Numero de pin por el cual se recibe informacion de la medioción
#define TAM_BUFFER          128  
#define CRITIC_DISTANCE     40

/*DEFINICIONES DE TIEMPO DE EJECUCION*/
#define F_Sensed              27 //revisar valores
#define F_Communication       50
#define F_Movement            97
#define F_Testing             2000  

/*DECLARACION DE VARIABLES GLOBALES*/
ControlMotor control(2,3,7,4,5,6); // MotorDer1,MotorDer2,MotorIzq1,MotorIzq2,PWM_Derecho,PWM_Izquierdo
HC_SR04X2 sensor_distance; 
ESP8266 wifi(Serial);

uint8_t buffer[TAM_BUFFER]={0};//buffer para enviar y recibir informacion
uint32_t len=0;//Tamaño del mensaje recibido o el mensaje a enviar
uint32_t frecuency=1; //variables que nos permite implementar un porgrmacion de tareas
String data_send;
//variables de almacenamiento de distancias
float distance_front=0;
float distance_back=0;
//variables de desplazamiento -> el vehiculo lee estas variables y decide su movimiento
uint8_t direction=0; // 0=stop, 1=front, 2=back //si va para adelante o para atras
uint8_t speed=0;     // 0=stop, 1=slow, 2=fast ,3=fasta&furious     
uint8_t turn=0;      // 0=forward, 1=left, 2=right

void setup(void)
{
  
    Serial.begin(BAUDRATE);
    Serial.print("setup begin\r\n");
    wifi.restart();
    wifi.Configuration();
    sensor_distance.BackSetup(HC_SR04BACK_OUT,HC_SR04BACK_IN);
    sensor_distance.FrontSetup(HC_SR04FRONT_OUT,HC_SR04FRONT_IN);
    Serial.print("\n");
    Serial.print(wifi.getLocalIP());
    Serial.print("\n");
    Serial.print("setup end\r\n");
}
 
void loop(void)
{
    
     
   // MEDICION DE DISTANCIA Y FRENO DE EMEGENCIA
   if (frecuency % F_Sensed == 0){ //SI es momento de sensar la distancia
      distance_front=sensor_distance.FrontMeasurement();
      Serial.print("front:");
      Serial.print(distance_front);
      Serial.print("\r\n");
      distance_back=sensor_distance.BackMeasurement();
      Serial.print("back:");
      Serial.print(distance_back);
      Serial.print("\r\n");
      
      if (distance_front <= CRITIC_DISTANCE && direction == 1){ //si hay poca distancia hacia adelante y voy hacia adelante//REVISAR SI NO MOLESTA PARA DOBLAR
          control.parar();
          direction=0;
          speed=0;
          Serial.print("STOP FRONT\r\n");
      }else{
          if (distance_back <= CRITIC_DISTANCE && direction == 2){ //si hay poca distancia hacia atras y voy hacia atras
              control.parar();// no se que es ese 10
              direction=0;
              speed=0;
              Serial.print("STOP BACKT\r\n");
           }
        
       }
   }
   // FIN MEDICION DE DISTANCIA Y FRENO DE EMEGENCIA


  // DEZPLAZAMIENTO
   if (frecuency % F_Movement == 0 ){ //Si es momento de realizar un movimiento
          if( direction == 1 && speed != 0  ) { // Si voy hacia adelante
              Serial.print("ADELANTE\r\n");
              if( turn==0){ // si voy hacia adelante en linea recta
                  control.avanzar(speed);
              }
              if( turn==1){ // si voy hacia adelante en hacia la izqueirda
                  control.girarIzquierda();
              }
              if( turn==2){ // si voy hacia adelante en hacia la izqueirda
                  control.girarDerecha();
              }          
            }
          if( direction == 2 && speed != 0  ) { // Si voy hacia atrás
              Serial.print("ATRAS\r\n");
              control.retroceder();
           }
          if (direction == 0 || speed == 0){
              control.parar();
           }
      Serial.print(direction);
      Serial.print(speed);
      Serial.print(turn);
      Serial.print("\r\n");
   
   }
    // FIN DEZPLAZAMIENTO


   // COMUNICACION
   if (frecuency % F_Communication == 0 ){ //SI es momento de procesar la comunicacion
          len=wifi.Receive_data(buffer);
          if (len == 3){ //SI SE RECIBE ORDEN
                     Serial.print("ORDEN");
                     Serial.print("\r\n");
               //REcibí un mensaje de desplazamiento, almacenamiento de los movimientos que decidió el usuario
                     direction=buffer[0]-48; //se resta 48, pues el valro que se recibe está en ascci
                     speed=buffer[1]-48;
                     turn=buffer[2]-48;
                     Serial.print(direction);
                     Serial.print(speed);
                     Serial.print(turn);
                     
                 
          }      
          if (len == 1){ //SI PIDE INFORMACION   
               Serial.print("Estadisticas");
               Serial.print("\r\n");
               data_send=(String(distance_front, 0)+'/'+String(distance_back, 0)+'/'+String(direction,DEC)+'/'+String(speed,DEC)+'/'+String(turn,DEC)+'/');//preparación de la informacio na enviar   
               for(len=0; len<data_send.length();len++){ // Se coloca el string en el buffer para su proximo envio
                     if(data_send[len]!= 32){ //en el caso de que la conversion a string me de un caracater espacio, lo reemplazo por '0'(En la práctica sucede)
                          buffer[len]=data_send[len];
                     }else{
                          buffer[len]='0'; 
                     }
                //Serial.print((char)buffer[len]);
                 }
               //Serial.print("\r\n");
               //envio del estado
               Serial.print(data_send.length());
                Serial.print("\r\n"); 
               
                wifi.Send_data(buffer,data_send.length());
                Serial.print("Envio de datos");
                Serial.print("\r\n");
                for (len=0;len<data_send.length();len++){
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
  // delay(1); //espera entre iteraciones

  frecuency++;//Aumento de la iteracion realizada
  if (frecuency == 6300){
      frecuency=0;
  }
// FIN CONFIGURACION DE TIEMPOS    
}
