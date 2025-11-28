#include "enfermointeligente.h"
#include <cstdlib>
#include <ctime>
#include <QDebug>
#include <QDir>
#include <QObject>
#include "jugador1.h"

enfermoInteligente::enfermoInteligente(QObject *parent)
    : enfermo(parent)
    , activo(false)
    , estado(Caminando)
    , umbralRobo(3)  // Persigue si jugador tiene 3+ items
    , robosExitosos(0)
    , robosFallidos(0)
    , boostVelocidad(0.0f)
    , itemObjetivo(nullptr)
    , logroRobar(false)
    , cuantoRobar(2)
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
    connect(timerDuracion, &QTimer::timeout, [this]() {
        if (logroRobar) {
            estado = Huyendo;
        } else {
            estado = Autonomo;
            reducirBoostVelocidad();
        }
    });
}

void enfermoInteligente::setPosicion(double x, double y) {
    posicionF.setX(x); //  Desplazamiento (decimal)
    posicionF.setY(y); // Desplazamiento (decimal)
    posicion.setX(x); // Dibujo (redondeado)
    posicion.setY(y); // Dibujo (redondeado)
}

void enfermoInteligente::comenzarActividad(int segundos){
    activo = true;
    timerDuracion->start(segundos * 1000);
}

void enfermoInteligente::update(jugador1* jugador, int cantidadItemsJugador, const QList<item*>& itemsEnSuelo) {
    if(estado != PersiguiendoJugador && estado != YendoAItem) {
        posicionF.setX(posicionF.x() - velocidad);
        posicion = posicionF.toPoint();
    }

    switch(estado) {
    case Caminando:
        comportamientoCaminando(jugador, cantidadItemsJugador, itemsEnSuelo);
        break;
    case PersiguiendoJugador:
        comportamientoPersiguiendo(jugador);
        break;
    case YendoAItem:
        comportamientoYendoAItem();
        break;
    case Huyendo:
        comportamientoHuyendo();
        break;
    case Autonomo:
        // movimiento base a la izquierda
        break;
    }
}

void enfermoInteligente::comportamientoCaminando(jugador1* jugador, int cantidadItemsJugador, const QList<item*>& itemsEnSuelo) {
    if(!activo) return;

    // evalua opciones
    item* itemCercano = encontrarItemMasCercano(itemsEnSuelo);
    double distanciaAlJugador = QLineF(posicionF, jugador->getPosicion()).length();
    double distanciaAlItem = itemCercano ? QLineF(posicionF, itemCercano->getPosicion()).length() : 9999;

    // toma de decisiones
    if(itemCercano && distanciaAlItem < 80) {
        estado = YendoAItem;
        itemObjetivo = itemCercano;
        return;
    }
    if(jugador->getInmunidadInteligente() == false) {
        if(cantidadItemsJugador >= umbralRobo && distanciaAlJugador < 200) {
            estado = PersiguiendoJugador;
            return;
        }
    }
    if(itemCercano && distanciaAlItem < distanciaAlJugador && distanciaAlItem < 200) {
        estado = YendoAItem;
        itemObjetivo = itemCercano;
        return;
    }
    if(itemCercano && distanciaAlItem < 150) {
        estado = YendoAItem;
        itemObjetivo = itemCercano;
        return;
    }

    // Sigue caminando si no hay elementos cercanos
}

void enfermoInteligente::comportamientoPersiguiendo(jugador1* jugador) {

    objetivoActual = jugador->getPosicion();
    double dx = jugador->getPosicion().x() - posicionF.x();
    double dy = jugador->getPosicion().y() - posicionF.y();

    double distancia = sqrt(dx*dx + dy*dy);

    if(jugador->getInmunidadInteligente()) {
        estado = Caminando;
        return;
    }

    if(distancia > 0) {
        dx = (dx / distancia) * velocidad;
        dy = (dy / distancia) * velocidad;
    }

    posicionF.setX(posicionF.x() + dx);
    posicionF.setY(posicionF.y() + dy);
    posicion = posicionF.toPoint();

    // Robo
    if(distancia < 30) {
        logroRobar = true;
        robosExitosos++;
        aplicarBoostVelocidad();
        estado = Huyendo;

        jugador->quitarItems(cuantoRobar);
    }
    // deja de perseguir si el jugador esta muy lejos
    else if(distancia > 400) {
        estado = Caminando;
    }
}

