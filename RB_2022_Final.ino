// ================================================================================
// RB_2022 1.0
// Controller of the kit RB.
//
// Author: Carlos Carreto
// Last revision: 17/06/2022
// ================================================================================


// ================================================================================
// Libraries
// ================================================================================
#include "Motor.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#include <FlameSensor.h>
#include <NewPing.h>
#include <Wire.h>

// ================================================================================
// Pins
// ================================================================================
#define RMOTOR_ENABLE_PIN 7
#define RMOTOR_DIR1_PIN 6
#define RMOTOR_DIR2_PIN 5

#define LMOTOR_ENABLE_PIN 2
#define LMOTOR_DIR1_PIN 3
#define LMOTOR_DIR2_PIN 4

#define LSONAR_PIN 16
#define FSONAR_PIN 15
#define RSONAR_PIN 14

#define LBUMPER_PIN 10
#define RBUMPER_PIN 11

#define LINE_ASENSOR_PIN A0
#define LINE_DSENSOR_PIN 53

#define LED_PIN 13

#define FAN_PIN 9

#define START_BUTTON_PIN 46
#define STOP_BUTTON_PIN 45

// ================================================================================
// Constants
// ================================================================================

// === State Identifiers ===
const int WAIT = 0;
const int NAVIGATE = 1;
const int CENTER = 2;
const int PUT_OUT = 3;
const int TEST = 100;

// === TAG Identifiers ===
const int NO_TAG = 0;
const int LINE_TAG = 1;
const int CIRCLE_TAG = 2;

// === Motor Control Configuration ===
const int MIN_RANGE = 90;  // 0 - 255
const int MAX_RANGE = 200; // 0 - 255
const int MAX_POWER = 32;

const int BASE_POWER = 18;
const int ROTATE_POWER = 12;

const int MAX_SONAR_DIST = 100;
const int MAX_RIGHT_DIST = 20;
const int MAX_FRONT_DIST = 20;
const float GAIN = 1.1;

const int LINE_LIMIT = 80;

const int DELTA_LIMIT = 12;
const float FLOOR1 = 7.78;
const float FLOOR2 = 6.67;

// ================================================================================
// Objects
// ================================================================================
Motor rightMotor(RMOTOR_ENABLE_PIN, RMOTOR_DIR1_PIN, RMOTOR_DIR2_PIN, MIN_RANGE, MAX_RANGE, MAX_POWER);
Motor leftMotor(LMOTOR_ENABLE_PIN, LMOTOR_DIR2_PIN, LMOTOR_DIR1_PIN, MIN_RANGE, MAX_RANGE, MAX_POWER);

Adafruit_SH1106 display(-1); // RESET Pin

NewPing leftSonar(LSONAR_PIN, LSONAR_PIN, MAX_SONAR_DIST);
NewPing frontSonar(FSONAR_PIN, FSONAR_PIN, MAX_SONAR_DIST);
NewPing rightSonar(RSONAR_PIN, RSONAR_PIN, MAX_SONAR_DIST);

FlameSensor flameSensor(3);

// ================================================================================
// Variables
// ================================================================================
int state; // The actual state of the FSM.

int floorTag = NO_TAG;
int room = 0;
bool flameInRoom = false;;

// ================================================================================
// Setup
// ================================================================================
void setup() {
  // === Pin Configuration ===
  pinMode(RMOTOR_ENABLE_PIN, OUTPUT);
  pinMode(RMOTOR_DIR1_PIN, OUTPUT);
  pinMode(RMOTOR_DIR2_PIN, OUTPUT);

  pinMode(LMOTOR_ENABLE_PIN, OUTPUT);
  pinMode(LMOTOR_DIR1_PIN, OUTPUT);
  pinMode(LMOTOR_DIR2_PIN, OUTPUT);

  pinMode(LBUMPER_PIN, INPUT_PULLUP);
  pinMode(RBUMPER_PIN, INPUT_PULLUP);

  pinMode(START_BUTTON_PIN, INPUT_PULLUP);
  pinMode(STOP_BUTTON_PIN, INPUT_PULLUP);

  pinMode(LED_PIN, OUTPUT);

  pinMode(FAN_PIN, OUTPUT);
  turnOFFFan();

  // === OLED Display ===
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.display();

  Wire.begin();
  delay(1000);
  flameSensor.begin();

  Serial.begin(9600);

  state = WAIT;
}

