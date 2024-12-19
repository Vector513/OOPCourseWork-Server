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

void TcpServer::sendMessage(QTcpSocket *clientSocket, const QString &response)
{
    if (clientSocket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "Отправка данных:" << response;
        clientSocket->write(response.toUtf8());

        if (!clientSocket->waitForBytesWritten(5000)) {
            qWarning() << "Ошибка при отправке данных:" << clientSocket->errorString();
        } else {
            qDebug() << "Данные отправлены!";
        }
    } else {
        qWarning() << "Нет подключения к серверу!";
    }
}

void TcpServer::onNewConnection()
{
    QTcpSocket *clientSocket = server->nextPendingConnection();

    connect(clientSocket, &QTcpSocket::readyRead, this, &TcpServer::onDataReceived);
    connect(clientSocket, &QTcpSocket::disconnected, this, &TcpServer::onClientDisconnected);
    connect(clientSocket, &QTcpSocket::errorOccurred, this, &TcpServer::onErrorOccurred);
    connect(clientSocket, &QTcpSocket::bytesWritten, this, &TcpServer::onBytesWritten);

    emit newConnection(clientSocket);

    qDebug() << "Новый клиент подключился:" << clientSocket->peerAddress().toString();
}

void TcpServer::onDataReceived()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (!clientSocket)
        return;

    QByteArray data = clientSocket->readAll();
    //qDebug() << "Получены данные от клиента" << clientSockets[clientSocket] << ":" << data;

    if (data.isEmpty()) {
        qWarning() << "Нет данных от клиента, возможно, клиент отключился.";
    } else {
        emit messageReceived(clientSocket, data);
        clientSocket->flush();
        //qDebug() << "Ответ отправлен клиенту" << clientSockets[clientSocket];
    }
}

void TcpServer::onClientDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (!clientSocket)
        return;

    emit clientDisconnected(clientSocket);

    //qDebug() << "Клиент отключился:" << clientSockets[clientSocket];
    //clientSockets.remove(clientSocket);
    clientSocket->deleteLater();
}

void TcpServer::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (!clientSocket)
        return;

    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        //qWarning() << "Удалённый хост закрыл соединение:" << clientSockets[clientSocket];
        break;
    case QAbstractSocket::HostNotFoundError:
        //qWarning() << "Хост не найден:" << clientSockets[clientSocket];
        break;
    case QAbstractSocket::ConnectionRefusedError:
        //qWarning() << "Подключение отклонено:" << clientSockets[clientSocket];
        break;
    default:
        //qWarning() << "Ошибка сокета у клиента" << clientSockets[clientSocket] << ":"
        //           << clientSocket->errorString();
        break;
    }

    emit clientDisconnected(clientSocket);

    //clientSockets.remove(clientSocket);
    clientSocket->deleteLater();
}

void TcpServer::onBytesWritten(qint64 bytes)
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (!clientSocket)
        return;

    //qDebug() << bytes << "байт отправлено клиенту" << clientSockets[clientSocket];
}
