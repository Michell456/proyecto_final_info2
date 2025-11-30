#ifndef DOCTOR_H
#define DOCTOR_H

#include <QObject>
#include <QPixmap>
#include <QTimer>
#include <QPointF>

class Doctor : public QObject
{
    Q_OBJECT

public:
    Doctor(QObject *parent = nullptr);
    ~Doctor();

    void cargarSpriteSheet();
    void actualizarAnimacionLanzamiento();
    void iniciarLanzamiento();
    void continuarLanzamiento();
    void detenerAnimacion();
    bool estaLanzando() const;

    // Métodos para QPainter
    QPointF getPosicion() const { return posicion; }
    QPixmap getSpriteActual() const { return spriteActual; }
    QRectF getAreaColision() const { return QRectF(posicion, spriteActual.size()); }

    void setPosicion(const QPointF &nuevaPos) { posicion = nuevaPos; }

signals:
    void animacionCompletada();

private:
    QPixmap spriteSheet;
    QPixmap spriteActual;
    QPointF posicion;
    int frameWidth;
    int frameHeight;
    int frameActual;
    int totalFrames;
    int filaLanzamiento;
    QTimer *timerAnimacion;
    bool lanzando;
    bool faseLanzamiento;

    void actualizarFrame();
    void generarSpriteFallback();
};

#endif // DOCTOR_H