// ================================================================================
// Loop (Implements the Finite State Machine)
// ================================================================================
void loop() {
  switch (state) {
    case WAIT:
      waitState();
      break;
    case NAVIGATE:
      //navigateState();
      navigateState2();
      break;
    case CENTER:
      centerState();
      break;
    case PUT_OUT:
      putOutState();
      break;
    case TEST:
      testState();
      break;
  }

  if (digitalRead(STOP_BUTTON_PIN) == LOW) {
    stop();
    state = WAIT;
  }
}


// ================================================================================
// State Functions
// ================================================================================
void waitState() {
  stop();
  turnOFFLED();
  screen("RB2022 v1.0", "Press START button", "");

  do {
    //hardwareTest();
  } while (digitalRead(START_BUTTON_PIN) == HIGH);

  // Initialize variables
  floorTag = NO_TAG;
  room = 0;
  flameInRoom = false;

  maneuverToCorrectDirection();
  maneuverToExitWhiteCircle();

  state = NAVIGATE;
}

void navigateState() {
  // 1ª Action
  int frontDist = getDistance(frontSonar);
  if (frontDist < MAX_FRONT_DIST) {
    rotateWhile(frontSonar, MAX_FRONT_DIST, true);
  }

  if (digitalRead(LBUMPER_PIN == LOW))
    move(-BASE_POWER, 10, 600);
  else if (digitalRead(RBUMPER_PIN == LOW))
    move(-BASE_POWER, -10, 600);

  int dist = getDistance(rightSonar);

  if (dist < 10) {
    rotateWhile(rightSonar, 10, true);
    dist = getDistance(rightSonar);
  }

  int error = dist - MAX_RIGHT_DIST; // cm
  int delta = GAIN * error + 0.5;

  if (delta > DELTA_LIMIT)
    delta = DELTA_LIMIT;
  else if (delta < -DELTA_LIMIT)
    delta = -DELTA_LIMIT;

  move(BASE_POWER, delta);

  // 2ª
  flameSensor.update();
  if (flameSensor.getFlameDirection() != 0) {
    state = CENTER;
    return;
  }
}

void navigateState2() {
  // 1º Action of the state
  int distF = getDistance(frontSonar);
  if (distF < MAX_FRONT_DIST) {
    rotateWhile(frontSonar, MAX_FRONT_DIST, true);
  }  

  if (digitalRead(LBUMPER_PIN) == LOW)
    move(-BASE_POWER, 10, 600);
  else if (digitalRead(RBUMPER_PIN) == LOW)
    move(-BASE_POWER, -10, 600);

  int dist = getDistance(rightSonar);

  if(dist < 10) {
    rotateWhile(rightSonar, 10, true); 
    dist = getDistance(rightSonar);
  }
 
  int error = dist - MAX_RIGHT_DIST;
  int delta = GAIN * error + 0.5;

  if (delta > DELTA_LIMIT)
    delta = DELTA_LIMIT;
  else if(delta < -DELTA_LIMIT)
    delta = -DELTA_LIMIT;  

  move(BASE_POWER, delta);
  
  screen2(getFloorTagName(floorTag), String(room), "");

  floorTag = getFloorTag();
  if(floorTag == CIRCLE_TAG)
    maneuverToGoToIslandRoom();
  else if (floorTag == LINE_TAG) {
    room = room + 1;
    brake();
    delay(1000);
    flameSensor.update();
    if(flameSensor.getFlameDirection() !=0)  
      flameInRoom = true;
    else
      maneuverToGoToNextRoom();  
  }
  
  // 2º Transition Conditions of the state 
  if(flameInRoom) { 
    flameSensor.update();
    if (flameSensor.getFlameDirection() != 0) {
      state = CENTER;
      return;
    }
  }
}

void centerState() {
  turnONLED();

  int dist = 0;
  flameSensor.update();
  int dir = flameSensor.getFlameDirection();
  switch (dir) {
    case 0:
      //state = NAVIGATE;
      //turnOFFLED();
      stop();
      state = PUT_OUT;
      return;
      break;
    case 1:
      rotate(-5);
      break;
    case 2:
      move(BASE_POWER, 0);
      dist = getDistance(frontSonar);
      if (dist < 15) {
        stop();
        state = PUT_OUT;
        return;
      }
      break;
    case 3:
      rotate(5);
      break;
  }

  screen("Dir: " + String(dir), "Dist: " + String(dist), "");
}

void putOutState() {
  turnONFan();
  delay(3000);
  turnOFFFan();

  move(-BASE_POWER, 0, 600);
  flameSensor.update();
  if (flameSensor.getFlameDirection() != 0) {
    state = CENTER;
    return;
  }

  turnOFFLED();
  state = WAIT;
}

