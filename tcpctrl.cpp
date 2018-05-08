#include "tcpctrl.h"
#include <iostream>
#include <bitset>
#include <QMutex>

#include <vector>
#include <utility>
#include <map>

#include <g.h>

#define SERVER_PORT   6666
#define SERVER_IP     "192.168.0.100"
#define TCP_SEND_LEN 50
#define TCP_RECIEVE_LEN 50

#define ORD1_MOTOR             0x01
#define ORD1_PLATFORM          0x02
#define ORD1_XSENS             0x03
#define ORD1_KELLER            0x04
#define ORD1_ALT               0x05
#define ORD1_TEMP              0x06
#define ORD1_LIGHT             0x07

#define MOTOR_INFO             0x00
#define MOTOR_ORDER            0x01

#define PLATFORM_INFO          0x00
#define PLATFORM_POS_ORDER     0x01
#define PLATFORM_SET_XYSPEED   0x02
#define PLATFORM_SET_ZSPEED    0x03
#define PLATFORM_SET_WSPEED    0x04
#define PLATFORM_SET_XZERO     0x05
#define PLATFORM_SET_YZERO     0x06
#define PLATFORM_SET_ZZERO     0X07

#define XSENS_DATA             0X00

#define KELLER_DATA            0X00

#define ALT_DATA               0X00

#define TEMP_DATA              0X00
#define TEMP_ORDER             0x01

#define LIGHT_DATA1            0x00
#define LIGHT_SET1             0x01
#define LIGHT_DATA2            0x02
#define LIGHT_SET2             0x03

TcpCtrl::TcpCtrl(){}


volatile bool TcpCtrl::stopped = false;
/***    tcpip    ***/
int TcpCtrl::sockServer;
sockaddr_in TcpCtrl::addrServer;

std::map<int,sockaddr_in> TcpCtrl::connVec;
int TcpCtrl::connCnt = 0;

void TcpCtrl::SendData(char* sendbuf, int n)
{

   for(std::map<int,sockaddr_in>::iterator it = connVec.begin();it != connVec.end();it++){
       int sockConn = it->first;
       int nn = send(sockConn,sendbuf,n,0);
       //send(sockConn,xsend,5,0);
   }
}

void TcpCtrl::SendXsensData(float* p_roll, float* p_pitch, float* p_yaw)
{
    char sendbuf[18];
    sendbuf[0] = 0xFF; sendbuf[1] = 0xFE;
    sendbuf[2] = 14;
    sendbuf[3] = ORD1_XSENS; sendbuf[4] = XSENS_DATA;
    memcpy(&sendbuf[5],p_roll,4);
    memcpy(&sendbuf[9],p_pitch,4);
    memcpy(&sendbuf[13],p_yaw,4);
    sendbuf[17] = 0x00; //CRC

    SendData(sendbuf,18);
}

void TcpCtrl::SendKellerData(float *p_pressval, float *p_tempval)
{
    char sendbuf[14];
    sendbuf[0] = 0xFF; sendbuf[1] = 0xFE;
    sendbuf[2] = 10;
    sendbuf[3] = ORD1_KELLER; sendbuf[4] = KELLER_DATA;
    memcpy(&sendbuf[5],p_pressval,4);
    memcpy(&sendbuf[9],p_tempval,4);
    sendbuf[13] = 0x00; //CRC

    SendData(sendbuf,14);
}

void TcpCtrl::SendLightData(int id, short int *brightness)
{
    char sendbuf[7];
    sendbuf[0] = 0xFF; sendbuf[1] = 0xFE;
    sendbuf[2] = 3;
    sendbuf[3] = ORD1_LIGHT;
    if(id == 1){
        sendbuf[4] = LIGHT_DATA1;
        sendbuf[5] = *brightness;
    }
    else{
        sendbuf[4] = LIGHT_DATA2;
        sendbuf[5] = *brightness;
    }
    sendbuf[6] = 0x00; //CRC

    SendData(sendbuf,7);
}

