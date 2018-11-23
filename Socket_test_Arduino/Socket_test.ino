#include "ESP8266.h"

ESP8266 esp01(Serial);

uint32_t data_length = 0;
uint8_t buffer[128] = {0};

void setup() {
  Serial.begin(38400);
  esp01.restart();
  esp01.Configuration();
  Serial.print("El buffer arranca con: ");
  Serial.print(buffer[0]);
  Serial.print("\n\n");
}

void loop() {
  data_length = esp01.Receive_data(buffer);
  if (data_length > 0){
    Serial.print("Se recibió: ");
    Serial.print(buffer[0]);
    Serial.print("\n");
    Serial.print("Data length: ");
    Serial.print(data_length);
    Serial.print("\n");
  }

  delay(1000);

}
