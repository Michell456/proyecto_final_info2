#ifndef PAJARO_H
#define PAJARO_H

#include <QObject>
#include <QTimer>
#include <QPainter>
#include <QPixmap>

class pajaro : public QObject{
    Q_OBJECT
public:
    pajaro(QObject *parent = nullptr);
    ~pajaro();

    void update();
    void draw(QPainter &painter);
    QRect getRect() const;

    void setPosicion(int x, int y);
    QPoint getPosicion()const;
    int getAncho() { return anchoSprite; }
    int getAltura() { return altoSprite; }

    void setParametrosAleatorios();

private:

    float velocidad;
    int frameActual;
    QPoint posicion;
    void cambiarFrame();

    void cargarSprites();
    QVector<QPixmap> sprites;
    QTimer *timerAnimacion;

    int skin;
    int anchoSprite = 10;
    int altoSprite = 20;

    int movimiento;

    double vx=0;
    double vy=0;
    double x0=0;
    double y0=0;
    double a=0;
    double amplitud=0;
    double frecuencia=0;
    double curva=0;
    double curvaFrecuencia=0;
};

#endif // PAJARO_H
