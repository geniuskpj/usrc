#ifndef COMMAND_Q_H
#define COMMAND_Q_H

#include "SharedMemModule.h"


void TPCANMsgCopy(TPCANMsg* pDest, TPCANMsg* pSrc);
void InitializeCommandQueue(SharedCommandQueue* pQ);
bool InsertAtTail(SharedCommandQueue* pQ, TPCANMsg* pMsg);
bool InsertAtHead(SharedCommandQueue* pQ, TPCANMsg* pMsg);
bool RemoveMsg(SharedCommandQueue* pQ, TPCANMsg* pMsg);

#endif
