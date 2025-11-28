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
{

    frameActual = 0;
    velocidad = 2;
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

/*void enfermoInteligente::update(const QPoint &posicionJugador) {

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

        posicion = posicionF.toPoint();
    }
    else{
        velocidad = 3;
        int nuevaX = posicionF.x() + (velocidad * -1);
        posicionF.setX(nuevaX);
        posicion = posicionF.toPoint();
    }
}*/

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
        // Solo movimiento base hacia izquierda (ya se aplicó arriba)
        break;
    }
}

void enfermoInteligente::comportamientoCaminando(jugador1* jugador, int cantidadItemsJugador, const QList<item*>& itemsEnSuelo) {
    // SOLO tomar decisiones si está activo
    if(!activo) return;

    // 1. EVALUAR OPCIONES
    item* itemCercano = encontrarItemMasCercano(itemsEnSuelo);
    double distanciaAlJugador = QLineF(posicionF, jugador->getPosicion()).length();
    double distanciaAlItem = itemCercano ? QLineF(posicionF, itemCercano->getPosicion()).length() : 9999;

    qDebug() << "Caminando - Dist Jugador:" << distanciaAlJugador << "Dist Item:" << distanciaAlItem;

    // 2. TOMAR DECISIÓN - ORDEN DE PRIORIDAD
    if(jugador->getInmunidadInteligente() == false) {
        // PRIORIDAD 1: Items muy cercanos
        if(itemCercano && distanciaAlItem < 100) {
            qDebug() << "Item MUY cercano - YendoAItem";
            estado = YendoAItem;
            itemObjetivo = itemCercano;
            return;
        }
        // PRIORIDAD 2: Jugador con muchos items y cercano
        else if(cantidadItemsJugador >= umbralRobo && distanciaAlJugador < 200) {
            qDebug() << "Jugador con items - Persiguiendo";
            estado = PersiguiendoJugador;
            return;
        }
        // PRIORIDAD 3: Items más cerca que jugador
        else if(itemCercano && distanciaAlItem < distanciaAlJugador && distanciaAlItem < 250) {
            qDebug() << "Item más cercano - YendoAItem";
            estado = YendoAItem;
            itemObjetivo = itemCercano;
            return;
        }
    }

    // Si no hay oportunidades interesantes, sigue caminando
    qDebug() << "Sin oportunidades - Sigo Caminando";
}

void enfermoInteligente::comportamientoPersiguiendo(jugador1* jugador) {
    // Movimiento hacia el jugador
    double dx = jugador->getPosicion().x() - posicionF.x();
    double dy = jugador->getPosicion().y() - posicionF.y();

    double distancia = sqrt(dx*dx + dy*dy);

    // Si jugador se volvió inmune, dejar de perseguir
    if(jugador->getInmunidadInteligente()) {
        qDebug() << "Jugador se volvió inmune - Volviendo a Caminar";
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

    qDebug() << "Persiguiendo - Distancia:" << distancia;

    // VERIFICAR SI ALCANZÓ AL JUGADOR PARA ROBAR
    if(distancia < 30) {
        qDebug() << "¡ROBO EXITOSO!";
        logroRobar = true;
        robosExitosos++;
        aplicarBoostVelocidad();
        estado = Huyendo;

        // Aquí deberías quitarle un item al jugador
        // jugador->quitarItem();
    }
    // Si el jugador se alejó demasiado, dejar de perseguir
    else if(distancia > 400) {
        qDebug() << "Jugador muy lejos - Volviendo a Caminar";
        estado = Caminando;
    }
}

void enfermoInteligente::comportamientoYendoAItem() {
    if(!itemObjetivo) {
        qDebug() << "Item objetivo perdido - Volviendo a Caminar";
        estado = Caminando;
        return;
    }

    // Movimiento hacia el item
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

    qDebug() << "YendoAItem - Distancia:" << distancia;

    // VERIFICAR SI ALCANZÓ EL ITEM
    if(distancia < 25) {
        qDebug() << "¡ITEM TOMADO!";
        tomarItem();
        estado = Huyendo;
    }
    // Si el item desapareció o está muy lejos, dejar de perseguirlo
    else if(distancia > 300) {
        qDebug() << "Item muy lejos - Volviendo a Caminar";
        estado = Caminando;
        itemObjetivo = nullptr;
    }
}

void enfermoInteligente::comportamientoHuyendo() {
    // Movimiento rápido hacia la izquierda
    int nuevaX = posicionF.x() + ((velocidad * 1.5) * -1); // 50% más rápido
    posicionF.setX(nuevaX);
    posicion = posicionF.toPoint();
}

item* enfermoInteligente::encontrarItemMasCercano(const QList<item*>& itemsEnSuelo) {
    item* masCercano = nullptr;
    double distanciaMinima = 9999;

    for(item* item : itemsEnSuelo) {
        double distancia = QLineF(posicionF, item->getPosicion()).length();
        if(distancia < distanciaMinima) {
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
    velocidad = velocidad * (1.0f + boostVelocidad);
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
}
