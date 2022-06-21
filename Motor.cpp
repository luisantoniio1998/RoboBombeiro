#include "Motor.h"

Motor::Motor(int enablePin, int dir1Pin, int dir2Pin, int minRange, int maxRange, int maxPower) {
   mEnablePin = enablePin;
   mDir1Pin = dir1Pin;
   mDir2Pin = dir2Pin;
   mMinRange = minRange;
   mMaxRange = maxRange;
   mMaxPower = maxPower;
}

void Motor::setPower(int power) {
  if(power == 0) {
    stop();
    return;
  }

  if(power > 0) {
    digitalWrite(mDir1Pin, LOW);
    digitalWrite(mDir2Pin, HIGH);
  }
  else {
    digitalWrite(mDir1Pin, HIGH);
    digitalWrite(mDir2Pin, LOW);
  }

  analogWrite(mEnablePin, map(abs(power), 1, mMaxPower, mMinRange, mMaxRange));  
}

void Motor::stop() {
  digitalWrite(mEnablePin, LOW);  
}

void Motor::brake() {
  digitalWrite(mDir1Pin, LOW);  
  digitalWrite(mDir2Pin, LOW);  
}
