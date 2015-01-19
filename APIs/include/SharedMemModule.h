#ifndef SHARED_MEM_MODULE_H
#define SHARED_MEM_MODULE_H

#include "Device.h"
#include "ElmoDriver.h"

#define MAX_ELMO_SIZE 10
#define COMMAND_Q_SIZE 100
struct SharedMemStruct
{
	AbsoluteEncoderSpace::SharedMemContent absoluteEncoderContent;
	IMU_Space::SharedMemContent imu_Content;
	ElmoSpace::SharedMemContent elmoContent[MAX_ELMO_SIZE];
	FT_Sensor_Space::SharedMemContent FT_Content;
};


struct SharedCommandQueue
{
	TPCANMsg msgList[COMMAND_Q_SIZE];
	unsigned int head;
	unsigned int tail;

};


bool InitSharedMemModule(int mode);

#endif
