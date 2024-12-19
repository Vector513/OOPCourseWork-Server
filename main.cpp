#include <QCoreApplication>
#include "application.h"
#include "tcpServer.h"

int main(int argc, char *argv[])
{
    TcpServer server;
    QCoreApplication a(argc, argv);
    Application app(argc, argv, &server, 10001);

    return a.exec();
}
