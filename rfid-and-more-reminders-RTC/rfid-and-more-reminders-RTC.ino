//This code is for the {fridge reminder} part for CCI PCOM Term 1 - Final Project
//fully coded by Nan Zhao. 

//used RTC to sense time
//used RFID module to sense different types of food tags
//one LED on when reading a card
//another three LEDs on when the food is on Best-Date-Before.
//Different kinds of food has different timer period. For demo, the timers are set only for a few seconds. 

//used board Arduino UNO

//=======RFID=========
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9
#define SS_PIN          10
#define Green 2 //the green led


MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

//=======RTC========
#include "RTClib.h"
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char date[10] = "hh:mm:ss";



/**********************************/

const int timerLedPin1 = 4;//4;//the led connect to pin 6//the red led
const int timerLedPin2 = 5;
const int timerLedPin3 = 6;
/**********************************/

//set timer virables.
int dayRecord = 0;
int monthRecord = 0;
int hourRecord = 0;
int minuteRecord = 0;
int secondRecord1 = 0;
int secondRecord2 = 0;
int secondRecord3 = 0;
bool timerState_1 = false; //timer state for card 1
bool timerState_2 = false;
bool timerState_3 = false;


//my cards:
//1-138 116 177 18 , 8A 99 B6 12
//2-186 68 13 17 , BA 44 0D 11
//3-138 153 182 18 , 8A 74 B1 12
int part1 = 138;//I convert the nuidPICC[0] into integer use int(), that works!
int part2 = 116;
int part3 = 177;
int part4 = 18;
int card1[] = {138, 116, 177, 18};
int card2[] = {186, 68, 13, 17};
int card3[] = {138, 153, 182, 18};


void setup() {
  pinMode(Green, OUTPUT);//sensing if the card is present or absense.
  Serial.begin(9600);                                           // Initialize serial communications with the PC
  SPI.begin();                                                  // Init SPI bus
  mfrc522.PCD_Init();

  pinMode(timerLedPin1, OUTPUT);
  pinMode(timerLedPin2, OUTPUT);
  pinMode(timerLedPin3, OUTPUT);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
}

//virables for storing RFID tag values and flags
//to check if the card is present or not.
uint8_t buf[10] = {};
MFRC522::Uid id;
MFRC522::Uid id2;
bool is_card_present = false;
uint8_t control = 0x00;

