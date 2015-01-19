
#ifndef COMM_H
#define COMM_H
#include <libpcan.h>

//#define unsigned char BYTE

//#define BYTE unsigned char 
//#define DWORD unsigned int 

#define MAIL_BOX_SIZE 100
#define WRITE_QUEUE_SIZE 100

#define CAN_SUCCESS 0

class MailItem
{
public:

	MailItem();

	DWORD canID;
	BYTE commandID[2];
	BYTE dataLength;
	BYTE data[8];
	BYTE status;

	void Copy(const MailItem& mailItem);
};


class MailBox
{
public:

	MailItem mail[MAIL_BOX_SIZE];
	int total;

	MailBox();
	bool AddMail(const MailItem& mailItem);
	bool GetMail(DWORD canID, BYTE commandID0, BYTE commandID1, MailItem& mailItem);
	bool GetMail(DWORD id, MailItem& mailItem);


}; 

void* ReceiveThread0(void *);
void* ReceiveThread1(void *);
void* ReceiveThread2(void *);
void* ReceiveThread3(void *);

void* SendThread(void *);



#endif
