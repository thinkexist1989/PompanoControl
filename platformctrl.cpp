#include "platformctrl.h"
#include <stdio.h>   /*标准输入输出定义*/
#include <stdlib.h>  /*标准函数库定义*/
#include <unistd.h>  /*UNIX标准函数定义*/
#include <sys/types.h>  /*基本系统数据类型*/
#include <sys/stat.h>   /*unix/linux系统定义文件状态*/
#include <fcntl.h>   /*文件控制定义*/
#include <termios.h>  /*PPSIX 终端控制定义*/
#include <errno.h>    /*错误号定义*/
#include <string.h>

#include <QDebug>
#include <QMutex>

extern QMutex mutex;

//PlatformCtrl::PlatformCtrl() {}

PlatformCtrl::PlatformCtrl(int fd) : fd(fd) {
    m_kp1 = 0.05;
    m_ki1 = 0;
    m_kd1 = 0;

    m_kp2 = 0.05;
    m_ki2 = 0;
    m_kd2 = 0;

    m_kp3 = 0.05;
    m_ki3 = 0;
    m_kd3 = 0;
}

void PlatformCtrl::run()
{
//    TargetPosition1 = 0;
//    TargetPosition2 = 0;
//    TargetPosition3 = 0;
    char CEP1[]="1,CEP,1\n";        //clear encoder1
    char CEP2[]="1,CEP,2\n";        //clear encoder2
    char CEP3[]="2,CEP,1\n";        //clear encoder3
    char CER1[]="1,CER\n";          //clear driver1 error
    char CER2[]="2,CER\n";          //clear driver2 error
    int len_cer=strlen(CER1);
    int len_send=strlen(CEP1);
    write(fd,CER1,len_cer);
    usleep(10000);
    write(fd,CER2,len_cer);
    usleep(10000);
    write(fd,CEP1,len_send);
    usleep(10000);
    write(fd,CEP2,len_send);
    usleep(10000);
    write(fd,CEP3,len_send);
    usleep(10000);

    char GEP1[]={"1,GEP,1\n"};
    char GEP2[]={"1,GEP,2\n"};
    char GEP3[]={"2,GEP,1\n"};
    char pos123[60]={0};
    char temp[60]={0};
    int Encoder1 = 0,   Encoder2 = 0,   Encoder3 = 0;
    int rem_pos1=0,     rem_pos2=0,     rem_pos3=0;
    int Speed1=0,       Speed2=0,       Speed3=0;
    int POSITION1=0, POSITION2=0, POSITION3=0;
    char SVS1[]="1,SVS,1,";
    char SVS2[]="1,SVS,2,";
    char SVS3[]="2,SVS,1,";
    char SVS4[]="2,SVS,2,";

    greg::PID PID1;                 //set axis1 PID parameters
    PID1.setRefreshInterval(0.05);
    PID1.setWeights(m_kp1,m_ki1,m_kd1);
    PID1.setErrorThreshold(80);    
//    PID1.setDesiredPoint(-20000);

    greg::PID PID2;                 //set axis2 PID parameters
    PID2.setRefreshInterval(0.05);
    PID2.setWeights(m_kp2,m_ki2,m_kd2);
    PID2.setErrorThreshold(60);
//    PID2.setDesiredPoint(5000);

    greg::PID PID3;                 //set axis3 PID parameters
    PID3.setRefreshInterval(0.1);
    PID3.setWeights(m_kp3,m_ki3,m_kd3);
    PID3.setErrorThreshold(50);    
//    PID3.setDesiredPoint(5000);

     ReadPosFile(rem_pos1, rem_pos2, rem_pos3);
     fclose(fopen("posdata.txt", "w"));
     int fileDes=open("posdata.txt",O_CREAT|O_RDWR|O_APPEND,0666);

     std::cout<<"x="<<rem_pos1<<std::endl;
     std::cout<<"y="<<rem_pos2<<std::endl;
     std::cout<<"z="<<rem_pos3<<std::endl;
     AxisSpeedXY = 500;
     AxisSpeedZ = 300;
     WireFeedSpeed = 300;

     int desiredpos1 = 30;     //The value is between -160 and 160
     int desiredpos2 = 30;
     int desiredpos3 = 30;

    while(!stopped)
    {
        // Axises Speed
        AxisSpeedXY = g::AxisSpeedXY;
        AxisSpeedZ = g::AxisSpeedZ;
        WireFeedSpeed = g::WireFeedSpeed;
        PID1.setOutputLowerLimit(-AxisSpeedXY*0.75);
        PID1.setOutputUpperLimit(AxisSpeedXY*0.75);
        PID2.setOutputLowerLimit(-AxisSpeedXY);
        PID2.setOutputUpperLimit(AxisSpeedXY);
        PID3.setOutputLowerLimit(-AxisSpeedZ);
        PID3.setOutputUpperLimit(AxisSpeedZ);
//        if((desiredpos1>160)||(desiredpos2>160)){
//            std::cout << "TargetPosition Out of Range!" << std::endl;
//            break;
//        }
//        TargetPosition1 = g::TargetPosition1;
//        TargetPosition2 = g::TargetPosition2;
//        TargetPosition3 = g::TargetPosition3;
//        POSITION1=TargetPosition1*90*64/70-rem_pos1;        //position of axis1
//        POSITION2=TargetPosition2*120*64/70-rem_pos2;       //position of axis2
//        POSITION3=TargetPosition3*270*64/52-rem_pos3;       //position of axis3
//        PID1.setDesiredPoint(POSITION1);
//        PID2.setDesiredPoint(POSITION2);
//        PID3.setDesiredPoint(POSITION3);

        POSITION1=desiredpos1*90*64/70-rem_pos1;
        POSITION2=desiredpos2*120*64/70-rem_pos2;
        POSITION3=desiredpos3*270*64/52-rem_pos3;
        PID1.setDesiredPoint(POSITION1);
        PID2.setDesiredPoint(POSITION2);
        PID3.setDesiredPoint(POSITION3);

    /***Axis1**/
        int temp1=GetPosition(fd, GEP1);
        if(temp1 != 0xffff){
            Encoder1 = temp1;
        }
        Speed1 = PID1.refresh(Encoder1);
        MotorControl(fd,Speed1, SVS1);
        if(abs(Encoder1 -POSITION1)<80){
            desiredpos1 = -desiredpos1;
        }

    /***Axis2**/
        int temp2 =GetPosition(fd, GEP2);
        if(temp2 != 0xffff){
            Encoder2 = temp2;
        }
        Speed2 = PID2.refresh(Encoder2);
        MotorControl(fd,Speed2, SVS2);
        if(abs(Encoder2 -POSITION2)<60){
            desiredpos2 = -desiredpos2;
        }

    /***Axis3**/
        int temp3 =GetPosition(fd, GEP3);
        if(temp3 != 0xffff){
            Encoder3 = temp3;
        }
        Speed3 = PID3.refresh(Encoder3);
        MotorControl(fd,Speed3, SVS3);
        if(abs(Encoder3 -POSITION3)<40){
            desiredpos3 = -desiredpos3;
        }

    /***Axis4***/
        MotorControl(fd,WireFeedSpeed, SVS4);

    /***Record position***/
        Encoder1+=rem_pos1;
        Encoder2+=rem_pos2;
        Encoder3+=rem_pos3;
        WritePosFile(Encoder1, Encoder2, Encoder3, pos123, temp, fileDes);

        CurrentPosition1=Encoder1*70/(64*90);
        CurrentPosition2=Encoder2*70/(64*120);
        CurrentPosition3=Encoder3*52/(64*270);

        g::CurrentPosition1 = CurrentPosition1;
        g::CurrentPosition2 = CurrentPosition2;
        g::CurrentPosition3 = CurrentPosition3;
    }

}

