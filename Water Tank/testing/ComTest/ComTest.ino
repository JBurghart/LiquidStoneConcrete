#include <Arduino.h>

int j = 0;
void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:

}

void loop() {
  for (int i = 0; i < 100; i++) {
    Serial.println(i); // percent of water in the tank
    delay(40); 
  }
  Serial.println("110");
  delay(4000);
  for (int i = 100; i >= 0; i--) {
    Serial.println(i); // percent of water in the tank
    delay(40); 
  }
  delay(40); 
  

}
