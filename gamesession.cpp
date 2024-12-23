#include "GameSession.h"
#include <QDebug>

GameSession::GameSession(QTcpSocket *p1, QTcpSocket *p2, TcpServer *otherServer, QObject *parent)
    : QObject(parent),
    player1(p1), player2(p2),
    server(otherServer),
    countGoldBoxes(15),
    goldBoxes(countGoldBoxes),
    maxCountTurns(5),
    turnP1(0), turnP2(0),
    prevOpenedIndex(0),
    isPrevOpenedP1(false), lootedP1(false), lootedP2(false),
    isPrevTurnP1(false),
    coinsTakenP1(0), coinsTakenP2(0)
{
    for (int i = 0; i < goldBoxes.size(); ++i) {
        goldBoxes[i] = new GoldBox(this);
    }
    gameTimer.start();

    server->sendData(player1, "GameInfoWidget YourTurn");
}

QTcpSocket* GameSession::getOpponentSocket(QTcpSocket* playerSocket)
{
    return (playerSocket == player1) ? player2 : player1;
}

void GameSession::update(QTcpSocket* playerSocket, const QString& event)
{
    QStringList parts = event.split(' ');
    QString command = parts[0];
    QString response;

    if (playerSocket == player1) {
        if (turnP1 < maxCountTurns && !lootedP1) {
            if (command == "GoldBoxOpened" && !isPrevTurnP1 && !isPrevOpenedP1) {
                bool ok;
                size_t i = parts[1].toUInt(&ok);
                if (!ok || i >= goldBoxes.size()) {
                    qDebug() << "Ошибка: неверный индекс сундука или индекс вне диапазона";
                    return;
                }

                if (!goldBoxes[i]->wasOpened()) {
                    response = "GoldBox" + QString::number(i) + " Opened";
                    server->sendData(player1, response.toUtf8());
                    server->sendData(player2, response.toUtf8());
                    response = "GameInfoWidget Coins";
                    server->sendData(player2, response.toUtf8());
                    response += " " + QString::number(goldBoxes[i]->getCountCoins());
                    server->sendData(player1, response.toUtf8());
                    isPrevOpenedP1 = true;
                    prevOpenedIndex = i;
                    //coinsTakenP1 += goldBoxes[i]->getCountCoins();
                }
            }
            else if (command == "Finish" && isPrevOpenedP1) {
                goldBoxes[prevOpenedIndex]->setIsLooted(true);
                response = "GoldBox" + QString::number(prevOpenedIndex) + " Looted";
                server->sendData(player1, response.toUtf8());
                server->sendData(player2, response.toUtf8());
                response = "GameInfoWidget OpponentLooted";
                server->sendData(player2, response.toUtf8());
                coinsTakenP1 = goldBoxes[prevOpenedIndex]->getCountCoins();
                lootedP1 = true;
                isPrevTurnP1 = true;
                turnP1++;
            }
            else if (command == "Continue" && isPrevOpenedP1 && (turnP1 + 1) < maxCountTurns) {
                response = "GameInfoWidget YourTurn";
                server->sendData(player2, response.toUtf8());
                response = "GameInfoWidget Wait";
                server->sendData(player1, response.toUtf8());
                isPrevTurnP1 = true;
                turnP1++;
                if (lootedP2) {
                    isPrevTurnP1 = false;
                    isPrevOpenedP1 = false;
                }
            }
        }
        if (turnP1 == maxCountTurns) {
            lootedP1 = true;
        }
    }
    else if (playerSocket == player2) {
        if (turnP2 < maxCountTurns && !lootedP2) {
            if (command == "GoldBoxOpened" && isPrevTurnP1 && isPrevOpenedP1) {
                qDebug() << "Ya tut";
                bool ok;
                size_t i = parts[1].toUInt(&ok);
                if (!ok || i >= goldBoxes.size()) {
                    qDebug() << "Ошибка: неверный индекс сундука или индекс вне диапазона";
                    return;
                }

                if (!goldBoxes[i]->wasOpened()) {
                    response = "GoldBox" + QString::number(i) + " Opened";
                    server->sendData(player2, response.toUtf8());
                    server->sendData(player1, response.toUtf8());
                    response = "GameInfoWidget Coins";
                    server->sendData(player1, response.toUtf8());
                    response += " " + QString::number(goldBoxes[i]->getCountCoins());
                    server->sendData(player2, response.toUtf8());
                    isPrevOpenedP1 = false;
                    prevOpenedIndex = i;
                    //coinsTakenP2 += goldBoxes[i]->getCountCoins();
                }
            }
            else if (command == "Finish" && !isPrevOpenedP1) {
                goldBoxes[prevOpenedIndex]->setIsLooted(true);
                response = "GoldBox" + QString::number(prevOpenedIndex) + " Looted";
                server->sendData(player2, response.toUtf8());
                server->sendData(player1, response.toUtf8());
                response = "GameInfoWidget OpponentLooted";
                server->sendData(player1, response.toUtf8());
                coinsTakenP2 = goldBoxes[prevOpenedIndex]->getCountCoins();
                lootedP2 = true;
                isPrevTurnP1 = false;
                turnP2++;
            }
            else if (command == "Continue" && !isPrevOpenedP1 && (turnP2 + 1) < maxCountTurns) {
                response = "GameInfoWidget YourTurn";
                server->sendData(player1, response.toUtf8());
                response = "GameInfoWidget Wait";
                server->sendData(player2, response.toUtf8());
                isPrevTurnP1 = false;
                turnP2++;
                if (lootedP1) {
                    isPrevTurnP1 = true;
                    isPrevOpenedP1 = true;
                }
            }
        }
        if (turnP2 == maxCountTurns) {
            lootedP2 = true;
        }
    }


    if (lootedP1 && lootedP2) {
        finish(player1, "GameOver");
    }
}

