#ifndef NIVEL_H
#define NIVEL_H

// Abstract

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>

enum class EstadoNivel { // Tipo de dato que solo puede tomar los 3 valores definidos
    jugando,
    ganado,
    perdido
};


class nivel
{
public:
    nivel(): estado(EstadoNivel::jugando), tiempoTranscurrido(0) {}

    virtual void update() = 0;
    virtual void draw(QPainter &p) = 0;
    virtual void handleInput(QKeyEvent *e) = 0;
    virtual void handleKeyRelease(QKeyEvent *event) = 0;

    virtual bool chequearVictoria() = 0;
    virtual bool chequearDerrota() = 0;

    virtual ~nivel() {}

protected:
    EstadoNivel estado;
    int tiempoTranscurrido;
};

#endif // NIVEL_H
