#ifndef ANT_H
#define ANT_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include "anthill.h"

class Ant : public QGraphicsPixmapItem
{
public:
    explicit Ant(AntHill * antHill);



    AntHill *antHill();
    void setAntHill(AntHill *antHill);


    void advance(int phase);

    qint64 lifeCycles() const;
    void setLifeCycles(const qint64 &lifeCycles);

signals:

public slots:

private:
    AntHill *m_antHill;
    qint64 m_lifeCycles;

};

#endif // ANT_H