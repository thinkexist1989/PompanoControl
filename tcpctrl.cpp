#include "tcpctrl.h"
#include <iostream>
#include <bitset>
#include <QMutex>

#include <vector>
#include <utility>
#include <map>

#define SERVER_PORT   6666
#define SERVER_IP     "192.168.0.100"
#define TCP_SEND_LEN 108
#define TCP_RECIEVE_LEN 56

TcpCtrl::TcpCtrl(){}

volatile bool TcpCtrl::stopped = false;

void TcpCtrl::Init()
{
    g::sockServer = socket(AF_INET, SOCK_STREAM, 0);
    if(g::sockServer < 0){
        perror("TCP: Error Opening Socket!!");
        return;
    }
    std::cout << "TCP: Open Socket Success!!" <<std::endl;

    bzero(&g::addrServer,sizeof(g::addrServer));
    g::addrServer.sin_family = AF_INET;
    g::addrServer.sin_addr.s_addr = inet_addr(SERVER_IP);
    g::addrServer.sin_port = htons(SERVER_PORT);

    if(bind(g::sockServer, (struct sockaddr*)&g::addrServer, sizeof(g::addrServer)) < 0){
        perror("TCP: Error Binding!!");
        return;
    }

    if(listen(g::sockServer, 5) < 0) {
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
        int sockConn = accept(g::sockServer, (struct sockaddr*)&addrConn, &sin_size);
        if(sockConn < 0){
           perror("TCP: Error Accepting");
           return NULL;
        }
        std::cout << "Get Connection from: " << inet_ntoa(addrConn.sin_addr) <<std::endl;

        std::pair<int, sockaddr_in> nextConn = std::make_pair(sockConn,addrConn);

       // g::connVec.push_back(nextConn);
        g::connVec.insert(nextConn);
        g::connCnt++;
      //  std::cout <<g::connVec.size() <<std::endl;
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
            g::connVec.erase(sockConn);
            g::connCnt--;
            perror("TCP: Error Recieving Data!!");
            return NULL;
        }

        std::cout << "TCP: Recieved DATA!!" << std::endl;
//        if(n == TCP_RECIEVE_LEN){
//            if(recvbuf[3] != 0){ //Recieved New Motor Order

//                can.bsCtrl = recvbuf[3];
//                for(int i = 0; i<8 ;i++){
//                     memcpy(&can.motorvec[i].pwm,&recvbuf[4+i*4],4);
//                 }
//                std::cout <<"Recieved New Motor Order" <<std::endl;
//            }
//            if(recvbuf[36] == 0x01){ //Recieved new three-axises platform Oder
//                for(int i = 0; i < 4 ;i++){
//                    //TODO:
//                }

//                std::cout <<"Recieved New Platform Order" <<std::endl;
//            }
//            if(recvbuf[53] != 0){
//                //TODO:SET LIGHT
//                light.brightness[0] = recvbuf[54];
//                light.brightness[1] = recvbuf[55];
//                light.start(); //thread light is not a while loop

//                std::cout <<"Recieved New Light Order" <<std::endl;
//            }
//        }
    }

}

//void TcpCtrl::run()
//{
//   while(!stopped){
//       socklen_t sin_size = sizeof(struct sockaddr_in);
//       sock_connect_fd = accept(sock_server_fd, (struct sockaddr*)&client_addr, &sin_size);
//       if(sock_connect_fd < 0){
//       //    perror("Error Accepting");
//           return;
//       }
//       std::cout << "Get Connection from: " << inet_ntoa(client_addr.sin_addr) <<std::endl;

//       while(!stopped){
//           char recvbuf[TCP_RECIEVE_LEN];
//           int n = recv(sock_connect_fd, recvbuf, TCP_RECIEVE_LEN, 0);
//           if(n < 0){
//               perror("Error Recieving data!!");
//           }

//           if(n == TCP_RECIEVE_LEN){
//               if(recvbuf[3] != 0){ //Recieved New Motor Order
//                   canmutex.lock();
//                   can.bsCtrl = recvbuf[3];
//                   for(int i = 0; i<8 ;i++){
//                        memcpy(&can.motorvec[i].pwm,&recvbuf[4+i*4],4);
//                    }
//                   canmutex.unlock();

//                   std::cout <<"Recieved New Motor Order" <<std::endl;
//               }
//               if(recvbuf[36] == 0x01){ //Recieved new three-axises platform Oder
//                   for(int i = 0; i < 4 ;i++){
//                       //TODO:
//                   }

//                   std::cout <<"Recieved New Platform Order" <<std::endl;
//               }
//               if(recvbuf[53] != 0){
//                   //TODO:SET LIGHT
//                   light.brightness[0] = recvbuf[54];
//                   light.brightness[1] = recvbuf[55];
//                   light.start(); //thread light is not a while loop

//                   std::cout <<"Recieved New Light Order" <<std::endl;
//               }
//           }
//       }
//   }
//}
