#include "goldbox.h"
#include <QRandomGenerator>

GoldBox::GoldBox(QObject *parent, int newMinCoins, int newMaxCoins)
    : QObject{parent}, minCoins(newMinCoins), maxCoins(newMaxCoins)
{
    setRandomCountCoins();
}

void GoldBox::setCountCoins(int countCoins)
{
    coins = countCoins;
}

void GoldBox::setIsOpen(bool wasOpened)
{
    isOpen = wasOpened;
}

void GoldBox::setRandomCountCoins()
{
    coins = QRandomGenerator::global()->bounded(minCoins, maxCoins);
}
