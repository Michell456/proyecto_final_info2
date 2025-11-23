#include "obstaculo.h"

Obstaculo::Obstaculo(int tipo, const QPointF &posicion, QGraphicsItem *parent)
    : ObjetoEstatico(parent)
    , destruido(false)
    , tipo(tipo)
{
    setPos(posicion);
    setZValue(5);
}

bool Obstaculo::estaDestruido() const
{
    return destruido;
}

void Obstaculo::destruir()
{
    destruido = true;
    setOpacity(0.3);
}

int Obstaculo::getTipo() const
{
    return tipo;
}
