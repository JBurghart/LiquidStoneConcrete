#include <SoftwareSerial.h>
#include <SPI.h>
#include <RF24.h>

// RS232 to TTL reading
SoftwareSerial sonarSerial(8, 7);  // RX (from MAX3232), TX (unused)

// nRF24L01+
RF24 radio(9, 10); // CE, CSN
const byte address[6] = "NODE1";   // Unique 5-byte pipe ID

// Timing
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 1000; // ms
const int samplesToAverage = 3;

// Reading storage
int sum = 0;
int readCount = 0;

void setup() {
  Serial.begin(9600);         // Debugging
  sonarSerial.begin(9600);    // MaxBotix sensor (RS232, TTL converted)

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);         // Change to RF24_PA_HIGH for longer range
  radio.setDataRate(RF24_250KBPS);       // More reliable for long range
  radio.openWritingPipe(address);
  radio.setRetries(3, 5);                // Retry 3 times, 5 * 250us delay
  radio.stopListening();                 // We're the sender
}

void loop() {
  // Check for new serial data
  if (sonarSerial.available()) {                            // **** replace with Serial.available for hardware serial 
    String reading = sonarSerial.readStringUntil('\r'); // Read until carriage return
    if (reading.startsWith("R")) {
      int value = reading.substring(1).toInt();  // Remove 'R' and convert to int
      if (value > 0 && value < 1000) {           // Basic filter for valid range
        sum += value;
        readCount++;
      }
    }
  }

  // If enough samples collected and time to send
  if (readCount >= samplesToAverage && millis() - lastSendTime >= sendInterval) {
    int average = sum / readCount;                                // 
    Serial.print("Sending average distance (inches): ");          //
    Serial.println(average);                                      //

    radio.write(&average, sizeof(average));  // Transmit as 2-byte int

    // Reset
    sum = 0;
    readCount = 0;
    lastSendTime = millis();
  }
}