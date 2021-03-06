#ifndef TESTKPJ_H
#define TESTKPJ_H

int elmoID[] = {100,101,102};
const int sizemotor = sizeof(elmoID)/sizeof(elmoID[0]);
//const int sizemotor = 1;
float pos_ref[sizemotor][2]={{0}};
float pos_fil[sizemotor][2]{{0}};
float current[sizemotor]={0};
float position[sizemotor][2]={{0}};
float posact_fil[sizemotor][2]={{0}};
int position_count[sizemotor]={0};
float tq[5][2]={{0}};
float tq_fil[5][2]={{0}};
float tq_init[5]={0};
float tq_zero[5]={0};
int acc[sizemotor],dcc[sizemotor],fs[sizemotor],sp[sizemotor]={0};

int ftID = 432;
int count1=0;
char mode;


float ts=0.01;
int gear=100;
int ctrcount=0;
int ctrtimecount=0;
float ctrtime=0.0;
int initcount=0;
float gcof[2]={6.2,6.2};



bool isMotorOn = false;
bool enctrloop = false;
bool forceinit = false;

////////////100Hz 1Hz//
const int NL = 3;
const double NUM[3] = {
   0.007820208033497,  0.01564041606699, 0.007820208033497
};
const int DL = 3;
const double DEN[3] = {
                   1,   -1.734725768809,   0.7660066009433
};

////////////////////////////////////////////
	float pi = 3.141592653;
	float counttodeg = 180.0*0.0005;
	float degtorad = pi/180.0;
	float radtodeg = 180.0/pi;

#endif // TESTKPJ_H
