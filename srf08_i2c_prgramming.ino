#include <Wire.h>

// The difference between the first two (" vs. <) has to do with where the include files are looked
// for. One indicates that the include file is local (") while the other indicates that the include 
// file is in a standard location (<,>).

// 0xE6 >> 1 = 0x73
// 0xE6 = 11100110
// 0xE6 >> 1 = 01110011 = 0x73
#define SRF_ADDRESS  0x73                           // Address of the SRF08 (0x70 = 0xE0 >> 1) Arduino automatically controls R/W lower bit
#define CMD_REGISTER 0x00                           // Command byte, values of 0 being sent with write have to be masked as a byte to stop them being misinterpreted as NULL this is a bug with arduino 1.0
#define SOFT_REGISTER 0x00                          // Register location to read software revision
#define LIGHT_REGISTER 0x01                         // Register location to read light sensor
#define RANGE_REGISTER 0x02                         // Register to write command to start a read

byte highByte = 0x00;                               // Stores high byte from ranging
byte lowByte = 0x00;                                // Stored low byte from ranging

void setup(){
  Serial.begin(9600);
  Wire.begin();                               
  delay(100);                                       // Waits to make sure everything is powered up before sending or receiving data
  
  int softRev = getSoft();                          // Calls function to get software revision
  Serial.print("SRF08 Soft Revision: ");  
  Serial.println(softRev);
}

void loop(){
  
  int rangeData = getRange();                       // Calls a function to get range
  int lightData = getLight();                       // Call function to get light reading and store in lightData

   Serial.print("Range: ");
   Serial.print(rangeData);
   Serial.print('\t');
   Serial.print("Light: ");
   Serial.println(lightData);
   delay(100);                                      // Wait before looping
}

int getRange(){                                     // This function gets a ranging from the SRF08
  int range = 0; 
  
  Wire.beginTransmission(SRF_ADDRESS);              // Start communticating with sensor
  Wire.write(CMD_REGISTER);                                  // Send Command register address
  Wire.write(0x51);                                 // Send 0x51 comand byte
  Wire.endTransmission();
  
  delay(100);                                       // Wait for ranging to be complete
  
  Wire.beginTransmission(SRF_ADDRESS);              // Start communicating with sensor
  Wire.write(RANGE_REGISTER);                       // Call the register for start of ranging data
  Wire.endTransmission();
  
  Wire.requestFrom(SRF_ADDRESS, 2);                 // Request 2 bytes from sensor
  while(Wire.available() < 2);                      // Wait for data to arrive
  highByte = Wire.read();                           // Get high byte
  lowByte = Wire.read();                            // Get low byte

  range = (highByte << 8) + lowByte;                // Put them together
  
  return(range);                                    // Returns Range
}


byte getByte(int add, int reg) {
  Wire.beginTransmission(add);
  Wire.write(reg);                           
  Wire.endTransmission();
  
  Wire.requestFrom(add, 1);                
  while(Wire.available() < 0);             
  byte b = Wire.read();                    
    
  return(b);                               
}

int getLight(){                                    // Function to get light reading
  /*
  Wire.beginTransmission(SRF_ADDRESS);
  Wire.write(LIGHT_REGISTER);                      // Call register to get light reading
  Wire.endTransmission();
  
  Wire.requestFrom(SRF_ADDRESS, 1);                // Request 1 byte
  while(Wire.available() < 0);                     // While byte available
  int lightRead = Wire.read();                     // Get light reading
    
  return(lightRead);                               // Returns lightRead
  */
  int light = getByte(SRF_ADDRESS, LIGHT_REGISTER);
  return(light);
}

int getSoft(){                                     // Function to get software revision
  /*
  Wire.beginTransmission(SRF_ADDRESS);             // Begin communication with the SRF module
  Wire.write(SOFT_REGISTER);                       // Sends the command bit, when this bit is read it returns the software revision
  Wire.endTransmission();
  
  Wire.requestFrom(SRF_ADDRESS, 1);                // Request 1 byte
  while(Wire.available() < 0);                     // While byte available
  int software = Wire.read();                      // Get byte
    
  return(software);                               
  */
  int soft = getByte(SRF_ADDRESS, SOFT_REGISTER);
  return(soft);
}
