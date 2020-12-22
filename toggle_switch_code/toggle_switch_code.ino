int switch_pin = 4;
int led_pin = 5;

byte leds = 0;

void setup() {
  pinMode(switch_pin, INPUT);
  pinMode(led_pin, OUTPUT);
}

void loop() {
  if(digitalRead(switch_pin) == HIGH){
    digitalWrite(led_pin, LOW);
  }
  if(digitalRead(switch_pin) == LOW){
    digitalWrite(led_pin, HIGH);
  }

}