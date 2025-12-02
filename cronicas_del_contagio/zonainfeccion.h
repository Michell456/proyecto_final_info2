#ifndef ZONAINFECCION_H
#define ZONAINFECCION_H

#include <QObject>
#include <QPainter>
#include <QPixmap>
#include <QTimer>
#include <QRandomGenerator>
#include <QVector>

class ZonaInfeccion : public QObject{
    Q_OBJECT
public:
    ZonaInfeccion(const QSize& tamanioVentana, QObject *parent = nullptr);

    void update(float deltaTime);
    void draw(QPainter &painter);

    QRect getRect() const;
    QPointF getPosicion() const { return posicion; }
    float getRadio() const { return radioActual; }
    float getRadioMaximo() const { return radioMaximo; }
    bool estaActiva() const { return activa; }

    void iniciarDesinfeccion();
    bool estaSiendoDesinfectada() const { return desinfectando; }
    float getTiempoDesinfeccion() const { return tiempoDesinfeccionActual; }
    float getTiempoDesinfeccionNecesario() const { return tiempoDesinfeccionNecesario; }

    void eliminar() { activa = false; }
    bool debeEliminarse() const { return !activa; }

    void detenerDesinfeccion();
    bool estaDemasiadoCercaDe(const QPointF& otraPos, float radioOtro) const;

private:
    void generarPosicionAleatoria(const QSize& tamanioVentana, const QVector<ZonaInfeccion*>& zonasExistentes);

    QPointF posicion;
    float radioActual;
    float radioMaximo;
    float velocidadCrecimiento;

    bool activa;
    bool desinfectando;
    float tiempoDesinfeccionActual;
    float tiempoDesinfeccionNecesario;

    // Para animación visual
    float pulso;
    float velocidadPulso;

    // Colores
    QColor colorInfeccion;
    QColor colorDesinfeccion;
};


#endif // ZONAINFECCION_H
