#include "basedecarga.h"
#include <cmath>

BaseDeCarga::BaseDeCarga(QPointF posicion)
    : posicion(posicion), radioCarga(50.0f)
{
    cargarSprite();
}

void BaseDeCarga::cargarSprite() {
    spriteBase.load(":/sprites/Nivel3/base_carga.png");

    if (spriteBase.isNull()) {
        spriteBase = QPixmap(100, 100);
        spriteBase.fill(QColor(100, 150, 255));
    }
}

void BaseDeCarga::draw(QPainter &p) {

    if (!spriteBase.isNull()) {
        p.drawPixmap(posicion.x() - 50, posicion.y() - 50, 100, 100, spriteBase);
    } else {
        p.setBrush(QColor(100, 150, 255));
        p.setPen(QPen(Qt::darkBlue, 3));
        p.drawEllipse(posicion, 50, 50);
    }
}

bool BaseDeCarga::estaCargando(QPointF posicionDron) const {
    QPointF diferencia = posicionDron - posicion;
    float distancia = sqrt(diferencia.x() * diferencia.x() + diferencia.y() * diferencia.y());

    return distancia <= radioCarga;
}
