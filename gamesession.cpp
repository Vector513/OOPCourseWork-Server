#include "GameSession.h"
#include <QDebug>
#include <sstream>

GameSession::GameSession(QTcpSocket *p1, QTcpSocket *p2, TcpServer *otherServer, QObject *parent)
    : QObject(parent),
    player1(p1), player2(p2),
    server(otherServer),
    countGoldBoxes(15),
    goldBoxes(countGoldBoxes),
    maxCountTurns(5),
    turnP1(0), turnP2(0),
    isPrevOpenedP1(false), finishedP1(false), finishedP2(false),
    isPrevTurnP1(false)
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

void GameSession::update(QTcpSocket* playerSocket, const QString& event)
{
    QString response = "";
    std::istringstream stream(event.toStdString());
    std::stringstream ss;
    std::string command;
    stream >> command;
    if (playerSocket == player1) {
        if (turnP1 < maxCountTurns && !finishedP1) {
            if (command == "opened" && !isPrevTurnP1 && !isPrevOpenedP1) {
                size_t i;
                stream >> i;
                if (!goldBoxes[i]->wasOpened()) {
                    response += "opened " + QString::number(i) + " ";
                    server->sendMessage(player2, response);
                    response += QString::number(goldBoxes[i]->getCountCoins());
                    server->sendMessage(player1, response);
                    isPrevOpenedP1 = true;
                }
            }
            else if (command == "finalturn" && isPrevOpenedP1) {
                finishedP1 = true;
                isPrevTurnP1 = true;
                turnP1++;
            }
            else if (command == "continue" && isPrevOpenedP1) {
                isPrevTurnP1 = true;
                turnP1++;
                if (finishedP2) {
                    isPrevTurnP1 = false;
                    isPrevOpenedP1 = false;
                }
            }
        }
    }
    else if (playerSocket == player2) {
        if (turnP2 < maxCountTurns && !finishedP2) {
            if (command == "opened" && isPrevTurnP1 && isPrevOpenedP1 && turnP2 < maxCountTurns) {
                size_t i;
                stream >> i;
                if (!goldBoxes[i]->wasOpened()) {
                    response += "opened " + QString::number(i) + " ";
                    server->sendMessage(player1, response);
                    response += QString::number(goldBoxes[i]->getCountCoins());
                    server->sendMessage(player2, response);
                    isPrevOpenedP1 = false;
                }
            }
            else if (command == "finalturn") {
                finishedP2 = true;
                isPrevTurnP1 = false;
                turnP2++;
            }
            else if (command == "continue") {
                isPrevTurnP1 = false;
                turnP2++;
                if (finishedP1) {
                    isPrevTurnP1 = true;
                    isPrevOpenedP1 = true;
                }
            }
        }
    }
}

void GameSession::finish(QTcpSocket* playerSocket, const QString& event)
{
    emit gameFinished(event, player1, player2);

    if (playerSocket == player1 || playerSocket == player2) {
        player1->disconnectFromHost();
        player2->disconnectFromHost();
    }
}
