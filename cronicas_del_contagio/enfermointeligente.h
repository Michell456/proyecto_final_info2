#ifndef ENFERMOINTELIGENTE_H
#define ENFERMOINTELIGENTE_H

#include "enfermo.h"
#include <QPainter>
#include <QRect>
#include <QPixmap>
#include <QKeyEvent>
#include <QObject>
#include <QTimer>

class enfermoInteligente : public enfermo  {
    Q_OBJECT
public:
    enfermoInteligente(QObject *parent = nullptr);
    QPointF posicionF;
    void setPosicion(double x, double y);
    bool estaActivo(){ return activo; }
    void setEstado(bool estado){ activo = estado; }
    void update(const QPoint &posicionJugador);
    void comenzarSeguimiento(int segundos);
private:
    bool activo;
    QTimer *timerDuracion;
};

#endif // ENFERMOINTELIGENTE_H
