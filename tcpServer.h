#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QByteArray>
#include <QMap>

class TcpServer : public QObject
{
    Q_OBJECT

public:
    TcpServer(QObject *parent = nullptr);
    void startServer(quint16 port);
    void sendMessage(QTcpSocket *clientSocket, const QString &response);

signals:
    void messageReceived(QTcpSocket *clientSocket, const QByteArray &message);
    void clientDisconnected(QTcpSocket *clientSocket);
    void newConnection(QTcpSocket *clientSocket);

private slots:
    void onNewConnection();
    void onDataReceived();
    void onClientDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);
    void onBytesWritten(qint64 bytes);

private:
    QTcpServer *server;
    //QMap<QTcpSocket*, QString> clientSockets;
};

#endif // TCPSERVER_H
