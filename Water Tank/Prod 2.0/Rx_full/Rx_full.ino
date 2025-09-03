#include <SPI.h>
#include <RF24.h>

RF24 radio(7, 10);  // CE, CSN
const uint64_t address = 0xE8E8F0F0E1LL;

// Mapping / limits
const int receivedMax = 260;   // sanity cap on inches
const int TankHigh    = 44;    // 100%
const int TankLow     = 254;   // 0%

// Watchdog
const uint32_t NO_PACKET_MS = 1000; // after this, emit -1
uint32_t lastRxMs    = 0;
uint32_t lastWdPrint = 0;

struct Payload {
  int distance;        // inches
  int signalStrength;  // 0–100 (%), derived from TX ARC
};

void setup() {
  Serial.begin(9600);

  if (!radio.begin()) {
    Serial.println("Radio not responding");
    while (1) {}
  }

  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(115);
  radio.openReadingPipe(0, address);
  radio.startListening();

  lastRxMs = millis();   // start the watchdog clock
  lastWdPrint = 0;

  // Optional: tell the PC app we're up but waiting for data
  // Serial.println("-1");
}

void loop() {
  bool gotPacket = false;

  while (radio.available()) {
    Payload received;
    radio.read(&received, sizeof(received));
    gotPacket = true;
    lastRxMs = millis();

    if (received.distance > 0 && received.distance <= receivedMax) {
      int percentage = map(received.distance, TankHigh, TankLow, 100, 0);
      percentage = constrain(percentage, 0, 100);

      Serial.print("LEVEL:");
      Serial.print(percentage);
      Serial.print(",SIGNAL:");
      Serial.println(received.signalStrength);
    } else {
      Serial.println("-1"); // invalid reading
    }
  }

  // Watchdog: if no packets for > NO_PACKET_MS, emit -1 once per second
  uint32_t now = millis();
  if (!gotPacket && (now - lastRxMs > NO_PACKET_MS)) {
    if (now - lastWdPrint >= NO_PACKET_MS) {
      Serial.println("-1");
      lastWdPrint = now;
    }
  }
}