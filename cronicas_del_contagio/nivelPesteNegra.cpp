#include "nivelPesteNegra.h"

nivelPesteNegra::nivelPesteNegra() {

    fondo.load("sprites/nivel_1/fondo_nivel1.png");

    if (fondo.isNull())
        qDebug() << "ERROR: fondo NO cargó";

    fondoX1 = 0;
    fondoX2 = fondo.width();

    velocidadFondo = 1;

    //spawnEnemigos();
    //spawnItems();

}

void nivelPesteNegra::update(){

    fondoX1 -= velocidadFondo;
    fondoX2 -= velocidadFondo;

    if (fondoX1 + fondo.width() < 0) {
        fondoX1 = fondoX2 + fondo.width();
    }
    if (fondoX2 + fondo.width() < 0) {
        fondoX2 = fondoX1 + fondo.width();
    }

}

void nivelPesteNegra::draw(QPainter &p){

    p.drawPixmap(fondoX1, 0, fondo);
    p.drawPixmap(fondoX2, 0, fondo);

/*    jugador.draw(p);
    for (auto e : enemigos) e->draw(p);
    for (auto it : items) it->draw(p);

    p.setPen(Qt::white);
    p.drawText(10, 20, "Vidas: " + QString::number(vidas));
    p.drawText(10, 40, "Hierbas: " + QString::number(hierbasRecogidas)); */

}

//void nivelPesteNegra::handleInput(QKeyEvent *event){}
//void nivelPesteNegra::handleKeyRelease(QKeyEvent *event){}

bool nivelPesteNegra::chequearVictoria(){}
bool nivelPesteNegra::chequearDerrota(){}
