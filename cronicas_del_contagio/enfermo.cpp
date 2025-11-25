#include "enfermo.h"
#include <cstdlib>
#include <ctime>
#include <QDebug>
#include <QDir>

enfermo::enfermo(QObject *parent)
    : QObject(parent)
    , frameActual(0)
    , velocidad(3)
    , anchoSprite(20)
    , altoSprite(50)
{
    timerAnimacion = new QTimer(this);
    connect(timerAnimacion, &QTimer::timeout, this, &enfermo::cambiarFrame);

    cargarSprites();
    timerAnimacion->start(100);
}

enfermo::~enfermo(){
    if(timerAnimacion->isActive()) {
        timerAnimacion->stop();
    }
}

void enfermo::cargarSprites(){
    for(int i = 1; i <= 9; i++) {
        QPixmap sprite(QString("sprites/nivel_1/enemigos/enemigo_1/izquierda/%1.png").arg(i));
        if(!sprite.isNull()) {
            sprites1.append(sprite);
        } else {
            qDebug() << "No se pudo cargar sprite:" << QString("%1.png").arg(i);
        }
    }

    for(int i = 1; i <= 9; i++) {
        QPixmap sprite(QString("sprites/nivel_1/enemigos/enemigo_2/izquierda/%1.png").arg(i));
        if(!sprite.isNull()) {
            sprites2.append(sprite);
        } else {
            qDebug() << "No se pudo cargar sprite:" << QString("%1.png").arg(i);
        }
    }

    for(int i = 1; i <= 9; i++) {
        QPixmap sprite(QString("sprites/nivel_1/enemigos/enemigo_3/izquierda/%1.png").arg(i));
        if(!sprite.isNull()) {
            sprites3.append(sprite);
        } else {
            qDebug() << "No se pudo cargar sprite:" << QString("%1.png").arg(i);
        }
    }

}

void enfermo::draw(QPainter &painter){

    QPixmap spriteActual;
    switch (skin) {
    case 1:
        spriteActual = sprites1[frameActual];
        break;
    case 2:
        spriteActual = sprites2[frameActual];
        break;
    case 3:
        spriteActual = sprites3[frameActual];
        break;
    }
    painter.drawPixmap(posicion.x(), posicion.y(), spriteActual);

}

void enfermo::seleccionarSkin(){

    std::srand(std::time(nullptr));
    skin = 1 + std::rand() % 3;

}

void enfermo::cambiarFrame(){

    if (frameActual >= sprites1.size()){
        frameActual = 0;
    }
    frameActual = (frameActual + 1) % 9;

}

void enfermo::update(){

    int nuevaX = posicion.x() + (velocidad * -1);
    posicion.setX(nuevaX);

}

void enfermo::setPosicion(int x, int y)
{
    posicion.setX(x);
    posicion.setY(y);
}

int enfermo::getAncho(){
    return anchoSprite;
}

QPoint enfermo::getPosicion() const
{
    return posicion;
}

QRect enfermo::getRect() const {
    return QRect(posicion.x(), posicion.y(), anchoSprite, altoSprite);
}