int RTC_SecTest = 10;
boolean RTCtest = false;
int currentSecond = 0;
//*****************************************************************************************//
void loop() {

  //========get time RTC=========
  DateTime now = rtc.now();
  currentSecond = now.second();
  //Serial.print("current second: ");Serial.println(currentSecond);
  /* Test the RTC value comparision
    if (!RTCtest) {
    RTC_SecTest = int(now.second());
    Serial.print("RTC_SecTest: ");
    Serial.println(RTC_SecTest);
    RTCtest = true;
    }

    if (int(now.second())-RTC_SecTest == 5) {
    Serial.println("RTC_SecTest time's up!");
    }
  */
  //***********for RFID card *********
  //create an object of MIFARE key
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  //reading the status of RFID by reading its status code it is returning.
  MFRC522::StatusCode status;

  //add two conditions, which checks if there is a card-present or not.
  // Look for new cards
  // if there is no new card, and nothing on serial. it will return none.
  if ( !mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if ( !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  //if card is present, according to above function, both conditions will be false
  //then we set the flag result.
  bool result = true;
  uint8_t buf_len = 4;
  //copy the id.
  cpid(&id);

  //************************************************
  //sense the card type. start a timer.
  //********************************************
  //check the card if it's my card for timer.
  //Serial.println(int(id.uidByte[0]));
  if (int(id.uidByte[0]) == card1[0] && int(id.uidByte[1]) == card1[1] && int(id.uidByte[2]) == card1[2] && int(id.uidByte[3]) == card1[3]) {
    Serial.println("card 1 sensed!");
    Serial.println("start the timer 1...");
    Serial.println(now.second());
    timerState_1 = true;
    secondRecord1 = int(now.second());
  }
  if (int(id.uidByte[0]) == card2[0] && int(id.uidByte[1]) == card2[1] && int(id.uidByte[2]) == card2[2] && int(id.uidByte[3]) == card2[3]) {
    Serial.println("card 2 sensed!");
    Serial.println("start the timer 2...");   
    Serial.println(now.second());
    timerState_2 = true;
    //record the current time
    secondRecord2 = int(now.second());
  }
  if (int(id.uidByte[0]) == card3[0] && int(id.uidByte[1]) == card3[1] && int(id.uidByte[2]) == card3[2] && int(id.uidByte[3]) == card3[3]) {
    Serial.println("card 3 sensed!");
    Serial.println("start the timer 3...");
    Serial.println(now.second());
    timerState_3 = true;
    //record the current time
    secondRecord3 = int(now.second());
  }

  //print the new car on serial monitor with its hex value.
  Serial.print("NewCard ");
  PrintHex(id.uidByte, id.size);
  Serial.println("");


  //**RFID********************************************************************
  //set a while loop. in which
  //we set the LED to high and then
  //continuously check the card status
  while (true) {
    
    currentSecond = rtc.now().second();//now.second();
    
    digitalWrite(Green, HIGH);

    control = 0;
    for (int i = 0; i < 3; i++) {
      if (!mfrc522.PICC_IsNewCardPresent()) {
        if (mfrc522.PICC_ReadCardSerial()) {
          control |= 0x16;
        }
        if (mfrc522.PICC_ReadCardSerial()) {
          control |= 0x16;
        }
        control += 0x1;
      }
      control += 0x4;
    }
    //Serial.println(control);

    //**RFID******
    //while the card is on reader, we set the control flag to 13 or 14.
    if (control == 13 || control == 14) {
      //card is still there
      
      //if card is still there and the timer is on
      if (timerState_1 == true) {
        //check if time is up=======================
        
        if ((currentSecond - secondRecord1 == 5) ) {//set the timer as 5s for testing.
          Serial.println("time's up! turn off the timer 1!");
          digitalWrite(timerLedPin1, HIGH);
        }
      } else {
        digitalWrite(timerLedPin1, LOW);
        secondRecord1 = 0;
      }

      if (timerState_2 == true) {
        //check if time is up=======================
        if ((currentSecond - secondRecord2 == 6) ) {//set the timer as 10s for testing.
          Serial.println("time's up! turn off the timer 2!");
          digitalWrite(timerLedPin2, HIGH);
        }
      } else {
        digitalWrite(timerLedPin2, LOW);
        secondRecord2 = 0;
      }

      if (timerState_3 == true) {
        //check if time is up=======================
        if ((currentSecond - secondRecord3 == 4) ) {//set the timer as 10s for testing.
          Serial.println("time's up! turn off the timer 3!");
          digitalWrite(timerLedPin3, HIGH);
        }
      } else {
        digitalWrite(timerLedPin3, LOW);
        secondRecord3 = 0;
      }

    } else {
      //ad soon as the card is removed, there will be no value on serial
      //and control flag values.
      //so get out of the loop.
      break;

      //********************************
      //break the timer.reset the timer.
      if (timerState_1 == true) { //if the timer is on
        Serial.println("just stopped the timer 1!");
        digitalWrite(timerLedPin1, LOW);
        timerState_1 = false;
      }
      if (timerState_2 == true) { //if the timer is on
        Serial.println("just stopped the timer 2!");
        digitalWrite(timerLedPin2, LOW);
        timerState_2 = false;
      }
      if (timerState_3 == true) { //if the timer is on
        Serial.println("just stopped the timer 3!");
        digitalWrite(timerLedPin3, LOW);
        timerState_3 = false;
      }
      //********************************
    }
  }

  //***RFID remove. ************************************************************
  //after getting out of the loop
  //we turn off the LED, and the print card removed.
  Serial.println("CardRemoved");
  digitalWrite(Green, LOW);
  digitalWrite(timerLedPin1, LOW);
  digitalWrite(timerLedPin2, LOW);
  digitalWrite(timerLedPin3, LOW);

  delay(50); //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

/*
  void printRTCCurTime() {
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  }
*/
