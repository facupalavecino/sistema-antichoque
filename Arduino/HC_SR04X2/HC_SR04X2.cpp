#include "HC_SR04X2.h"
 
HC_SR04X2::HC_SR04X2(){} //Constructor: no tiene que hacer nada en especial
int FrontStart_pin;
int FrontData_pin;
int RightStart_pin;
int RightData_pin;
int LeftStart_pin;
int LeftData_pin;
int BackStart_pin; 
int BackData_pin;  

void HC_SR04X2::FrontSetup(int FrontPin_start, int FrontPin_data)
{
    //Recibe el numero de pin por el cual se envirá el pulso de start al sensor
    //Recibe el pin por el cual se obtendrá la informacion que envia el sensor
    //Establecer pin como salida
    pinMode(FrontPin_start, OUTPUT);//Se establece como salida el pin TRIG
    FrontStart_pin=FrontPin_start;//Se almacena en variable interna el valor del pin
  	pinMode(FrontPin_data, INPUT);//Se establece el pin como entrada el ping de ECHO
  	FrontData_pin=FrontPin_data;//Se almacena en variable interna el valor del pin
}

void HC_SR04X2::RightSetup(int RightPin_start, int RightPin_data)
{
    //Recibe el numero de pin por el cual se envirá el pulso de start al sensor
    //Recibe el pin por el cual se obtendrá la informacion que envia el sensor
    //Establecer pin como salida
    pinMode(RightPin_start, OUTPUT);//Se establece como salida el pin TRIG
    RightStart_pin=RightPin_start;//Se almacena en variable interna el valor del pin
    pinMode(RightPin_data, INPUT);//Se establece el pin como entrada el ping de ECHO
    RightData_pin=RightPin_data;//Se almacena en variable interna el valor del pin
}

void HC_SR04X2::LeftSetup(int LeftPin_start, int LeftPin_data)
{
    //Recibe el numero de pin por el cual se envirá el pulso de start al sensor
    //Recibe el pin por el cual se obtendrá la informacion que envia el sensor
    //Establecer pin como salida
    pinMode(LeftPin_start, OUTPUT);//Se establece como salida el pin TRIG
    LeftStart_pin=LeftPin_start;//Se almacena en variable interna el valor del pin
    pinMode(LeftPin_data, INPUT);//Se establece el pin como entrada el ping de ECHO
    LeftData_pin=LeftPin_data;//Se almacena en variable interna el valor del pin
}



void HC_SR04X2::BackSetup(int BackPin_start, int BackPin_data)
{
    //Recibe el numero de pin por el cual se envirá el pulso de start al sensor
    //Recibe el pin por el cual se obtendrá la informacion que envia el sensor
    pinMode(BackPin_start, OUTPUT);//Se establece como salida el pin TRIG
    BackStart_pin=BackPin_start;//Se almacena en variable interna el valor del pin
    pinMode(BackPin_data, INPUT);//Se establece el pin como entrada el ping de ECHO
    BackData_pin=BackPin_data;//Se almacena en variable interna el valor del pin
}


float HC_SR04X2::FrontMeasurement()
{
    long time;//vaiable necesaria para almacenar el tiempo
    float distance;//vaiable necesaria para almacenar la distancia
    digitalWrite(FrontStart_pin,HIGH);//Inicializacion del pulso de start
    delayMicroseconds(10);
    digitalWrite(FrontStart_pin, LOW);//Finalizacion del pulso de start
    // medición el pulso de respuesta
  	time = (pulseIn(FrontData_pin, HIGH)/2);
    // dividido por 2 porque es el tiempo que el sonido tarda en ir y en volver
  	// ahora calcularemos la distancia en cm
  	// sabiendo que el espacio es igual a la velocidad por el tiempo,
  	//  que la velocidad del sonido es de 343m/s y que el tiempo se encuentra en
  	// tenemos en millonesimas de segundo
  	distance = float(time * 0.0343);
  	return (distance); //Se retonra le valor de la medición
}

