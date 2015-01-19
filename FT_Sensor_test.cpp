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

using namespace std;

pthread_t receiveThreadHandler;
pthread_t sendThreadHandler;

RT_TASK ctrlThreadHandler;

void CtrlThread(void* );

extern MailBox mailBox;
extern boost::lockfree::queue<TPCANMsg> CAN_Write_Queue;

//pthread_t receiveThreadHandler2;
//pthread_t receiveThreadHandler0;


const DWORD id = 432;
FT_Sensor ftSensor(id);

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


		ftSensor.SetHandle(handle);



		StartThreads(handle);


		for(;;)
		{


			sleep(5);

			cout << "I am main thread" << endl;



		}

	}
/*

	catch(DeveceException E)
	{
		cout << "Device Error Caught" << endl;
	}
*/
	catch(DeviceException E)	
	{
		cout << "Devic Error " << endl;
	}
	catch (...)
	{	
		cout << "Other Error" << endl; 	
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

	float Fx, Fy, Fz, Tx, Ty, Tz;

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

		if (ftSensor.RequestData() != true)
		{	cout << "no data" << endl;
			continue;
		}

		cout << "next step" << k++ << endl;


		usleep(1000);


		if(mailBox.GetMail(ftSensor.id+5, mailItem1) && mailBox.GetMail(ftSensor.id+6, mailItem2))
		{

			ftSensor.GetValues(mailItem1.data, mailItem2.data, Fx, Fy, Fz, Tx, Ty, Tz);

			printf("Fx: %.2f Fy: %.2f Fz: %.2f  Tx: %.2f Ty: %.2f Tz: %.2f \n", Fx, Fy, Fz, Tx, Ty, Tz);

			//cout << "got it" << endl;


		}

		else cout << "did not get" << endl;

	}

}
