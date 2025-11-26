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
    void setParametrosAleatorios();
    void cargarTexturas();
    void draw(QPainter &painter);
    void setTipo(int tipo_);
private:
    int tipo;
    QPoint posicion;
    QTimer *timerAnimacion;
    int textura;
    int anchoSprite;
    int altoSprite;
    const double gravedad;
    double velocidadY;
    double velocidadX;
    int yf;
    int factorRebote;
    int rebotesRestantes;
    QVector<QPixmap> texturas;
    Estado estado;
};

#endif // ITEM_H
