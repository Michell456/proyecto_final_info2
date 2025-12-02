#include "objetoestatico.h"

ObjetoEstatico::ObjetoEstatico(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
{
}

QRectF ObjetoEstatico::getAreaColision() const
{
    return areaColision;
}

void ObjetoEstatico::setAreaColision(const QRectF &newAreaColision)
{
    areaColision = newAreaColision;
}
