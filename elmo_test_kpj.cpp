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
#include <cmath>
#include "SharedMemModule.h"
#include "testkpj.h"
using namespace std;

pthread_t receiveThreadHandler;
pthread_t sendThreadHandler;

RT_TASK ctrlThreadHandler;

void CtrlThread(void* );
bool InitSharedMemory(void);

SharedMemStruct* pSharedMemStruct;
SharedCommandQueue* pSharedCommandQueue;

FT_Sensor ftSensor(ftID);
extern MailBox mailBox;
extern boost::lockfree::queue<TPCANMsg> CAN_Write_Queue;

void updatevalue(ElmoDriver elmoDriver);
void Ctrlloop(ElmoDriver elmoDriver);
void Ctrlpos(ElmoDriver elmoDriver);
void updateforce();
void initforce();


float I[2]={0.1,100};
float C[2]={0,0};
float K[2]={50,10};

ElmoDriver elmoDriver[3] = { ElmoDriver(elmoID[0]), ElmoDriver(elmoID[1]),ElmoDriver(elmoID[2])};
//AbsoluteEncoder absoluteEncoder(0x14c);

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

		if(!InitSharedMemory())

			throw DeviceException();	

		for (int i=0; i<sizemotor; i++)
		{
        		elmoDriver[i].SetHandle(handle);
		}

        	ftSensor.SetHandle(handle);

		StartThreads(handle);


		//for(;;)
		//	sleep(5);
		char k;

		  for (int i=0; i<sizemotor; i++)
       		{
        		elmoDriver[i].ResetComm();
       		}

		usleep(10000);
		sleep(1);

		cout << "Reset Comm done" << endl;

		for (int i=0; i<sizemotor; i++)
        	{
	        elmoDriver[i].ResetNode();
        	}
		
		sleep(5);
		

		for(int i=0; i<sizemotor; i++)
    			{
            			elmoDriver[i].GotoOpMode();
    			}


		cout << "Reset Node Done..." << endl;
		cout << "s : start control loop" << endl;
		cout << "i : initialize" << endl;
		cout << "o : motor on" << endl;
		cout << "k : stop control loop" << endl;
		cout << "q : quit" << endl;

		

	

		//elmoDriver.TCEqualOne();


		//elmoDriver.PA();

		//elmoDriver.BG();

		sleep(1);

		//elmoDriver.PR((float) 270.0);

		//elmoDriver.BG();

		cout << "Press a key " << endl;

		
		for(;;)
		{


			sleep(1);
			cin >> mode;
		if (mode == 'q')
		{
			
			exit(0);
			
		}
		else if (mode == 'k')
		{
				for(int i=0; i<sizemotor; i++)
    			{
    				elmoDriver[i].PA(0);
				elmoDriver[i].BG();
    			}			
			sleep(3);
			for(int i=0; i<sizemotor; i++)
    			{
    				elmoDriver[i].MotorOff();
    			}
			cout << "motor stopped&ctrl stopped" << endl;
			isMotorOn = false;
			enctrloop = false;
		}

		else if (mode == 'o')
		{
			for(int i=0; i<sizemotor; i++)
            		{
                		elmoDriver[i].MotorOn();
            		}

			sleep(1);

			isMotorOn = true;
			cout <<"motor is on" << endl;
		}

		else if (mode == 'i')
		{
			for(int i=0; i<sizemotor; i++)
    			{
            			//elmoDriver[i].GotoOpMode();
				elmoDriver[i].InterpolatedPositionMode();
    			}
			initforce();
		}

		else if (mode == 's')
		{
			for(int i=0; i<sizemotor; i++)
    			{
			elmoDriver[i].InterpolatedPositionMode();
			}			
			if(isMotorOn !=true)
			{
				for(int i=0; i<sizemotor; i++)
            			{
                			elmoDriver[i].MotorOn();
				}

				sleep(1);
				
				
			}
				
				for(int i=0; i<sizemotor; i++)
    			{
				elmoDriver[i].CW();
			}
				enctrloop = true;
				isMotorOn = true;
				cout <<"motor is on" << endl;	

		}

			//cout << "I am main thread" << endl;


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

	tID = pthread_create(&receiveThreadHandler, NULL, ReceiveThread0, (void *) handle);
	if(tID < 0)
		throw DeviceException(); 

	tID = pthread_create(&sendThreadHandler, NULL, SendThread, (void *) handle);
	if (tID < 0)
		throw DeviceException(); 

//	return;
	if(rt_task_create(&ctrlThreadHandler, "TEST", 0, 99, 0) != 0)
		throw DeviceException();

	if(rt_task_start(&ctrlThreadHandler, &CtrlThread, NULL) != 0)
		throw DeviceException();

}



void CtrlThread(void *param)
{

		

	static int k=0;
	MailItem mailItem1, mailItem2;


	if (rt_task_set_periodic(NULL, TM_NOW, ts*1000000000)!=0)
	{
		cout << "Error in Xenomai init" << endl;
		return;
	}
	float previousRoll = 0.0;
	float motorRoll;
	const float threshold = 0.2;

			
	for(;;)
	{
		if(rt_task_wait_period(NULL)!=0)
		{
			cout << "Error in Xenomai loop" << endl;
			return;
		}
		
		if(enctrloop == true)
		{	
		
			//cout<< "ctr is on" << endl;
			updatevalue(elmoDriver[0]);
			//updatevalue(elmoDriver[1]);
			//cout << ctrcount++ << endl;
			updateforce();
			//cout << tq[0] << "Nm" << endl;
			Ctrlloop(elmoDriver[0]);
			Ctrlpos(elmoDriver[1]);
			Ctrlpos(elmoDriver[2]);
			//Ctrlloop(elmoDriver[1]);
            	}
		//cout << "next step" << k++ << endl;


		usleep(1000);

		


	}

}

