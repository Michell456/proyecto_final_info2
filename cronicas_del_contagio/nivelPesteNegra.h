#ifndef NIVELPESTENEGRA_H
#define NIVELPESTENEGRA_H

#include "nivel.h"
//#include "jugadorn1.h"
//#include "enemigo.h"
//#include "item.h"

#include <QVector>
#include <QPixmap>

class nivelPesteNegra : public nivel{

public:
    nivelPesteNegra();

    void update() override;
    void draw(QPainter &p) override;
    //void handleInput(QKeyEvent *event) override;
    //void handleKeyRelease(QKeyEvent *event) override;

    bool chequearVictoria() override;
    bool chequearDerrota() override;

private:
    //JugadorN1 jugador;
    //QVector<Enemigo*> enemigos;
    //QVector<Item*> items;

    // fondo con scroll
    QPixmap fondo;
    int fondoX1;
    int fondoX2;
    int velocidadFondo;

};

#endif // NIVELPESTENEGRA_H
