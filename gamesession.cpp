#include "GameSession.h"
#include <QDebug>
#include <QRandomGenerator>

GameSession::GameSession(QTcpSocket *p1, QTcpSocket *p2, QObject *parent)
    : QObject(parent), player1(p1), player2(p2), status("Игра началась"), goldBoxes(countGoldBoxes)
{}

QTcpSocket* GameSession::getOpponentSocket(QTcpSocket* playerSocket)
{
    if (playerSocket == player1)
        return player2;
    else if (playerSocket == player2)
        return player1;
    else
        return nullptr;
}



void GameSession::finish(QTcpSocket* playerSocket, QString event)
{
    status = "Игра завершена";

    emit gameFinished(event, player1, player2);

    if (playerSocket == player1 || playerSocket == player2) {
        player1->disconnectFromHost();
        player2->disconnectFromHost();
        qDebug() << "Game finished. Player disconnected:" << playerSocket->peerAddress().toString();
    }
}