void testState() {

  /*
    leftMotor.setPower(8);
    rightMotor.setPower(8);
    delay(3000);
    leftMotor.brake();
    rightMotor.brake();
    delay(2000);
    leftMotor.setPower(-4);
    rightMotor.setPower(4);
    delay(1000);
    leftMotor.setPower(-2);
    rightMotor.setPower(-2);
    delay(4000);
    leftMotor.stop();
    rightMotor.stop();
  */

  moveForward(8, 3000);
  delay(2000);
  rotate(4, 1000);
  moveForward(-4, 4000);

  moveForward(4);
  delay(3000);
  stop();

  state = WAIT;
}


// ================================================================================
// Actuator Functions
// ================================================================================
void stop() {
  leftMotor.stop();
  rightMotor.stop();
}

void brake() {
  leftMotor.brake();
  rightMotor.brake();
}

void moveForward(int power, int time) {
  leftMotor.setPower(power);
  rightMotor.setPower(power);
  delay(time);
  leftMotor.brake();
  rightMotor.brake();
}

void moveForward(int power) {
  leftMotor.setPower(power);
  rightMotor.setPower(power);
}

void moveForwardDist(int power, int dist) {

}

void rotate(int power, int time) {
  // if power > 0 -> rotate left
  // if power < 0 -> rotate right
  leftMotor.setPower(-power);
  rightMotor.setPower(power);
  delay(time);
  leftMotor.brake();
  rightMotor.brake();
}

void rotate(int power) {
  // if power > 0 -> rotate left
  // if power < 0 -> rotate right
  leftMotor.setPower(-power);
  rightMotor.setPower(power);
}

void rotateAngle(int angle, float floor) {
  // 180 ------> time ms
  // angle ----> x = angle * (time / 180)

  if (angle >= 0)
    rotate(ROTATE_POWER, angle * floor);
  else
    rotate(-ROTATE_POWER, -angle * floor);
}

void rotateWhile(NewPing & sonar, int dist, bool left) {
  if (left == true)
    rotate(ROTATE_POWER);
  else
    rotate(-ROTATE_POWER);
  while (getDistance(sonar) < dist);
}

void move(int power, int delta) {
  leftMotor.setPower(power + delta);
  rightMotor.setPower(power - delta);
}

void move(int power, int delta, int ms) {

  leftMotor.setPower(power + delta);
  rightMotor.setPower(power - delta);
  delay(ms);
  stop();
}

void turnONFan() {
  digitalWrite(FAN_PIN, LOW);
}

void turnOFFFan() {
  digitalWrite(FAN_PIN, HIGH);
}

void maneuverToCorrectDirection() {
  if (getDistance(leftSonar) < 30)
    rotateAngle(-90, FLOOR2);
}

void maneuverToExitWhiteCircle() {
  move(BASE_POWER, 0);
  while (digitalRead(LINE_DSENSOR_PIN) == LOW);
}

void maneuverToGoToIslandRoom() {
  move(-BASE_POWER, 0);
  while (getDistance(leftSonar) > 50);
  rotateAngle(180, FLOOR1);
  //move(BASE_POWER, 0, 2000);
}

void maneuverToGoToNextRoom() {
  switch (room) {
    case 1:
      move(-BASE_POWER, 0, 500);
      rotateAngle(180, FLOOR1);
      move(BASE_POWER, 0);
      while (getDistance(frontSonar) > 15);
      // wait(2000);
      break;
    case 2:
      move(-BASE_POWER, 0, 500);
      rotateAngle(180, FLOOR1);
      move(BASE_POWER, 3, 1000);
      break;
    case 3:
      break;
    case 4:
      break;
  }
}

// ================================================================================
// Sensor Functions
// ================================================================================
int getDistance(NewPing & sonar) {
  int dist = sonar.ping_cm();
  if (dist == 0) dist = MAX_SONAR_DIST;
  return dist;
}

int getFloorTag() {
  if (analogRead(LINE_ASENSOR_PIN) < LINE_LIMIT) {
    move(BASE_POWER, 0, 400);
    if (analogRead(LINE_ASENSOR_PIN) < LINE_LIMIT) {
      return CIRCLE_TAG;
    }
    return LINE_TAG;
  }
  return NO_TAG;
}

// ================================================================================
// Interface Functions
// ================================================================================
void msg(int x, int y, String s, int size) {
  display.clearDisplay();
  display.setTextSize(size);
  //display.setTextColor(WHITE);
  display.setCursor(x, y);
  display.print(s);
  display.display();
}

