#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <pthread.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <math.h>
#include <iostream>

using namespace std;

#define PI 3.141592

sockaddr_in Timaddr;
int TimHandle;
int m_conn=0;
char r_buf[1400]={0,};

typedef struct{
    int x,y,z;
    int dis;} TimArr;

TimArr final_arr[271];


int fd;
struct termios options; //serial comm.
float roll,pitch,yaw;

void* func_xsens(void*);
pthread_t thr1;
int ST;


int main(void)
{

    fd = open("/dev/ttyUSB1",O_RDWR|O_NOCTTY|O_NDELAY);  // port /dev/ttyUSB0을 연다



    if(fd == -1){
         printf("open problem\n");

         return 1;
    }


    fcntl(fd, F_SETFL, 0);

    tcgetattr(fd, &options);	// 통신설정 파일을 불러온다.

    cfsetispeed(&options, B115200);	// 읽기 속도를
    cfsetospeed(&options, B115200);	// 쓰기 속도를
    options.c_cflag |= (CLOCAL|CREAD);
    options.c_cflag &= ~PARENB; //no parity
//    options.c_cflag |= PARODD;	// parity odd
//    options.c_cflag &= ~CSTOPB;	// stop bit  ??
    options.c_cflag |=CSTOPB;	// stop bit 2  ??
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;	// 8bit
    options.c_cflag &= ~CRTSCTS;	// 흐름제어 없음
    options.c_lflag &= ~(ICANON|ECHO|ECHOE|ISIG);
    options.c_oflag &= ~OPOST;	// raw mode
    options.c_cc[VMIN] =0;
    options.c_cc[VTIME] = 100;	// time out 10s
    tcsetattr(fd, TCSANOW, &options);	// 설정 입력

    ST=pthread_create(&thr1,NULL, func_xsens,0);

	for(;;);

/*

    connect(&timer,SIGNAL(timeout()),this,SLOT(show_res()));
   // timer.start(50);


    TimHandle=socket(AF_INET,SOCK_STREAM,0);
    Timaddr.sin_family=AF_INET;
    Timaddr.sin_addr.s_addr=inet_addr("143.248.204.100");
    Timaddr.sin_port=htons(2111);
    ::connect(TimHandle, (struct sockaddr *)&Timaddr, sizeof(Timaddr));

    connect(&timer0,SIGNAL(timeout()),this,SLOT(tim()));
    timer0.start(50);
*/

}

/*
void MainWindow::tim()
{
    char s_buf[128]={0,};
    sprintf(s_buf,"%c%s%c",0x02,"sRN LMDscandata",0x03);
    write(TimHandle,s_buf,strlen(s_buf));

    int res=read(TimHandle,r_buf,sizeof(r_buf));

    strtok(r_buf," ");// discard 1
    for(int i=0;i<=24;i++){ // discard 26
        strtok(NULL," ");
    }

    char *temp=0;
    char temp_str[128]={0,};

    for(int i=0;i<=270;i++){
        temp=strtok(NULL," ");
        int temp_value=0;
        int temp_len=strlen(temp);

        for(int j=temp_len-1;j>=0;j--){
            if(temp[j]=='A'){temp_value+=10*pow((double)16,temp_len-j-1);}
            else if(temp[j]=='B'){temp_value+=11*pow((double)16,temp_len-j-1);}
            else if(temp[j]=='C'){temp_value+=12*pow((double)16,temp_len-j-1);}
            else if(temp[j]=='D'){temp_value+=13*pow((double)16,temp_len-j-1);}
            else if(temp[j]=='E'){temp_value+=14*pow((double)16,temp_len-j-1);}
            else if(temp[j]=='F'){temp_value+=15*pow((double)16,temp_len-j-1);}
            else{temp_value+=(temp[j]-48)*pow((double)16,temp_len-j-1);}
        }
        final_arr[i].dis=temp_value; // -45 ~ +225 center 90,  res 1degree

        final_arr[i].x=temp_value*cos(-pitch*(PI/180))*cos((i-45)*(PI/180));//final_arr[i].x=temp_value*cos((i-45)*(PI/180));
        final_arr[i].y=temp_value*cos(-pitch*(PI/180))*sin((i-45)*(PI/180));//final_arr[i].y=temp_value*sin((i-45)*(PI/180));
        final_arr[i].z=temp_value*sin(-pitch*(PI/180));

        //sprintf(temp_str,"deg:%d, dis:%d, x:%d, y:%d",i-45,final_arr[i].dis,final_arr[i].x,final_arr[i].y);
       // int k;
       // k++;
        char result[255]={0,};
        sprintf(result,"Roll: %.2f Pitch: %.2f Yaw: %.2f \n x: %d y: %d z: %d",roll,pitch,yaw,final_arr[45+90].x,final_arr[45+90].y,final_arr[45+90].z );
        ui->m_result->setText(result);

    }
}

*/

void* func_xsens(void* args)
{
    char buffer[255]={0,};
    unsigned char temp[8]={0,};
    char temp2[8]={0,};
    float temp3=0;

	cout << "hello" << endl;

    while(1){
        int cnt=0;
        while(cnt!=12){ // 시리얼 포트에서 값을 읽는다.
            ::read(fd,temp,2 );
             if(temp[0]==0xfa && temp[1]==0xff){
                 ::read(fd,temp,2 ); //discard
                  for(cnt=0;cnt<12;cnt++){
                      ::read(fd,temp,1 );
                      buffer[cnt]=temp[0];
                 }
              }

        }

	//cout << "next step" << endl;

        temp2[0]=buffer[3];
        temp2[1]=buffer[2];
        temp2[2]=buffer[1];
        temp2[3]=buffer[0];        
        memcpy(&temp3,temp2,4);
        //if((temp3<180) && (temp3>-180) && (temp3!=0)) 
		roll=temp3;//memcpy(&roll,temp2,4);;

        temp2[0]=buffer[7];
        temp2[1]=buffer[6];
        temp2[2]=buffer[5];
        temp2[3]=buffer[4];
        memcpy(&temp3,temp2,4);
        //if((temp3<180) && (temp3>-180) && (temp3!=0)) 
		pitch=temp3;//memcpy(&pitch,temp2,4);;

        temp2[0]=buffer[11];
        temp2[1]=buffer[10];
        temp2[2]=buffer[9];
        temp2[3]=buffer[8];
        memcpy(&temp3,temp2,4);
        //if((temp3<180) && (temp3>-180) && (temp3!=0)) 
		yaw=temp3;//memcpy(&yaw,temp2,4);;

	char str[200];
	sprintf(str, "Roll: %.2f Pitch %.2f Yaw: %.2f \n", roll, pitch, yaw);

	cout << str << endl; 
	usleep(1000000);
   }

    return 0;
}


