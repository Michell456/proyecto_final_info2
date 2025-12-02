#include "zonainfeccion.h"

#include <QDebug>
#include <QtMath>

ZonaInfeccion::ZonaInfeccion(const QSize& tamanioVentana, QObject *parent)
    : QObject(parent)
    , radioActual(15.0f)
    , radioMaximo(90)
    , velocidadCrecimiento(QRandomGenerator::global()->bounded(5, 15) / 10.0f)
    , activa(true)
    , desinfectando(false)
    , tiempoDesinfeccionActual(0.0f)
    , tiempoDesinfeccionNecesario(4)
    , pulso(0.0f)
    , velocidadPulso(QRandomGenerator::global()->bounded(1, 3))
{
    colorInfeccion = QColor(255, 50, 50, 180);
    colorDesinfeccion = QColor(50, 200, 255, 180);

    int margen = qRound(radioMaximo + 50);
    posicion.setX(QRandomGenerator::global()->bounded(margen, tamanioVentana.width() - margen));
    posicion.setY(QRandomGenerator::global()->bounded(margen, tamanioVentana.height() - margen));

}


void ZonaInfeccion::generarPosicionAleatoria(const QSize& tamanioVentana, const QVector<ZonaInfeccion*>& zonasExistentes)
{
    const int maxIntentos = 50;
    int intentos = 0;

    do {
        int margen = qRound(radioMaximo * 1.5);
        posicion.setX(QRandomGenerator::global()->bounded(margen, tamanioVentana.width() - margen));
        posicion.setY(QRandomGenerator::global()->bounded(margen, tamanioVentana.height() - margen));

        intentos++;

        bool demasiadoCerca = false;
        float distanciaMinima = radioMaximo * 2.0f;

        for (ZonaInfeccion* zona : zonasExistentes) {
            if (zona && zona != this) {
                float dx = posicion.x() - zona->getPosicion().x();
                float dy = posicion.y() - zona->getPosicion().y();
                float distancia = qSqrt(dx*dx + dy*dy);

                if (distancia < distanciaMinima) {
                    demasiadoCerca = true;
                    break;
                }
            }
        }

        if (!demasiadoCerca) {
            break;
        }

    } while (intentos < maxIntentos);
}

void ZonaInfeccion::update(float deltaTime)
{
    if (!activa) return;

    pulso += velocidadPulso * deltaTime;
    if (pulso > 2 * M_PI) pulso -= 2 * M_PI;

    if (radioActual < radioMaximo) {
        radioActual += velocidadCrecimiento * deltaTime;
        if (radioActual > radioMaximo) {
            radioActual = radioMaximo;
        }
    }

    if (desinfectando) {
        tiempoDesinfeccionActual += deltaTime;

        radioActual -= velocidadCrecimiento * 2.0f * deltaTime;
        if (radioActual < 0) radioActual = 0;

        if (tiempoDesinfeccionActual >= tiempoDesinfeccionNecesario || radioActual <= 5.0f) {
            activa = false;
        }
    }
}

void ZonaInfeccion::draw(QPainter &painter)
{
    if (!activa) return;

    painter.save();

    QColor colorActual = desinfectando ? colorDesinfeccion : colorInfeccion;

    float escalaPulso = 1.0f + 0.1f * qSin(pulso);
    float radioDibujo = radioActual * escalaPulso;

    QBrush brush(colorActual);
    painter.setBrush(brush);
    painter.setPen(QPen(Qt::black, 2));

    QRectF rect(posicion.x() - radioDibujo,
                posicion.y() - radioDibujo,
                radioDibujo * 2,
                radioDibujo * 2);

    painter.drawEllipse(rect);

    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(colorActual.darker(), 3));
    painter.drawEllipse(rect.adjusted(5, 5, -5, -5));

    if (desinfectando) {
        float porcentaje = tiempoDesinfeccionActual / tiempoDesinfeccionNecesario;

        painter.setPen(QPen(QColor(255, 255, 255, 200), 4));
        int startAngle = 90 * 16;
        int spanAngle = -porcentaje * 360 * 16;

        painter.drawArc(rect.adjusted(10, 10, -10, -10), startAngle, spanAngle);

        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 12, QFont::Bold));
        QString texto = QString("%1%").arg(qRound(porcentaje * 100));
        painter.drawText(rect, Qt::AlignCenter, texto);
    } else {
        painter.setPen(QPen(Qt::white, 3));
        painter.setFont(QFont("Arial", 14, QFont::Bold));
        painter.drawText(rect, Qt::AlignCenter, "!");
    }

    painter.restore();
}

QRect ZonaInfeccion::getRect() const
{
    int margen = 5;
    return QRect(posicion.x() - radioActual - margen,
                 posicion.y() - radioActual - margen,
                 (radioActual + margen) * 2,
                 (radioActual + margen) * 2);
}

void ZonaInfeccion::iniciarDesinfeccion()
{
    if (!desinfectando) {
        desinfectando = true;
        tiempoDesinfeccionActual = 0.0f;
    }
}

void ZonaInfeccion::detenerDesinfeccion()
{
    if (desinfectando) {
        desinfectando = false;
        tiempoDesinfeccionActual *= 0.5f; // Mantener 50% del progreso
    }
}

bool ZonaInfeccion::estaDemasiadoCercaDe(const QPointF& otraPos, float radioOtro) const
{
    float dx = posicion.x() - otraPos.x();
    float dy = posicion.y() - otraPos.y();
    float distancia = qSqrt(dx*dx + dy*dy);

    float distanciaMinima = (radioActual + radioOtro + 60.0f);

    return distancia < distanciaMinima;
}

float ZonaInfeccion::getProgresoDesinfeccion() const {
    if (tiempoDesinfeccionNecesario > 0) {
        return tiempoDesinfeccionActual / tiempoDesinfeccionNecesario;
    }
    return 0.0f;
}
