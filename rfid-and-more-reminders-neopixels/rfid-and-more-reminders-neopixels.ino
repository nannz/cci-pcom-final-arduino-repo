//used board Arduino UNO
//doesn't work on Arduino nano 33 BLE

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
int card1[] = {138, 116, 177, 18};//1 min
int card2[] = {186, 68, 13, 17};
int card3[] = {138, 153, 182, 18};


//=======NEOPIXEL========
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#define PIN        7//pin 7 on the Arduino is connected to the NeoPixels
#define NUMPIXELS 5
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

//green 0, 150, 0
//yellow 155,155,0
//red 155,0,0
//blue 0,0,155
int neoPixel1 = 0;
int neoPixel2 = 2;
int neoPixel3 = 4;



void setup() {
  pinMode(Green, OUTPUT);//sensing if the card is present or absense.
  Serial.begin(9600);                                           // Initialize serial communications with the PC
  SPI.begin();                                                  // Init SPI bus
  mfrc522.PCD_Init();

  pinMode(timerLedPin1,OUTPUT);
  pinMode(timerLedPin2,OUTPUT);
  pinMode(timerLedPin3,OUTPUT);

  pixels.begin();
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
  pixels.clear();
  //pixels.setPixelColor(0, pixels.Color(155,155,0));
  //pixels.show();  
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
    Serial.println("start the timer 1...");
    timerState_1 = true;
    //record the current time
    //hourRecord = hour();
    //minuteRecord = minute();
    secondRecord1 = second();
  }
   if (int(id.uidByte[0]) == card2[0] && int(id.uidByte[1]) == card2[1] && int(id.uidByte[2]) == card2[2] && int(id.uidByte[3]) == card2[3]) {
    Serial.println("card 2 sensed!");
    Serial.println("start the timer 2...");
    timerState_2 = true;
    //record the current time
    secondRecord2 = second();
  }
  if (int(id.uidByte[0]) == card3[0] && int(id.uidByte[1]) == card3[1] && int(id.uidByte[2]) == card3[2] && int(id.uidByte[3]) == card3[3]) {
    Serial.println("card 3 sensed!");
    Serial.println("start the timer 3...");
    timerState_3 = true;
    //record the current time
    secondRecord3 = second();
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
        //check if time is up=======================
        if ((second() - secondRecord1 == 5) ) {//set the timer as 5s for testing. 
           Serial.println("time's up! turn off the timer 1!");
           digitalWrite(timerLedPin1,HIGH);
           pixels.setPixelColor(neoPixel1, pixels.Color(155,0,0));
           pixels.show();  
        }
      } else {
        digitalWrite(timerLedPin1,LOW);
        secondRecord1 = 0;
        pixels.setPixelColor(neoPixel1, pixels.Color(0,0,0));
        pixels.show(); 
      }
      
      if (timerState_2 == true) {
        //check if time is up=======================
        if ((second() - secondRecord2 == 6) ) {//set the timer as 10s for testing. 
          Serial.println("time's up! turn off the timer 2!");
           digitalWrite(timerLedPin2,HIGH);
           pixels.setPixelColor(neoPixel2, pixels.Color(155,155,0));
           pixels.show();  
        }
      } else {
        digitalWrite(timerLedPin2,LOW);
        secondRecord2 = 0;
        pixels.setPixelColor(neoPixel2, pixels.Color(0,0,0));
        pixels.show(); 
      }
      
      if (timerState_3 == true) {
        //check if time is up=======================
        if ((second() - secondRecord3 == 5) ) {//set the timer as 10s for testing. 
          Serial.println("time's up! turn off the timer 3!");
           digitalWrite(timerLedPin3,HIGH);
           pixels.setPixelColor(neoPixel3, pixels.Color(0,0,155));
           pixels.show(); 
        }
      } else {
        digitalWrite(timerLedPin3,LOW);
        secondRecord3 = 0;
        pixels.setPixelColor(neoPixel3, pixels.Color(0,0,0));
        pixels.show(); 
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
        pixels.setPixelColor(neoPixel1, pixels.Color(0,0,0));
        pixels.show(); 
      }
       if (timerState_2 == true) { //if the timer is on
        Serial.println("just stopped the timer 2!");
        digitalWrite(timerLedPin2,LOW);
        timerState_2 = false;
        pixels.setPixelColor(neoPixel2, pixels.Color(0,0,0));
        pixels.show();
      }
      if (timerState_3 == true) { //if the timer is on
        Serial.println("just stopped the timer 3!");
        digitalWrite(timerLedPin3,LOW);
        timerState_3 = false;
        pixels.setPixelColor(neoPixel3, pixels.Color(0,0,0));
        pixels.show(); 
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
  digitalWrite(timerLedPin2,LOW);
  digitalWrite(timerLedPin3,LOW);
  pixels.setPixelColor(neoPixel1, pixels.Color(0,0,0));
  pixels.setPixelColor(neoPixel2, pixels.Color(0,0,0));
  pixels.setPixelColor(neoPixel3, pixels.Color(0,0,0));
  pixels.show(); 
  delay(50); //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();


}
