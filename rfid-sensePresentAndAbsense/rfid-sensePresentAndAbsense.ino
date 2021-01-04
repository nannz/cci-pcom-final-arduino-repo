
//tutorial:https://highvoltages.co/tutorial/arduino-tutorial/arduino-mfrc522-tutorial-is-rfid-tag-present-or-removed/
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9
#define SS_PIN          10
#define Green 7

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

void setup() {
  pinMode(Green, OUTPUT);
  Serial.begin(9600);                                           // Initialize serial communications with the PC
  SPI.begin();                                                  // Init SPI bus
  mfrc522.PCD_Init();

}

//virables for storing RFID tag values and flags
//to check if the card is present or not. 
uint8_t buf[10] = {};
MFRC522::Uid id;
MFRC522::Uid id2;
bool is_card_present = false;
uint8_t control = 0x00;


void PrintHex(uint8_t *data, uint8_t length) // prints 8-bit data in hex with leading zeroes
{
  char tmp[16];
  for (int i = 0; i < length; i++) {
    sprintf(tmp, "0x%.2X", data[i]);
    Serial.print(tmp); Serial.print(" ");
  }
}

//*****************************************************************************************//
//copying the id of the tag
void cpid(MFRC522::Uid *id) {
  memset(id, 0, sizeof(MFRC522::Uid));
  memcpy(id->uidByte, mfrc522.uid.uidByte, mfrc522.uid.size);
  id->size = mfrc522.uid.size;
  id->sak = mfrc522.uid.sak;
}

//*****************************************************************************************//
void loop() {
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
  cpid(&id);
  //preint the new car on serial monitor with its hex value. 
  Serial.print("NewCard ");
  PrintHex(id.uidByte, id.size);
  Serial.println("");

  //a while loop. in which
  //we set the LED to high and then 
  //continuously check the card status
  while (true) {
    digitalWrite(Green, HIGH);
    
    control = 0;
    for (int i = 0; i < 3; i++) {
      if (!mfrc522.PICC_IsNewCardPresent()) {
        if (mfrc522.PICC_ReadCardSerial()) {
          //Serial.print('a');
          control |= 0x16;
        }
        if (mfrc522.PICC_ReadCardSerial()) {
          //Serial.print('b');
          control |= 0x16;
        }
        //Serial.print('c');
        control += 0x1;
      }
      //Serial.print('d');
      control += 0x4;
    }
    Serial.println(control);

    
    //while the card is on reader, we set the control flag to 13 or 14. 
    if (control == 13 || control == 14) {
      //card is still there
    } else {
      //ad soon as the card is removed, there will be no value on serial
      //and control flag values. 
      //so get out of the loop. 
      break;
    }
  }

  //after getting out of the loop
  //we turn off the LED, and the print card removed. 
  Serial.println("CardRemoved");
  digitalWrite(Green, LOW);
  delay(500); //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

}