float HC_SR04X2::RightMeasurement()
{
    long time;//vaiable necesaria para almacenar el tiempo
    float distance;//vaiable necesaria para almacenar la distancia
    digitalWrite(RightStart_pin,HIGH);//Inicializacion del pulso de start
    delayMicroseconds(10);
    digitalWrite(RightStart_pin, LOW);//Finalizacion del pulso de start
    // medición el pulso de respuesta
    time = (pulseIn(RightData_pin, HIGH)/2);
    // dividido por 2 porque es el tiempo que el sonido tarda en ir y en volver
    // ahora calcularemos la distancia en cm
    // sabiendo que el espacio es igual a la velocidad por el tiempo,
    //  que la velocidad del sonido es de 343m/s y que el tiempo se encuentra en
    // tenemos en millonesimas de segundo
    distance = float(time * 0.0343);
    return (distance); //Se retonra le valor de la medición
}

float HC_SR04X2::LeftMeasurement()
{
    long time;//vaiable necesaria para almacenar el tiempo
    float distance;//vaiable necesaria para almacenar la distancia
    digitalWrite(LeftStart_pin,HIGH);//Inicializacion del pulso de start
    delayMicroseconds(10);
    digitalWrite(LeftStart_pin, LOW);//Finalizacion del pulso de start
    // medición el pulso de respuesta
    time = (pulseIn(LeftData_pin, HIGH)/2);
    // dividido por 2 porque es el tiempo que el sonido tarda en ir y en volver
    // ahora calcularemos la distancia en cm
    // sabiendo que el espacio es igual a la velocidad por el tiempo,
    // que la velocidad del sonido es de 343m/s y que el tiempo se encuentra en
    // tenemos en millonesimas de segundo
    distance = float(time * 0.0343);
    return (distance); //Se retonra le valor de la medición
}

float HC_SR04X2::BackMeasurement()
{
    long time;
    float distance;
    //Realiza la medición
  //Encenderlo y apagarlo
    digitalWrite(BackStart_pin,HIGH);
    delayMicroseconds(10);
    digitalWrite(BackStart_pin, LOW);
     // medimos el pulso de respuesta
    time = (pulseIn(BackData_pin, HIGH)/2); // dividido por 2 por que es el tiempo que el sonido tarda en ir y en volver
    // ahora calcularemos la distancia en cm
    // sabiendo que el espacio es igual a la velocidad por el tiempo
    // y que la velocidad del sonido es de 343m/s y que el tiempo lo 
    // tenemos en millonesimas de segundo
    distance = float(time * 0.0343);
    return (distance);
}



float HC_SR04X2::FrontMeasurement_for( int cant_measurement)
{
    int i;
    long time;//vaiable necesaria para almacenar el tiempo
    float distance;//vaiable necesaria para almacenar la distancia    
  for (i = 0; i < cant_measurement; i++){
    digitalWrite(FrontStart_pin,HIGH);//Inicializacion del pulso de start
    delayMicroseconds(10);
    digitalWrite(FrontStart_pin, LOW);//Finalización del pulso de start
     // medimos el pulso de respuesta
    time = (pulseIn(FrontData_pin, HIGH)/2); 
   // dividido por 2 porque es el tiempo que el sonido tarda en ir y en volver
    // ahora calcularemos la distancia en cm
    // sabiendo que el espacio es igual a la velocidad por el tiempo,
    //  que la velocidad del sonido es de 343m/s y que el tiempo se encuentra en
    // tenemos en millonesimas de segundo
    distance = distance + float(time * 0.0343);
    delayMicroseconds(60);//espero 60 ms entre mediciones
  }
   return (distance/cant_measurement);//Retorno el Promedio de las mediciones
}

