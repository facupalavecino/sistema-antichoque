/*Librería menejo de sensor de distancia HC_SRO4*/

#ifndef HC_SR04X2_H
	#define HC_SR04X2_H
#include "Arduino.h"


class HC_SR04X2 //Definicion de la clase
{
 	public:
 	//Constructor de la clase
    HC_SR04X2();
 
    //Funcion setup: Realiza la configuracion de los pines de Trigger y Echo del sensor
    void FrontSetup(int FrontPin_start, int FrontPin_data);
    void BackSetup(int BackPin_start, int Backin_data);
    void HC_SR04X2::RightSetup(int RightPin_start, int RightPin_data);
	void HC_SR04X2::LeftSetup(int LeftPin_start, int LeftPin_data);
    //Funcion measurement: Realiza la medicion y devuelva la distancia
    float FrontMeasurement();
 	float FrontMeasurement_for(int cant_measurement);
 	float FrontMeasurement_Mediana();
 	float RightMeasurement();
 	float LeftMeasurement();
 	float BackMeasurement();
 	float BackMeasurement_for(int cant_measurement);
 	float BackMeasurement_Mediana();

 
    private:
 
    //Nada que declarar
};
#endif