void TcpCtrl::SendAltData(float *p_dis1, float *p_enr1, float *p_cor1, float *p_dis2, float *p_enr2, float *cor2, float* p_watertemp)
{
    char sendbuf[34];
    sendbuf[0] = 0xFF; sendbuf[1] = 0xFE;
    sendbuf[2] = 30;
    sendbuf[3] = ORD1_ALT; sendbuf[4] = ALT_DATA;
    memcpy(&sendbuf[5] ,p_dis1,4);
    memcpy(&sendbuf[9] ,p_enr1,4);
    memcpy(&sendbuf[13],p_cor1,4);
    memcpy(&sendbuf[17],p_dis1,4);
    memcpy(&sendbuf[21],p_enr1,4);
    memcpy(&sendbuf[25],p_cor1,4);
    memcpy(&sendbuf[29],p_watertemp,4);
    sendbuf[33] = 0x00; //CRC

    SendData(sendbuf,34);
}

void TcpCtrl::SendMotorData()
{
    char sendbuf[38];
    sendbuf[0] = 0xFF; sendbuf[1] = 0xFE;
    sendbuf[2] = 34;
    sendbuf[3] = ORD1_MOTOR; sendbuf[4] = MOTOR_INFO;
    for(int i = 0; i<8; i++){
        memcpy(&sendbuf[5+i*4],&g::motorvec[i].speed,2); //speed of motors
        memcpy(&sendbuf[7+i*4],&g::motorvec[i].current,2); // current of motors
    }
    sendbuf[37] = 0x00;

    SendData(sendbuf,38);
}

void TcpCtrl::SendPlatData()
{
    char sendbuf[18];
    sendbuf[0] = 0xFF; sendbuf[1] = 0xFE;
    sendbuf[2] = 14;
    sendbuf[3] = ORD1_PLATFORM; sendbuf[4] = PLATFORM_INFO;
    memcpy(&sendbuf[5] , &g::plat.CurrentPosition1,4);
    memcpy(&sendbuf[9] , &g::plat.CurrentPosition2,4);
    memcpy(&sendbuf[13] ,&g::plat.CurrentPosition3,4);
    sendbuf[17] = 0x00;

    SendData(sendbuf,18);
}

void TcpCtrl::SendTempData()
{
    char sendbuf[14];
    sendbuf[0] = 0xFF; sendbuf[1] = 0xFE;
    sendbuf[2] = 10;
    sendbuf[3] = ORD1_TEMP; sendbuf[4] = TEMP_DATA;
    memcpy(&sendbuf[5] , &g::cabin_temp, 4);
    memcpy(&sendbuf[9] , &g::wall_temp,  4);
    sendbuf[13] = 0x00;

    SendData(sendbuf, 14);

}


void TcpCtrl::Init()
{
    sockServer = socket(AF_INET, SOCK_STREAM, 0);
    if(sockServer < 0){
        perror("TCP: Error Opening Socket!!");
        return;
    }
    std::cout << "TCP: Open Socket Success!!" <<std::endl;

    bzero(&addrServer,sizeof(addrServer));
    addrServer.sin_family = AF_INET;
    addrServer.sin_addr.s_addr = inet_addr(SERVER_IP);
    addrServer.sin_port = htons(SERVER_PORT);

    if(bind(sockServer, (struct sockaddr*)&addrServer, sizeof(addrServer)) < 0){
        perror("TCP: Error Binding!!");
        return;
    }

    if(listen(sockServer, 5) < 0) {
        perror("TCP: Error Listening!!");
        return;
    }

    std::cout << "TCP: listening success!!"<< std::endl;

    pthread_t TPWFS; //ThreadProcWaitForSocket
    int arg = 0;
    int ret = pthread_create( &TPWFS, NULL, ThreadProcWaitForSocket, & arg );
}

void* TcpCtrl::ThreadProcWaitForSocket(void *arg)
{
    int a = *(int*)arg;

    while(!stopped) {
        socklen_t sin_size = sizeof(struct sockaddr_in);
        sockaddr_in addrConn;
        int sockConn = accept(sockServer, (struct sockaddr*)&addrConn, &sin_size);
        if(sockConn < 0){
           perror("TCP: Error Accepting");
           return NULL;
        }
        std::cout << "Get Connection from: " << inet_ntoa(addrConn.sin_addr) <<std::endl;

        std::pair<int, sockaddr_in> nextConn = std::make_pair(sockConn,addrConn);

       // g::connVec.push_back(nextConn);
        connVec.insert(nextConn);
        connCnt++;
      //  std::cout <<connVec.size() <<std::endl;
        pthread_t TP; //ThreadProc
        int ret = pthread_create(&TP, NULL, ThreadProc, & nextConn);
    }
}

