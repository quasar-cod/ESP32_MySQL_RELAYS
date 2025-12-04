//valori per scheda 220v due relè
#define LED_ROSSO 23
#define RELE1 16
#define RELE2 17

#include <Arduino.h>
void setup() {
  Serial.begin(115200); //--> Initialize serial communications with the PC.
  Serial.println("-------------");
  Serial.println("Initialized serial communications with the PC.");
 const int myArray []= {13,14,16,17,18,19,21,22,23,25,26,27,32,33};
 for (int i : myArray) {
    Serial.println(i);
    pinMode(i,OUTPUT);
    digitalWrite(i, HIGH);
    delay(5000);    
    digitalWrite(i, LOW);
    delay(5000);
  }
}
void loop() {

}