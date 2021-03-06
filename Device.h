#ifndef DEVICE_H
#define DEVICE_H

#include <libpcan.h>
#include <exception>

#define HAVE_COMMAND_ID(X)  (X > 5*128)

class Device
{

public:
	DWORD id;
	HANDLE handle;
	Device(DWORD _id);
	void SetHandle(HANDLE _handle);
	virtual bool RequestData()=0;
	virtual ~Device();

};

class AbsoluteEncoder : public Device
{
public:
	AbsoluteEncoder(DWORD _id);
	virtual bool RequestData();
	virtual ~AbsoluteEncoder();
	void GetValues(const BYTE* data, int& pos, int& degree);
	void GetValues(const BYTE* data, int& pos, float& degree);
};

namespace AbsoluteEncoderSpace
{
	struct SharedMemContent
	{
		int position;
		float angle;
	};
}


class IMU : public Device
{
public:
	IMU(DWORD _id);
	virtual bool RequestData();
	virtual ~IMU();
	void GetValues(const BYTE* data1, const BYTE* data2, float& roll, float& pitch, float& yaw);
};

namespace IMU_Space
{
	struct SharedMemContent
	{
		float roll;
		float pitch;
		float yaw;
	
	};
}

class FT_Sensor : public Device
{
	static const float scalingFactorForce = 11598;
//	static const float sf = 11598;
	static const float countPerForce = 100000;
	static const float scalingFactorTorque = 245;
	static const float countPerTorque = 100000;

	float ToRealValue(int value, bool isForce);
public:
	FT_Sensor(DWORD _id);
	virtual ~FT_Sensor();
	virtual bool RequestData();

	void GetValues(BYTE* data1, BYTE* data2, float& Fx, float& Fy, float& Fz, float& Tx, float& Ty, float& Tz);
	
};

namespace FT_Sensor_Space
{
	struct SharedMemContent
	{
		float Fx;
		float Fy;
		float Fz;
		float Tx;
		float Ty;
		float Tz;
	
	};
}

class DeviceException: public std::exception
{
public:
	virtual const char* what() const throw();
};


#endif 
