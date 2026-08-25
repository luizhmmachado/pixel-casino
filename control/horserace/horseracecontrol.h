#ifndef HORSERACECONTROL_H
#define HORSERACECONTROL_H

#include <QObject>
#include <QList>
#include <QVariantList>

class HorseRaceControl : public QObject {
    Q_OBJECT
    Q_PROPERTY( QVariantList horsesList READ horsesList WRITE setHorsesList NOTIFY horsesListChanged FINAL )
public:
    HorseRaceControl();

    QVariantList horsesList() const;
    void setHorsesList( QVariantList horsesList );

public slots:
    void startGame();
    void restartRace();

signals:
    void horsesListChanged();
    void gameRestarted();

private:
    QString getRandomName();
    void createHorses();
    QVariantList _horsesList{};
    QStringList _imageList;
};

#endif