void msg(int x, int y, String s, int size, bool clear) {
  if (clear) display.clearDisplay();
  display.setTextSize(size);
  //display.setTextColor(WHITE);
  display.setCursor(x, y);
  display.print(s);
  display.display();
}


void screen(String s1, String s2, String s3) {
  display.clearDisplay();
  display.setTextSize(2);
  //display.setTextColor(WHITE);

  display.setCursor(0, 0);
  display.print(getStateName());

  display.setTextSize(1);
  display.setCursor(0, 30);
  display.print(s1);

  display.setCursor(0, 40);
  display.print(s2);

  display.setCursor(0, 50);
  display.print(s3);

  display.display();
}

void screen2(String s1, String s2, String s3) {

  display.clearDisplay();
  display.setTextSize(2);
  //display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print(s1);

  display.setCursor(0, 30);
  display.print(s2);

  display.setCursor(0, 60);
  display.print(s3);

  display.display();
}


void turnONLED() {
  digitalWrite(LED_PIN, HIGH);
}

void turnOFFLED() {
  digitalWrite(LED_PIN, LOW);
}


// ================================================================================
// Utility Functions
// ================================================================================
String getStateName() {
  String s = "";

  switch (state) {
    case WAIT:
      s = "WAIT";
      break;
    case NAVIGATE:
      s = "NAVIGATE";
      break;
    case CENTER:
      s = "CENTER";
      break;
    case PUT_OUT:
      s = "PUT_OUT";
      break;
    case TEST:
      s = "TEST";
      break;
  }
  return s;
}

String getFloorTagName(int tag) {
  String s = "";
  switch (tag) {
    case NO_TAG:
      s = "NO_TAG";
      break;
    case LINE_TAG:
      s = "LINE_TAG";
      break;
    case CIRCLE_TAG:
      s = "CIRCLE_TAG";
      break;
  }
  return s;
}

void hardwareTest() {

  // === SONAR ===
  /*
    Serial.println("Left SONAR: " + String(leftSonar.ping_cm()));
    Serial.println("Front SONAR: " + String(frontSonar.ping_cm()));
    Serial.println("Right SONAR: " + String(rightSonar.ping_cm()));

    Serial.println("Right SONAR: " + String(getDistance(rightSonar)));

    screen("L: " + String(getDistance(leftSonar)), "F: " + String(getDistance(frontSonar)), "R: " + String(getDistance(rightSonar)));
  */

  // === Bumpers ===
  /*
    Serial.print("L: ");
    Serial.print(digitalRead(LBUMPER_PIN));
    Serial.print('\t');
    Serial.print("R: ");
    Serial.println(digitalRead(RBUMPER_PIN));

    String s = "L: " + String(digitalRead(LBUMPER_PIN)) +
    " R: " + String(digitalRead(RBUMPER_PIN));
    msg(0, 0, s, 2);
  */

  // === Line Sensor ===
  /*
    Serial.print("A: ");
    Serial.print(analogRead(LINE_ASENSOR_PIN));
    Serial.print('\t');
    Serial.print("D: ");
    Serial.println(digitalRead(LINE_DSENSOR_PIN));
  */

  /*
    String s1 = "A: " + String(analogRead(LINE_ASENSOR_PIN));
    String s2 = "D: " + String(digitalRead(LINE_DSENSOR_PIN));
    msg(0, 0, s1, 2, true);
    msg(0, 20, s2, 2, false);

    screen(s1, s2, "");
  */

  // === Buttons ===
  /*
    Serial.print("Start: ");
    Serial.print(digitalRead(START_BUTTON_PIN));
    Serial.print('\t');
    Serial.print("Stop: ");
    Serial.println(digitalRead(STOP_BUTTON_PIN));
  */

  // === LED ===
  /*
    digitalWrite(LED_PIN, HIGH);
    delay(2000);
    digitalWrite(LED_PIN, LOW);
    delay(2000);
  */

  // === FAN ===
  /*
    digitalWrite(FAN_PIN, HIGH);
    delay(2000);
    digitalWrite(FAN_PIN, LOW);
    delay(4000);
  */

  // === FlameSensor ===
  flameSensor.update();
  Serial.print("D: ");
  Serial.print(flameSensor.getFlameDirection());
  Serial.print('\t');
  Serial.print("X: ");
  Serial.print(flameSensor.getFlameX());
  Serial.print('\t');
  Serial.print("Y: ");
  Serial.println(flameSensor.getFlameY());
}
