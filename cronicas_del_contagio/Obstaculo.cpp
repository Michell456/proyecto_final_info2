#include "obstaculo.h"

Obstaculo::Obstaculo(const QPointF &posicion, const QPixmap &sprite, QGraphicsItem *parent)
    : ObjetoEstatico(parent)
    , destruido(false)
    , tipo(0)
{
    setPos(posicion);
    setPixmap(sprite);
    setZValue(5);

    areaColision = QRectF(0, 0, sprite.width(), sprite.height());

    qDebug() << "Obstáculo creado - Pos:" << posicion
             << "Tamaño sprite:" << sprite.size()
             << "Área colisión:" << areaColision;
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

QPointF Obstaculo::getPosicion() const
{
    return pos();
}

QPixmap Obstaculo::getSprite() const
{
    return pixmap();
}
