
// This example demonstrates the programming of a force sensitive
// resistor type sensor. This type of sensors are connected to 
// analog ports through a voltage divider circuit that allows 
// having an output voltage proportional to the resistance 
// variation obtained from the sensor.

#define FSR_PIN A0

int fsrVal = 0;
int fsrVoltage = 0;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  fsrVal = analogRead(FSR_PIN);
  fsrVoltage = map(fsrVal, 0, 1023, 0, 5000);
  
  Serial.print("SSR: ");
  Serial.print(fsrVal);
  Serial.print(", ");
  Serial.print(fsrVoltage);
  Serial.print("mV ");
  
  if(fsrVal < 100) 
    Serial.println("- No pressure");
  else if(fsrVal < 200)
    Serial.println("- Light touch");
  else if(fsrVal < 500)
    Serial.println("- Light squeeze");
  else if(fsrVal < 800)
    Serial.println("- Medium squeeze");
  else
    Serial.println("- Big squeeze");
  
  delay(50);
}