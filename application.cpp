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

void Application::onMessageReceived(QTcpSocket *clientSocket, const QByteArray &message)
{
    qDebug() << "Получено сообщение от клиента:" << message;
    QString messageStr = QString::fromUtf8(message);
    processMessage(clientSocket, messageStr);
}

void Application::onNewConnection(QTcpSocket *clientSocket)
{
    clientSockets[clientSocket] = clientSocket->peerAddress().toString();
    if (clientSockets.size() % 2 == 0) {
        GameSession* session = new GameSession(waitingClientSocket, clientSocket);
        gameSessions[waitingClientSocket] = session;
        gameSessions[clientSocket] = session;
        connect(session, &GameSession::gameFinished, this, &Application::onGameFinished);
    }
    else {
        waitingClientSocket = clientSocket;
    }
    /*clientSockets[clientSocket] = clientSocket->peerAddress().toString();
    if (waitingClientSockets.size() % 2 == 1) {
        GameSession* session = new GameSession(waitingClientSockets.head(), clientSocket);
        gameSessions[waitingClientSockets.head()] = session;
        gameSessions[clientSocket] = session;
        waitingClientSockets.dequeue();
    }
    else {
        waitingClientSockets.enqueue(clientSocket);
    }*/
}

void Application::onClientDisconnected(QTcpSocket *clientSocket)
{
    if (clientSocket != waitingClientSocket) {
        gameSessions[clientSocket]->finish(clientSocket, "disconnected");
    }
    else {
        clientSockets.remove(clientSocket);
    }
}

void Application::onGameFinished(QString result, QTcpSocket *player1, QTcpSocket *player2)
{
    gameSessions.remove(player1);
    gameSessions.remove(player2);
    clientSockets.remove(player1);
    clientSockets.remove(player2);
}

void Application::processMessage(QTcpSocket *clientSocket, const QString &message)
{
    /*
    QString response = "";
    std::istringstream stream(message.toStdString());
    std::stringstream ss;
    std::string command;
    stream >> command;

    server->sendMessage(clientSocket, response);
    */
}
