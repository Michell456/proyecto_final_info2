#ifndef ITEM_H
#define ITEM_H

#include <QObject>
#include <QTimer>
#include <QPainter>

class item : public QObject {
    Q_OBJECT
public:

    enum Estado {
        posado,
        rebotando,
    };

    item(QObject *parent = nullptr);
    void update(int velocidadFondo);
    void setParametrosAleatorios(int yfinal);
    void cargarTexturas();
    void draw(QPainter &painter);
    void setTipo(int tipo_);
    int getTipo(){ return tipo;}
    void setPosicion(int x, int y);
    QPoint getPosicion()const;
    QRect getRect() const;
    int getAncho()const{ return anchoItem; }
private:
    int tipo;
    QPoint posicion;
    QTimer *timerAnimacion;
    int textura;
    const double gravedad;
    double velocidadY;
    double velocidadX;
    int yf;
    double coefRestitucion;
    double friccion;
    int rebotesRestantes;
    QVector<QPixmap> texturas;
    Estado estado;
    int anchoItem;
    int altoItem;
};

#endif // ITEM_H
