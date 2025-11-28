#ifndef BASEDECARGA_H
#define BASEDECARGA_H

#include <QPainter>
#include <QPointF>
#include <QRectF>
#include <QPixmap>

class BaseDeCarga
{
public:
    BaseDeCarga(QPointF posicion = QPointF(100, 100));

    void draw(QPainter &p);
    bool estaCargando(QPointF posicionDron) const;
    float getRadioCarga() const { return radioCarga; }
    QPointF getPosicion() const { return posicion; }

private:
    QPointF posicion;
    float radioCarga;
    QPixmap spriteBase;

    void cargarSprite();
};

#endif // BASEDECARGA_H
