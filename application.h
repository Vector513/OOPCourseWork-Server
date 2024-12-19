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
    void onMessageReceived(QTcpSocket *clientSocket, const QByteArray &message);
    void onNewConnection(QTcpSocket *clientSocket);
    void onClientDisconnected(QTcpSocket *clientSocket);

    void onGameFinished(QString result, QTcpSocket *player1, QTcpSocket *player2);

private:
    void processMessage(QTcpSocket *clientSocket, const QString &message);

    TcpServer* server;
    QTcpSocket* waitingClientSocket;
    //QQueue<QTcpSocket*> waitingClientSockets;
    QMap<QTcpSocket*, QString> clientSockets;
    QMap<QTcpSocket*, GameSession*> gameSessions;
};

#endif // APPLICATION_H
