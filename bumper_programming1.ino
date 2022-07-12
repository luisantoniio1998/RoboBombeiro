// This example demonstrates programming a bumper sensor. 
// The switch button circuit includes a pull-up resistor 
// that has been activated by software.

#define BUMPER_PIN 2

void setup()
{
  pinMode(BUMPER_PIN, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop()
{
  int buttonState = digitalRead(BUMPER_PIN);
  
  Serial.println(buttonState);

  delay(50);
}