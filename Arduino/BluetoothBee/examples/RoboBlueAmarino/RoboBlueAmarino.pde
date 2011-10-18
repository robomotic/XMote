/*
  Receives Test Events from your phone.
  After it gets a test message the led 13 will blink
  for one second.
*/
 
#include <MeetAndroid.h>
#include <BluetoothBee.h>
#define BAUDRATE 57600
MeetAndroid meetAndroid;
int onboardLed = 13;
BluetoothBee btconnection;
void setup()  
{
    pinMode(onboardLed, OUTPUT);

  Serial.begin(BAUDRATE); 
  // use the baud rate your bluetooth module is configured to 
  // not all baud rates are working well, i.e. ATMEGA168 works best with 57600
  //setupBlueToothConnection();
  
  if(btconnection.startPairing())
  {
    while(btconnection.waitForConnection())
    {
        flushLed(300);
        //waiting for the user to put PIN on the mobile phone
    }
  }//pairing didn't work
  else flushLed(300);

  // register callback functions, which will be called when an associated event occurs.
  // - the first parameter is the name of your function (see below)
  // - match the second parameter ('A', 'B', 'a', etc...) with the flag on your Android application
  meetAndroid.registerFunction(testEvent, 'A');  



}

void loop()
{
  if(btconnection.isConnected)
  meetAndroid.receive(); // you need to keep this in your loop() to receive events
}

void flushLed(int time)
{
  digitalWrite(onboardLed, LOW);
  delay(time);
  digitalWrite(onboardLed, HIGH);
  delay(time);
  digitalWrite(onboardLed, LOW);
  delay(time);
}
/*
 * This method is called constantly.
 * note: flag is in this case 'A' and numOfValues is 0 (since test event doesn't send any data)
 */
void testEvent(byte flag, byte numOfValues)
{
  flushLed(300);
  flushLed(300);
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

