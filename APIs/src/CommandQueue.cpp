#include "CommandQueue.h"




void TPCANMsgCopy(TPCANMsg* pDest, TPCANMsg* pSrc)
{
	int i;
	for (i=0; i<pSrc->LEN; i++)
		pDest->DATA[i] = pSrc->DATA[i];

	pDest->ID = pSrc->ID;
	pDest->LEN = pSrc->LEN;
	pDest->MSGTYPE = pSrc->MSGTYPE;

}

void InitializedCommandQueue(SharedCommandQueue* pQueue)
{
	pQueue->head = pQueue->tail = 0;
}

bool InsertAtTail(SharedCommandQueue* pQueue, TPCANMsg* pMsg)
{
	if ((pQueue->tail+1)%COMMAND_Q_SIZE == pQueue->head)
		return false;

	TPCANMsgCopy(&pQueue->msgList[pQueue->tail], pMsg);
	pQueue->tail = (pQueue->tail+1)%COMMAND_Q_SIZE;
	return true;

}

bool InsertAtHead(SharedCommandQueue* pQueue, TPCANMsg* pMsg)
{

	if ((pQueue->head+COMMAND_Q_SIZE-1)%COMMAND_Q_SIZE == pQueue->tail)
		return false;

	pQueue->head = (pQueue->head+COMMAND_Q_SIZE-1)%COMMAND_Q_SIZE;
 	TPCANMsgCopy(&pQueue->msgList[pQueue->head], pMsg);
	return true;

}

bool RemoveMsg(SharedCommandQueue* pQueue, TPCANMsg* pMsg)
{
	if(pQueue->head == pQueue->tail)
		return false;
	pQueue->tail = (pQueue->tail - 1 + COMMAND_Q_SIZE)%COMMAND_Q_SIZE;
	TPCANMsgCopy(pMsg, &pQueue->msgList[pQueue->tail]);

	return true;

}


