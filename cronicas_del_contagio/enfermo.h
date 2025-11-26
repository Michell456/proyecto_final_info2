#ifndef ENFERMO_H
#define ENFERMO_H

#include <QPainter>
#include <QRect>
#include <QPixmap>
#include <QKeyEvent>
#include <QObject>
#include <QTimer>

class enfermo : public QObject{
    Q_OBJECT
public:

    enfermo(QObject *parent = nullptr);
    ~enfermo();

    void update();
    void draw(QPainter &painter);
    QRect getRect() const;

    void setPosicion(int x, int y);
    virtual QPoint getPosicion()const;
    int getAncho();

    void seleccionarSkin();

protected:

    float velocidad;
    int frameActual;
    QPoint posicion;
    void cambiarFrame();

    void cargarSprites();
    QVector<QPixmap> sprites1;
    QVector<QPixmap> sprites2;
    QVector<QPixmap> sprites3;
    QTimer *timerAnimacion;

    int skin;
    int anchoSprite;
    int altoSprite;

};

#endif // ENFERMO_H
