#include <iostream>
#include <libpcan.h>
#include <unistd.h>
#include <fcntl.h>
#include "Device.h"
#include <stdio.h>


using namespace std;

pthread_t receiveThreadHandler0;

HANDLE globalHandle;
//pthread_t receiveThreadHandler2;
//pthread_t receiveThreadHandler0;




int StartThread();


int main()
{
	HANDLE handle;

	DWORD id = 0x14C;
	try 
	{
		AbsoluteEncoder absoluteEncoder(id);

		handle = LINUX_CAN_Open("/dev/pcanusb1", O_RDWR);
		if (handle == NULL)
			throw DeviceException();

		unsigned int returnValue;

		returnValue = CAN_Init(handle, CAN_BAUD_500K, CAN_INIT_TYPE_ST); 

		if (returnValue != CAN_ERR_OK)
			throw DeviceException();

		int i, k;
		int pos, deg;
		float degFromPos;

		TPCANRdMsg rdMsg;
		TPCANMsg* pMsg = &rdMsg.Msg;

		TPCANMsg msg;

		MailItem mailItem;

		char str[200];
		char str1[200];

		absoluteEncoder.SetHandle(handle);

		globalHandle = handle;
		StartThread();

		for(i=0; i<5; i++)
		{


			if ((k = absoluteEncoder.RequestData()) != CAN_ERR_OK)
				throw DeviceException();





			usleep(1000);


			if(mailBox(absoluteEncoder.id+5), mailItem)
			{
				pos = (mailItem.data[1] << 8) + mailItem.data[0];
				deg = (mailItem.data[3] << 8) + mailItem.data[2];

				degFromPos = (float)pos * 360.0/(1 << 16);

				printf("Pos: %d, Deg: %d, Degree from pos: %f\n", pos, deg, degFromPos);


			}

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


int StartThread()
{
	int tID = pthread_create(&receiveThreadHandler, NULL ReceiveThread0, NULL);
	if(tID < 0)
		throw DeviceException(); 
}

void* ReceiveThread0(void *param)
{
	HANDLE handle = globalHandle;
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
		mailItem.dataLength = msg.LENGTH;
		for(int i=0; i<msg.LENGTH; i++)
			mailItem.data[i] = msg.DATA[i]
		mailItem.status = mailBox(msg.ID, temp) ? 'O' : 'N';

		mailBox.AddMail(mailItem);
	}
}
