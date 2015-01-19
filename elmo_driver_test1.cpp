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

using namespace std;

pthread_t receiveThreadHandler;
pthread_t sendThreadHandler;

RT_TASK ctrlThreadHandler;

void CtrlThread(void* );

extern MailBox mailBox;
extern boost::lockfree::queue<TPCANMsg> CAN_Write_Queue;



const DWORD id = 100;

ElmoDriver elmoDriver(id);
AbsoluteEncoder absoluteEncoder(0x14c);

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

		elmoDriver.SetHandle(handle);

		StartThreads(handle);


		static TPCANMsg msg;
		static TPCANRdMsg msg_R;

/*
		msg.ID = 0;
		msg.LEN = 2;
		msg.DATA[0] = 130;
		msg.DATA[1] = 100;
		returnValue = LINUX_CAN_Write_Timeout(handle, &msg, 500000);

		if (returnValue != CAN_ERR_OK)
			cout << "Error " << endl;

		returnValue = LINUX_CAN_Read_Timeout(handle, &msg_R, 500000);

		if (returnValue != CAN_ERR_OK)
			cout << "Error" << endl;

*/	
		elmoDriver.ResetComm();
		
		usleep(10000);
/*
		msg.ID = 0;
		msg.LEN = 2;
		msg.DATA[0] = 129;
		msg.DATA[1] = 100;
		returnValue = LINUX_CAN_Write_Timeout(handle, &msg, 50000);

		if (returnValue != CAN_ERR_OK)
			cout << "Error in writing" << endl;

		returnValue = LINUX_CAN_Read_Timeout(handle, &msg_R, 5000000);

		if(returnValue != CAN_ERR_OK)
			cout << "Error in reading" << endl;


//		elmoDriver.ResetNode();

		char k;
		//usleep(10000);
		cout << "Comm reset done"<< endl;

		cin >> k;	
	

		msg.ID = 0;
		msg.LEN = 2;
		msg.DATA[0] = 1;
		msg.DATA[1] = 100;
		returnValue = LINUX_CAN_Write_Timeout(handle, &msg, 500000);

		if(returnValue != CAN_ERR_OK)
			cout << "Error in op write" << endl;

		returnValue = LINUX_CAN_Read_Timeout(handle, &msg_R, 500000);

		if(returnValue != CAN_ERR_OK)
			cout << "Error in readinng op" << endl;

		cout << "Node reset done "<< endl;
		cin >> k;
//		elmoDriver.GotoOpMode();

//		usleep(10000);

		msg.ID = 6*128+100;
		msg.LEN = 8;
		msg.DATA[0] = 'M';
		msg.DATA[1] = 'O';

		msg.DATA[2] = 0;
		msg.DATA[3] = 0;

		msg.DATA[4] = 1;
		msg.DATA[5] = 0;
		msg.DATA[6] = 0;
		msg.DATA[7] = 0;


		LINUX_CAN_Write_Timeout(handle, &msg, 500000);

		//LINUX_CAN_Read_Timeout(handle, &msg_R, 500000);


//		elmoDriver.MotorOn();

		usleep(10000);

		float temp_c = 1.0;
		char current[4];
		memcpy(current, &temp_c, sizeof(float));

		msg.ID = 6*128+100;
		msg.LEN = 8;
		msg.DATA[0] = 'T';
		msg.DATA[1] = 'C';

		msg.DATA[2] = 0x0;
		msg.DATA[3] = 128;

		msg.DATA[4] = current[0];
		msg.DATA[5] = current[1];
		msg.DATA[6] = current[2];
		msg.DATA[7] = current[3];


		LINUX_CAN_Write_Timeout(handle, &msg, 500000);


//		elmoDriver.TCEqualOne();

//		sleep(5);

		cout << "press any key" << endl;
		cin >> k;

		elmoDriver.MotorOff();

		cout << "press a key one more time" << endl;		

		cin >> k;
		msg.ID = 6*128+100;
		msg.LEN = 8;
		msg.DATA[0] = 'M';
		msg.DATA[1] = 'O';

		msg.DATA[2] = 0;
		msg.DATA[3] = 0;

		msg.DATA[4] = 0;
		msg.DATA[5] = 0;
		msg.DATA[6] = 0;
		msg.DATA[7] = 0;


		LINUX_CAN_Write_Timeout(handle, &msg, 500000);



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

	int tID;
/*
	tID = pthread_create(&receiveThreadHandler, NULL, ReceiveThread0, (void *) handle);
	if(tID < 0)
		throw DeviceException(); 
*/
	tID = pthread_create(&sendThreadHandler, NULL, SendThread, (void *) handle);
	if (tID < 0)
		throw DeviceException(); 

	return;
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

	static int k=0;
	MailItem mailItem;


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

		if (absoluteEncoder.RequestData() != true)
		{	cout << "no data" << endl;
			continue;
		}

		cout << "next step" << k++ << endl;


		usleep(1000);


		if(mailBox.GetMail(absoluteEncoder.id+5, mailItem))
		{
			//pos = (mailItem.data[1] << 8) + mailItem.data[0];
			//deg = (mailItem.data[3] << 8) + mailItem.data[2];

			//degFromPos = (float)pos * 360.0/(1 << 16);
			absoluteEncoder.GetValues(mailItem.data, pos, degFromPos);
			//absoluteEn

			printf("Pos: %d, Degree from pos: %f\n", pos, degFromPos);
			//cout << "got it" << endl;


		}

		else cout << "did not get" << endl;

	}

}
