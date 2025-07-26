#include <SPI.h>
#include <RF24.h>


RF24 radio(7, 10); // CE, CSN
const uint64_t address = 0xE8E8F0F0E1LL;

int valueToSend = 0;
int direction = 1;  // 1 = up, -1 = down

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.setChannel(115); 
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(address);
  radio.stopListening();
}

void loop() {
  Serial.print("Sending: ");
  Serial.println(valueToSend);

  radio.write(&valueToSend, sizeof(valueToSend));

  valueToSend += direction;

  if (valueToSend >= 100 || valueToSend <= 0) {
    direction *= -1;  // Reverse direction
  }

  delay(200); // Faster bounce for testing
}