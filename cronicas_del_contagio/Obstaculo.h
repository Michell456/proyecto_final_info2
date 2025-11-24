#ifndef OBSTACULO_H
#define OBSTACULO_H

#include "objetoestatico.h"

class Obstaculo : public ObjetoEstatico
{
public:
    Obstaculo(const QPointF &posicion, const QPixmap &sprite, QGraphicsItem *parent = nullptr);

    bool estaDestruido() const;
    void destruir();
    int getTipo() const;
    QPointF getPosicion() const;
    QPixmap getSprite() const;

private:
    bool destruido;
    int tipo;
};

#endif // OBSTACULO_H
