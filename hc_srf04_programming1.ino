// Simulation of the control of an HC-SR04 sensor with one pin 
// for the Trigger signal and another pin for the Echo signal.

#define TRIG_PIN 6
#define ECHO_PIN 5


void setup()
{
  Serial.begin(9600);
  
  pinMode(TRIG_PIN, OUTPUT); 
  pinMode(ECHO_PIN, INPUT);
}

void loop()
{
  // Clean the Trigger signal.
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  // Send a trigger pulse with at leas 10us to trigger ranging.
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(15);
  digitalWrite(TRIG_PIN, LOW);
  
  
  // Read the time that the Echo signal remained in the high
  // state and convert that time into centimeters by dividing it
  // by the constant 58.
  int distance = pulseIn(ECHO_PIN, HIGH)/58;
  
   
  // Write the distance value on the serial monitor.
  Serial.println(distance);
  
   
  // Wait 50mS before next ranging.
  delay(50);
  
}