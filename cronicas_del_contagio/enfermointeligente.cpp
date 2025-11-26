#include "enfermointeligente.h"
#include <cstdlib>
#include <ctime>
#include <QDebug>
#include <QDir>
#include <QObject>

enfermoInteligente::enfermoInteligente(QObject *parent)
    : enfermo(parent)
    , activo(false)
{

    frameActual = 0;
    velocidad = 3;
    anchoSprite = 20;
    altoSprite = 50;

    timerAnimacion = new QTimer(this);
    connect(timerAnimacion, &QTimer::timeout, this, &enfermoInteligente::cambiarFrame);

    cargarSprites();
    timerAnimacion->start(100);

    timerDuracion = new QTimer(this);
    timerDuracion->setSingleShot(true);
    connect(timerDuracion, &QTimer::timeout, [this]() {activo = false;});
}

void enfermoInteligente::update(const QPoint &posicionJugador) {

    if(activo){
        double dx = posicionJugador.x() - posicionF.x();
        double dy = posicionJugador.y() - posicionF.y();

        double distancia = sqrt(dx*dx + dy*dy);
        if(distancia > 0) {
            dx = (dx / distancia) * velocidad;
            dy = (dy / distancia) * velocidad;
        }

        posicionF.setX(posicionF.x() + dx);
        posicionF.setY(posicionF.y() + dy);

        // actualiza el QPoint entero para dibujar
        posicion = posicionF.toPoint();
    }
    else{
        velocidad = 3;
        int nuevaX = posicionF.x() + (velocidad * -1);
        posicionF.setX(nuevaX);
        posicion = posicionF.toPoint();
    }
}

void enfermoInteligente::setPosicion(double x, double y) {
    posicionF.setX(x); //  Desplazamiento (decimal)
    posicionF.setY(y); // Desplazamiento (decimal)
    posicion.setX(x); // Dibujo (redondeado)
    posicion.setY(y); // Dibujo (redondeado)
}

void enfermoInteligente::comenzarSeguimiento(int segundos){
    activo = true;
    timerDuracion->start(segundos * 1000);
}
