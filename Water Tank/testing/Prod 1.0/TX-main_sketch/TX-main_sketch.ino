#include <SoftwareSerial.h> //REMOVE
#include <SPI.h>
#include <RF24.h>

// RS232 to TTL reading
SoftwareSerial sonarSerial(8, 9);  // RX (from sonar TX Pin 5), TX (unused) //REMOVE

// nRF24L01+
RF24 radio(7, 10); // CE, CSN
const uint64_t address = 0xE8E8F0F0E1LL;   // Unique pipe ID

// Timing
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 1000; // ms
const int samplesToAverage = 3;

// Reading storage
int sum = 0;
int readCount = 0;

void setup() {
  Serial.begin(9600);         // Debugging to Serial Monitor //REMOVE
  sonarSerial.begin(9600);    // MaxBotix TX pin 5 into pin 8 

  radio.begin();
  radio.setChannel(115);                 // Match RX
  radio.setPALevel(RF24_PA_LOW);         // Can change to HIGH if needed
  radio.setDataRate(RF24_250KBPS);       // Reliable at longer range
  radio.setRetries(3, 5);                // Retry settings
  radio.openWritingPipe(address);
  radio.stopListening();                 // TX mode
}

void loop() {
  // Check for new data from sonar
  while (sonarSerial.available()) {  //while (Serial.available()) {  // Changed from sonarSerial
    char c = sonarSerial.read();     //char c = Serial.read();     // Changed from sonarSerial

    static String buffer = "";
    static bool reading = false;

    if (c == 'R') {
      buffer = "";
      reading = true;
    } else if (reading) {
      if (c == '\r') {
        int distance = buffer.toInt();
        if (distance > 0 && distance < 1000) {
          sum += distance;
          readCount++;
        }
        reading = false;
      } else {
        buffer += c;
      }
    }
  }

  // Average and send over radio
  if (readCount >= samplesToAverage && millis() - lastSendTime >= sendInterval) {
    int average = sum / readCount;
    Serial.print("Sending average distance (inches): ");
    Serial.println(average);

    radio.write(&average, sizeof(average)); // Transmit as int

    sum = 0;
    readCount = 0;
    lastSendTime = millis();
  }
}