void enfermoInteligente::comportamientoYendoAItem() {
    if(!itemObjetivo) {
        estado = Caminando;
        return;
    }

    objetivoActual = itemObjetivo->getPosicion();
    QPoint posItem = itemObjetivo->getPosicion();
    double dx = posItem.x() - posicionF.x();
    double dy = posItem.y() - posicionF.y();

    double distancia = sqrt(dx*dx + dy*dy);

    if(distancia > 0) {
        dx = (dx / distancia) * velocidad;
        dy = (dy / distancia) * velocidad;
    }

    posicionF.setX(posicionF.x() + dx);
    posicionF.setY(posicionF.y() + dy);
    posicion = posicionF.toPoint();

    // Toma item del suelo
    if(distancia < 25) {
        tomarItem();
        estado = Huyendo;
    }
    else if(distancia > 300) {
        estado = Caminando;
        itemObjetivo = nullptr;
    }
}

void enfermoInteligente::comportamientoHuyendo() {
    int nuevaX = posicionF.x() + ((velocidad * 1.5) * -1); // 50% más rápido
    posicionF.setX(nuevaX);
    posicion = posicionF.toPoint();
}

item* enfermoInteligente::encontrarItemMasCercano(const QList<item*>& itemsEnSuelo) {
    item* masCercano = nullptr;
    double distanciaMinima = 9999;

    for(item* item : itemsEnSuelo) {
        double distancia = QLineF(posicionF, item->getPosicion()).length();
        if(distancia < distanciaMinima && item->getTipo() != 3) {
            distanciaMinima = distancia;
            masCercano = item;
        }
    }
    return masCercano;
}


void enfermoInteligente::tomarItem() {
    if(itemObjetivo) {
        recogeItem(itemObjetivo);
    }
}

void enfermoInteligente::aplicarBoostVelocidad() {
    boostVelocidad += 0.2f; // +20% de velocidad
    if (velocidad * (1.0f + boostVelocidad) >= 6.5){
        velocidad = velocidad * (1.0f + boostVelocidad);
    }
}

void enfermoInteligente::reducirBoostVelocidad() {
    if(boostVelocidad > 0) {
        boostVelocidad -= 0.1f; // -10% de velocidad
        velocidad = velocidad * (1.0f + boostVelocidad);
    }
}

void enfermoInteligente::resetRobo(){
    logroRobar = false;
}

void enfermoInteligente::verificarEstadoRobo(){
    if(logroRobar == false){
        robosFallidos--;
    }
    reducirBoostVelocidad();
}

void enfermoInteligente::cargarSprites(){

    for(int i = 1; i <= 9; i++) {
        QPixmap sprite(QString("sprites/nivel_1/enemigos/enemigo_1/derecha/%1.png").arg(i));
        if(!sprite.isNull()) {
            sprites1.append(sprite);
        } else {
            qDebug() << "No se pudo cargar sprite:" << QString("%1.png").arg(i);
        }
    }

    for(int i = 1; i <= 9; i++) {
        QPixmap sprite(QString("sprites/nivel_1/enemigos/enemigo_2/derecha/%1.png").arg(i));
        if(!sprite.isNull()) {
            sprites2.append(sprite);
        } else {
            qDebug() << "No se pudo cargar sprite:" << QString("%1.png").arg(i);
        }
    }

    for(int i = 1; i <= 9; i++) {
        QPixmap sprite(QString("sprites/nivel_1/enemigos/enemigo_3/derecha/%1.png").arg(i));
        if(!sprite.isNull()) {
            sprites3.append(sprite);
        } else {
            qDebug() << "No se pudo cargar sprite:" << QString("%1.png").arg(i);
        }
    }
}

void enfermoInteligente::draw(QPainter &painter){
    QPixmap spriteActual;
    int direccion = getDireccionActual();

    if(direccion == -1){
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
    }
    else if (direccion == 1){
        switch (skin) {
        case 1:
            spriteActual = sprites1[frameActual+9];
            break;
        case 2:
            spriteActual = sprites2[frameActual+9];
            break;
        case 3:
            spriteActual = sprites3[frameActual+9];
            break;
        }
    }

    painter.drawPixmap(posicion.x(), posicion.y(), spriteActual);

}

int enfermoInteligente::getDireccionActual() const {
    switch(estado) {
    case PersiguiendoJugador:
        return (objetivoActual.x() > posicionF.x()) ? 1 : -1;

    case YendoAItem:
        if(itemObjetivo) {
            return (itemObjetivo->getPosicion().x() > posicionF.x()) ? 1 : -1;
        }
        return -1;

    case Huyendo:
        return -1;

    case Caminando:
    case Autonomo:
    default:
        return -1;
    }
}
