#include "simulation.h"
#include <QDebug>
#include <QRandomGenerator64>
#include <QDateTime>

Simulation * Simulation::instance;

qint64 Simulation::getFoodQueen() const
{
    return m_foodQueen;
}

void Simulation::setFoodQueen(const qint64 &foodQueen)
{
    m_foodQueen = foodQueen;
}

qint64 Simulation::getFoodAnt() const
{
    return m_foodAnt;
}

void Simulation::setFoodAnt(const qint64 &foodAnt)
{
    m_foodAnt = foodAnt;
}

qreal Simulation::getRatioWorkerSoldier() const
{
    return m_ratioWorkerSoldier;
}

void Simulation::setRatioWorkerSoldier(const qreal &ratioWorkerSoldier)
{
    m_ratioWorkerSoldier = ratioWorkerSoldier;
}

qint64 Simulation::getAntLifeTime() const
{
    return m_antLifeTime;
}

void Simulation::setAntLifeTime(const qint64 &antLifeTime)
{
    m_antLifeTime = antLifeTime;
}

qint64 Simulation::getAntLimit() const
{
    return m_antLimit;
}

void Simulation::setAntLimit(const qint64 &antLimit)
{
    m_antLimit = antLimit;
}

qint64 Simulation::getEe() const
{
    return m_ee;
}

void Simulation::setEe(const qint64 &ee)
{
    if(ee >= 0 && ee <=2)
    m_ee = ee;
    QList <QGraphicsItem*> l(this->items());
    foreach (QGraphicsItem * i, l) {
        if(SimulationPixmapItem::isSimulationPixmapItem(i)){
            SimulationPixmapItem* p = (SimulationPixmapItem*) i;
            p->updateEE();
        }
    }
}

Simulation::Simulation(qint64 foodQueen, qint64 foodAnt, qreal ratioWorkerSoldier, qint64 antLifeTime, qint64 antLimit):
    m_foodQueen(foodQueen),
    m_foodAnt(foodAnt),
    m_ratioWorkerSoldier(ratioWorkerSoldier),
    m_antLifeTime(antLifeTime),
    m_antLimit(antLimit),
    m_antHillList()
{
    QGraphicsScene::setBackgroundBrush(QBrush(QPixmap(":/img/resources/background.png"))); //Qt::red
    qDebug() << "Create simulation :";
    qDebug() << "m_foodQueen:" << foodQueen;
    qDebug() << "m_foodAnt:" << foodAnt;
    qDebug() << "m_ratioWorkerSoldier:" << ratioWorkerSoldier;
    qDebug() << "m_antLifeTime:" << antLifeTime;
    qDebug() << "m_antLimit:" << antLimit;
}

Simulation * Simulation::getInstance()
{
    if(nullptr == Simulation::instance){
        Simulation::instance = new Simulation();
    }
    return Simulation::instance;
}

Simulation::~Simulation()
{
    QList<QGraphicsItem*> l(this->items());
    foreach (QGraphicsItem * i, l) {
        delete i;
    }

}

void Simulation::init()
{
    qreal height(10000);
    qreal width(20000);

    AntHill * antHill = new AntHill();

    this->addItem(antHill);
    for(qint64 i(0); i<100;i++ ){
        this->addFood();

    }
    antHill->setPos(this->w()/2.0,this->h()/2.0);
    m_antHillList.append(antHill);
}

void Simulation::advance(int phase)
{
    QGraphicsScene::advance();
    emit updateStats(m_antHillList);

}

void Simulation::createAnt(AntHill * antHill)
{

    qint64 food = antHill->food();
    Ant * ant = nullptr;
    if (food >= m_foodQueen){
        ant = new Queen(antHill);
        antHill->consume(m_foodQueen);
    } else if(antHill->size() < m_antLimit && food >= m_foodAnt) {
        //qreal r = rand();
        /*if (r < m_ratioWorkerSoldier){
            ant = new Worker(antHill);
        } else {
            ant = new Soldier(antHill);
        }*/
        ant = new Worker(antHill);
        antHill->consume(m_foodAnt);
    }

    if (nullptr != ant){
        QGraphicsScene::addItem(ant);
        antHill->growUp();
        qDebug() << "Simulation::createAnt:" << ant;
    }


}

void Simulation::createAntHill(Queen * queen)
{
    QPointF pos = queen->pos();
    QGraphicsScene::removeItem(queen);
    AntHill * antHill = new AntHill();
    antHill->setPos(pos);
    m_antHillList.append(antHill);
    qDebug() << "Simulation::createAntHill:" << antHill;

}

void Simulation::createPheromon(Worker * worker)
{
    Pheromone * pheromone = new Pheromone(worker->antHill());
    this->addItem(pheromone);
    pheromone->setPos(worker->pos());
    qDebug() << "Simulation::createPheromone:" << pheromone;
}

void Simulation::pheromoneOutOfDate(Pheromone * pheromone)
{
    qDebug() << "Simulation::removePheromone:" << pheromone;
    this->removeItem(pheromone);
    delete pheromone;
}


void Simulation::deleteAnt(Ant * ant)
{
    if(ant->lifeCycles() >= m_antLifeTime*25){
        qDebug() << "Simulation::removeAnt:" << ant;
        QGraphicsScene::removeItem(ant);
        delete ant;
    }
}

void Simulation::addFood()
{
    Food * f = new Food(Simulation::rand(this->w()),Simulation::rand(this->h()),Simulation::rand(15.0));
    QList<QGraphicsItem *> l(this->items());

    QRectF r = f->sceneBoundingRect();
    qreal dr = qSqrt(qPow(r.width(),2)+qPow(r.height(),2));
    bool toClose = true;
    while(toClose){
        toClose = false;
        foreach (QGraphicsItem * i, l) {
            QRectF r2 = i->sceneBoundingRect();
            qreal dr2 = qSqrt(qPow(r2.width(),2)+qPow(r2.height(),2));
            if(QLineF(f->pos(),i->pos()).length() < (dr+dr2)/2.0){
                toClose = true;
                f->setPos(Simulation::rand(this->w()+200)-100,Simulation::rand(this->h()+200)-100);
                break;
            }

        }
    }



    this->addItem(f);
}

void Simulation::noMoreFood(Food * food)
{
    qDebug() << "Simulation::removeFood:" << food;
    this->removeItem(food);
    delete food;
}

qreal Simulation::rand(qint64 min, qint64 max)
{
    QRandomGenerator64 rg(QDateTime::currentDateTime().toMSecsSinceEpoch());
    qreal randomReal(qreal(rg.bounded(0,10000000))/10000000.0);
    randomReal *= ((qreal)(qrand()%1000000)/1000000.0) ;
    if(max > min){
        randomReal *= (max - min);
        randomReal += min;
    } else if(max < min){
        randomReal *= (min - max);
        randomReal += max;
    }
    return randomReal;
}

qreal Simulation::w()
{
    return this->sceneRect().width();
}

qreal Simulation::h()
{
    return this->sceneRect().height();
}
