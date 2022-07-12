
#define D1_PIN 8
#define D2_PIN 9
#define SPEED_PIN 10

#define POT_PIN A0
#define BUTTON_DIR_PIN 6
#define BUTTON_STOP_PIN 5

int val = 0;
bool dir = true; 
bool stop = false;

void setup()
{
  pinMode(D1_PIN, OUTPUT);
  pinMode(D1_PIN, OUTPUT);
  pinMode(SPEED_PIN, OUTPUT);
  
  pinMode(BUTTON_DIR_PIN, INPUT_PULLUP); 
  pinMode(BUTTON_STOP_PIN, INPUT_PULLUP); 
}

void loop()
{
  // Transform pot value into speed value.
  val = analogRead(POT_PIN);  // [0 - 1023]
  val = map(val, 0, 1023, 0, 255);  
  //val = val / 4;  
  analogWrite(SPEED_PIN, val); // [0 - 255]
  
  
  // Check buttons.
  if(digitalRead(BUTTON_DIR_PIN) == LOW) {
     dir = !dir;
      
    while(digitalRead(BUTTON_DIR_PIN) == LOW);
  }
  
  if(digitalRead(BUTTON_STOP_PIN) == LOW) {
     stop = !stop;
      
     while(digitalRead(BUTTON_STOP_PIN) == LOW);
  }
  
  // Stop or change direction in function of the buttons.  
  if(!stop) {
    if(dir) {
      digitalWrite(D1_PIN, HIGH);
      digitalWrite(D2_PIN, LOW);
    }  
    else {  
      digitalWrite(D1_PIN, LOW);
      digitalWrite(D2_PIN, HIGH);
    }  
  }
  else {
      digitalWrite(D1_PIN, HIGH);
      digitalWrite(D2_PIN, HIGH);
  }
}