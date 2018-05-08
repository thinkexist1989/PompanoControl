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


#include <altctrl.h>
#include <canctrl.h>
#include <kellerctrl.h>
#include <lightctrl.h>
#include <platformctrl.h>
#include <tcpctrl.h>
#include <xsensctrl.h>



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

    /******keller******/
    static float pressval; // pressure value
    static float tempval;  // tempreture value

    /******light******/
    static short int brightness[2]; //current brightness value from 0 to 76==> 0x00 ~ 0x4c
    static short int lighttemp[2];

    /******altimeter******/
    static float distance[2];
    static float energy[2];
    static float correlation[2];
    static float temperature[2];
    static float watertemp;

    static LightCtrl light;
    static AltCtrl   alt;
    static CanCtrl   can;
    static KellerCtrl keller;
    static PlatformCtrl plat;
    static TcpCtrl    tcp;
    static XsensCtrl xsens;
};

#endif // G_H
