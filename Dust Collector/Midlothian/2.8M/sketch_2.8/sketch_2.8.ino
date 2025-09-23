/*/////////////////////
Last midlo edit

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
const int PurgeValve = (R0_3); //water purge valve for air line
const int DustCollectorRelay = (R0_4);
const int VaneFeederRelay = (R0_5);
const int BlowerRelay = (R0_6);
const int VibratorRelay = (R0_7);
const int PulsersRelay = (R0_8);
//IN
const int BindicatorInput = (I0_6);
const int PlantOnInput = (I0_7);
const int SysTestButtonInput = (I0_8); //24v momentary push button

int ShutdownDelay = 10000;
int StartUpDelay = 5000; //5 sec between starting motors to prevent excessive inrush current
unsigned long TimeToEmptyDustCollector = 420000; // how long to run 7 min 420,000ms
unsigned long TimeToPurgeAirLine = 2000; // purge air line for 2 sec
unsigned long AirLinePurgePeriod = 14400000; // purge air line every 4 hrs

unsigned long TimeToPurgeAirLineLastReset = 0;
unsigned long PulserTimeLastReset = 0;
unsigned long MainTimeLastReset   = 0;
unsigned long VibTimeLastReset    = 0;

int VibratorPulseOn = 5000; //Vib on duration 5 sec
unsigned long VibratorPulseOff = 55000; //Vib off duration

unsigned long PulsersRelayCloseTime = 180000;//Relay closed duing automated bag cleaning 16,400ms per row *10 rows = 180,000
unsigned long PulsersInterval = 3420000; //1000ms * 60Sec * 60Min = 3,600,000----------how often to clean bags **minus** pulsers run time 57min


void setup() 
{
  unsigned long TimeToPurgeAirLineLastReset = millis();
  unsigned long PulserTimeLastReset = millis();
  unsigned long MainTimeLastReset = millis();
  unsigned long VibTimeLastReset = millis();

  pinMode(DustCollectorRelay, OUTPUT);
  digitalWrite(DustCollectorRelay, LOW); 
  pinMode(BlowerRelay, OUTPUT);
  digitalWrite(BlowerRelay, LOW);
  pinMode(VaneFeederRelay, OUTPUT);
  digitalWrite(VaneFeederRelay, LOW);
  pinMode(PulsersRelay, OUTPUT);
  digitalWrite(PulsersRelay, LOW);
  pinMode(VibratorRelay, OUTPUT);
  digitalWrite(VibratorRelay, LOW);
  pinMode(PurgeValve, OUTPUT);
  digitalWrite(PurgeValve, LOW);

  pinMode(BindicatorInput, INPUT);
  pinMode(PlantOnInput, INPUT);
  pinMode(SysTestButtonInput, INPUT);
  Serial.begin(9600);
  delay(1000);
  PulserTimeLastReset = millis();
  TimeToPurgeAirLineLastReset = millis();
}
void loop()
{

  if(digitalRead(PlantOnInput) == HIGH)
  {
    Serial.println("");
    Serial.println("plant On");
    Serial.print("Pulsers Timer 57min or 3,420 Sec   ");

    if ((millis() - PulserTimeLastReset >= PulsersInterval))//time to run pulsers
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

      MainTimeLastReset = millis();
      while (millis() - MainTimeLastReset <= TimeToEmptyDustCollector)// Emptying period 7 min
      {
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

      Serial.println("Shutting down");
      //Begin Shutdown *****

      digitalWrite(VaneFeederRelay, LOW);
      MainTimeLastReset = millis();
      Serial.println("VaneFeeder Off");
      while (millis() - MainTimeLastReset <= ShutdownDelay)// delay 10 sec
      {}
      
      digitalWrite(BlowerRelay, LOW);
      MainTimeLastReset = millis();
      Serial.println("Blower Off");
      while (millis() - MainTimeLastReset <= ShutdownDelay)// delay 10 sec
      {}

      digitalWrite(DustCollectorRelay, LOW);
      MainTimeLastReset = millis();
      Serial.println("DustCollector Off");
      while (millis() - MainTimeLastReset <= ShutdownDelay)// delay 10 sec to stop suction + pulsers 15 sec self delay
      {}

      digitalWrite(PulsersRelay, HIGH);
      MainTimeLastReset = millis();
      while (millis() - MainTimeLastReset <= PulsersRelayCloseTime) //delay 150 sec
      {
        Serial.print("Pulsers Running");
        delay(50);
      }
    
      digitalWrite(PulsersRelay, LOW);
      PulserTimeLastReset = millis();
      Serial.println("cycle Done");
    }
    

    else 
    {
      Serial.println("Waiting for trigger");
      delay(10);
      if ((millis() - TimeToPurgeAirLineLastReset >= AirLinePurgePeriod)) //time to purge water
      {
        Serial.println("purge valve triggered");
        digitalWrite(PurgeValve, HIGH);
        delay(TimeToPurgeAirLine);
        digitalWrite(PurgeValve, LOW);
        TimeToPurgeAirLineLastReset = millis();

      }
      //**
      if (digitalRead(SysTestButtonInput) == HIGH) //**** test button trigger
      {
        //BEGIN ELECTRICAL TESTS
        Serial.println("SysTestButton Triggered");
        digitalWrite(DustCollectorRelay, HIGH);// dust collector ON

        MainTimeLastReset = millis();
        while (millis() - MainTimeLastReset <= StartUpDelay)// delay 5 sec
        {}

        digitalWrite(BlowerRelay, HIGH);// blower ON
        
        MainTimeLastReset = millis();
        while (millis() - MainTimeLastReset <= StartUpDelay)// delay 5 sec
        {}

        digitalWrite(VaneFeederRelay, HIGH);// vane feeder ON

        MainTimeLastReset = millis();
        while (millis() - MainTimeLastReset <= StartUpDelay)// delay 5 sec
        {}
        // Startup Complete shutting down
        digitalWrite(VaneFeederRelay, LOW);
        MainTimeLastReset = millis();
        Serial.println("VaneFeeder Off");
        while (millis() - MainTimeLastReset <= ShutdownDelay)// delay 10 sec
        {}
        
        digitalWrite(BlowerRelay, LOW);
        MainTimeLastReset = millis();
        Serial.println("Blower Off");
        while (millis() - MainTimeLastReset <= ShutdownDelay)// delay 10 sec
        {}

        digitalWrite(DustCollectorRelay, LOW);
        MainTimeLastReset = millis();
        Serial.println("DustCollector Off");
        while (millis() - MainTimeLastReset <= ShutdownDelay)// delay 10 sec to stop suction + pulsers 15 sec self delay
        {}

        //END ELECTRICAL TESTS
        //BEGIN AIR TESTS

        //Pulsers test
        digitalWrite(PulsersRelay, HIGH); 
        MainTimeLastReset = millis();
        while (millis() - MainTimeLastReset <= PulsersRelayCloseTime) //delay 150 sec
        {
          Serial.println("Pulsers Running");
          delay(50);
        }
        digitalWrite(PulsersRelay, LOW);

        //Vibrator test
        digitalWrite(VibratorRelay, HIGH);
        delay(VibratorPulseOn);
        digitalWrite(VibratorRelay, LOW);
        delay(3000);
        digitalWrite(VibratorRelay, HIGH);
        delay(VibratorPulseOn);
        digitalWrite(VibratorRelay, LOW);

        //Purge valve test
        digitalWrite(PurgeValve, HIGH);
        delay(TimeToPurgeAirLine);
        digitalWrite(PurgeValve, LOW);
        delay(1000);
        digitalWrite(PurgeValve, HIGH);
        delay(TimeToPurgeAirLine);
        digitalWrite(PurgeValve, LOW);
        //END AIR TESTS


      }
    }
    
  }
  
  else
  {
    Serial.println("Plant off, waiting");
    PulserTimeLastReset = millis();
    delay(10);
    if (digitalRead(SysTestButtonInput) == HIGH) //**** test button trigger
    {
      //BEGIN ELECTRICAL TESTS
      Serial.println("SysTestButton Triggered");
      digitalWrite(DustCollectorRelay, HIGH);// dust collector ON

      MainTimeLastReset = millis();
      while (millis() - MainTimeLastReset <= StartUpDelay)// delay 5 sec
      {}

      digitalWrite(BlowerRelay, HIGH);// blower ON
      
      MainTimeLastReset = millis();
      while (millis() - MainTimeLastReset <= StartUpDelay)// delay 5 sec
      {}

      digitalWrite(VaneFeederRelay, HIGH);// vane feeder ON

      MainTimeLastReset = millis();
      while (millis() - MainTimeLastReset <= StartUpDelay)// delay 5 sec
      {}
      // Startup Complete shutting down
      digitalWrite(VaneFeederRelay, LOW);
      MainTimeLastReset = millis();
      Serial.println("VaneFeeder Off");
      while (millis() - MainTimeLastReset <= ShutdownDelay)// delay 10 sec
      {}
      
      digitalWrite(BlowerRelay, LOW);
      MainTimeLastReset = millis();
      Serial.println("Blower Off");
      while (millis() - MainTimeLastReset <= ShutdownDelay)// delay 10 sec
      {}

      digitalWrite(DustCollectorRelay, LOW);
      MainTimeLastReset = millis();
      Serial.println("DustCollector Off");
      while (millis() - MainTimeLastReset <= ShutdownDelay)// delay 10 sec to stop suction + pulsers 15 sec self delay
      {}

      //END ELECTRICAL TESTS
      //BEGIN AIR TESTS

      //Pulsers test
      digitalWrite(PulsersRelay, HIGH); 
      MainTimeLastReset = millis();
      while (millis() - MainTimeLastReset <= PulsersRelayCloseTime) //delay 150 sec
      {
        Serial.println("Pulsers Running");
        delay(50);
      }
      digitalWrite(PulsersRelay, LOW);

      //Vibrator test
      digitalWrite(VibratorRelay, HIGH);
      delay(VibratorPulseOn);
      digitalWrite(VibratorRelay, LOW);
      delay(3000);
      digitalWrite(VibratorRelay, HIGH);
      delay(VibratorPulseOn);
      digitalWrite(VibratorRelay, LOW);

      //Purge valve test
      digitalWrite(PurgeValve, HIGH);
      delay(TimeToPurgeAirLine);
      digitalWrite(PurgeValve, LOW);
      delay(1000);
      digitalWrite(PurgeValve, HIGH);
      delay(TimeToPurgeAirLine);
      digitalWrite(PurgeValve, LOW);
      //END AIR TESTS


    }
    
  }
}

