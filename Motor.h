#ifndef MOTOR_H
#define MOTOR_H
#include <Arduino.h>

class Motor {
  private:
    int mEnablePin;
    int mDir1Pin;
    int mDir2Pin;
    int mMinRange;
    int mMaxRange;
    int mMaxPower;
    
  public:
    Motor(int enablePin, int dir1Pin, int dir2Pin, int minRange, int maxRange, int maxPower);
    void setPower(int power);
    void stop();
    void brake();
};

#endif
