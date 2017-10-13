#ifndef PLATFORMCTRL_H
#define PLATFORMCTRL_H

#include <QThread>
#include <PID.h>
#include <g.h>

class PlatformCtrl : public QThread
{
public:
 //   PlatformCtrl();
    PlatformCtrl(int fd = 0);

  //PID parameter
    float m_kp1;
    float m_ki1;
    float m_kd1;

    float m_kp2;
    float m_ki2;
    float m_kd2;

    float m_kp3;
    float m_ki3;
    float m_kd3;

    // Axises Target Position
    float TargetPosition1;
    float TargetPosition2;
    float TargetPosition3;
    // Axises Speed
    int AxisSpeedXY;
    int AxisSpeedZ;
    int WireFeedSpeed;
    //Axises Current Location
    float CurrentPosition1;
    float CurrentPosition2;
    float CurrentPosition3;


    int OpenCommPort(char *devname);  //open serial port
    int GetPosition(int fd, char *GEP);
    void MotorControl(int fd,int  Speed, char *SVS);
    int ReadPosFile(int &rem_pos1, int &rem_pos2, int &rem_pos3);
    void WritePosFile(int Encoder1, int Encoder2, int Encoder3, char *pos123, char *temp, int fileDes);


private:
    volatile bool stopped; // thread stop signal
    int 	fd;			// Handle of port
protected:
    void run();
};

#endif // PLATFORMCTRL_H
