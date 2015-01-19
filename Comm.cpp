#include "Comm.h"
#include <stdio.h>
#include <boost/lockfree/queue.hpp>

////////////////////////////////  Global Space /////////////////////////////////
MailBox mailBox;

boost::lockfree::queue<TPCANMsg> CAN_Write_Queue(WRITE_QUEUE_SIZE);



////////////////////////////////////////////////////////////////////////////////


MailItem::MailItem() 
{
}

void MailItem::Copy(const MailItem& mailItem)
{
	this->canID = mailItem.canID;
	this->commandID[0] = mailItem.commandID[0];
	this->commandID[1] = mailItem.commandID[1];

	this->dataLength = mailItem.dataLength;
	this->status = mailItem.status;

	for (int i=0; i<8; i++)
		this->data[i] = mailItem.data[i];
}


MailBox::MailBox() 
	: total(0)
{

}

bool MailBox::AddMail(const MailItem& mailItem)
{
	int i;

	for (i=0; i<total; i++)
		if(mailItem.canID == mail[i].canID && mailItem.commandID[0] == mail[i].commandID[0] && mailItem.commandID[1] == mail[i].commandID[1])
			break;

	if (i==total && total == MAIL_BOX_SIZE)
		return false;

	mail[i].Copy(mailItem);

	if (i==total)
		total++;
	
	return true;
}


bool MailBox::GetMail(DWORD canID, BYTE commandID0, BYTE commandID1, MailItem& mailItem)
{
	int i;
	for (i=0; i< total; i++)
		if (canID == mail[i].canID && commandID0 == mail[i].commandID[0] && commandID1 == mail[i].commandID[1])
		{
			mailItem.Copy(mail[i]);
			return true;
		}

	return false;
}

bool MailBox::GetMail(DWORD canID, MailItem& mailItem)
{
	return GetMail(canID, (BYTE)0, (BYTE)0, mailItem);
}

