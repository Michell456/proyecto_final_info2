#ifndef JUGADOR1_H
#define JUGADOR1_H

#include <QPainter>
#include <QRect>
#include <QPixmap>
#include <QKeyEvent>
#include <QObject>
#include <QTimer>

class jugador1 : public QObject{
    Q_OBJECT
public:

    enum Estado {
        quieto,
        caminandoX,
        caminandoY,
    };

    jugador1(QObject *parent = nullptr);
    ~jugador1();

    void update(const QSize &tamanioVentana);
    void draw(QPainter &painter);
    void moverDerecha();
    void moverIzquierda();
    void parar();

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    QRect getRect() const;

    void setPosicion(int x, int y);
    QPoint getPosicion() const;

private:
    float x, y;
    float velocidad;
    bool moverIzq;
    bool moverDer;
    bool moverArriba;
    bool moverAbajo;

    void cargarSprites();
    void cambiarFrame();
    QVector<QPixmap> spritesCaminandoDerecha;
    QVector<QPixmap> spritesCaminandoIzquierda;
    QPixmap spriteQuieto;

    int frameActual;
    int direccion; // 1: derecha, -1: izquierda, 2: arriba, -2: abajo
    Estado estado;
    QPoint posicion;

    QTimer *timerAnimacion;
    bool teclaPresionada;

    int ancho = 40;
    int alto = 60;
};


#endif // JUGADOR1_H
