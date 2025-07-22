#include <SPI.h>
#include <RF24.h>

RF24 radio(9, 10);  // CE, CSN
const byte address[6] = "NODE1";

void setup() {
  Serial.begin(9600);        // USB debug
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.openReadingPipe(0, address);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    int received;
    radio.read(&received, sizeof(received));
    Serial.print("Received distance: ");
    Serial.print(received);
    Serial.println(" inches");
  }
}