#include <SoftwareSerial.h>

SoftwareSerial sonarSerial(10, 9); // RX, TX (TX unused)

String buffer = "";
bool reading = false;

void setup() {
  Serial.begin(9600);         // Monitor output
  sonarSerial.begin(9600);    // Sensor output (9600 8N1)
  delay(1000); 
}

void loop() {
  while (sonarSerial.available()) {
    char c = sonarSerial.read();

    // Begin reading when 'R' is detected
    if (c == 'R') {
      buffer = "";
      reading = true;
    } else if (reading) {
      if (c == '\r') { // Carriage return indicates end of reading
        int distance = buffer.toInt();
        Serial.print("Distance (inches): ");
        Serial.println(distance);
        reading = false;
      } else {
        buffer += c;
      }
    }
  }
}