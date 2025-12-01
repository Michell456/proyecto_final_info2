#include "pajaro.h"
#include <QRandomGenerator>
#include <QDebug>
#include <random>
#include <QtMath>

pajaro::pajaro(QObject *parent)
    : QObject(parent)
    , frameActual(0)
    , velocidad(3)
    , anchoSprite(20)
    , altoSprite(50)
{
    timerAnimacion = new QTimer(this);
    connect(timerAnimacion, &QTimer::timeout, this, &pajaro::cambiarFrame);

    cargarSprites();
    timerAnimacion->start(100);
}

pajaro::~pajaro(){
    if(timerAnimacion->isActive()) {
        timerAnimacion->stop();
    }
}

void pajaro::cambiarFrame(){

    if (frameActual >= 8){
        frameActual = 0;
    }
    frameActual = (frameActual + 1) % 8;

}

void pajaro::cargarSprites(){
    for(int i = 1; i <= 8; i++) {
        QPixmap sprite(QString("sprites/Nivel3/pajaro/%1.png").arg(i));
        if(!sprite.isNull()) {
            sprites.append(sprite);
        } else {
            qDebug() << "No se pudo cargar sprite:" << QString("%1.png").arg(i);
        }
    }
}

void pajaro::draw(QPainter &painter){

    QPixmap spriteActual;
    spriteActual = sprites[frameActual];
    painter.drawPixmap(posicion.x(), posicion.y(), spriteActual);

}

void pajaro::update() {
    int nuevoX;
    int nuevoY;

    switch (movimiento) {

    case 1: // Lineal
        nuevoX = posicion.x() + vx;
        nuevoY = posicion.y() + vy;
        posicion.setX(nuevoX);
        posicion.setY(nuevoY);
        break;

    case 2: // Parabolico
        nuevoX = posicion.x() + vx;
        nuevoY = posicion.y() + (a * (posicion.x() - x0)*(posicion.x() - x0));
        posicion.setX(nuevoX);
        posicion.setY(nuevoY);
        break;

    case 3: // Senoidal
        nuevoX = posicion.x() + vx ;
        nuevoY = posicion.y() + y0 + amplitud * qSin(frecuencia * (posicion.x() - x0));
        posicion.setX(nuevoX);
        posicion.setY(nuevoY);
        break;

    case 4: // Curvo
        nuevoX = posicion.x() + vx;
        nuevoY = posicion.y() + vy + curva * qSin(frecuencia);
        posicion.setX(nuevoX);
        posicion.setY(nuevoY);
        break;

    case 5: // Inestable
        double r = -0.5 + 1.0 * QRandomGenerator::global()->bounded(1.0);
        nuevoX = posicion.x() + vx + r;
        nuevoY = posicion.y() + vy + r;
        posicion.setX(nuevoX);
        posicion.setY(nuevoY);
        break;
    }

}

void pajaro::setParametrosAleatorios(){

    static std::random_device rd;  // Fuente de entropía (para la semilla)
    static std::mt19937 gen(rd());  // Genera numero
    std::uniform_int_distribution<> dist(1, 5);  // 1-5
    movimiento = dist(gen);

    x0 = posicion.x();
    y0 = posicion.y();

    a = 0.001 + (0.01 - 0.001) * QRandomGenerator::global()->bounded(1.0);
    amplitud = 30.0 + (150.0 - 30.0) * QRandomGenerator::global()->bounded(1.0);
    frecuencia = 0.01 + (0.05 - 0.01) * QRandomGenerator::global()->bounded(1.0);
    curva = 2.0 + (15.0 - 2.0) * QRandomGenerator::global()->bounded(1.0);
    curvaFrecuencia = 0.05 + (0.2 - 0.05) * QRandomGenerator::global()->bounded(1.0);

    double angulo = QRandomGenerator::global()->bounded(360.0);
    double velocidad = 2.0 + (7.0 - 2.0) * QRandomGenerator::global()->bounded(1.0);
    vx = velocidad * qCos(qDegreesToRadians(angulo));
    vy = velocidad * qSin(qDegreesToRadians(angulo));

}

void pajaro::setPosicion(int x, int y){
    posicion.setX(x);
    posicion.setY(y);
}

QPoint pajaro::getPosicion()const{
    return posicion;
}

QRect pajaro::getRect() const {
    return QRect(posicion.x(), posicion.y(), anchoSprite, altoSprite);
}
