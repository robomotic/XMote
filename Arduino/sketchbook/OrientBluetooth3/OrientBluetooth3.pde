#include <ADXL345.h>
#include <HMC58X3.h>
#include <ITG3200.h>
#include <bma180.h>

#define DEBUG
#ifdef DEBUG
#include "DebugUtils.h"
#endif

#include "CommunicationUtils.h"
#include "FreeIMU.h"
#include <Wire.h>
#define BAUDRATE 57600
#define FASTMODE false

#define FIRMWARE "1.0"
#define PRODUCER "Robomotic ltd"
#define IMUV "FreeIMU v0.3.1"
#define FREQ "8 MHz"

float q[4];
int raw_values[9];
//char str[512];
float ypr[3]; // yaw pitch roll
char str[512];
float val[9];
enum FSM {
  INIT,
  RUN,
  PAUSE
};

enum MODE {
  RAW,
  QUATERNION,
  YPR,
  FIRMWAREV
};

//The initial state
FSM state=INIT;
//The initial mode
MODE output=RAW;
// Set the FreeIMU object
FreeIMU my3IMU = FreeIMU();
//The command from the PC
char cmd;

void setup() 
{ 
  Serial.begin(BAUDRATE); 
  Wire.begin();
  //initialise the IMU
  setupIMU();
  //initial status
  state=INIT;
  //initial mode
  output=RAW;
  //initialise the bluetooth module
  setupBlueToothConnection();
  
} 
void setupIMU()
{
  delay(5);
  my3IMU.init(FASTMODE);
  delay(5);  
  
} 
void loop() 
{ 
 
    if(Serial.available())
    {

      cmd = Serial.read();
      if(cmd=='a') state=RUN;
      else if(cmd=='p') state=PAUSE;
      else if(cmd=='r') output=RAW;
      else if(cmd=='y') output=YPR;
      else if(cmd=='q') output=QUATERNION;
      else if(cmd=='f') output=FIRMWAREV;
      
    }
    
    if(state==RUN)
    {
      switch(output)
      {
       case RAW:
            my3IMU.getRawValues(raw_values);
            sprintf(str, "%d,%d,%d,%d,%d,%d,%d,%d,%d,", raw_values[0], raw_values[1], raw_values[2], raw_values[3], raw_values[4], raw_values[5], raw_values[6], raw_values[7], raw_values[8]); 
            Serial.print(str);
            Serial.print(10, BYTE);

       break; 
       case YPR:
            my3IMU.getYawPitchRoll(ypr);
            Serial.print("Yaw: ");
            Serial.print(ypr[0]);
            Serial.print(" Pitch: ");
            Serial.print(ypr[1]);
            Serial.print(" Roll: ");
            Serial.print(ypr[2]);
            Serial.println("");
       break;
       case QUATERNION:
             my3IMU.getQ(q);
             serialPrintFloatArr(q, 4);
             Serial.println("");
             delay(10); 
       break;
       case FIRMWAREV:
            sprintf(str, "Software: %s, FCPU:%s, FIRMWARE: %s, IMU: %s", PRODUCER, FREQ,FIRMWARE,IMUV); 
            Serial.print(str);
            Serial.print(10, BYTE);
            state=PAUSE;
       break;
       default: 
       
       break;
       
       
      }
    }
 
} 
 
 
void setupBlueToothConnection()
{
    // use the baud rate your bluetooth module is configured to 
    // not all baud rates are working well, i.e. ATMEGA168 works best with 57600
 

    delay(1000);
    sendBlueToothCommand("\r\n+STWMOD=0\r\n");
    sendBlueToothCommand("\r\n+STNA=RoboBlue\r\n");
    sendBlueToothCommand("\r\n+STAUTO=0\r\n");
    sendBlueToothCommand("\r\n+STOAUT=1\r\n");
    sendBlueToothCommand("\r\n+STPIN=0000\r\n");
    delay(2000); // This delay is required.
    sendBlueToothCommand("\r\n+INQ=1\r\n");
    delay(2000); // This delay is required.
}


//Checks if the response "OK" is received
void CheckOK()
{
  char a,b;
  while(1)
  {
    if(Serial.available())
    {

    a = Serial.read();

    if(a=='O')
    {
      // Wait for next character K. available() is required in some cases, as K is not immediately available.
      while( (b = Serial.read()) != -1)
      {
        if(b=='K')
        {
        break;
        }
      }
      break;
    }
   }
  }

}
 
void sendBlueToothCommand(char command[])
{
    Serial.print(command);
     
    CheckOK();   
}

