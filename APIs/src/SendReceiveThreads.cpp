#include <iostream>
#include "SendReceiveThreads.h"
#include <boost/lockfree/queue.hpp>
#include "Device.h"
#include "CommandQueue.h"

using namespace std;


extern MailBox mailBox;
extern boost::lockfree::queue<TPCANMsg> CAN_Write_Queue;
extern SharedCommandQueue* pSharedCommandQueue;

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
		if (RemoveMsg(pSharedCommandQueue, &msg))
		{
			cout << "got it ........................................." << endl;
			if(CAN_Write(handle, &msg) != CAN_ERR_OK)
				cout << "Can write error (CommandQ)" << endl;

			continue;
		}

		if (!CAN_Write_Queue.pop(msg))
			continue;

		if (CAN_Write(handle, &msg) != CAN_ERR_OK)
		{
			cout << "Can write error" << endl;
			continue;
		}

	}

}

