/*Librería menejo de sensor de distancia HC_SRO4*/

#ifndef HC_SR04X2_H
	#define HC_SR04X2_H
#include "Arduino.h"


class HC_SR04X2 //Definicion de la clase
{
 	public:
 	//Constructor de la clase
    HC_SR04X2();
 
    //Funcion setup:Realiza la configuracion de los pines de Trigger y Echo del sensor
    void FrontSetup(int FrontPin_start, int FrontPin_data);
    void BackSetup(int BackPin_start, int Backin_data);
    //Funcion setup:Realiza la medicion y devuelva la distancia
    float FrontMeasurement();
 	float FrontMeasurement_for(int cant_measurement);
 	float FrontMeasurement_Mediana();
 	float BackMeasurement();
 	float BackMeasurement_for(int cant_measurement);
 	float BackMeasurement_Mediana();

 
    private:
 
    //Nada que declarar
};
#endif