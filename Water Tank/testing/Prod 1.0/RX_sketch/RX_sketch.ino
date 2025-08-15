#include <SPI.h>
#include <RF24.h>

RF24 radio(7, 10);  // CE, CSN - hardware jumpers on NRF shield
const uint64_t address = 0xE8E8F0F0E1LL;  //Generated address, other transmitters in aread may have same

const int receivedMax = 270; // Above maximum distance the ultrasonic sensor should ever read
//const int receivedMin = 20; // Below minimum distance the ultrasonic sensor should ever read, propably over flowing

const int TankHigh = 44; // Distance in inches for the tank to be considered full - sensor at top of tank
const int TankLow = 262; // Distance in inches for the tank to be considered empty - sensor at top of tank


void setup() {
  Serial.begin(9600);        // USB to PC
  radio.begin();
  delay(100); // Give RF24 module time to stabilize
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_HIGH);  // Or MAX if needed
  radio.setChannel(115);
  radio.openReadingPipe(0, address);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    int received;
    radio.read(&received, sizeof(received));
      if (received > 0 and received <= receivedMax) { //Math converting raw distance to percentage
        int percentage = map(received, TankHigh, TankLow, 100, 0); //overfill handled by PC alert
                                                                   //adjust TankHigh and TankLow to reduce alerts
                                                                   //**EXCETPTION NEGATIVE PERCENTAGE BETWEEN receivedMax AND TankLow**
        Serial.println(percentage); // Send percentage to PC
      }
      else {
        Serial.println("-1"); // Out of range handled by PC alert
      }
    //Serial.print("Received distance: "); // debug
    //Serial.print(received);
    //Serial.println(" inches");
  }
}