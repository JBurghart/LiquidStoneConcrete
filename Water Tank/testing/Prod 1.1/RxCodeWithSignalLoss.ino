#include <SPI.h>
#include <RF24.h>

RF24 radio(7, 10);  // CE, CSN
const uint64_t address = 0xE8E8F0F0E1LL;

const int receivedMax = 270;
const int TankHigh = 44;
const int TankLow = 262;

struct Payload {
  int distance;
  int signalStrength;
};

void setup() {
  Serial.begin(9600);
  radio.begin();
  delay(100);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setChannel(115);
  radio.openReadingPipe(0, address);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    Payload received;
    radio.read(&received, sizeof(received));

    if (received.distance > 0 && received.distance <= receivedMax) {
      int percentage = map(received.distance, TankHigh, TankLow, 100, 0);
      percentage = constrain(percentage, 0, 100);
      Serial.print("LEVEL:");
      Serial.print(percentage);
      Serial.print(",SIGNAL:");
      Serial.println(received.signalStrength);
    } else {
      Serial.println("-1");
    }
  }
}
