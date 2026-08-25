#ifndef HORSEMODEL_H
#define HORSEMODEL_H

#include <QObject>

class HorseModel : public QObject {
    Q_OBJECT
    Q_PROPERTY( double stars READ stars NOTIFY starsChanged )
    Q_PROPERTY( double bettingOdds READ bettingOdds NOTIFY bettingOddsChanged FINAL )
    Q_PROPERTY( int speed READ speed NOTIFY speedChanged )
    Q_PROPERTY( QString name READ name WRITE setName NOTIFY nameChanged )
    Q_PROPERTY( QString image READ image NOTIFY imageChanged )
    Q_PROPERTY( bool hasHalfStar READ hasHalfStar NOTIFY hasHalfStarChanged FINAL )

public:
    HorseModel();

    double stars() const;

    int speed() const;

    QString name() const;
    void setName( const QString& name );

    QString image();

    bool hasHalfStar() const;

    double bettingOdds() const;

signals:
    void starsChanged();
    void speedChanged();
    void nameChanged();
    void imageChanged();
    void hasHalfStarChanged();
    void bettingOddsChanged();

private:
    void setHasHalfStar( bool hasHalfStar );
    void setImage( QString image );
    void setStars();
    void setSpeed( double stars );
    void setBettingOdds( double stars );

    bool _hasHalfStar;
    int _speed;
    double _bettingOdds;
    double _stars;
    QString _name;
    QString _image;
};

#endif
