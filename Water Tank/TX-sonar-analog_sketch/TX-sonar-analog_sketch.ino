#include <SPI.h>
#include <RF24.h>

// Analog distance input
const int analogPin = A0;      // Connected to AN (Pin 3 of sonar)
const float Vcc = 4.8;         // Measured supply voltage to sensor
const float scaleFactor = Vcc / 512.0;  // ~9.375 mV/inch at 4.8V

// nRF24L01+
RF24 radio(7, 10); // CE, CSN
const uint64_t address = 0xE8E8F0F0E1LL; // Pipe address

void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.setChannel(115);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(address);
  radio.stopListening();  // Set as TX
}

void loop() {
  // Read analog pin and convert to voltage
  int analogValue = analogRead(analogPin);
  float voltage = analogValue * (Vcc / 1023.0);
  float distanceInches = voltage / scaleFactor;

  int valueToSend = (int)(distanceInches + 0.5);  // Round to nearest int

  Serial.print("Sending: ");
  Serial.println(valueToSend);

  radio.write(&valueToSend, sizeof(valueToSend));

  delay(200);
}