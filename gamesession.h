#ifndef GAMESESSION_H
#define GAMESESSION_H

#include "GoldBox.h"
#include "TcpServer.h"

#include <QObject>
#include <QVector>
#include <QElapsedTimer>

class GameSession : public QObject
{
    Q_OBJECT

public:
    GameSession(QTcpSocket *p1, QTcpSocket *p2, TcpServer* otherServer, QObject *parent = nullptr);

    QTcpSocket* getOpponentSocket(QTcpSocket* playerSocket);
    void update(QTcpSocket *playerSocket, const QString& event);
    void finish(QTcpSocket* playerSocket, const QString& event);

signals:
    void gameFinished(QTcpSocket *player1, QTcpSocket *player2);

private:
    QTcpSocket *player1;
    QTcpSocket *player2;
    TcpServer *server;
    const size_t countGoldBoxes;
    QVector<GoldBox*> goldBoxes;
    const int maxCountTurns;
    int turnP1, turnP2;
    int prevOpenedIndex;
    bool isPrevOpenedP1, lootedP1, lootedP2;
    bool isPrevTurnP1;
    int coinsTakenP1, coinsTakenP2;

    QElapsedTimer gameTimer;

};

#endif // GAMESESSION_H
