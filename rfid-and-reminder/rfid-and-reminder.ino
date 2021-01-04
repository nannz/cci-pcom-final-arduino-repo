
//tutorial:https://highvoltages.co/tutorial/arduino-tutorial/arduino-mfrc522-tutorial-is-rfid-tag-present-or-removed/
//=======RFID=========
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9
#define SS_PIN          10
#define Green 2 //the green led


MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

//=======TIMER========
#include <TimeLib.h>

#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

/**********************************/

const int timerLedPin1 = 6;//the led connect to pin 6//the red led
boolean ledOn = false;
/**********************************/
int switchState1 = 0;         // variable for reading the pushbutton status
int lastSwitchState1 = 0;

//set timer virables.
int dayRecord = 0;
int monthRecord = 0;
int hourRecord = 0;
int minuteRecord = 0;
int secondRecord = 0;
bool timerState_1 = false; //timer state for card 1


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

  pinMode(timerLedPin1,OUTPUT);
}

//virables for storing RFID tag values and flags
//to check if the card is present or not.
uint8_t buf[10] = {};
MFRC522::Uid id;
MFRC522::Uid id2;
bool is_card_present = false;
uint8_t control = 0x00;


//*****************************************************************************************//
void loop() {


  //========get time =========

  //------GET and PRINT time--------//
  if (Serial.available()) {
    processSyncMessage();
  }
  if (timeStatus() != timeNotSet) {
    //digitalClockDisplay();//display the current time
  }
  if (timeStatus() == timeSet) {
    //Serial.println("synced time yes");
    //digitalWrite(13, HIGH); // LED on if synced time
  } else {
    Serial.println(" time needs refresh");
    //digitalWrite(13, LOW);  // LED off if time needs refresh
  }
  delay(10);


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
    Serial.println("start the timer...");
    timerState_1 = true;
    //record the current time
    hourRecord = hour();
    minuteRecord = minute();
    secondRecord = second();
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
        //check if time is up
        if ((second() - secondRecord == 10) ) {//set the timer as 10s for testing. 
          Serial.println("time's up! turn off the timer 1!");
           digitalWrite(timerLedPin1,HIGH);
        }
      } else {
        digitalWrite(timerLedPin1,LOW);
        minuteRecord = 0;
        secondRecord = 0;
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
        digitalWrite(timerLedPin1,LOW);
        timerState_1 = false;
      }
      //********************************
    }
  }

  //***RFID remove. ************************************************************
  //after getting out of the loop
  //we turn off the LED, and the print card removed.
  Serial.println("CardRemoved");
  digitalWrite(Green, LOW);
  digitalWrite(timerLedPin1,LOW);
  delay(50); //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();


}
