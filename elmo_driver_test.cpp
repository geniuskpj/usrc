#include <iostream>
#include <libpcan.h>
#include <unistd.h>
#include <fcntl.h>
#include "Device.h"
#include "ElmoDriver.h"
#include <stdio.h>
#include "Comm.h"
#include <boost/lockfree/queue.hpp>
#include <native/task.h>
#include <native/timer.h>
#include <rtdm/rtdm.h>
#include <cmath>

using namespace std;

pthread_t receiveThreadHandler;
pthread_t sendThreadHandler;

RT_TASK ctrlThreadHandler;

void CtrlThread(void* );

extern MailBox mailBox;
extern boost::lockfree::queue<TPCANMsg> CAN_Write_Queue;



const DWORD elmoID = 100;
const DWORD imuID = 0x0E8;

bool isMotorOn = false;

ElmoDriver elmoDriver(elmoID);
IMU imu(imuID);
//AbsoluteEncoder absoluteEncoder(0x14c);

void StartThreads(HANDLE handle);


int main()
{
	HANDLE handle;


	try 
	{

		handle = LINUX_CAN_Open("/dev/pcan1", O_RDWR);
		if (handle == NULL)
			throw DeviceException();

		unsigned int returnValue;

		returnValue = CAN_Init(handle, 0x001C, CAN_INIT_TYPE_ST); 

		if (returnValue != CAN_ERR_OK)
			throw DeviceException();

		imu.SetHandle(handle);

		elmoDriver.SetHandle(handle);

		StartThreads(handle);


		//for(;;)
		//	sleep(5);
		char k;

		elmoDriver.ResetComm();

		usleep(10000);
		sleep(1);

		cout << "Reset Comm done" << endl;

		elmoDriver.ResetNode();
		
		sleep(5);

		cout << "Reset Node Done...Press any key" << endl;

		cin >> k;

		elmoDriver.GotoOpMode();

		cout << "In Op Mode" << endl;

		sleep(1);

//		cin >> k;

		elmoDriver.MotorOn();

		sleep(1);

		isMotorOn = true;

		//elmoDriver.TCEqualOne();


		//elmoDriver.PA();

		//elmoDriver.BG();

		sleep(1);

		//elmoDriver.PR((float) 270.0);

		//elmoDriver.BG();

		cout << "Press a key " << endl;

		cin >> k;
		
		elmoDriver.MotorOff();


		for(;;)
		{


			sleep(1);

			//cout << "I am main thread" << endl;

			return 0;

		}

	}


	catch(...)
	{
		cout << "Error " << endl;
	}
	


}


void StartThreads(HANDLE handle)
{

	int tID;

	tID = pthread_create(&receiveThreadHandler, NULL, ReceiveThread0, (void *) handle);
	if(tID < 0)
		throw DeviceException(); 

	tID = pthread_create(&sendThreadHandler, NULL, SendThread, (void *) handle);
	if (tID < 0)
		throw DeviceException(); 

//	return;
	if(rt_task_create(&ctrlThreadHandler, "TEST", 0, 99, 0) != 0)
		throw DeviceException();

	if(rt_task_start(&ctrlThreadHandler, &CtrlThread, NULL) != 0)
		throw DeviceException();

}

void* ReceiveThread0(void *param)
{
	HANDLE handle = (HANDLE) param;
	TPCANMsg msg;
	MailItem mailItem, temp;

	while(1)
	{
		if (CAN_Read(handle, &msg) != CAN_ERR_OK)
		{
			cout << "Can read error" << endl;
			continue;
		}

		mailItem.id = msg.ID;
		mailItem.dataLength = msg.LEN;
		for(int i=0; i<msg.LEN; i++)
			mailItem.data[i] = msg.DATA[i];
		mailItem.status = mailBox.GetMail(msg.ID, temp) ? 'O' : 'N';

		mailBox.AddMail(mailItem);
	}
}

void* SendThread(void* param)
{
	HANDLE handle = (HANDLE) param;
	static TPCANMsg msg;


	while(1)
	{
	//	cout << "I am sending thread !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;

		if (!CAN_Write_Queue.pop(msg))
			continue;

		if (CAN_Write(handle, &msg) != CAN_ERR_OK)
		{
			cout << "Can write error" << endl;
			continue;
		}

	}

}

void CtrlThread(void *param)
{

	int pos, deg;
	float degFromPos;
	float roll, pitch, yaw;

	static int k=0;
	MailItem mailItem1, mailItem2;


	if (rt_task_set_periodic(NULL, TM_NOW, 5000000)!=0)
	{
		cout << "Error in Xenomai init" << endl;
		return;
	}
	float previousRoll = 0.0;
	float motorRoll;
	const float threshold = 0.2;
	for(;;)
	{
		if(rt_task_wait_period(NULL)!=0)
		{
			cout << "Error in Xenomai loop" << endl;
			return;
		}


		if (imu.RequestData() != true)
		{	cout << "no data" << endl;
			continue;
		}

		//cout << "next step" << k++ << endl;


		usleep(1000);

		elmoDriver.UMEqualFive();


		if(mailBox.GetMail(imu.id+1, mailItem1) && mailBox.GetMail(imu.id+2, mailItem2))
		{

			imu.GetValues(mailItem1.data, mailItem2.data, roll, pitch, yaw);
			
			if (isMotorOn)
			{
				printf("Roll: %.2f, Pitch: %.2f, Yaw: %.2f\n", roll, pitch, yaw);


				motorRoll = roll;
				//if (abs((float)(roll-previousRoll)) < threshold)
				//	motorRoll = previousRoll;

//				motorRoll = -0.63;
/*
				if (motorRoll >= previousRoll)
					elmoDriver.TCEqualZero();
				else 
					elmoDriver.TCEqualZero();
*/

				//if (motorRoll != previousRoll)
				{
	//			if(motorRoll != previousRoll)
	//				elmoDriver.TCEqualOne();

/*
	static int k=0;
	float pos=0;
	//while(k!=4000){
		pos=180*sin(2*3.141592*k/100);
		elmoDriver.PA(pos);
		k++;
*/
	//}		   				
							
	//elmoDriver.PA();	


				elmoDriver.PA((float)(motorRoll*1.0));
				elmoDriver.BG();


				}
				//isMotorOn = false;
				//previousRoll = motorRoll;

			}


		}

		//else cout << "did not get" << endl;

	}

}