void GameSession::finish(QTcpSocket* playerSocket, const QString& event)
{
    qint64 elapsedTimeMs = gameTimer.elapsed();
    int minutes = elapsedTimeMs / 60000;
    int seconds = (elapsedTimeMs % 60000) / 1000;

    if (event == "GameOver") {
        if (coinsTakenP1 > coinsTakenP2) {
            server->sendData(player1, QString("GameResultWidget Winner %1 %2 %3")
                                          .arg(coinsTakenP1 - coinsTakenP2)
                                          .arg(minutes)
                                          .arg(seconds).toUtf8());
            server->sendData(player2, QString("GameResultWidget Loser %1 %2 %3")
                                          .arg(coinsTakenP1 - coinsTakenP2)
                                          .arg(minutes)
                                          .arg(seconds).toUtf8());
        }
        else if (coinsTakenP1 < coinsTakenP2) {
            server->sendData(player1, QString("GameResultWidget Loser %1 %2 %3")
                                 .arg(coinsTakenP2 - coinsTakenP1)
                                 .arg(minutes)
                                 .arg(seconds).toUtf8());
            server->sendData(player2, QString("GameResultWidget Winner %1 %2 %3")
                                          .arg(coinsTakenP2 - coinsTakenP1)
                                          .arg(minutes)
                                          .arg(seconds).toUtf8());
        }
        else {
            server->sendData(player1, QString("GameResultWidget Draw %1 %2")
                                 .arg(minutes)
                                 .arg(seconds).toUtf8());
            server->sendData(player2, QString("GameResultWidget Draw %1 %2")
                                          .arg(minutes)
                                          .arg(seconds).toUtf8());
        }
        player1->disconnectFromHost();
        player2->disconnectFromHost();
    }

    else if (event == "Disconnected") {
        if (playerSocket == player1) {
            server->sendData(player2, QString("GameResultWidget Winner Disconnected %1 %2")
                                          .arg(minutes)
                                          .arg(seconds).toUtf8());
            player2->disconnectFromHost();
        }
        else if (playerSocket == player2) {
            server->sendData(player1, QString("GameResultWidget Winner Disconnected %1 %2")
                                          .arg(minutes)
                                          .arg(seconds).toUtf8());
            player1->disconnectFromHost();
        }
    }

    emit gameFinished(player1, player2);
}

