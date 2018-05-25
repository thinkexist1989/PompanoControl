#ifndef TCPCTRL_H
#define TCPCTRL_H

#include <QThread>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <pthread.h>   //POSIX THREAD HEADER

#include <canctrl.h>

class TcpCtrl : public QThread
{
public:
   TcpCtrl();
protected:
  // void run();
private:
   static volatile bool stopped; // thread stop signal

public:
   /******TCP******/
   static int sockServer;
   static sockaddr_in addrServer;
   static std::map<int,sockaddr_in> connVec;
   static int connCnt;


   void StartServer();
   static void SendData(char* sendbuf, int n);

   static void SendXsensData(float* p_roll, float* p_pitch, float* p_yaw);
   static void SendKellerData(float* p_pressval, float* p_tempval);
   static void SendLightData(int id, short int* pbrightness);
   static void SendAltData(float* p_dis1, float* p_enr1, float* p_cor1, float* p_dis2, float* p_enr2, float* cor2, float *p_watertemp);
   static void SendMotorData();
   static void SendPlatData();
   static void SendTempData();
   static void Init();

   static void* ThreadProcWaitForSocket(void *arg);
   static void* ThreadProc(void *arg);

   static void FrameAnalysis(char* recvbuf, int n);
   static char checksum(char* sendbuf, int n);
};

#endif // TCPCTRL_H
