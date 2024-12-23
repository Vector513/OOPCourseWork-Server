#include "tcpServer.h"
#include <QDebug>

TcpServer::TcpServer(QObject *parent)
    : QObject(parent)
    , server(new QTcpServer(this))
{
    connect(server, &QTcpServer::newConnection, this, &TcpServer::onNewConnection);
}

void TcpServer::startServer(quint16 port)
{
    if (server->listen(QHostAddress::Any, port)) {
        qDebug() << "Сервер запущен на порту" << port;

        const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
        for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
            if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
                qDebug() << "Сервер слушает на IP-адресе:" << address.toString();
        }
    } else {
        qCritical() << "Ошибка запуска сервера!";
    }
}

void TcpServer::sendData(QTcpSocket *clientSocket, const QByteArray& data)
{
    if (clientSocket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "Отправка данных:" << data;

        // Добавление разделителя (например, символ новой строки)
        QByteArray dataWithDelimiter = data + ";";

        clientSocket->write(dataWithDelimiter);

        if (!clientSocket->flush()) {
            qWarning() << "Не удалось отправить данные немедленно: " << clientSocket->errorString();
        } else {
            qDebug() << "Данные отправлены!";
        }
    } else {
        qWarning() << "Сокет отключен. Не удалось отправить данные: " << data;
    }
}


void TcpServer::onNewConnection()
{
    QTcpSocket *clientSocket = server->nextPendingConnection();

    connect(clientSocket, &QTcpSocket::readyRead, this, &TcpServer::onDataReceived);
    connect(clientSocket, &QTcpSocket::disconnected, this, &TcpServer::onClientDisconnected);
    connect(clientSocket, &QTcpSocket::errorOccurred, this, &TcpServer::onErrorOccurred);

    emit newConnection(clientSocket);

    qDebug() << "Новый клиент подключился:" << clientSocket->peerAddress().toString();
}

void TcpServer::onDataReceived()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (!clientSocket)
        return;

    QByteArray data = clientSocket->readAll();

    if (data.isEmpty()) {
        qWarning() << "Нет данных от клиента, возможно, клиент отключился.";
    } else {
        emit dataReceived(clientSocket, data);
    }
}

void TcpServer::onClientDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (!clientSocket)
        return;

    emit clientDisconnected(clientSocket);

    clientSocket->deleteLater();
}

void TcpServer::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (!clientSocket)
        return;

    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        break;
    case QAbstractSocket::ConnectionRefusedError:
        break;
    default:
        break;
    }

    emit clientDisconnected(clientSocket);

    clientSocket->deleteLater();
}
