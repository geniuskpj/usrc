#include "ElmoDriver.h"
#include <boost/lockfree/queue.hpp>

using namespace std;

extern boost::lockfree::queue<TPCANMsg> CAN_Write_Queue;
int gear_m = 100;

ElmoDriver::ElmoDriver(DWORD _id)
	: Device(_id)
{

}

ElmoDriver::~ElmoDriver()
{

}

bool ElmoDriver::RequestData()
{
	return true;
}

bool ElmoDriver::ResetComm()
{

	static TPCANMsg message;

	message.ID = 0;
	message.LEN = 2;
	message.DATA[0] = 130;
	message.DATA[1] = id;

	return CAN_Write_Queue.push(message);
	

}

bool ElmoDriver::ResetNode()
{
	static TPCANMsg message;

	message.ID = 0;
	message.LEN = 2;

	message.DATA[0] = 129;
	message.DATA[1] = id;


	cout << "in reset nod id = " << id << endl;


	return CAN_Write_Queue.push(message);

	
}

bool ElmoDriver::GotoOpMode()
{

	static TPCANMsg message;

	message.ID = 0;
	message.LEN = 2;

	message.DATA[0] = 1;
	message.DATA[1] = 0;

	return CAN_Write_Queue.push(message);
	
}

bool ElmoDriver::MotorOn()
{
	static TPCANMsg message;

	message.ID = 6*128+id;
	message.LEN = 8;

	message.DATA[0] = 'M';
	message.DATA[1] = 'O';

	//for(int i=2; i<8; i++)
	//	message.DATA[i] = (i == 4 ? 1 : 0);

	message.DATA[2] = 0;
	message.DATA[3] = 0;

	message.DATA[4] = 1;
	message.DATA[5] = 0;
	message.DATA[6] = 0;
	message.DATA[7] = 0;


	return CAN_Write_Queue.push(message);
	
}

bool ElmoDriver::MotorOff()
{
	static TPCANMsg message;

	message.ID = 6*128+id;
	message.LEN = 8;

	message.DATA[0] = 'M';
	message.DATA[1] = 'O';

	for(int i=2; i<8; i++)
		message.DATA[i] = 0;


	return CAN_Write_Queue.push(message);
	
}

bool ElmoDriver::TCEqualOne()
{

	static TPCANMsg message;

	message.ID = 6*128+id;
	message.LEN = 8;

	float temp = 0.5;

	BYTE current[4];

	memcpy(current, &temp, sizeof(float));

	message.DATA[0] = 'T';
	message.DATA[1] = 'C';
	message.DATA[2] = 0x0;
	message.DATA[3] = 128;

	//for(int i=0; i<3; i++)
	//	message.DATA[4+i] = current[i];

	message.DATA[4] = current[0];
	message.DATA[5] = current[1];
	message.DATA[6] = current[2];
	message.DATA[7] = current[3];

	return CAN_Write_Queue.push(message);

}


bool ElmoDriver::TCEqualZero()
{

	static TPCANMsg message;

	message.ID = 6*128+id;
	message.LEN = 8;

	float temp = 0.0;

	BYTE current[4];

	memcpy(current, &temp, sizeof(float));

	message.DATA[0] = 'T';
	message.DATA[1] = 'C';
	message.DATA[2] = 0x0;
	message.DATA[3] = 128;

	for(int i=0; i<3; i++)
		message.DATA[4+i] = current[i];

	return CAN_Write_Queue.push(message);

}

bool ElmoDriver::TCEqualMinusOne()
{

	static TPCANMsg message;

	message.ID = 6*128+id;
	message.LEN = 8;

	float temp = -0.5;

	BYTE current[4];

	memcpy(current, &temp, sizeof(float));

	message.DATA[0] = 'T';
	message.DATA[1] = 'C';
	message.DATA[2] = 0x0;
	message.DATA[3] = 128;

	//for(int i=0; i<3; i++)
	//	message.DATA[4+i] = current[i];

	message.DATA[4] = current[0];
	message.DATA[5] = current[1];
	message.DATA[6] = current[2];
	message.DATA[7] = current[3];

	return CAN_Write_Queue.push(message);

}

