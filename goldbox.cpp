#include "GoldBox.h"
#include <QRandomGenerator>

GoldBox::GoldBox(QObject *parent, int minCoins, int maxCoins)
    : QObject{parent}, coins(0), isOpen(false), isLooted(false)
{
    setRandomCountCoins(minCoins, maxCoins);
}

bool GoldBox::wasOpened() const
{
    return isOpen;
}

int GoldBox::getCountCoins() const
{
    return coins;
}

void GoldBox::setCountCoins(const int countCoins)
{
    coins = countCoins;
}

void GoldBox::setIsOpen(const bool wasOpened)
{
    isOpen = wasOpened;
}

void GoldBox::setIsLooted(const bool wasLooted)
{
    isLooted = wasLooted;
}

void GoldBox::setRandomCountCoins(int minCoins, int maxCoins)
{
    if (minCoins >= maxCoins) {
        coins = minCoins;
    } else {
        coins = QRandomGenerator::global()->bounded(minCoins, maxCoins);
    }
}
