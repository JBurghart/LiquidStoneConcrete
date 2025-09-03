#include <AltSoftSerial.h>
AltSoftSerial mySerial; // RX on 8, TX on 9 (Uno)

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
}
void loop() {
  while (mySerial.available()) Serial.write(mySerial.read());
}