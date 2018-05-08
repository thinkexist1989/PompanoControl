#include <QCoreApplication>
#include <g.h>
//#include <tcpctrl.h>
//#include <xsensctrl.h>
//#include <lightctrl.h>
//#include <altctrl.h>
//#include <kellerctrl.h>
//#include <canctrl.h>
//#include <platformctrl.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    /****tcp****/
    g::tcp.Init();
    /****xsens****/
    g::xsens.OpenCommPort("/dev/ttyO1"); //ttyO1 RS232
    g::xsens.start();
    /****keller****/
    g::keller.InitCommunication();
    g::keller.OpenCommPort("/dev/ttyUSB1");
    g::keller.start();
    /****light****/
    g::light.OpenCommPort("/dev/ttyUSB2");
    g::light.start();
    /****can****/
    g::can.InitCan();
    g::can.start();
    /****altimeter****/
    g::alt.OpenCommPort("/dev/ttyO2"); //ttyO2 is RS485
    g::alt.start();
    /****platform****/
    g::plat.OpenCommPort("/dev/ttyUSB0");
    g::plat.start();

    return a.exec();
}
