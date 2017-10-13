#include "g.h"

g::g(){}

/***    tcpip    ***/
int g::sockServer;
struct sockaddr_in g::addrServer;

//std::vector<std::pair<int, sockaddr_in> > g::connVec;
std::map<int,sockaddr_in> g::connVec;
int g::connCnt = 0;

/***    xsens    ***/
float g::roll = 0;
float g::pitch = 0;
float g::yaw = 0;

/***    can    ***/
std::vector<Motor> g::motorvec(8);
float g::cabin_temp;
float g::wall_temp;

/******platform******/
// Axises Target Position
float g::TargetPosition1;
float g::TargetPosition2;
float g::TargetPosition3;
// Axises Speed
int g::AxisSpeedXY;
int g::AxisSpeedZ;
int g::WireFeedSpeed;
//Axises Current Location
float g::CurrentPosition1;
float g::CurrentPosition2;
float g::CurrentPosition3;
