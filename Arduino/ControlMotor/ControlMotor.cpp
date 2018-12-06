// Incluimos las librerías necesarias según el arduino que estemos utilizando
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

// Incluimos la librería cabecera
#include "ControlMotor.h"

// Con esta función configuramos los pines seleccionamos para que funcione el driver motor.
//ControlMotor control(2,3,7,4,5,6); // 
ControlMotor::ControlMotor(int md1, int md2, int mi1, int mi2, int pwmD, int pwmI){
   pinMode(md1,OUTPUT);  // Configuramos como salida el Enable 1 del Derecho
   pinMode(md2,OUTPUT);  // Configuramos como salida el Enable 2 del Derecho
   pinMode(mi1,OUTPUT);  // Configuramos como salida el Enable 1 del Izquierdo
   pinMode(mi2,OUTPUT);  // Configuramos como salida el Enable 2 del Izquierdo
   pinMode(pwmD,OUTPUT); // PWM usado para el motor Derecho
   pinMode(pwmI,OUTPUT); // PWM usado para el motor Izquierdo

   motorDerechoA=md1;
   // Almacenamos el pin seleccionado para el motor Derecho Enable 1 en la variable 
   motorDerechoB=md2;  
   // Almacenamos el pin seleccionado para el motor Derecho Enable 2 en la variable
   motorIzquierdoA=mi1; 
   // Almacenamos el pin seleccionado para el motor Izquierdo Enable 1 en la variable 
   motorIzquierdoB=mi2; 
   // Almacenamos el pin seleccionado para el motor Izquierdo Enable 2 en la variable
   pwmDerecho=pwmD;
   // Almacenamos el pin seleccionado para el PWM Derecho en la variable correspondiente 
   pwmIzquierdo=pwmI;   
    // Almacenamos el pin seleccionado para el PWM Izquierdo en la variable
}

/*Para las siguientes funciones es necesario saber que para indicar el sentido de giro de las ruedas se requieren dos variables por motor, específicamente la variable A y B de cada motor:
Si queremos que se gire hacia el frente, la variable del motor B se coloca en  Alto y la variable A del motor se coloca en Bajo.
Si queremos que se gire hacia atrás, la variable del motor B se coloca en  bajo  y la variable A del motor se coloca en Alto.
La velocidad de giro de cada rueda depende del pwm con el cual se alimente al motor.
*/

void ControlMotor::retroceder(){ 
  //120 derecha, 85 izquierda va bien
  Serial.print("\n\nDENTRO DEL CONTROL.RETROCEDER\n\n");
  digitalWrite(motorDerechoA,HIGH); 
  digitalWrite(motorDerechoB,LOW);
  digitalWrite(motorIzquierdoA,HIGH);
  digitalWrite(motorIzquierdoB,LOW);
  analogWrite(pwmDerecho,120);
  analogWrite(pwmIzquierdo,85);

}
void ControlMotor::avanzar(int v_left, int v_right){ 
  digitalWrite(motorDerechoA,LOW);
  digitalWrite(motorDerechoB,HIGH);
  digitalWrite(motorIzquierdoA,LOW);
  digitalWrite(motorIzquierdoB,HIGH);
  analogWrite(pwmDerecho, v_right);
  analogWrite(pwmIzquierdo, v_left);

}
void ControlMotor::girarIzquierda(int v_left, int v_right){
  //90 es una buena velocidad 
  digitalWrite(motorDerechoA,LOW);
  digitalWrite(motorDerechoB,LOW);
  digitalWrite(motorIzquierdoA,LOW);
  digitalWrite(motorIzquierdoB,HIGH);
  analogWrite(pwmDerecho,v_right);
  analogWrite(pwmIzquierdo,v_left);

}
void ControlMotor::girarDerecha(int v_left, int v_right){ 
  //75 esta bien para andar
  digitalWrite(motorDerechoA,LOW);
  digitalWrite(motorDerechoB,HIGH);
  digitalWrite(motorIzquierdoA,LOW);
  digitalWrite(motorIzquierdoB,LOW);
  analogWrite(pwmDerecho,v_right); 
  analogWrite(pwmIzquierdo,v_left);

}
void ControlMotor::parar(){ 
  digitalWrite(motorDerechoA,LOW);
  digitalWrite(motorDerechoB,LOW);
  digitalWrite(motorIzquierdoA,LOW);
  digitalWrite(motorIzquierdoB,LOW);
  analogWrite(pwmDerecho, 250);
  analogWrite(pwmIzquierdo, 250);

}

