#ifndef GAMESESSION_H
#define GAMESESSION_H

#include "GoldBox.h"
#include "TcpServer.h"

#include <QObject>
#include <QVector>

class GameSession : public QObject
{
    Q_OBJECT

public:
    GameSession(QTcpSocket *p1, QTcpSocket *p2, TcpServer* otherServer, QObject *parent = nullptr);

    QTcpSocket* getOpponentSocket(QTcpSocket* playerSocket);
    void update(QTcpSocket *playerSocket, const QString& event);
    void finish(QTcpSocket* playerSocket, const QString& event);

signals:
    void gameFinished(QString result, QTcpSocket *player1, QTcpSocket *player2);

private:
    QTcpSocket *player1;
    QTcpSocket *player2;
    TcpServer *server;
    const size_t countGoldBoxes;
    QVector<GoldBox*> goldBoxes;
    const int maxCountTurns;
    int turnP1, turnP2;
    bool isPrevOpenedP1, finishedP1, finishedP2;
    bool isPrevTurnP1;

};

#endif // GAMESESSION_H
