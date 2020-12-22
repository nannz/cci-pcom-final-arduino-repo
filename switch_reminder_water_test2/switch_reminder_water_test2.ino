//consider the switch as the conductive ink
//the count-down feature
//1st switch - water the plant every day (daily)
//2nd switch - water the plant once every 3 days
//3rd switch - water the plant once every 7 days (weekly)

//in test 2, add the reminder function

/*
   Messages consist of the letter T followed by ten digit time (as seconds since Jan 1 1970)
   you can send the text on the next line using Serial Monitor to set the clock to noon Jan 1 2013
  T1357041600
   website to get the number: https://www.epochconverter.com/
*/

#include <TimeLib.h>

#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

/**********************************/
const int switchPin1 = 12; //the switch connect to pin 12
const int ledPin = 3;//the led connect to pin 6
boolean ledOn = false;
/**********************************/
int switchState1 = 0;         // variable for reading the pushbutton status
int lastSwitchState1 = 0;
//switchState1 --- every day, so count for a whole day
int dayRecord = 0;
int monthRecord = 0;
int hourRecord = 0;
int minuteRecord = 0;
int secondRecord = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) ; // Needed for Leonardo only
  pinMode(switchPin1, INPUT); //initialize thebuttonPin as input
  pinMode(ledPin, OUTPUT); //initialize the led pin as output
  setSyncProvider( requestSync);  //set function to call when sync required
  Serial.println("Waiting for sync message");
}
/**********************************/
void loop() {
  //------GET and PRINT time--------//
  if (Serial.available()) {
    processSyncMessage();
  }
  if (timeStatus() != timeNotSet) {
    digitalClockDisplay();//display the current time
  }
  if (timeStatus() == timeSet) {
    digitalWrite(13, HIGH); // LED on if synced time
  } else {
    digitalWrite(13, LOW);  // LED off if time needs refresh
  }
  delay(1000);
  /**********************************/
  //-----SWITCH TO TURN ON TIMER-------//
  //read the state of the switch value
  switchState1 = digitalRead(switchPin1);
  //  Serial.println(switchState1);

  //SWITCH 1
  if (switchState1 == HIGH && timeStatus() != timeNotSet && switchState1 != lastSwitchState1) //if it is,the state is HIGH
  {
    //turn on the timer for 24hours.//in test, set 1 minute.
    Serial.println("set the 1 minute timer!");
    dayRecord = day();
    minuteRecord = minute();
    secondRecord = second();
    //digitalWrite(ledPin, HIGH); //turn the led on
  }
  else {
    //digitalWrite(ledPin, LOW); //turn the led off
  }
  lastSwitchState1 = switchState1;
  /**********************************/
  //-----RUNNING TIMER-----//
  //if passing 1 minute (indicating 1 day
  if ((minute() - minuteRecord == 1) && (second() == secondRecord)) {
    ledOn = true;
    Serial.println("time's up! turn on the led!");
  } 
  
  /**********************************/
  // the led light
  if (ledOn) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }

}

void digitalClockDisplay() {
  // digital clock display of the time
  //hour();minute();second();day();month();year();

  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());
  Serial.println();
}


void printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  if (Serial.find(TIME_HEADER)) {
    pctime = Serial.parseInt();
    if ( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
      setTime(pctime); // Sync Arduino clock to the time received on the serial port
    }
  }
}

time_t requestSync()
{
  Serial.write(TIME_REQUEST);
  return 0; // the time will be sent later in response to serial mesg
}
