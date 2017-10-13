#ifndef G_H
#define G_H

#include <vector>
#include <utility>
#include <map>
// using by TCPIP
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

struct Motor
{
    short int   status; // motor status : fault==>-1, stop==>0, start==>2, enable==>1
    float pwm; //-99.99~99.99 %
    short int   speed; //-9999~9999 r/min
    short int   current; //0 ~ 9999mA
};


class g
{
public:
    g();

    /******TCP******/
    static int sockServer;
    static struct sockaddr_in addrServer;
    static std::map<int,sockaddr_in> connVec;
    static int connCnt;

    /******xsens******/
    static float roll;
    static float pitch;
    static float yaw;

    /******can******/
    static std::vector<Motor> motorvec;
    static float cabin_temp;
    static float wall_temp;

    /******platform******/
    // Axises Target Position
    static float TargetPosition1;
    static float TargetPosition2;
    static float TargetPosition3;
    // Axises Speed
    static int AxisSpeedXY;
    static int AxisSpeedZ;
    static int WireFeedSpeed;
    //Axises Current Location
    static float CurrentPosition1;
    static float CurrentPosition2;
    static float CurrentPosition3;
};

#endif // G_H
