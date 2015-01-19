#include "Device.h"
#include <stddef.h>
#include <iostream>
#include <boost/lockfree/queue.hpp>
using namespace std;

extern boost::lockfree::queue<TPCANMsg> CAN_Write_Queue;

Device::Device(DWORD _id)
	: id(_id)
	, handle(NULL)
{

}

void Device::SetHandle(HANDLE _handle)
{

	handle = _handle;
}

Device::~Device()
{

}

AbsoluteEncoder::AbsoluteEncoder(DWORD _id)
	: Device(_id)
{

}

AbsoluteEncoder::~AbsoluteEncoder()
{
}


//TPCANMsg message;
bool AbsoluteEncoder::RequestData()
{
	static TPCANMsg message;
	message.ID = id;
	message.LEN = 1;
	message.DATA[0] = 2;
//	for (int i=0; i<8; i++)
//		message.DATA[i] = (i==0 ? 0x02 : 0); 

	if (!handle)
		return CAN_ERRMASK_ILLHANDLE;

	//return CAN_Write(handle, &message);
	
	return CAN_Write_Queue.push(message);

}

void AbsoluteEncoder::GetValues(const BYTE* data, int& position, int& degree)
{
	position = (data[1] << 8) + data[0];
    degree = ((data[3] << 8) + data[2]);
}

void AbsoluteEncoder::GetValues(const BYTE* data, int& position, float& degree)
{
	int pos, deg;
	GetValues(data, pos, deg);
	position = pos;
	degree = (float)pos * 360.0/(1 << 16);
}
IMU::IMU(DWORD _id)
	: Device(_id)
{
}

IMU::~IMU()
{
	
}

bool IMU::RequestData()
{
	static TPCANMsg message;
	message.ID = id;
	message.LEN = 2;
	message.DATA[0] = 0x01;
	message.DATA[1] = 0;

	if(!handle)
		return CAN_ERRMASK_ILLHANDLE;

	return CAN_Write_Queue.push(message);
}

void IMU::GetValues(const BYTE* data1, const BYTE* data2, float& roll, float& pitch, float& yaw)
{
	BYTE temp[4];
	int i;

	for(i=0; i<4; i++)
		temp[i] = data1[i];

	memcpy(&roll, temp, 4);

	for(i=0; i<4; i++)
		temp[i] = data1[4+i];

	memcpy(&pitch, temp, 4);

	for(i=0; i<4; i++)
		temp[i] = data2[i];

	memcpy(&yaw, temp, 4);

}

FT_Sensor::FT_Sensor(DWORD _id)
	: Device(_id)
{

}

FT_Sensor::~FT_Sensor()
{

}

bool FT_Sensor::RequestData()
{
	static TPCANMsg message;
	message.ID = id;
	message.LEN = 1;
	message.DATA[0] = 0x02;

	if (!handle)
		return CAN_ERRMASK_ILLHANDLE;

	return CAN_Write_Queue.push(message);
}

float FT_Sensor::ToRealValue(int value, bool isForce)
{
	if(value & 0x8000)
		value |= 0xFFFF0000;

	float realValue = (float) value;
	realValue = realValue*(isForce ? scalingFactorForce : scalingFactorTorque)/(isForce ? countPerForce : countPerTorque);

	return realValue;
}
void FT_Sensor::GetValues(BYTE* data1, BYTE* data2, float& Fx, float& Fy, float& Fz, float& Tx, float& Ty, float& Tz)
{
	int fx, fy, fz, tx, ty, tz;

	fx = data1[1];
	fx = (fx << 8) + data1[0];

	fy = data1[5];
	fy = (fy << 8) + data1[4];

	fz = data2[1];
	fz = (fz << 8) + data2[0];
	
	tx = data1[3];
	tx = (tx << 8) + data1[2];

	ty = data1[7];
	ty = (ty << 8) + data1[6];

	tz = data2[3];
	tz = (tz << 8) + data2[2];

	Fx = ToRealValue(fx, true);
	Fy = ToRealValue(fy, true);
	Fz = ToRealValue(fz, true);

	Tx = ToRealValue(tx, false);
	Ty = ToRealValue(ty, false);
	Tz = ToRealValue(tz, false);

//	cout << "fx: " << fx << endl;

} 

const char* DeviceException::what() const throw()
{
	return "Device Error";

}



