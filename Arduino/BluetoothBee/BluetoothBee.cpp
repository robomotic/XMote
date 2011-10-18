/*
  BluetoothBee.cpp - Arduino Library for Amarino
  Copyright (c) 2009 Bonifaz Kaufmann.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

// Includes
#include "WProgram.h"
#include "HardwareSerial.h"
#include "WConstants.h"
#include "BluetoothBee.h"

extern "C" {
#include <stdlib.h>
}



// Private methods


void BluetoothBee::init()
{
	waitTime = 5000;
	separator=':';
	cr='\r';
	lf='\n';
	plus='+'; // the '+' prefix for each ATI command
	isConnected=false;
	numberOfValues = 0;
}


// public methods
BluetoothBee::BluetoothBee()
{
    // it is hard to use member function pointer together with normal function pointers.
    init();
}

bool BluetoothBee::setSlave()
{
  Serial.print("\r\n+STWMOD=0\r\n");  
}
	
	
bool BluetoothBee::setMaster()
{
  Serial.print("\r\n+STWMOD=1\r\n");  
}
	
bool BluetoothBee::setPin(char pin[])
{
	Serial.print("\r\n+STPIN="); 
	//wait for the null char
  	for(int a = 0;a < PinLength;a++){
		
		Serial.print(pin[a]);  
	}
	Serial.print("\r\n");  
}

bool BluetoothBee::setName(char name[])
{
	Serial.print("\r\n+STNA="); 
	//wait for the null char
  	for(int a = 0;a < NameLength;a++){
		
		if(name[a]=='\0') break;
		Serial.print(name[a]);  
	}
	Serial.print("\r\n");  
  
}


bool BluetoothBee::startPairing()
{
Serial.print("\r\n+INQ=1\r\n"); 
flush(); 
receive();
if (strncmp(buffer, "OK",2)  == 0)  
{ 
  flush();  
  receive();
  if (strncmp(buffer, "+BTSTATE:2",10)  == 0)    {
    flush();   
    isConnected=true;
    return true;
   } else return false;
} else return false;
}

bool BluetoothBee::waitForConnection()
{
    receiveLoop();
 
}

bool BluetoothBee::receive(){
	uint8_t lastByte;
	boolean wait_cmd = true;
	uint8_t count_cr=0;
	uint8_t count_lf=0;

	while(wait_cmd)
	{
	if(Serial.available() > 0)
	{
		lastByte = Serial.read();
		
		if(lastByte == cr){
			count_cr=count_cr+1;
		}
		else if(lastByte == lf){
			count_lf=count_lf+1;
			//skip initial line feed!
			if(count_cr+count_lf==2 && bufferCount==0 )
			{
				count_cr=0;
				count_lf=0;				
			}
			if(count_cr+count_lf>=4)
			{
			  wait_cmd=true;
			  break;
			}
		}
		else if(bufferCount < ByteBufferLenght){
			buffer[bufferCount] = lastByte;
			bufferCount++;
		}
		else return false;
	}
	}
	return true;

}

bool BluetoothBee::receiveLoop(){
	uint8_t lastByte;
	boolean timeout = false;
	uint8_t count_cr=0;
	uint8_t count_lf=0;
	while(!timeout)
	{
		if(Serial.available() > 0)
		{
			lastByte = Serial.read();
			
			if(lastByte == cr){
				count_cr=count_cr+1;
			}
			else if(lastByte == lf){
				count_lf=count_lf+1;
				if(count_cr+count_lf==2)
				{
				  if (strncmp(buffer, "+BTSTATE:4",10)  == 0)  // test to see if the two strings are equal
				  { 
  					
				    flush();  
				    return false;
				  }

				}//skip this command
				else if (count_cr+count_lf==4)
				{
				    flush(); 
				    count_cr=0;
				    count_lf=0;				  
				}
			}
			else if(bufferCount < ByteBufferLenght){
				buffer[bufferCount] = lastByte;
				bufferCount++;
			}
			else return false;
		}
		/*
		if(Serial.available() <= 0 && !timeout){
			if(waitTime > 0) delay(waitTime);
			if(Serial.available() <= 0) timeout = true;
		}
		*/
	}
	return timeout;

}


