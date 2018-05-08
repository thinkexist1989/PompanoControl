#ifndef CANCTRL_H
#define CANCTRL_H

#include <QThread>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include <vector>
#include <bitset>

#define CAN_INIT_OK     0
#define CAN_INIT_FAIL   -1

struct Motor
{
    short int   status; // motor status : fault==>-1, stop==>0, start==>2, enable==>1
    float pwm; //-99.99~99.99 %
    short int   speed; //-9999~9999 r/min
    short int   current; //0 ~ 9999mA
};



class CanCtrl : public QThread
{
public:
    CanCtrl();
protected:
    void run();
private:
    static volatile bool stopped; //线程停止标志
public:
    static std::bitset<8> bs; //bits = 00000001  then motor1's data is new
    static struct sockaddr_can addr;
    static struct can_frame frame;
    static struct can_frame send_frame;
    static struct ifreq ifr;
    static int s;

    static bool btemp;

    static std::vector<Motor> motorvec;

    static float cabin_temp;
    static float wall_temp;

    static std::bitset<8> bsCtrl;
    static int cntarray[8];


    //inline std::vector<Motor>& get_motorvec() {return motorvec;}
    inline float get_cabin_temp() {return cabin_temp;}
    inline float get_wall_temp() {return wall_temp;}
    inline int sgn(float a) {return a >= 0?1:-1;}

    static void motorctrl(); //run motorctrl can send the Motor.pwm to motors
    static void motorctrl(int id, float pwm);
    static void check();
    static void frameanalysis();
    static void motorstart();
    static void motorstop();
    static void get_motor_info();

    static void get_temp();

    int InitCan();
    static void *RecvProc(void *arg);
    static void* SendProc(void *arg);
};


#endif // CANCTRL_H