int PlatformCtrl::OpenCommPort(char *devname)
{
    int ttyd;
    struct termios ttyopt;

    if (devname == NULL)
    {
        perror("invalid devname!");
        return -EINVAL;
    }

    ttyd = open(devname, O_RDWR | O_NOCTTY);

   // fcntl(ttyd,F_SETFL,0);
   // ttyd = open(devname, O_RDWR | O_NOCTTY);

    if (ttyd < 0)
    {
        perror("cannot open serial port!");
        return -EIO;
    }

    fcntl(ttyd, F_SETFL, FNDELAY);

   // tcgetattr(ttyd, &ttyopt);
    memset(&ttyopt, 0, sizeof ttyopt);      //在一段内存块中填充某个给定的值，它对较大的结构体或数组进行清零操作的一种最快方法
                                            //void *memset(void *s,  int c, size_t n)
                                            //把一个char a[20]清零, 是 memset(a, 0, 20)

    /* Set baudrate 9600bps */
    cfsetispeed(&ttyopt, B57600);
    cfsetospeed(&ttyopt, B57600);


    /* Set parity none, 1 stop bit, 8 data bits */
    ttyopt.c_cflag &= ~PARENB;
    ttyopt.c_cflag &= ~CSTOPB;
    ttyopt.c_cflag &= ~CSIZE;
    ttyopt.c_cflag |= CS8;
    ttyopt.c_iflag |= INPCK;
    ttyopt.c_iflag &= ~(ISTRIP | INLCR | IGNCR | ICRNL);

    /* Enable reciever and set local mode */
    ttyopt.c_cflag |= (CLOCAL | CREAD);

    /* Ignore control input */
    ttyopt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    /* Disable flow control */
    ttyopt.c_iflag &= ~(IXON | IXOFF | IXANY);
    ttyopt.c_oflag &= ~(OPOST | ONLCR | OCRNL);

    ttyopt.c_cc[VTIME] = 1;                 //设置超时0 seconds
    ttyopt.c_cc[VMIN] = 50;                 //define the minimum bytes data to be readed
                                            // opt.c_cc[VMIN]=0:Update the options and do it NOW  !!!!!!

    tcsetattr(ttyd, TCSANOW, &ttyopt);

    fd = ttyd;
    return ttyd;
}