bool InitSharedMemory(void)
{


	if(!InitSharedMemModule(0))
		return false;
	for(int i=0; i<sizemotor; i++)
    			{
	pSharedMemStruct->elmoContent[i].position = 0.0;
	pSharedMemStruct->elmoContent[i].current = 0.0;

			}
	pSharedMemStruct->FT_Content.Tx= 0.0;
	return true;

}

void updatevalue(ElmoDriver elmoDriver)
{
	MailItem mailItem1;
	elmoDriver.GetPosition();
	static int i=elmoDriver.id-100;
	mailBox.GetMail(elmoDriver.id+5*128,'P','X', mailItem1);
	elmoDriver.readintdata(mailItem1.data, position_count[i]);
	position[i][0]=(float) (position_count[i]*counttodeg/gear);
	posact_fil[i][0] = NUM[0]*position[i][0]+NUM[1]*position[i][1]+NUM[2]*position[i][2]-DEN[1]*posact_fil[i][1]-DEN[2]*posact_fil[i][2];
	pSharedMemStruct->elmoContent[i].position = position[i][0];
        elmoDriver.GetCurrent();
	mailBox.GetMail(elmoDriver.id+5*128,'I','Q', mailItem1);
	elmoDriver.readfloatdata(mailItem1.data, current[i]);
	pSharedMemStruct->elmoContent[i].current = current[i];	

	position[i][1]=position[i][0];
    	position[i][2]=position[i][1];
    	posact_fil[i][1]=posact_fil[i][0];
    	posact_fil[i][2]=posact_fil[i][1];

}

void updateforce()
{
	float Fx, Fy, Fz, Tx, Ty, Tz;
	MailItem mailItem1,mailItem2;
	 if (ftSensor.RequestData() != true)
    	{	cout << "no data" << endl;
    	}
	if(mailBox.GetMail(ftSensor.id+5, mailItem1) && mailBox.GetMail(ftSensor.id+6, mailItem2))
    {
        ftSensor.GetValues(mailItem1.data, mailItem2.data, Fx, Fy, Fz, Tx, Ty, Tz);
        //printf("Fx: %.2f Fy: %.2f Fz: %.2f  Tx: %.4f Ty: %.2f Tz: %.2f \n", Fx, Fy, Fz, Tx, Ty, Tz);
	tq[0][0]=Tx-tq_init[0];
	pSharedMemStruct->FT_Content.Tx=tq[0][0];

	tq[1][0]=-Ty-tq_init[1];
	pSharedMemStruct->FT_Content.Ty=tq[1][0];
	tq_fil[0][0] = NUM[0]*tq[0][0]+NUM[1]*tq[0][1]+NUM[2]*tq[0][2]-DEN[1]*tq_fil[0][1]-DEN[2]*tq_fil[0][2];
	tq[0][1]=tq[0][0];
    	tq[0][2]=tq[0][1];
    	tq_fil[0][1]=tq_fil[0][0];
    	tq_fil[0][2]=tq_fil[0][1];
	
	//cout << tq[0] << endl;
    }
	

}

void Ctrlloop(ElmoDriver elmoDriver)
{	
	static int i=0;
	i=elmoDriver.id-100;
	//pos_ref[i][0] = 5*degtorad*sin(2*3.141592*ctrcount/200);
	pos_ref[i][0] = ((2*I[i]+C[i]*ts)*pos_ref[i][1]-I[i]*pos_ref[i][2]+(tq_fil[i][0]-sin(posact_fil[i][0]*degtorad)*gcof[i])*ts*ts)/(I[i]+C[i]*ts+K[i]*ts*ts);
    pos_fil[i][0] = NUM[0]*pos_ref[i][0]+NUM[1]*pos_ref[i][1]+NUM[2]*pos_ref[i][2]-DEN[1]*pos_fil[i][1]-DEN[2]*pos_fil[i][2];
	elmoDriver.iPA((float)(pos_fil[i][0]*radtodeg));
	cout << (float)(pos_fil[i][0]*radtodeg) << endl;
	//elmoDriver.BG();
	pos_ref[i][1]=pos_ref[i][0];
    	pos_ref[i][2]=pos_ref[i][1];
    	pos_fil[i][1]=pos_fil[i][0];
    	pos_fil[i][2]=pos_fil[i][1];
	ctrcount++;
	
}

void Ctrlpos(ElmoDriver elmoDriver)
{	
	static int i=0;
	i=elmoDriver.id-100;
	elmoDriver.iPA(0);
	
}

void initforce()
{	
	while (initcount <50)
	{
		updateforce();
				usleep(10000);
		//cout << tq[0][0] << endl;
		//cout << tq[1][0] << endl;
		tq_zero[0]= tq_zero[0]+pSharedMemStruct->FT_Content.Tx;
		tq_zero[1]= tq_zero[1]+pSharedMemStruct->FT_Content.Ty;
		initcount++;

		//cout << tq[0]<< endl;
		//cout << initcount<< endl;
		
	}
		cout << initcount << endl;		
		tq_init[0] = tq_zero[0]/(initcount-1);
		tq_init[1] = tq_zero[1]/(initcount-1);
		cout << tq_init[0]<< "Nm" << endl;
		cout << tq_init[1]<< "Nm" << endl;
	forceinit=true;
	
}
