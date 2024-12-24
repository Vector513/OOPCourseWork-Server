#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QByteArray>

class TcpServer : public QObject
{
    Q_OBJECT

public:
    TcpServer(QObject *parent = nullptr);
    void startServer(quint16 port);
    void sendData(QTcpSocket *clientSocket, const QByteArray& data);

signals:
    void clientDisconnected(QTcpSocket *clientSocket);
    void newConnection(QTcpSocket *clientSocket);
    void dataReceived(QTcpSocket *clientSocket, QByteArray &data);

private slots:
    void onNewConnection();
    void onDataReceived();
    void onClientDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);

private:
    QTcpServer *server;
};

#endif // TCPSERVER_H
