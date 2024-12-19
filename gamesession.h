#ifndef GAMESESSION_H
#define GAMESESSION_H

#include "GoldBox.h"

#include <QObject>
#include <QTcpSocket>
#include <QVector>

class GameSession : public QObject
{
    Q_OBJECT

public:
    QTcpSocket *player1;
    QTcpSocket *player2;
    QString status;

    GameSession(QTcpSocket *p1, QTcpSocket *p2, QObject *parent = nullptr);

    QTcpSocket* getOpponentSocket(QTcpSocket* playerSocket);
    void finish(QTcpSocket* playerSocket, QString event);

signals:
    void gameFinished(QString result, QTcpSocket *player1, QTcpSocket *player2); // Раскомментирован сигнал

private:
    const size_t countGoldBoxes = 15;
    QVector<GoldBox*> goldBoxes;

};

#endif // GAMESESSION_H