void* TcpCtrl::ThreadProc(void *arg)
{
    std::pair<int, sockaddr_in> conn = *(std::pair<int, sockaddr_in>*) arg;
    int sockConn = conn.first;
    sockaddr_in addrConn = conn.second;

    while(!stopped){
        char recvbuf[TCP_RECIEVE_LEN];
        int n = recv(sockConn, recvbuf, TCP_RECIEVE_LEN, 0);
        if(n <= 0){
            connVec.erase(sockConn);
            connCnt--;
            perror("TCP: Error Recieving Data!!");
            return NULL;
        }
     //   std::cout << "TCP: Recieved DATA!!" << std::endl;
        FrameAnalysis(recvbuf,n);
    }

}

void TcpCtrl::FrameAnalysis(char *recvbuf, int n)
{
    char header1 = 0xFF; char header2 = 0xFE;
    if((recvbuf[0] == header1) && (recvbuf[1] == header2)){ //frame header
         switch (recvbuf[3]) {
         case ORD1_MOTOR:
             switch (recvbuf[4]) {
             case MOTOR_INFO:
                 break;
             case MOTOR_ORDER:
                 for(int i = 0; i<8 ;i++){
                //      memcpy(&g::can.motorvec[i].pwm,&recvbuf[5+i*4],4);
                      memcpy(&g::motorvec[i].pwm,&recvbuf[5+i*4],4);
                 }
                 std::cout <<"Recieved New Motor Order" <<std::endl;
                 break;
             default:
                 break;
             }
             break;
         case ORD1_PLATFORM:
             switch (recvbuf[4]) {
             case PLATFORM_INFO:
                break;
             case PLATFORM_POS_ORDER:
                 memcpy(&g::plat.TargetPosition1,&recvbuf[5],4);
                 memcpy(&g::plat.TargetPosition2,&recvbuf[9],4);
                 memcpy(&g::plat.TargetPosition3,&recvbuf[13],4);
                 std::cout <<"Recieved New Platform Order" <<std::endl;
                 break;
             case PLATFORM_SET_XYSPEED:
                 memcpy(&g::plat.AxisSpeedXY,&recvbuf[5],4);
                 break;
             case PLATFORM_SET_ZSPEED:
                 memcpy(&g::plat.AxisSpeedZ,&recvbuf[5],4);
                 break;
             case PLATFORM_SET_WSPEED:
                 memcpy(&g::plat.WireFeedSpeed,&recvbuf[5],4);
                 break;
             case PLATFORM_SET_XZERO:
                 g::plat.SetZeroFlag = 0x01;
                 std::cout << "TCP:recieved set x zero!" << std::endl;
                 break;
             case PLATFORM_SET_YZERO:
                 g::plat.SetZeroFlag = 0x02;
                 break;
             case PLATFORM_SET_ZZERO:
                 g::plat.SetZeroFlag = 0x03;
                 break;
             default:
                 break;
             }
             break;
         case ORD1_XSENS:
             switch (recvbuf[4]) {
             case XSENS_DATA:
                 break;
             default:
                 break;
             }
             break;
         case ORD1_KELLER:
             switch (recvbuf[4]) {
             case KELLER_DATA:
                 break;
             default:
                 break;
             }
             break;
         case ORD1_ALT:
             switch (recvbuf[4]) {
             case ALT_DATA:
                 break;
             default:
                 break;
             }
             break;
         case ORD1_TEMP:
             switch (recvbuf[4]) {
             case TEMP_DATA:
                 break;
             case TEMP_ORDER:
                 g::can.btemp = true;
                 std::cout <<"Recieved New Temp Order" <<std::endl;
                 break;
             default:
                 break;
             }
             break;
         case ORD1_LIGHT:
             switch (recvbuf[4]) {
             case LIGHT_DATA1:
                 break;
             case LIGHT_SET1:
                 g::light.isnew = 1;
                 g::light.brightness[0] = recvbuf[5];
                 g::light.start();
                 break;
             case LIGHT_DATA2:
                 break;
             case LIGHT_SET2:
                 g::light.isnew = 2;
                 g::light.brightness[1] = recvbuf[5];
                 g::light.start();
                 break;
             default:
                 break;
             }
         default:
             break;
         }
     }
}

char TcpCtrl::checksum(char *sendbuf, int n)
{

}
