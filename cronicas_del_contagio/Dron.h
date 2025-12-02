#ifndef DRON_H
#define DRON_H

#include <QPainter>
#include <QPixmap>
#include <QVector>
#include <QPointF>
#include <QKeyEvent>
#include <QObject>
#include <QTimer>

class Dron : public QObject{
    Q_OBJECT
public:
    Dron();

    QRect getRect() const;

    int anchoSprite;
    int altoSprite;

    void update(QSize tamanioVentana);
    void draw(QPainter &p);
    void handleInput(QKeyEvent *e);
    void handleKeyRelease(QKeyEvent *event);

    QPointF getPosition() const { return posicion; }

    float getBateria() const { return bateria; }
    float getBateriaMaxima() const { return bateriaMaxima; }
    void cargarBateria(float cantidad);
    void quitarBateria();

    bool estaInmune() const { return inmune; }
    void activarInmunidad(int milisegundos);
    void finInmunidad() { inmune = false; }

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

    bool inmune;
    QTimer *timerInmunidad;
};

#endif // DRON_H