float HC_SR04X2::BackMeasurement_for( int cant_measurement)
{
    int i;
    long time;
    float distance;
    //Realiza la medición
    //Encenderlo y apagarlo
    
  for (i = 0; i < cant_measurement; i++){
    digitalWrite(BackStart_pin,HIGH);
    delayMicroseconds(10);
    digitalWrite(BackStart_pin, LOW);
     // medimos el pulso de respuesta
    time = (pulseIn(BackData_pin, HIGH)/2); // dividido por 2 por que es el tiempo que el sonido tarda en ir y en volver
    // ahora calcularemos la distancia en cm
    // sabiendo que el espacio es igual a la velocidad por el tiempo
    // y que la velocidad del sonido es de 343m/s y que el tiempo lo 
    // tenemos en millonesimas de segundo
    distance = distance + float(time * 0.0343);
    delayMicroseconds(60);//espero 60 ms entre mediciones
  }
   
    return (distance/cant_measurement);
}

float HC_SR04X2::FrontMeasurement_Mediana(){
    float array[3]={500.0,500.0,500.0};
    float distance=0;
    int i,aux=0;
    long time;
    for (i = 0; i < 3; i++){ //realiza una mediana de 3 valores
        digitalWrite(FrontStart_pin,HIGH);//Inicializacion del pulso de start
        delayMicroseconds(10);
        digitalWrite(FrontStart_pin, LOW);//Finalización del pulso de start
        // medimos el pulso de respuesta
        time = (pulseIn(FrontData_pin, HIGH)/2); 
        //dividido por 2 porque es el tiempo que el sonido tarda en ir y en volver
        // ahora calcularemos la distancia en cm
        // sabiendo que el espacio es igual a la velocidad por el tiempo,
        //  que la velocidad del sonido es de 343m/s y que el tiempo se encuentra en
        // tenemos en millonesimas de segundo
        distance = float(time * 0.0343);
        if(distance < array[0]){ //si la distancia es la mas reducida obtenida
          array[2]=array[1]; // Se realiza el corrimiiento de la posicion 1 a la 2
          array[1]=array[0]; // Se realiza el corrimiiento de la posicion 0 a la 1
          array[0]=distance;}// Se alamcena la nueva medicion
        else{ 
          if(distance < array[1]){ //si la distancia es la segunda mas reducida
            array[2]=array[1];// Se realiza el corrimiiento de la posicion 1 a la 2
            array[1]=distance;}// Se alamcena la nueva medicion
          else{ //si la distancia es la segunda tercera mas reducida
            array[2]=distance;// Se alamcena la nueva medicion
          }  
        }
        delayMicroseconds(60);//espero 60 ms entre mediciones
      }
    return array[1]; //obtengo la mediana
  
}

float HC_SR04X2::BackMeasurement_Mediana(){
    float array[3]={500.0,500.0,500.0}; //se rellena con un valor extremadamente alto
    float distance=0;
    int i,aux=0;
    long time;
    for (i = 0; i < 3; i++){ //realiza una mediana de 3 valores
        digitalWrite(FrontStart_pin,HIGH);//Inicializacion del pulso de start
        delayMicroseconds(10);
        digitalWrite(FrontStart_pin, LOW);//Finalización del pulso de start
        // medimos el pulso de respuesta
        time = (pulseIn(FrontData_pin, HIGH)/2); 
        //dividido por 2 porque es el tiempo que el sonido tarda en ir y en volver
        // ahora calcularemos la distancia en cm
        // sabiendo que el espacio es igual a la velocidad por el tiempo,
        //  que la velocidad del sonido es de 343m/s y que el tiempo se encuentra en
        // tenemos en millonesimas de segundo
        distance = float(time * 0.0343);
        if(distance < array[0]){ //si la distancia es la mas reducida obtenida
          array[2]=array[1]; // Se realiza el corrimiiento de la posicion 1 a la 2
          array[1]=array[0]; // Se realiza el corrimiiento de la posicion 0 a la 1
          array[0]=distance;}// Se almacena la nueva medicion
        else{ 
          if(distance < array[1]){ //si la distancia es la segunda mas reducida
            array[2]=array[1];// Se realiza el corrimiiento de la posicion 1 a la 2
            array[1]=distance;
          }// Se alamcena la nueva medicion
          else{ //si la distancia es la tercera mas reducida
            array[2]=distance;// Se almacena la nueva medicion
          }  
        }
        delayMicroseconds(60);//espero 60 ms entre mediciones
      }
    return array[1]; //obtengo la mediana  
}