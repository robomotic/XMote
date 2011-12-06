/*
  BluetoothBee.h - Arduino Library for the BluetoothBee
  Copyright (c) 2011 Robomotic ltd.  All right reserved.
  
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

  Acknowledgements:
  This library is based on the MeetAndroid library from the Amarino team. 
  The original library has been modified to address the 
  specific demand to communicate with the Amarino toolkit.
  
*/

#ifndef BluetoothBee_h
#define BluetoothBee_h

#include <inttypes.h>
#include "Print.h"


/******************************************************************************
* Definitions
******************************************************************************/

class BluetoothBee : public Print

{
#define ByteBufferLenght 64
#define PinLength 4
#define NameLength 8
#define _BLUETOOTH_BEE_VERSION 1 // software version of this library
private:
	// per object data
	uint8_t bufferCount;
	char buffer[ByteBufferLenght];

	int numberOfValues;
	
	char separator;
	char cr;
	char lf;
	char plus; // the '+' prefix for each ATI command
	
	bool customErrorFunc;

	// private methods
	void processCommand(void);
	void init(void);
	int getArrayLength();

public: 
	// public methods
	BluetoothBee(void);
	
	void flush(void);
	bool receive(void);
	int bufferLength(){return bufferCount;} // buffer withouth ACK
	int stringLength(){return bufferCount;} // string without flag but '/0' at the end
	void getBuffer(uint8_t[]);
	
	void getString(char[]);
	int getInt();
	long getLong();
	float getFloat();
	double getDouble();
	void getIntValues(int[]);
	void getFloatValues(float[]);
	void getDoubleValues(float[]); // in Arduino double and float are the same
	
	void write(uint8_t);

	bool setSlave();
	bool setMaster();
	bool setPin(char pin[]);
	bool setName(char name[]);
	bool startPairing();
	bool waitForConnection();
	bool receiveLoop();
	bool isConnected;
	void CheckOK();
	bool setAutoConnect(bool);
	bool setPermitPaired(bool);
	uint16_t waitTime;
	
	static int library_version() { 
		return _BLUETOOTH_BEE_VERSION;} 
};

// Arduino 0012 workaround
#undef int
#undef char
#undef long
#undef byte
#undef float
#undef abs
#undef round 

#endif
