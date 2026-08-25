#ifndef BLACKJACKCONTROL_H
#define BLACKJACKCONTROL_H

#include <QObject>

class BlackJackControl : public QObject {
    Q_OBJECT
    Q_PROPERTY( QStringList userCardsList READ userCardsList NOTIFY userCardsListChanged )
    Q_PROPERTY( QStringList CPUCardsList READ CPUCardsList NOTIFY CPUCardsListChanged )
    Q_PROPERTY( int userCardsSum READ userCardsSum WRITE setUserCardsSum NOTIFY userCardsSumChanged FINAL )
    Q_PROPERTY( int CPUCardsSum READ CPUCardsSum WRITE setCPUCardsSum NOTIFY cpuCardsSumChanged FINAL )
public:
    BlackJackControl();

    Q_INVOKABLE void startGame();
    Q_INVOKABLE void buy();

    Q_INVOKABLE void userHold();
    Q_INVOKABLE void onRestartGame();

    QStringList imageList() const;
    QStringList userCardsList();

    QStringList CPUCardsList() const;

    int userCardsSum() const;
    void setUserCardsSum( int userCardsSum );

    int CPUCardsSum() const;
    void setCPUCardsSum( int CPUCardsSum );

    int cardIndex();
    void clearCardsList();

signals:
    void error( QString msg );

    void userCardsListChanged();

    void CPUCardsListChanged();

    void userCardsSumChanged();

    void cpuCardsSumChanged();

    void userWon();
    void userLost();
    void userBlackJack();

    void cpuBlackJack();

    void restartGame();

    void releaseBuy();

private slots:
    void checkWinner();
    void refreshCards();

private:
    void resetDeck();
    QString drawCard();
    int calculateHandValue( const QStringList& hand ) const;
    QString cardRank( const QString& cardPath ) const;

    QStringList _imageList;
    QStringList _drawPile;
    QStringList _userCardsList;
    QStringList _CPUCardsList;
    int _userCardsSum;
    int _CPUCardsSum;
    bool _userHeld;
};

#endif
