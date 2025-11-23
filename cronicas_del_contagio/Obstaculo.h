#ifndef OBSTACULO_H
#define OBSTACULO_H

#include "objetoestatico.h"

class Obstaculo : public ObjetoEstatico
{
public:
    Obstaculo(int tipo, const QPointF &posicion, QGraphicsItem *parent = nullptr);

    bool estaDestruido() const;
    void destruir();
    int getTipo() const;

private:
    bool destruido;
    int tipo;
};

#endif // OBSTACULO_H
