/*
This is CAN stuff
*/

#include "CAN.h"

char CAN_Init()
{
	if (!CAN.begin(500E3))
	{
		return ERROR;
	}
}

char CAN_Receive()
{
	int packetSize = CAN.parsePacket();
	//int packetID, requestLength;

	char data;

	if (packetSize)
	{
		//received packet
		if (CAN.available())
		{
			data = CAN.read();
			return data;
		}

	}
}


//byte at a time?
//extended packets?
void CAN_Transmit(char data)
{
	//packet ID is 11 bits, can contain 8bytes of data
	CAN.beginPacket(0x12);
	CAN.write(data);
	CAN.endPacket();
}