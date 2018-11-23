/*************************************************************************************************************************************************/
/*
/* Esta librería controla la velocidad y la dirección de los motores del robot.
/* La forma de usarla es de la siguiente manera.
/*
/* En esta primera parte agregamos la librería y configuramos los pines.
/*------------------------------------------------------------------------------------------------------
/* #include <ControlMotor.h>
/* ControlMotor control(2,3,7,4,5,6);   // MotorDer1,MotorDer2,MotorIzq1,MotorIzq2,PWM_Derecho,PWM_Izquierdo
/*------------------------------------------------------------------------------------------------------
/*
/* - MotorDer1: Enable 1 del motor Derecho
/* - MotorDer2: Enable 2 del motor Derecho
/* - MotorIzq1: Enable 1 del motor Izquierdo
/* - MotorIzq2: Enable 2 del motor Izquierdo
/* - PWM_Derecho: PWM del motor derecho (Debe ser un pin que admita PWM de arduinov
/* - PWM_Izquierdo: PWM del motor Izquierdo (Debe ser un pin que admita PWM de arduino
/*
/*-------------------------------------------------------------------------------------------------------
/* 
/* En esta segunda parte, controlamos el motor. La primera varaible es la velocidad que va de 0 a 255 y la segunda el % de giro que va de 0 a 100.
/* Si la velocidad es positiva el robot avanza, si la velocidad es negativa el robot retrocede.
/* Si el giro es positivo, el robot va recto (0%) o a la derecha (>0%), si es negativo el robot va recto (0%) o a la izquierda (<0%),
// Si el giro es 100, el robot en vez de avanzar girando, gira sobre su propio eje (Una rueda avanza y otra retrocede.
/*-------------------------------------------------------------------------------------------------------
/*
/* control.Motor(255, 0);   // Velocidad 255 (Máxima y positiva por lo que avanza) y 0% de giro, por lo que va recto.
/* control.Motor(220, -20); // Velocidad 220 (Positiva por lo que avanza) y giro -20% por lo que va girando levemente a la izquierda.
/* control.Motor(130, 100); // Velocidad 130 (Positivo por lo que avanza) y giro 100, el robot gira sobre su eje.
/* control.Motor(-255, 0);  // Velocidad 255 (Máxima y negativa por lo que retrocede) y 0% de giro, por lo que va recto.
/*
/*************************************************************************************************************************************************/

#ifndef ControlMotor_h
#define ControlMotor_h

// Incluimos las librerías necesarias según el arduino que estemos utilizando
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

class ControlMotor{
   private:
    int motorDerechoA;    // Esta variable la usa internamente la librería para el enable A motor derecho
    int motorDerechoB;    // Esta variable la usa internamente la librería para el enable B motor derecho
    int motorIzquierdoA;    // Esta variable la usa internamente la librería para el enable A motor izquierdo
    int motorIzquierdoB;    // Esta variable la usa internamente la librería para el enable B motor izquierdo
    int pwmDerecho;       // Esta variable la usa internamente la librería para el PWM del motor Derecho
    int pwmIzquierdo;       // Esta variable la usa internamente la librería para el PWM del motor Izquierdo
  public:
    ControlMotor(int md1, int md2, int mi1, int mi2, int pwmD, int pwmI);  // Asignamos los pines
    void girarDerecha();
    void girarIzquierda();
    void avanzar(int velocidad);
    void retroceder();
    void parar();
 
};

#endif
