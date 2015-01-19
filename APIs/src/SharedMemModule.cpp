#include "SharedMemModule.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>


extern SharedMemStruct* pSharedMemStruct;
extern SharedCommandQueue* pSharedCommandQueue;

bool InitSharedMemModule(int mode)
{
	int fd;


	if(mode == 0)
		shm_unlink("SHARED_MEM");

	fd = shm_open("SHARED_MEM", O_CREAT | O_RDWR, 0666);
	if (fd == -1 )
		return false;

	if(ftruncate(fd, sizeof(SharedMemStruct)) == -1)
		return false;

	pSharedMemStruct = (SharedMemStruct* )mmap(NULL, sizeof(SharedMemStruct), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (pSharedMemStruct == MAP_FAILED)
		return false;


	if(mode == 0)
		shm_unlink("SHARED_COMMAND_QUEUE");

	fd = shm_open("SHARED_COMMAND_QUEUE", O_CREAT | O_RDWR, 0666);
	if (fd == -1 )
		return false;

	if(ftruncate(fd, sizeof(SharedCommandQueue)) == -1)
		return false;

	pSharedCommandQueue = (SharedCommandQueue* )mmap(NULL, sizeof(SharedCommandQueue), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (pSharedCommandQueue == MAP_FAILED)
		return false;

	return true;


}



