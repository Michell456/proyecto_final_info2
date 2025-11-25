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
        caminando,
    };

    jugador1(QObject *parent = nullptr);
    ~jugador1();

    void update(const QSize &tamanioVentana, int velocidadFondo);
    void draw(QPainter &painter);
    void actualizarDireccion();

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    QRect getRect() const;

    void setPosicion(int x, int y);
    QPoint getPosicion() const;

    int consultarVida();
    void quitarVida();
    bool estaInmune() const { return inmune; }
    void activarInmunidad(int milisegundos);
    void finInmunidad() { inmune = false; }

private:

    int vida;
    bool inmune;
    int tiempoInmunidad;
    QTimer *timerInmunidad;

    void cargarSprites();
    void cambiarFrame();
    QVector<QPixmap> spritesCaminandoDerecha;
    QVector<QPixmap> spritesCaminandoIzquierda;
    QPixmap spriteQuieto;
    int anchoSprite;
    int altoSprite;

    int frameActual;
    Estado estado;
    QPoint posicion;
    QPoint direccionActual;
    float velocidad;

    QTimer *timerAnimacion;
    bool teclaDerecha;
    bool teclaIzquierda;
    bool teclaArriba;
    bool teclaAbajo;

};


#endif // JUGADOR1_H
