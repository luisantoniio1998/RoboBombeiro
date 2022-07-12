
#include <Servo.h>

#define POT_PIN A0
#define SERVO_PIN 2

int analogVal;
int voltVal;
int servoPosition;

Servo myServo;

void setup()
{
  Serial.begin(9600);
  myServo.attach(SERVO_PIN);  
}

void loop()
{
  analogVal = analogRead(POT_PIN);
  voltVal = map(analogVal, 0, 1023, 0, 5000);
  // 1023 ---> 5000
  // analogVal -----> x = analogVal * 5000 / 1023;
  
  servoPosition = map(analogVal, 0, 1023, 0, 180);
  // 1023 ---> 180
  // servoPosition -----> x = servoPosition * 180 / 1023;
  
  myServo.write(servoPosition);
  
  Serial.print("Analog: ");
  Serial.print(analogVal);
  Serial.print('\t');
  Serial.print("Volts: ");
  Serial.print(voltVal);
  Serial.print('\t');
  Serial.print("Servo: ");
  Serial.println(servoPosition);
}