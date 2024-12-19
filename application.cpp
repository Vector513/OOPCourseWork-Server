#include "application.h"
#include <QDebug>
#include <sstream>

Application::Application(int &argc, char **argv, TcpServer *otherServer, quint16 port)
    : QCoreApplication(argc, argv), server(otherServer)
{
    server->startServer(port);
    connect(server, &TcpServer::newConnection, this, &Application::onNewConnection);
    connect(server, &TcpServer::clientDisconnected, this, &Application::onClientDisconnected);
    connect(server, &TcpServer::messageReceived, this, &Application::onMessageReceived);
}

Application::~Application() {};

void Application::onNewConnection(QTcpSocket *clientSocket)
{
    clientSockets[clientSocket] = clientSocket->peerAddress().toString();
    if (clientSockets.size() % 2 == 0) {
        GameSession* session = new GameSession(waitingClientSocket, clientSocket, server);
        gameSessions[waitingClientSocket] = session;
        gameSessions[clientSocket] = session;
        server->sendMessage(clientSocket, "opponentfound");
        server->sendMessage(waitingClientSocket, "opponentfound");
        connect(session, &GameSession::gameFinished, this, &Application::onGameFinished);
    }
    else {
        waitingClientSocket = clientSocket;
    }
}

void Application::onClientDisconnected(QTcpSocket *clientSocket)
{
    qDebug() << "onClientDisconnected\n";
    if (clientSocket != waitingClientSocket && clientSockets.contains(clientSocket)) {
        qDebug() << "stardisc\n";
        gameSessions[clientSocket]->finish(clientSocket, "disconnected");
        qDebug() << "finishdisc\n";
    }
    else {
        clientSockets.remove(clientSocket);
    }
}

void Application::onGameFinished(QString result, QTcpSocket *player1, QTcpSocket *player2)
{
    qDebug() << "onGameFinished\n";
    gameSessions.remove(player1);
    gameSessions.remove(player2);
    clientSockets.remove(player1);
    clientSockets.remove(player2);
}

void Application::onMessageReceived(QTcpSocket *clientSocket, const QByteArray &message)
{
    qDebug() << "Получено сообщение от клиента:" << message;
    QString messageStr = QString::fromUtf8(message);
    processMessage(clientSocket, messageStr);
}

void Application::processMessage(QTcpSocket *clientSocket, const QString &message)
{
    if (gameSessions.contains(clientSocket)) {
        gameSessions[clientSocket]->update(clientSocket, message);
    }
}
