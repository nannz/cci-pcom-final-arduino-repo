//consider the switch as the conductive ink 
//the count-down feature
//1st switch - water the plant every day (daily)
//2nd switch - water the plant once every 3 days
//3rd switch - water the plant once every 7 days (weekly)



/**********************************/
const int switchPin1 = 12; //the switch connect to pin 12
const int ledPin = 3;//the led connect to pin 6
/**********************************/
int switchState1 = 0;         // variable for reading the pushbutton status

void setup()
{
  pinMode(switchPin1, INPUT); //initialize thebuttonPin as input
  pinMode(ledPin, OUTPUT); //initialize the led pin as output
  Serial.begin(9600);
}
/**********************************/
void loop()
{
  //read the state of the switch value
  switchState1 = digitalRead(switchPin1);
//  Serial.println(switchState1);
  if (switchState1 == HIGH ) //if it is,the state is HIGH
  {
    digitalWrite(ledPin, HIGH); //turn the led on
  }
  else
  {
    digitalWrite(ledPin, LOW); //turn the led off
  }
}
