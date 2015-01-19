#include <iostream>
#include <libpcan.h>
#include <unistd.h>
#include <fcntl.h>
#include "Device.h"
#include <stdio.h>
#include "Comm.h"
#include <boost/lockfree/queue.hpp>
#include <native/task.h>
#include <native/timer.h>
#include <rtdm/rtdm.h>
#include "SharedMemModule.h"

using namespace std;

pthread_t receiveThreadHandler;
pthread_t sendThreadHandler;

SharedMemStruct* pSharedMemStruct;
SharedCommandQueue* pSharedCommandQueue;

RT_TASK ctrlThreadHandler;

void CtrlThread(void* );

bool InitSharedMemory(void);

extern MailBox mailBox;
extern boost::lockfree::queue<TPCANMsg> CAN_Write_Queue;

//pthread_t receiveThreadHandler2;
//pthread_t receiveThreadHandler0;


const DWORD id = 0x0E8;
IMU imu(id);

void StartThreads(HANDLE handle);


int main()
{
	HANDLE handle;

	try 
	{

		handle = LINUX_CAN_Open("/dev/pcanusb1", O_RDWR);
		if (handle == NULL)
			throw DeviceException();

		unsigned int returnValue;

		returnValue = CAN_Init(handle, CAN_BAUD_500K, CAN_INIT_TYPE_ST); 

		if (returnValue != CAN_ERR_OK)
			throw DeviceException();

		if(!InitSharedMemory())

			throw DeviceException();


		imu.SetHandle(handle);

		StartThreads(handle);


		for(;;)
		{


			sleep(5);

			cout << "I am main thread" << endl;
/*


			if (LINUX_CAN_Read_Timeout(handle, &rdMsg, 500000) != CAN_ERR_OK)
				throw DeviceException();



			if (LINUX_CAN_Read_Timeout(handle, &rdMsg, 500000) != CAN_ERR_OK)
				throw DeviceException();

			

			usleep(1000);
			
			if(pMsg->ID == 337 || pMsg->ID == 338)
			{
				pos = (pMsg->DATA[1] << 8) + pMsg->DATA[0];
				deg = (pMsg->DATA[3] << 8) + pMsg->DATA[2];

				degFromPos = (float)pos * 360.0/(1 << 16); 
			}
			sprintf(str, "Length = %d %0x %0x %0x %0x %0x %0x %0x %0x", pMsg->LEN, pMsg->DATA[0], pMsg->DATA[1], pMsg->DATA[2], pMsg->DATA[3], pMsg->DATA[4], pMsg->DATA[5], pMsg->DATA[6], pMsg->DATA[7]);

			

			cout << str << endl;

			printf("Pos: %d, Deg: %d, Degree from pos: %f\n", pos, deg, degFromPos);
*/

		}

	}


	catch(...)
	{
		cout << "Error " << endl;
	}
	


}


void StartThreads(HANDLE handle)
{
	int tID = pthread_create(&receiveThreadHandler, NULL, ReceiveThread0, (void *) handle);
	if(tID < 0)
		throw DeviceException(); 

	tID = pthread_create(&sendThreadHandler, NULL, SendThread, (void *) handle);
	if (tID < 0)
		throw DeviceException(); 


	if(rt_task_create(&ctrlThreadHandler, "TEST", 0, 99, 0) != 0)
		throw DeviceException();

	if(rt_task_start(&ctrlThreadHandler, &CtrlThread, NULL) != 0)
		throw DeviceException();

}

/*
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

		mailItem.canID = msg.ID;
		if(HAVE_COMMAND_ID(msg.ID))
		{
			mailItem.commandID[0] = msg.DATA[0];
			mailItem.commandID[1] = msg.DATA[1];
		}
		else 
			mailItem.commandID[0] = mailItem.commandID[1] = 0;

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
*/
void CtrlThread(void *param)
{

	int pos, deg;
	float roll, pitch, yaw;
	float degFromPos;

	static int k=0;
	MailItem mailItem1, mailItem2;


	if (rt_task_set_periodic(NULL, TM_NOW, 100000000)!=0)
	{
		cout << "Error in Xenomai init" << endl;
		return;
	}

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

		cout << "next step" << k++ << endl;


		usleep(1000);


		if(mailBox.GetMail(imu.id+1, mailItem1) && mailBox.GetMail(imu.id+2, mailItem2))
		{

			imu.GetValues(mailItem1.data, mailItem2.data, roll, pitch, yaw);

			pSharedMemStruct->imu_Content.roll = roll;

			printf("Roll: %.2f, Pitch: %.2f, Yaw: %.2f\n", roll, pitch, yaw);

			//cout << "got it" << endl;


		}

		else cout << "did not get" << endl;

	}

}


bool InitSharedMemory(void)
{


	if(!InitSharedMemModule(0))
		return false;

	pSharedMemStruct->imu_Content.roll = 0.0;
	pSharedMemStruct->imu_Content.pitch = 0.0;
	pSharedMemStruct->imu_Content.yaw = 0.0;

	return true;

}
