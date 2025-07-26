#include <SPI.h>
#include <RF24.h>
RF24 radio(7, 10); // CE, CSN
//RF24 radio(9, 10);  // CE, CSN
const uint64_t address = 0xE8E8F0F0E1LL;  // Must match transmitter

void setup() {
  Serial.begin(9600);        
  radio.begin();
  radio.setChannel(115); 
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_HIGH);  // Or HIGH/MAX if needed
  radio.openReadingPipe(0, address);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    int received;
    radio.read(&received, sizeof(received));
    Serial.println(received);
  }
  // else {
  //   Serial.println("no Radio");
  //   delay(200);
  // }
}