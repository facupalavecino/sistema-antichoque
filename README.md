# Sistema Antichoque para un vehículo controlado remotamente mediante un Arduino

Desarrollo de un sistema que permite a un vehículo leer distancia de elementos adelante y atrás para evitar colisionar con ellos. 

El vehículo se compone de:
- Controlador: Arduino UNO
- Sensores de distancia: 4x Sensores de ultrasonido HC-SR04 (3 adelante y 1 atrás)
- Controlador L298n: para manejar 2 motores de corriente contínua
- 2 motores de corriente contínua (1 en cada rueda delantera)
- Módulo WiFi ESP8266 que permite comunicar al Arduino con un servidor remoto a través del cual el usuario puede controlar el vehículo.
