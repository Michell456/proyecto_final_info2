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

    void update(float dt, const QSize& tamanioVentana);
    void draw(QPainter &painter);
    QRect getRect() const;

    void setPosicion(float x, float y);
    QPointF getPosicion()const;
    int getAncho() { return anchoSprite; }
    int getAltura() { return altoSprite; }

    void setParametrosAleatorios(const QSize& tamanioVentana);

private:

    float velocidad;
    int frameActual;
    QPointF posicion;
    void cambiarFrame();

    void cargarSprites();
    QVector<QPixmap> sprites;
    QTimer *timerAnimacion;
    void inicializarMovimientoDesdeBorde(const QSize& tamanioVentana);

    int skin;
    int anchoSprite = 10;
    int altoSprite = 20;

    int movimiento;

    double vx = 0;
    double vy = 0;
    double x = 0;
    double y = 0;
    double a = 0;
    double amplitud = 0;
    double frecuencia = 0;
    double fase = 0;
    double tiempo = 0;
    float deltaTime = 1.0f/60.0f;

    bool spawnDesdeBorde = true;

};

#endif // PAJARO_H
