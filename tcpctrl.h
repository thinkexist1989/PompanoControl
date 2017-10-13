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

#include <g.h>  //GLOBAL VARIABLES
#include <pthread.h>   //POSIX THREAD HEADER

class TcpCtrl : public QThread
{
public:
   TcpCtrl();
protected:
  // void run();
private:
   static volatile bool stopped; // thread stop signal
public:
   void StartServer();
   void Send();
   void Init();
   static void* ThreadProcWaitForSocket(void *arg);
   static void* ThreadProc(void *arg);
};

#endif // TCPCTRL_H
