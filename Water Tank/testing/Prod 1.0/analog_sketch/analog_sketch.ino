const int analogPin = A0;      // Connected to AN (Pin 3 of sensor)
const float Vcc = 4.8;         // Supply voltage to sensor (change to 3.3 if needed)
const float scaleFactor = Vcc / 512.0;  // Scale: ~9.8 mV/inch @ 5V

void setup() {
  Serial.begin(9600);
}

void loop() {
  int analogValue = analogRead(analogPin);             // 10-bit ADC: 0–1023
  float voltage = analogValue * (Vcc / 1023.0);         // Convert to voltage
  float distanceInches = voltage / scaleFactor;         // Convert to inches

  Serial.print("Distance (inches): ");
  Serial.println(distanceInches, 1);  // 1 decimal place

  delay(200);  // Adjust for desired update rate
}