bool ElmoDriver::UMEqualOne()
{
	static TPCANMsg message;
	
	message.ID = 6*128+id;
	message.LEN = 8;

	message.DATA[0] = 'U';
	message.DATA[1] = 'M';

	for(int i=2; i<8; i++)
		message.DATA[i] = (i == 4 ? 1 : 0);

	return CAN_Write_Queue.push(message);

 
}

bool ElmoDriver::UMEqualFive()
{
	static TPCANMsg message;
	
	message.ID = 6*128+id;
	message.LEN = 8;

	message.DATA[0] = 'U';
	message.DATA[1] = 'M';

	for(int i=2; i<8; i++)
		message.DATA[i] = (i == 4 ? 5 : 0);
    return CAN_Write_Queue.push(message);

}

bool ElmoDriver::PA()
{
	static TPCANMsg message;

	message.ID = 6*128+id;
	message.LEN = 8;

	int temp = 2000;
	BYTE value[4];

	memcpy(value, &temp, sizeof(int));

	message.DATA[0] = 'P';
	message.DATA[1] = 'A';

	message.DATA[2] = 0;
	message.DATA[3] = 0;

	for(int i=0; i<4; i++)
		message.DATA[4+i] = value[i];

	return CAN_Write_Queue.push(message);
 
}

bool ElmoDriver::PA(float degree)
{
	static TPCANMsg message;

	message.ID = 6*128+id;
	message.LEN = 8;

    int temp = (int) (2000.0*degree/180.0*gear_m);
	//cout << "angle = " << temp << endl; 
	BYTE value[4];

	memcpy(value, &temp, sizeof(int));

	message.DATA[0] = 'P';
	message.DATA[1] = 'A';

	message.DATA[2] = 0;
	message.DATA[3] = 0;

	for(int i=0; i<4; i++)
		message.DATA[4+i] = value[i];

	return CAN_Write_Queue.push(message);
 
}

bool ElmoDriver::PR()
{
	static TPCANMsg message;

	message.ID = 6*128+id;
	message.LEN = 8;

	int temp = 2000;
	BYTE value[4];

	memcpy(value, &temp, sizeof(int));

	message.DATA[0] = 'P';
	message.DATA[1] = 'R';

	message.DATA[2] = 0;
	message.DATA[3] = 0;

	for(int i=0; i<4; i++)
		message.DATA[4+i] = value[i];

	return CAN_Write_Queue.push(message);
 
}

bool ElmoDriver::PA(int count)
{
//	float deg = (float) degree;
//	return PA(deg);

	static TPCANMsg message;

	message.ID = 6*128+id;
	message.LEN = 8;

	int temp = count;
	//cout << "angle = " << temp << endl; 
	BYTE value[4];

	memcpy(value, &temp, sizeof(int));

	message.DATA[0] = 'P';
	message.DATA[1] = 'A';

	message.DATA[2] = 0;
	message.DATA[3] = 0;

	for(int i=0; i<4; i++)
		message.DATA[4+i] = value[i];

	return CAN_Write_Queue.push(message);
}

bool ElmoDriver::PR(float degree)
{
	static TPCANMsg message;

	message.ID = 6*128+id;
	message.LEN = 8;

    int temp = (int) (2000.0*degree/180.0*gear_m);
	BYTE value[4];

	memcpy(value, &temp, sizeof(int));

	message.DATA[0] = 'P';
	message.DATA[1] = 'R';

	message.DATA[2] = 0;
	message.DATA[3] = 0;

	for(int i=0; i<4; i++)
		message.DATA[4+i] = value[i];

	return CAN_Write_Queue.push(message);
 
}

bool ElmoDriver::PR(int degree)
{
	float deg = (float) degree;
	return PR(deg);
}


bool ElmoDriver::BG()
{
	static TPCANMsg message;

	message.ID = 6*128+id;
	message.LEN = 8;

	message.DATA[0] = 'B';
	message.DATA[1] = 'G';

	for(int i=2; i<4; i++)
		message.DATA[i] = (i==4 ? 1 : 0);

	return CAN_Write_Queue.push(message);
 
}


