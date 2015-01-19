#ifndef ELMO_DRIVER_H
	#define ELMO_DRIVER_H

#include "Device.h"
#include "Comm.h"




class ElmoDriver : public Device
{
public:
	ElmoDriver(DWORD _id);
	virtual bool RequestData();
	virtual ~ElmoDriver();
	bool ResetComm();
	bool ResetNode();
	bool GotoOpMode();	//Start

	bool MotorOn();
	bool MotorOff();
	bool TCEqualOne();
	bool TCEqualZero();
	bool TCEqualMinusOne();
	bool UMEqualOne();
	bool UMEqualFive();
	bool PA(); 	// default +180 degree fron the top
	bool PA(int count);
	bool PA(float degree);
	bool PR();	// default +180 degree from the top
	bool PR(int degree);
	bool PR(float degree);
	bool BG();
    bool FS(float endvel, bool set);
    bool SP(float vel, bool set);
    bool AC(float acc, bool set);
    bool DC(float dcc, bool set);
    bool GetPosition();
	bool GetCurrent();
    bool readintdata(BYTE* data1, int& val);
    bool readfloatdata(BYTE* data1, float& val);
	bool ProfilePositionMode();
	bool InterpolatedPositionMode();
	bool SetIPMode();
	bool SetPeriod();
	bool CW();
	bool iPA(float degree);
	void setDataInt(unsigned char *data, int i);
	void setDataFloat(unsigned char *data, float f);

};

namespace ElmoSpace
{
	struct SharedMemContent
	{
		float position;
		float current;
	
	};
}



#endif 
