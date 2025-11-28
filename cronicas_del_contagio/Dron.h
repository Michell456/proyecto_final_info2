#ifndef DRON_H
#define DRON_H

#include <QPainter>
#include <QPixmap>
#include <QVector>
#include <QPointF>
#include <QKeyEvent>

class Dron
{
public:
    Dron();

    void update();
    void draw(QPainter &p);
    void handleInput(QKeyEvent *e);
    void handleKeyRelease(QKeyEvent *event);

    QPointF getPosition() const { return posicion; }

    float getBateria() const { return bateria; }
    float getBateriaMaxima() const { return bateriaMaxima; }
    void cargarBateria(float cantidad);

private:
    QPointF posicion;
    QPointF velocidad;
    QPointF aceleracion;
    QVector<QPixmap> sprites;
    int frameActual;
    int contadorAnimacion;

    float bateria;
    float bateriaMaxima;
    float consumoMovimiento;
    float consumoBase;

    bool teclas[4]; // [W, S, A, D]

    const float ACELERACION = 0.2f;
    const float FRICCION = 0.96f;
    const float VELOCIDAD_MAXIMA = 10.0f;

    void cargarSprites();
};

#endif // DRON_H
