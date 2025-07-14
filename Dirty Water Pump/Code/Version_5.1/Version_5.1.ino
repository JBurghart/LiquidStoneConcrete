////////////////////////////
//                        // 
//    JACOB BURGHART &    //
//     JOHN ANDERSON      //
//  UPDATED JUL 14 2025   // 
// SCHEMATICS AVAILIBLE   //
////////////////////////////



//pump sucks air at 24 inches
//pit overflows at 16 inches
const int highLevel = 17;
const int lowLevel = 20;
const int AlarmHigh = 5;
const int AlarmLow = 30;

long Distance;// Mulitplier * readUltrasonicDistance
float Mulitplier = 0.006783; //0.006783

// 0.0067 = Micoseconds for sound to travel 1/2 inch; 0.0134 for 1 in

const int Alarm = 12;
const int cleanWaterPump = 7; //OUTPUT and INPUT; Sensor and pushbutton
const int dirtyWaterPump = 6; //OUTPUT and INPUT; Sensor and pushbutton
const int overfillWaterSensor = 5; //LED and SENSOR
const int lowWaterSensor = 4; //LED and SENSOR
const int trigger = 3;
const int echo = 2;

const long lineFlush = 60000; // Time in milliseconds


long readUltrasonicDistance(int echoPin, int triggerPin)
{
  pinMode(triggerPin, OUTPUT);  // Clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2); // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT); // Reads the echo pin, and returns the sound wave travel time in microseconds
  return pulseIn(echoPin, HIGH);
}

void setup()
{
  pinMode(cleanWaterPump, OUTPUT); // D7
  pinMode(dirtyWaterPump, OUTPUT); // D6
  pinMode(overfillWaterSensor, INPUT); // D5
  pinMode(lowWaterSensor, INPUT); // D4
  pinMode(Alarm, OUTPUT); //D12
  
  Serial.begin(9600);
}

void loop()
{
  pinMode(dirtyWaterPump, INPUT); // Read button switch to overide this if statement
  
  Distance = (Mulitplier * readUltrasonicDistance(echo, trigger)); 
  // RUN IF SYSTEM TRIGGERED AUTOMATICALLY OR MANUALLY AND SENSORS ARE NORMAL (ultrasonic sensor is above high level, & overfill and tank empty sensors are normal, and distance is within expected range, or triggered manually)
  if ((Distance <= highLevel) && (digitalRead(lowWaterSensor) == HIGH) && (digitalRead(overfillWaterSensor) == LOW) && (Distance >= AlarmHigh) && (Distance <= AlarmLow) || ((Distance <= lowLevel) && (digitalRead(lowWaterSensor) == HIGH) && (digitalRead(dirtyWaterPump) == HIGH) && (Distance >= AlarmHigh) && (Distance <= AlarmLow) ))  
  {  
    digitalWrite(Alarm, LOW); //reset alarm if triggered
    pinMode(dirtyWaterPump, OUTPUT);
    digitalWrite(dirtyWaterPump, HIGH);
    Serial.println("System normal and emptying, Dirty Pump On");

    Distance = (Mulitplier * readUltrasonicDistance(echo, trigger));
    while (Distance <= lowLevel)  // Run dirty pump till water below low level
    {
      delay(1000); // Wait for .2 sec while water is above low level to avoid const sampling
      Distance = (Mulitplier * readUltrasonicDistance(echo, trigger));
      Serial.print("NORMAL OPERATION ");
      Serial.println("EMPTYING");
      Serial.print(Distance);
      Serial.println(" Current measured distance_Normal=5 to 30");
      Serial.print("Low Cutoff = ");
      Serial.println(lowLevel);
      Serial.print("High Trigger = ");
      Serial.println(highLevel);
      Serial.println();

      if ((Distance <= AlarmHigh) || (Distance >= AlarmLow) || (digitalRead(lowWaterSensor) == LOW) || (digitalRead(overfillWaterSensor) == HIGH)) //ERROR BREAK
      {
        digitalWrite(Alarm, HIGH);
        Serial.print(Distance);
        Serial.println("____Error 0x1____ OUT OF EXPECTED RANGE OR WATER OVERFILL/EMPTY");
        break;
      }
    }

    digitalWrite(dirtyWaterPump, LOW);
    
    delay(2000); // Wait for 2 sec to avoid water hammer

    digitalWrite(cleanWaterPump, HIGH);
    Serial.println("Line flushing");
    Serial.println();
    delay(lineFlush); // Wait for 60 sec to flush line

    digitalWrite(cleanWaterPump, LOW);
    
    delay(5000); // Wait to avoid 1st if statement false water hammer
  
  
  } 
  
  else //waiting till water is high
  {
    delay(50); // Wait for .05s to be responsive
    
    Distance = (Mulitplier * readUltrasonicDistance(echo, trigger));
    // lowWaterSensor or overfillWaterSensor are triggered, Ultrasonic Sensor is unplugged or reading erroneous distances
    if ((Distance <= AlarmHigh) || (Distance >= AlarmLow) || (digitalRead(lowWaterSensor) == LOW) || (digitalRead(overfillWaterSensor) == HIGH))//trigger alarm
    {
      digitalWrite(Alarm, HIGH);
      Serial.println("ERROR");
      Serial.print(Distance);
      Serial.println(" <<< Current measured distance_Normal=5 to 30");
      Serial.print(digitalRead(overfillWaterSensor));
      Serial.println(" Overfill Sensor state_Normal=0");
      Serial.print(digitalRead(lowWaterSensor));
      Serial.println(" Low Water Sensor state_Normal=1");
      Serial.println();
      //alarm range  05 >> 10 << 30
      //normal range 24 >> 26 << 28

      delay(500);
    }
    else //normal waiting state
    {
      if (digitalRead(Alarm)==HIGH)
        {
          digitalWrite(Alarm, LOW);
        }
      Serial.print("NORMAL OPERATION "); ");
      Serial.println("WAITING");
      Serial.print(Distance);
      Serial.println(" <<< Current measured distance_Normal=5 to 30");
      Serial.print(digitalRead(overfillWaterSensor));
      Serial.println(" Overfill Sensor state_Normal=0");
      Serial.print(digitalRead(lowWaterSensor));
      Serial.println(" Low Water Sensor state_Normal=1");
      Serial.println();

      delay(1000);
    }
  }
}