void BluetoothBee::getBuffer(uint8_t buf[]){

	for(int a = 0;a < bufferCount;a++){
		buf[a] = buffer[a];
	}
}

void BluetoothBee::getString(char string[]){

	for(int a = 1;a < bufferCount;a++){
		string[a-1] = buffer[a];
	}
	string[bufferCount-1] = '\0';
}

int BluetoothBee::getInt()
{
	uint8_t b[bufferCount];
	for(int a = 1;a < bufferCount;a++){
		b[a-1] = buffer[a];
	}

	b[bufferCount-1] = '\0';
	return atoi((char*)b);
}

long BluetoothBee::getLong()
{
	uint8_t b[bufferCount];
	for(int a = 1;a < bufferCount;a++){
		b[a-1] = buffer[a];
	}

	b[bufferCount-1] = '\0';
	return atol((char*)b);
}

float BluetoothBee::getFloat()
{
	return (float)getDouble();
}

int BluetoothBee::getArrayLength()
{
	if (bufferCount == 1) return 0; // only a flag and ack was sent, not data attached
	numberOfValues = 1;
	// find the amount of values we got
	for (int a=1; a<bufferCount;a++){
		if (buffer[a]==separator) numberOfValues++;
	}
	return numberOfValues;
}

void BluetoothBee::getFloatValues(float values[])
{
	int t = 0; // counter for each char based array
	int pos = 0;

	int start = 1; // start of first value
	for (int end=1; end<bufferCount;end++){
		// find end of value
		if (buffer[end]==separator) {
			// now we know start and end of a value
			char b[(end-start)+1]; // create container for one value plus '\0'
			t = 0;
			for(int i = start;i < end;i++){
				b[t++] = (char)buffer[i];
			}
			b[t] = '\0';
			values[pos++] = atof(b);
			start = end+1;
		}
	}
	// get the last value
	char b[(bufferCount-start)+1]; // create container for one value plus '\0'
	t = 0;
	for(int i = start;i < bufferCount;i++){
		b[t++] = (char)buffer[i];
	}
	b[t] = '\0';
	values[pos] = atof(b);
}

// not tested yet
void BluetoothBee::getDoubleValues(float values[])
{
	getFloatValues(values);
}

// not tested yet
void BluetoothBee::getIntValues(int values[])
{
	int t = 0; // counter for each char based array
	int pos = 0;

	int start = 1; // start of first value
	for (int end=1; end<bufferCount;end++){
		// find end of value
		if (buffer[end]==separator) {
			// now we know start and end of a value
			char b[(end-start)+1]; // create container for one value plus '\0'
			t = 0;
			for(int i = start;i < end;i++){
				b[t++] = (char)buffer[i];
			}
			b[t] = '\0';
			values[pos++] = atoi(b);
			start = end+1;
		}
	}
	// get the last value
	char b[(bufferCount-start)+1]; // create container for one value plus '\0'
	t = 0;
	for(int i = start;i < bufferCount;i++){
		b[t++] = (char)buffer[i];
	}
	b[t] = '\0';
	values[pos] = atoi(b);
}


double BluetoothBee::getDouble()
{
	char b[bufferCount];
	for(int a = 1;a < bufferCount;a++){
		b[a-1] = (char)buffer[a];
	}

	b[bufferCount-1] = '\0';
	return atof(b);
	
}


void BluetoothBee::write(uint8_t b){
	Serial.print(b);
}

void BluetoothBee::flush(){
	for(uint8_t a=0; a < ByteBufferLenght; a++){
		buffer[a] = 0;
	}
	bufferCount = 0;
	numberOfValues = 0;
}