//kpj edit

bool ElmoDriver::FS(float endvel, bool set)
{
	static TPCANMsg message;

    message.ID = 6*128+id;
    if (set == true)
    {
        message.LEN = 8;
        int temp = (int) (endvel*gear_m);
        BYTE value[4];
        memcpy(value, &temp, sizeof(int));
        for(int i=0; i<4; i++)
        message.DATA[4+i] = value[i];
    }
    else
    {
        message.LEN = 4;
    }


	message.DATA[0] = 'F';
	message.DATA[1] = 'S';

	message.DATA[2] = 0;
	message.DATA[3] = 0;


	return CAN_Write_Queue.push(message);
 
}

bool ElmoDriver::SP(float vel, bool set)
{
	static TPCANMsg message;

    message.ID = 6*128+id;
    if (set == true)
    {
        message.LEN = 8;
        int temp = (int) (vel*gear_m);
        BYTE value[4];
        memcpy(value, &temp, sizeof(int));
        for(int i=0; i<4; i++)
        message.DATA[4+i] = value[i];
    }
    else
    {
        message.LEN = 4;
    }


	message.DATA[0] = 'S';
	message.DATA[1] = 'P';

	message.DATA[2] = 0;
	message.DATA[3] = 0;


	return CAN_Write_Queue.push(message);
 
}

bool ElmoDriver::AC(float acc, bool set)
{
	static TPCANMsg message;

	message.ID = 6*128+id;
    if (set == true)
    {
        message.LEN = 8;
        int temp = (int) (acc*gear_m);
        BYTE value[4];
        memcpy(value, &temp, sizeof(int));
        for(int i=0; i<4; i++)
        message.DATA[4+i] = value[i];
    }
    else
    {
        message.LEN = 4;
    }


	message.DATA[0] = 'A';
	message.DATA[1] = 'C';

	message.DATA[2] = 0;
	message.DATA[3] = 0;

	return CAN_Write_Queue.push(message);
 
}

bool ElmoDriver::DC(float dcc, bool set)
{
	static TPCANMsg message;

	message.ID = 6*128+id;
    if (set == true)
    {
        message.LEN = 8;
        int temp = (int) (dcc*gear_m);
        BYTE value[4];
        memcpy(value, &temp, sizeof(int));
        for(int i=0; i<4; i++)
        message.DATA[4+i] = value[i];
    }
    else
    {
        message.LEN = 4;
    }




    message.DATA[0] = 'D';
    message.DATA[1] = 'C';

    message.DATA[2] = 0;
    message.DATA[3] = 0;



	return CAN_Write_Queue.push(message);
 
}

bool ElmoDriver::GetPosition()
{

    static TPCANMsg message;
    message.ID = 6*128+id;
    message.LEN = 4;
    message.DATA[0] = 'P';
    message.DATA[1] = 'X';
    message.DATA[2] = 0;
    message.DATA[3] = 0;
    return CAN_Write_Queue.push(message);

/*
    int temppos = 0x00;

    if (data1[0]=='P' && data1[1]=='X')
    {
    temppos |= (data1[7] << 24);
    temppos |= (data1[6] << 16);
    temppos |= (data1[5] << 8);
    temppos |= (data1[4]);
    val=temppos;
    return 1;
    }
    val=temppos;
    return 0;
*/
}

bool ElmoDriver::GetCurrent()
{

    static TPCANMsg message;
    message.ID = 6*128+id;
    message.LEN = 4;
    message.DATA[0] = 'I';
    message.DATA[1] = 'Q';
    message.DATA[2] = 0;
    message.DATA[3] = 0;
    return CAN_Write_Queue.push(message);
/*
    int temppos = 0x00;
if (data1[0]=='I' && data1[1]=='Q')
    {
    temppos |= (data1[7] << 24);
    temppos |= (data1[6] << 16);
    temppos |= (data1[5] << 8);
    temppos |= (data1[4]);
    val= *(float *)&temppos;
    return 1;
    }
    val=temppos;
    return 0;
*/
}

