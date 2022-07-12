#define CHANEL_A_PIN 2
#define CHANEL_B_PIN 3

const int COUNTS_PER_REVOLUTION = 1669;  // Encoder output counts per revolution.
const int TIME_INTERVAL = 1000;          // Time interval for measurement.

volatile long encoderCounts = 0;         // Pulse counter.

long previousMillis = 0;  // Counter for milliseconds during interval.
long currentMillis = 0;   // Counter for milliseconds during interval.

float rpm = 0.0;          // Calculated value of rotations per minute.

void setup()
{
  Serial.begin(9600);
  
  // Activate pullup resistors od the input pins.
  pinMode(CHANEL_A_PIN, INPUT_PULLUP);
  pinMode(CHANEL_B_PIN, INPUT_PULLUP);  // CHANEL_B_PIN is not used in this example.

  
  // Attach an interruption in the encoder pin to ensure that all pulses are counted. The pin will be monitored 
  // by the Arduino hardware itself, which automatically executes the code to count pulses ( updateEncoder), 
  // whenever they occur (whenever the encoder signal changes from LOW to HIGH).
  // See documentation about interrupts https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
  attachInterrupt(digitalPinToInterrupt(CHANEL_A_PIN), updateEncoder, RISING);

  // Start counting miliseconds to implement the measurement interval.
  previousMillis = millis();
}

void loop()
{
  // . . .
  
  // Update RPM valeu every TIME_INTERVAL
  currentMillis = millis();
  if(currentMillis - previousMillis > TIME_INTERVAL) {
    previousMillis = currentMillis;
    
	// Calculate the rpm.
	// encoderCounts * 60.0 gives the total counts per minute. By dividing that value by COUNTS_PER_REVOLUTION
	// we get the total of rotations per minute (rpm)
    rpm = encoderCounts * 60.0 / COUNTS_PER_REVOLUTION;
    
    Serial.print("Encoder Counts: ");
    Serial.print(encoderCounts);
    Serial.print('\t');
    Serial.print(" RPM: ");
    Serial.println(rpm);
    
    encoderCounts = 0;
  }
    
  // . . .
}

void updateEncoder() {
  encoderCounts++;
}