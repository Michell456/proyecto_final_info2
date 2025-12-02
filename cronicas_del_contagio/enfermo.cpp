#include "enfermo.h"
#include <cstdlib>
#include <random>
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
        QPixmap sprite(QString(":/sprites/Nivel1/enemigos/enemigo_1/izquierda/%1.png").arg(i));
        if(!sprite.isNull()) {
            sprites1.append(sprite);
        } else {
            qDebug() << "No se pudo cargar sprite:" << QString("%1.png").arg(i);
        }
    }

    for(int i = 1; i <= 9; i++) {
        QPixmap sprite(QString(":/sprites/Nivel1/enemigos/enemigo_2/izquierda/%1.png").arg(i));
        if(!sprite.isNull()) {
            sprites2.append(sprite);
        } else {
            qDebug() << "No se pudo cargar sprite:" << QString("%1.png").arg(i);
        }
    }

    for(int i = 1; i <= 9; i++) {
        QPixmap sprite(QString(":/sprites/Nivel1/enemigos/enemigo_3/izquierda/%1.png").arg(i));
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

    static std::random_device rd;  // Fuente de entropía (para la semilla)
    static std::mt19937 gen(rd());  // Genera numero
    std::uniform_int_distribution<> dist(1, 3);  // 1-3
    skin = dist(gen);

}

void enfermo::cambiarFrame(){

    if (frameActual >= 9){
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

void enfermo::setVelocidad(float vel){
    velocidad = vel;
}