bool ElmoDriver::readintdata(BYTE* data1, int& val)
{
    int temppos = 0x00;
    temppos |= (data1[7] << 24);
    temppos |= (data1[6] << 16);
    temppos |= (data1[5] << 8);
    temppos |= (data1[4]);
    val=temppos;
    return 1;
}

bool ElmoDriver::readfloatdata(BYTE* data1, float& val)
{
    int temppos = 0x00;
    temppos |= (data1[7] << 24);
    temppos |= (data1[6] << 16);
    temppos |= (data1[5] << 8);
    temppos |= (data1[4]);
    val= *(float *)&temppos;
    return 1;
}

bool ElmoDriver::ProfilePositionMode()
{
	static TPCANMsg message;
	
	message.ID = 6*128+id;
	message.LEN = 8;

	message.DATA[0] = 'O';
	message.DATA[1] = 'F';
	message.DATA[2] = 0x07;

	for(int i=3; i<8; i++)
		message.DATA[i] = (i == 4 ? 1 : 0);

	return CAN_Write_Queue.push(message);

 
}

bool ElmoDriver::InterpolatedPositionMode()
{
	SetIPMode();
	return SetPeriod();
	 
}

bool ElmoDriver::SetIPMode()
{
	static TPCANMsg message;
	
	message.ID = 6*128+id;
	message.LEN = 8;

	message.DATA[0] = 'O';
	message.DATA[1] = 'F';
	message.DATA[2] = 0x07;

	for(int i=3; i<8; i++)
		message.DATA[i] = (i == 4 ? 0x07 : 0);
	/*
	CAN_Write_Queue.push(message);

	message.DATA[0] = 'O';
	message.DATA[1] = 'V';
	message.DATA[2] = 0x17;

	for(int i=3; i<8; i++)
		message.DATA[i] = (i == 4 ? 0x0A : 0);
	*/
	return CAN_Write_Queue.push(message); 
}

bool ElmoDriver::SetPeriod()
{
	static TPCANMsg message;
	
	message.ID = 6*128+id;
	message.LEN = 8;


	CAN_Write_Queue.push(message);

	message.DATA[0] = 'O';
	message.DATA[1] = 'V';
	message.DATA[2] = 0x17;

	for(int i=3; i<8; i++)
		message.DATA[i] = (i == 4 ? 0x0A : 0);
	
	return CAN_Write_Queue.push(message); 
}

bool ElmoDriver::CW()
{
	static TPCANMsg message;
	
	message.ID = 6*128+id;
	message.LEN = 8;

	message.DATA[0] = 'C';
	message.DATA[1] = 'W';

	for(int i=2; i<8; i++)
		message.DATA[i] = (i == 4 ? 0x1F : 0);

	return CAN_Write_Queue.push(message);

 
}

bool ElmoDriver::iPA(float degree)
{
	static TPCANMsg message;

	message.ID = 6*128+id;
	message.LEN = 8;

    int temp = (int) (2000.0*degree/180.0*gear_m);
	//cout << "angle = " << temp << endl; 
	BYTE value[4];

	memcpy(value, &temp, sizeof(int));

	message.DATA[0] = 'O';
	message.DATA[1] = 'V';

	message.DATA[2] = 0x13;
	message.DATA[3] = 0;

	for(int i=0; i<4; i++)
		message.DATA[4+i] = value[i];

	return CAN_Write_Queue.push(message);
 
}

void setDataInt(unsigned char *data, int i)
{
data[7] = (i >> 24);
data[6] = ((i << 8) >> 24);
data[5] = ((i << 16) >> 24);
data[4] = ((i << 24) >> 24);
}

void setDataFloat(unsigned char *data, float f)
{
int i = *(int *)&f;
data[7] = (i >> 24);
data[6] = ((i << 8) >> 24);
data[5] = ((i << 16) >> 24);
data[4] = ((i << 24) >> 24);
data[3] |= 1 << 7; /* float mode ON */
}
