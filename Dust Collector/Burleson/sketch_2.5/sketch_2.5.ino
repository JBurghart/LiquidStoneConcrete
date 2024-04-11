  /*/////////////////////
FINAL JUN 8 2023

*//////////////////////

//#include <ARDBOX-Relay.h>

const int R0_1 = 10;
const int R0_2 = 9;
const int R0_3 = 6;
const int R0_4 = 5;
const int R0_5 = 3;
const int R0_6 = 7;
const int R0_7 = 0;
const int R0_8 = 1;

const int I0_0 = 2;
const int I0_1 = 12;
const int I0_2 = 4;
const int I0_3 = 8;
const int I0_4 = 18;
const int I0_5 = 19;
const int I0_6 = 20;
const int I0_7 = 21;
const int I0_8 = 22;
const int I0_9 = 23;

//OUT
const int DustCollectorRelay = (R0_4);
const int BlowerRelay = (R0_6);
const int VaneFeederRelay = (R0_5);
const int VibratorRelay = (R0_7);
const int PulsersRelay = (R0_8);
//IN
const int BindicatorInput = (I0_6);
const int PlantOnInput = (I0_7);

int ShutdownDelay = 15000;
int StartUpDelay = 5000; //sec between starting motors to prevent excessive inrush current reset to 5 sec
unsigned long TimeToEmptyDustCollecter = 600000; // how long to run 10 min 600000ms

unsigned long PulserTimeLastReset = millis();
unsigned long MainTimeLastReset = millis();
unsigned long VibTimeLastReset = millis();

int VibratorPulseOn = 5000; //Vib on duration
unsigned long VibratorPulseOff = 55000; //Vib off duration

int PulsersRelayCloseTime = 1000;//close relay to trigger automated bag cleaning
unsigned long PulsersInterval = 3600000; //1000ms * 1Min * 1HR = 3,600,000----------how often to clean bags
unsigned long PulsersCycleTime = 100000; //1000ms * 100Sec = 100,000


void setup() 
{
  pinMode(DustCollectorRelay, OUTPUT);
  pinMode(BlowerRelay, OUTPUT);
  pinMode(VaneFeederRelay, OUTPUT);
  pinMode(PulsersRelay, OUTPUT);
  pinMode(VibratorRelay, OUTPUT);

  pinMode(BindicatorInput, INPUT);
  pinMode(PlantOnInput, INPUT);
  Serial.begin(9600);
  delay(3000);
  PulserTimeLastReset = millis();
}
void loop() 
{
  if(digitalRead(PlantOnInput) == HIGH)
  {
    Serial.println("plant On");

    if ((unsigned long)(millis() - PulserTimeLastReset >= PulsersInterval))
    {
      Serial.println("Pulsers Triggered");
      digitalWrite(PulsersRelay, HIGH);
      delay(PulsersRelayCloseTime);
      digitalWrite(PulsersRelay, LOW);
      PulserTimeLastReset = millis();
    }
    if (digitalRead(BindicatorInput) == HIGH)
    {  //Begin startup process
      Serial.println("Bindicator Triggered, starting up");

      delay(500);
      digitalWrite(DustCollectorRelay, HIGH);//ON

      MainTimeLastReset = millis();
      while (millis() - MainTimeLastReset <= StartUpDelay)// delay 5 sec
      {}

      digitalWrite(BlowerRelay, HIGH);//ON
      
      MainTimeLastReset = millis();
      while (millis() - MainTimeLastReset <= StartUpDelay)// delay 5 sec
      {}

      digitalWrite(VaneFeederRelay, HIGH);//ON

      Serial.println("Startup Complete");
      
      MainTimeLastReset = millis();
      while (millis() - MainTimeLastReset <= StartUpDelay)// delay 5 sec
      {}

      Serial.println("vibrator on");

      MainTimeLastReset = millis();
      while (millis() - MainTimeLastReset <= TimeToEmptyDustCollecter)// Empty for 10 min

        Serial.print("Emptying... Time1 elapsed = ");
        Serial.println(millis() - MainTimeLastReset);
        Serial.println();

        digitalWrite(VibratorRelay, HIGH);
        VibTimeLastReset = millis();

        while(millis() - VibTimeLastReset <= VibratorPulseOn)
        {
          Serial.print("Vibrator On... Time on = ");
          Serial.print(millis() - VibTimeLastReset);
          Serial.println();
          delay(50);
        }
        digitalWrite(VibratorRelay, LOW);
        VibTimeLastReset = millis();

        while(millis() - VibTimeLastReset <= VibratorPulseOff)
        {
          Serial.print("Vibrator Off... Time off = ");
          Serial.print(millis() - VibTimeLastReset);
          Serial.print("      Elapsed Time out of 600,000= ");
          Serial.print(millis() - MainTimeLastReset);
          Serial.println();
          delay(50);
        }
      }
      
      digitalWrite(VibratorRelay, LOW);

      Serial.println("vibrator off");
      //Begin Shutdown

      digitalWrite(VaneFeederRelay, LOW);
      MainTimeLastReset = millis();
      while (millis() - MainTimeLastReset <= ShutdownDelay)// delay 15 sec
      {}
      
      digitalWrite(BlowerRelay, LOW);
      MainTimeLastReset = millis();
      while (millis() - MainTimeLastReset <= ShutdownDelay)// delay 15 sec
      {}

      digitalWrite(DustCollectorRelay, LOW);
      MainTimeLastReset = millis();
      while (millis() - MainTimeLastReset <= 30000)// delay 30 sec to stop suction
      {}

      digitalWrite(PulsersRelay, HIGH);
      MainTimeLastReset = millis();
      while (millis() - MainTimeLastReset <= PulsersRelayCloseTime); //delay 1 sec
      {}
    
      digitalWrite(PulsersRelay, LOW);
      PulserTimeLastReset = millis();
      Serial.println("cycle Done");

      MainTimeLastReset = millis();
      while (millis() - MainTimeLastReset <= PulsersCycleTime); 
      {}
    }
    else 
    {Serial.println("bindicator off, waiting");
      delay(80);
    }
  }
  else
  {Serial.println("Plant off");
    delay(80);
  }
}
