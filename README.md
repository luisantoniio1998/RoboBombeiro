# Firefighter robot

My Robotics course project: an Arduino-controlled robot that navigates, finds a flame, and uses a fan to put it out.

The main sketch, `RB_2022_Final.ino`, uses a state machine for waiting, navigation, centring on the flame, and extinguishing it. The repo also contains the motor class and separate sketches for testing sensors and actuators.

## Hardware and libraries

The main sketch uses motors, ultrasonic sensors, bumpers, a flame sensor, an OLED display, and a fan. Pin assignments are at the top of the sketch.

Its includes list the required libraries: `Adafruit_GFX`, `Adafruit_SH1106`, `FlameSensor`, `NewPing`, and `Wire`, plus the local `Motor` class.

## Working with the code

Open the sketch in Arduino IDE, install the matching libraries, and check the wiring against the pin definitions before uploading. The test sketches let you check individual components before running the navigation code.
