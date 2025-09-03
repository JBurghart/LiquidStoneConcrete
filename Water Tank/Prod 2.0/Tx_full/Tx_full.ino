#include <SPI.h>
#include <RF24.h>

const uint8_t PIN_ECHO = 6;
const float US_PER_IN = 147.0f;           // µs per inch
const uint32_t PULSE_TIMEOUT_US = 120000; // fits your ~98.5 ms period

RF24 radio(7, 10);  // CE, CSN (adjust pins if needed)
const uint64_t address = 0xE8E8F0F0E1LL;

struct Payload {
  int distance;        // inches
  int signalStrength;  // 0–100 %
};

void setup() {
  Serial.begin(9600);
  pinMode(PIN_ECHO, INPUT);

  if (!radio.begin()) {
    Serial.println("Radio not responding");
    while (1) {}
  }
  radio.setChannel(115);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.setRetries(3, 15);  // 15 retries, 750 µs delay
  radio.openWritingPipe(address);
  radio.stopListening();    // TX mode

  Serial.println("TX ready");
}

void loop() {
  // Read distance
  unsigned long us = pulseIn(PIN_ECHO, HIGH, PULSE_TIMEOUT_US);
  int distance = -1;
  if (us > 0) {
    distance = (int)((us / US_PER_IN) + 0.5f);  // round to int inches
  }

  Payload payload;
  payload.distance = distance;

  // Attempt to send
  bool success = radio.write(&payload, sizeof(payload));
  int arc = radio.getARC();  // Auto Retransmit Count (0–15)

  // Convert retries to signal %
  int strength = map(arc, 0, 15, 100, 0);
  strength = constrain(strength, 0, 100);
  payload.signalStrength = strength;

  // Debug
  Serial.print("Distance: ");
  Serial.print(payload.distance);
  Serial.print(" in | Retries: ");
  Serial.print(arc);
  Serial.print(" | Signal: ");
  Serial.print(strength);
  Serial.println("%");

  // Re-send with signal included (same as your old sketch)
  radio.write(&payload, sizeof(payload));

  delay(200);
}