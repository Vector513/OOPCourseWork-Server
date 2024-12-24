#include "application.h"

Application::Application(int &argc, char **argv, TcpServer *otherServer, quint16 port)
    : QCoreApplication(argc, argv), server(otherServer)
{
    server->startServer(port);
    connect(server, &TcpServer::newConnection, this, &Application::onNewConnection);
    connect(server, &TcpServer::clientDisconnected, this, &Application::onClientDisconnected);
    connect(server, &TcpServer::dataReceived, this, &Application::onDataReceived);
}

Application::~Application() {}

void Application::onNewConnection(QTcpSocket *clientSocket)
{
    server->sendData(clientSocket, "Application FindOpponentWidget");
    clientSockets[clientSocket] = clientSocket->peerAddress().toString();

    for (QTcpSocket* socket : clientSockets.keys()) {
        QString message = QString("FindOpponentWidget OnlinePlayers %1").arg(clientSockets.size());
        server->sendData(socket, message.toUtf8());
    }

    if (clientSockets.size() % 2 == 0) {
        GameSession* session = new GameSession(waitingClientSocket, clientSocket, server);
        gameSessions[waitingClientSocket] = session;
        gameSessions[clientSocket] = session;

        server->sendData(clientSocket, "Application OpponentFound");
        server->sendData(waitingClientSocket, "Application OpponentFound");

        waitingClientSocket = nullptr;

        connect(session, &GameSession::gameFinished, this, &Application::onGameFinished);
    } else {
        waitingClientSocket = clientSocket;
    }
}

void Application::onClientDisconnected(QTcpSocket *clientSocket)
{
    qDebug() << "onClientDisconnected";
    if (clientSocket != waitingClientSocket) {
        if (gameSessions.contains(clientSocket)) {
            GameSession* session = gameSessions[clientSocket];
            if (session) {
                session->finish(clientSocket, "Disconnected");
            }
        }
    } else {
        waitingClientSocket = nullptr;
    }

    clientSockets.remove(clientSocket);

    for (QTcpSocket* socket : clientSockets.keys()) {
        QString message = QString("FindOpponentWidget OnlinePlayers %1").arg(clientSockets.size());
        server->sendData(socket, message.toUtf8());
    }
}

void Application::onGameFinished(QTcpSocket *player1, QTcpSocket *player2)
{
    if (gameSessions.contains(player1) && gameSessions.contains(player2)) {
        delete gameSessions[player2];
    }
    if (gameSessions.contains(player1)) {
        gameSessions.remove(player1);
    }

    if (gameSessions.contains(player2)) {
        gameSessions.remove(player2);
    }

    if (clientSockets.contains(player1)) clientSockets.remove(player1);
    if (clientSockets.contains(player2)) clientSockets.remove(player2);

    if (player1) player1->disconnectFromHost();
    if (player2) player2->disconnectFromHost();
}

void Application::onDataReceived(QTcpSocket *clientSocket, QByteArray &data)
{
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
