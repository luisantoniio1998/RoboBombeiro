#define RMOTOR_ENABLE_PIN 7
#define RMOTOR_DIR1_PIN 6
#define RMOTOR_DIR2_PIN 5

#define LMOTOR_ENABLE_PIN 2
#define LMOTOR_DIR1_PIN 3
#define LMOTOR_DIR2_PIN 4

int power = 0;

void setup() {

  Serial.begin(9600);
  pinMode(RMOTOR_ENABLE_PIN, OUTPUT);
  pinMode(RMOTOR_DIR1_PIN, OUTPUT);
  pinMode(RMOTOR_DIR2_PIN, OUTPUT);

  pinMode(LMOTOR_ENABLE_PIN, OUTPUT);
  pinMode(LMOTOR_DIR1_PIN, OUTPUT);
  pinMode(LMOTOR_DIR2_PIN, OUTPUT);
}

void loop() {
  digitalWrite(RMOTOR_DIR1_PIN, LOW);
  digitalWrite(RMOTOR_DIR2_PIN, HIGH);
  
  //analogWrite(RMOTOR_ENABLE_PIN, power); 
  analogWrite(RMOTOR_ENABLE_PIN, map(power, 1, 16, 90, 255));
  
  digitalWrite(LMOTOR_DIR1_PIN, HIGH);
  digitalWrite(LMOTOR_DIR2_PIN, LOW);
  //analogWrite(LMOTOR_ENABLE_PIN, power); 
  analogWrite(LMOTOR_ENABLE_PIN, map(power, 1, 16, 90, 255));

  Serial.print("Power: ");
  Serial.println(power);
  
  power = power + 1;
  if(power > 16) power = 0;

  delay(500);

}