int PlatformCtrl::GetPosition(int fd, char *GEP)
{

    char RcvBuf[20];
    int  n = 0;
    int  Encoder = 0;
    memset(RcvBuf,0,20);
    write(fd,GEP,strlen(GEP));
    usleep(15000);
    n = read(fd,RcvBuf,20);
    if(RcvBuf[n-1]!='\n'){
        std::cout << "PlATFORM:Recieve Data Error!" << std::endl;
        return 0xFFFF;
    }
    else{
        Encoder = atoi(RcvBuf);
        return Encoder;
    }
}

void PlatformCtrl::MotorControl(int fd, int Speed, char *SVS)
{
    char SpdSbuf[20];
    char CER1[]="1,CER\n";
    char CER2[]="2,CER\n";
    if(SVS[0]=='1'){
        write(fd,CER1,6);
    }
    if(SVS[0]=='2'){
        write(fd,CER2,6);
    }
    memset(SpdSbuf,0,20);
    if((abs(Speed) < 300)&&(Speed != 0))
        Speed = (Speed > 0)?300:-300;
    sprintf(SpdSbuf,"%s%d\n",SVS,Speed);
    std::cout << SpdSbuf << std::endl;
    write(fd,SpdSbuf,strlen(SpdSbuf));
    usleep(10000);
}

int PlatformCtrl::ReadPosFile(int &rem_pos1, int &rem_pos2, int &rem_pos3)
{
    char* fileName="posdata.txt";
    int fileDes=open(fileName,O_CREAT|O_RDWR|O_APPEND,0666);                     //open position file, creat it if don't exist.
    if(fileDes==-1){
      std::cout<<"the file open failed with the code : "<<errno<< std::endl;
      return 0;
    }
     std::cout<<"the file open success\n"<< std::endl;

     /*Read the last line of "posdata.txt" */
     FILE *fp = fopen(fileName,"r");
     char sline[1024]={0};
     char *lastline;
     int i,j,k=0,last_len=0;
     int flag=0;
     char last_pos1[20]={0}, last_pos2[20]={0}, last_pos3[20]={0};
     size_t rd;
     fseek(fp, 0, SEEK_END);
     if(ftell(fp) > 1023)
        fseek(fp, -1023, SEEK_CUR);
     else
        fseek(fp, -ftell(fp), SEEK_CUR);
     rd=fread(sline, 1, 1023, fp);
     if(sline[rd-1] == '\n')
        sline[rd-1] = 0;

     for(i=(1023>rd? rd:1023); i>=0; i--){
         if(sline[i] == '\n'){
             break;
         }
     }
     if(i<0){
         std::cout<<"Read Data Error!\n"<<std::endl;
     }
//        qDebug("last line is:\n%s\n", sline+i+1);
     lastline=sline+i+1;
     last_len=strlen(lastline);

     for(j=0;j<last_len-1;j++){                 //extract the position data in the last line
         switch(flag){
         case 0:
             if(lastline[j]!='*')
             {
                last_pos1[j]=lastline[j];
             }
             else
             {
                 flag=1;
                 k=j;
             }
             break;
         case 1:
             if(lastline[j]!='*')
             {
                last_pos2[j-k-1]=lastline[j];
             }
             else
             {
                 flag=2;
                 k=j;
             }
             break;
         case 2:
             if(lastline[j]!='*')
             {
                last_pos3[j-k-1]=lastline[j];
             }
             break;
         }

     }
     rem_pos1=atoi(last_pos1);
     rem_pos2=atoi(last_pos2);
     rem_pos3=atoi(last_pos3);
}

void PlatformCtrl::WritePosFile(int Encoder1, int Encoder2, int Encoder3, char *pos123, char *temp, int fileDes)
{
    sprintf(pos123,"%d*%d*%d\n",Encoder1,Encoder2,Encoder3);
    std::cout << pos123 << std::endl;
    if(strcmp(pos123,temp)!=0)              //Don't reacrd it if same as previous data.
    {
        if(write(fileDes,pos123,strlen(pos123))==-1)    //鏂囦欢鍐欏叆
           {
              std::cout<<"write file error with the code :"<<errno<< std::endl;
           }
    }
    strcpy(temp,pos123);
}
