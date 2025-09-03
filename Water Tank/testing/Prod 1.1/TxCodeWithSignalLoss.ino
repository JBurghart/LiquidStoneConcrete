#include <SPI.h>
#include <RF24.h>

// Analog distance input
const int analogPin = A0;
const float Vcc = 4.8;
const float scaleFactor = Vcc / 512.0;

RF24 radio(7, 10); // CE, CSN
const uint64_t address = 0xE8E8F0F0E1LL;

struct Payload {
  int distance;
  int signalStrength;
};

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.setChannel(115);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.setRetries(3, 15);  // 15 retries, 750µs delay
  radio.openWritingPipe(address);
  radio.stopListening();  // Set as TX
}

void loop() {
  // Convert analog to inches
  int analogValue = analogRead(analogPin);
  float voltage = analogValue * (Vcc / 1023.0);
  float distanceInches = voltage / scaleFactor;
  int distance = (int)(distanceInches + 0.5);

  Payload payload;
  payload.distance = distance;

  // Attempt to send
  bool success = radio.write(&payload, sizeof(payload));
  int arc = radio.getARC();  // Auto Retransmit Count (0-15)

  // Convert retries to signal %
  payload.signalStrength = map(arc, 0, 15, 100, 0);
  payload.signalStrength = constrain(payload.signalStrength, 0, 100);

  // Print for debug
  Serial.print("Distance: ");
  Serial.print(payload.distance);
  Serial.print(" in | Retries: ");
  Serial.print(arc);
  Serial.print(" | Signal: ");
  Serial.print(payload.signalStrength);
  Serial.println("%");

  // Re-send with updated payload including signal strength
  radio.write(&payload, sizeof(payload));

  delay(200);
}
