#ifndef APPLICATION_H
#define APPLICATION_H

#include <QCoreApplication>
#include "TcpServer.h"
#include "GameSession.h"

#include <QQueue>

class Application : public QCoreApplication
{
    Q_OBJECT

public:
    Application(int &argc, char **argv, TcpServer *otherServer, quint16 port);
    ~Application();

private slots:
    void onDataReceived(QTcpSocket *clientSocket, QByteArray& data);
    void onNewConnection(QTcpSocket *clientSocket);
    void onClientDisconnected(QTcpSocket *clientSocket);
    void onGameFinished(QTcpSocket *player1, QTcpSocket *player2);

private:
    void processData(QTcpSocket *clientSocket, QByteArray& data);

    TcpServer* server;
    QTcpSocket* waitingClientSocket;
    QMap<QTcpSocket*, QString> clientSockets;
    QMap<QTcpSocket*, GameSession*> gameSessions;
};

#endif // APPLICATION_H
