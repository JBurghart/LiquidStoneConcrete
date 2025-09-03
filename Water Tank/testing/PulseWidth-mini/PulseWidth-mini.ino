int pin = 6;  // or any digital pin
unsigned long duration;
float convert = 147.0; // microseconds per in

void setup() {
  Serial.begin(9600);
  pinMode(pin, INPUT);
}

void loop() {
  duration = pulseIn(pin, HIGH);  // microseconds
  float distance_in = duration / convert; 
  float distance_ft = distance_in / 12.0; 
  Serial.print(distance_ft);
  Serial.println(" ft");
}