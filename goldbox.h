#ifndef GOLDBOX_H
#define GOLDBOX_H

#include <QObject>

class GoldBox : public QObject
{
    Q_OBJECT
public:
    //explicit GoldBox(QObject *parent = nullptr);
    explicit GoldBox(QObject *parent = nullptr, int newMinCoins = 1, int newMaxCoins = 10);

    bool wasOpened();

    int getCountCoins();

    void setCountCoins(int countCoins);
    void setIsOpen(bool wasOpened);
    void setRandomCountCoins();

private:
    int minCoins, maxCoins;
    int coins;
    bool isOpen;
};

#endif // GOLDBOX_H
