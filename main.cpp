#include <QCoreApplication>
#include <g.h>
#include <tcpctrl.h>
#include <xsensctrl.h>
#include <lightctrl.h>
#include <altctrl.h>
#include <kellerctrl.h>
#include <canctrl.h>
#include <platformctrl.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    /****tcp****/
    TcpCtrl tcp;
    tcp.Init();
    /****xsens****/
    XsensCtrl xsens;
    xsens.OpenCommPort("/dev/ttyO2");
    xsens.start();
    /****keller****/
    KellerCtrl keller;
    keller.InitCommunication();
    keller.OpenCommPort("/dev/ttyUSB1");
    keller.start();
    /****keller****/
    LightCtrl light;
    light.OpenCommPort("/dev/ttyUSB2");
    light.start();
    /****can****/
    CanCtrl can;
    can.InitCan();
    can.start();
    /****platform****/
    PlatformCtrl plat;
    plat.OpenCommPort("/dev/ttyUSB0");
    plat.start();

    return a.exec();
}
