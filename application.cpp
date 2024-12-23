#include "application.h"
#include <QDebug>
#include <QTimer>

Application::Application(int &argc, char **argv, TcpServer *otherServer, quint16 port)
    : QCoreApplication(argc, argv), server(otherServer)
{
    server->startServer(port);
    connect(server, &TcpServer::newConnection, this, &Application::onNewConnection);
    connect(server, &TcpServer::clientDisconnected, this, &Application::onClientDisconnected);
    connect(server, &TcpServer::dataReceived, this, &Application::onDataReceived);
}

Application::~Application() {};

void Application::onNewConnection(QTcpSocket *clientSocket)
{
    server->sendData(clientSocket, "Application FindOpponentWidget");
    clientSockets[clientSocket] = clientSocket->peerAddress().toString();
    if (clientSockets.size() % 2 == 0) {
        GameSession* session = new GameSession(waitingClientSocket, clientSocket, server);
        gameSessions[waitingClientSocket] = session;
        gameSessions[clientSocket] = session;

        server->sendData(clientSocket, "Application OpponentFound");
        server->sendData(waitingClientSocket, "Application OpponentFound");

        waitingClientSocket = nullptr;

        connect(session, &GameSession::gameFinished, this, &Application::onGameFinished);
    }
    else {
        waitingClientSocket = clientSocket;
    }
}

void Application::onClientDisconnected(QTcpSocket *clientSocket)
{
    qDebug() << "onClientDisconnected";
    if (clientSocket != waitingClientSocket) { //&& clientSockets.contains(clientSocket)) {
        gameSessions[clientSocket]->finish(clientSocket, "Disconnected");
    }
    else {
        clientSockets.remove(clientSocket);
    }
}

void Application::onGameFinished(QTcpSocket *player1, QTcpSocket *player2)
{
    qDebug() << "onGameFinished";
    delete gameSessions[player1];
    gameSessions.remove(player1);
    gameSessions.remove(player2);
    clientSockets.remove(player1);
    clientSockets.remove(player2);
}

void Application::onDataReceived(QTcpSocket *clientSocket, QByteArray &data)
{
    qDebug() << "Получено сообщение от клиента:" << data;
    processData(clientSocket, data);
}

void Application::processData(QTcpSocket *clientSocket, QByteArray& data)
{
    if (data.startsWith("GameSession")) {
        data = data.mid(data.indexOf(' ') + 1);

        if (gameSessions.contains(clientSocket)) {
            gameSessions[clientSocket]->update(clientSocket, QString::fromUtf8(data));
        }
    }
}
