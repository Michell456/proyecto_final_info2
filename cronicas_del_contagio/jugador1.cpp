#include "jugador1.h"
#include <QDebug>
#include <QDir>

jugador1::jugador1(QObject *parent)
    : QObject(parent)
    , frameActual(0)
    , direccion(1) // Mirando a la derecha
    , estado(quieto)
    , velocidad(2)
    , teclaPresionada(false)
{
    timerAnimacion = new QTimer(this);
    connect(timerAnimacion, &QTimer::timeout, this, &jugador1::cambiarFrame);

    cargarSprites();
}

jugador1::~jugador1(){
    if(timerAnimacion->isActive()) {
        timerAnimacion->stop();
    }
}

void jugador1::cargarSprites(){
    for(int i = 1; i <= 8; i++) {
        QPixmap sprite(QString("sprites/nivel_1/jugador/corre_derecha/%1.png").arg(i));
        if(!sprite.isNull()) {
            spritesCaminandoDerecha.append(sprite);
        } else {
            qDebug() << "No se pudo cargar sprite:" << QString("%1.png").arg(i);
        }
    }

    for(int i = 1; i <= 8; i++) {
        QPixmap sprite(QString("sprites/nivel_1/jugador/corre_izquierda/%1.png").arg(i));
        if(!sprite.isNull()) {
            spritesCaminandoIzquierda.append(sprite);
        } else {
            qDebug() << "No se pudo cargar sprite:" << QString("%1.png").arg(i);
        }
    }

    spriteQuieto = QPixmap("sprites/nivel_1/jugador/jugador_quieto.png");
    if(spriteQuieto.isNull()) {
        qDebug() << "Sprite quieto no encontrado";
    }

}

void jugador1::draw(QPainter &painter){

    QPixmap spriteActual;

    if((estado == caminandoX || estado == caminandoY) && !spritesCaminandoDerecha.isEmpty() && !spritesCaminandoIzquierda.isEmpty()) {
        if(direccion == -1) {
            spriteActual = spritesCaminandoIzquierda[frameActual];
        }
        else{
            spriteActual = spritesCaminandoDerecha[frameActual];
        }
    } else {
        spriteActual = spriteQuieto;
    }

    painter.drawPixmap(posicion.x(), posicion.y(), spriteActual);

}

void jugador1::update(const QSize &tamanioVentana)
{
    if(estado == caminandoX) {
        int nuevaX = posicion.x() + (velocidad * direccion);

        int anchoSprite = 20;
        int margen = 10;

        if(direccion == 1) {
            if(nuevaX + anchoSprite <= tamanioVentana.width() - margen) {
                posicion.setX(nuevaX);
            }
        } else {
            if(nuevaX + anchoSprite >= margen) {
                posicion.setX(nuevaX);
            }
        }
    }
    if(estado == caminandoY) {
        int nuevaY = posicion.y() + (velocidad * (direccion==-2?1:-1));

        int altoSprite = 50;
        int margen = 10;

        if(nuevaY >= margen && nuevaY + altoSprite <= tamanioVentana.height() - margen) {
            posicion.setY(nuevaY);
        }
    }
}

void jugador1::cambiarFrame()
{
    if((estado == caminandoX || estado == caminandoY) && !spritesCaminandoDerecha.isEmpty() && !spritesCaminandoIzquierda.isEmpty()) {
        if (frameActual >= spritesCaminandoDerecha.size()){
            frameActual = 0;
        }
        frameActual = (frameActual + 1) % 8;
    }
}

void jugador1::keyPressEvent(QKeyEvent *event)
{
    if(event->isAutoRepeat()) return; // Evitar múltiples presiones

    switch(event->key()) {
    case Qt::Key_D:
        estado = caminandoX;
        direccion = 1;
        teclaPresionada = true;
        timerAnimacion->start(100); // Cambiar frame cada 100ms
        break;
    case Qt::Key_A:
        estado = caminandoX;
        direccion = -1;
        teclaPresionada = true;
        timerAnimacion->start(100);
        break;
    case Qt::Key_W:
        estado = caminandoY;
        direccion = 2;
        teclaPresionada = true;
        timerAnimacion->start(100);
        break;
    case Qt::Key_S:
        estado = caminandoY;
        direccion = -2;
        teclaPresionada = true;
        timerAnimacion->start(100);
        break;
    }
}

void jugador1::keyReleaseEvent(QKeyEvent *event)
{

    if(event->isAutoRepeat()) return;
    switch(event->key()) {
    case Qt::Key_D:
        if(direccion == 1) {
            estado = quieto;
            teclaPresionada = false;
            timerAnimacion->stop();
            frameActual = 0;
        }
        break;
    case Qt::Key_A:
        if(direccion == -1) {
            estado = quieto;
            teclaPresionada = false;
            timerAnimacion->stop();
            frameActual = 0;
        }
        break;
    case Qt::Key_W:
        if(direccion == 2) {
            estado = quieto;
            teclaPresionada = false;
            timerAnimacion->stop();
            frameActual = 0;
        }
        break;
    case Qt::Key_S:
        if(direccion == -2) {
            estado = quieto;
            teclaPresionada = false;
            timerAnimacion->stop();
            frameActual = 0;
        }
        break;
    }
}

void jugador1::setPosicion(int x, int y)
{
    posicion.setX(x);
    posicion.setY(y);
}

QPoint jugador1::getPosicion() const
{
    return posicion;
}
