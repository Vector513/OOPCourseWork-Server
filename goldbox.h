#ifndef GOLDBOX_H
#define GOLDBOX_H

#include <QObject>

class GoldBox : public QObject
{
    Q_OBJECT
public:
    explicit GoldBox(QObject *parent = nullptr, int minCoins = 0, int maxCoins = 10);

    bool wasOpened() const;
    int getCountCoins() const;

    void setCountCoins(const int countCoins);
    void setIsOpen(const bool wasOpened);
    void setIsLooted(const bool wasLooted);

private:
    int coins;
    bool isOpen;
    bool isLooted;

    void setRandomCountCoins(int minCoins, int maxCoins);
};

#endif // GOLDBOX_H
