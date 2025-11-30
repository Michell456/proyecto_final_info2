#include "jugador1.h"
#include <QDebug>
#include <QDir>

jugador1::jugador1(QObject *parent)
    : QObject(parent)
    , frameActual(0)
    , estado(quieto)
    , vida(5)
    , velocidad(4.5)
    , anchoSprite(20)
    , altoSprite(50)
    , teclaAbajo(false)
    , teclaArriba(false)
    , teclaDerecha(false)
    , teclaIzquierda(false)
    , inmune(false)
    , items(0)
    , inmuneInteligente(false)
{
    timerAnimacion = new QTimer(this);
    connect(timerAnimacion, &QTimer::timeout, this, &jugador1::cambiarFrame);

    cargarSprites();

    timerInmunidad = new QTimer(this);
    timerInmunidad->setSingleShot(true);
    connect(timerInmunidad, &QTimer::timeout, this, &jugador1::finInmunidad);

    timerInmunidadInteligente = new QTimer(this);
    timerInmunidadInteligente->setSingleShot(true);
    connect(timerInmunidadInteligente, &QTimer::timeout, this, &jugador1::finInmunidadInteligente);
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

void jugador1::activarInmunidad(int milisegundos) {
    inmune = true;
    timerInmunidad->start(milisegundos);
}

void jugador1::setInmuneInteligente(int milisegundos){
    inmuneInteligente = true;
    timerInmunidadInteligente->start(milisegundos);
}

void jugador1::draw(QPainter &painter){

    QPixmap spriteActual;

    if(estado == caminando && !spritesCaminandoDerecha.isEmpty() && !spritesCaminandoIzquierda.isEmpty()) {
        if(direccionActual.x() == -1) {
            spriteActual = spritesCaminandoIzquierda[frameActual];
        }
        else{
            spriteActual = spritesCaminandoDerecha[frameActual];
        }
    } else {
        spriteActual = spriteQuieto;
    }

    if(inmune){
        painter.setOpacity(0.5);
    }
    else{
        painter.setOpacity(1.0);
    }
    painter.drawPixmap(posicion.x(), posicion.y(), spriteActual);

}

void jugador1::update(const QSize &tamanioVentana, int velocidadFondo)
{
    int nuevaX = 0;
    int nuevaY = 0;

    int margen = 10;

    if(estado == caminando) {
        nuevaX = posicion.x() + (velocidad * direccionActual.x());
        nuevaY = posicion.y() + (velocidad * direccionActual.y());

        if(nuevaX >= margen && nuevaX + anchoSprite <= tamanioVentana.width() - margen) {
            posicion.setX(nuevaX);
        }

        if((nuevaY + altoSprite <= tamanioVentana.height() - margen -125) && (nuevaY + altoSprite >= margen +220)) {
            posicion.setY(nuevaY);
        }
    }
    else{
        nuevaX = posicion.x() + (velocidadFondo * -1);
        if(nuevaX >= margen){
            posicion.setX(nuevaX);
        }
    }
}

void jugador1::actualizarDireccion() {
    int dx = 0, dy = 0;

    if(teclaDerecha) dx += 1;
    if(teclaIzquierda) dx -= 1;
    if(teclaAbajo) dy += 1;
    if(teclaArriba) dy -= 1;

    direccionActual = QPoint(dx, dy);

    if(dx != 0 || dy != 0) {
        estado = caminando;
        if(!timerAnimacion->isActive()) {
            timerAnimacion->start(100);
        }
    } else {
        estado = quieto;
        timerAnimacion->stop();
        frameActual = 0;
    }
}

void jugador1::cambiarFrame()
{
    if(estado == caminando && !spritesCaminandoDerecha.isEmpty() && !spritesCaminandoIzquierda.isEmpty()) {
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
        teclaDerecha = true;
        break;
    case Qt::Key_A:
        teclaIzquierda = true;
        break;
    case Qt::Key_W:
        teclaArriba = true;
        break;
    case Qt::Key_S:
        teclaAbajo = true;
        break;
    }
    actualizarDireccion();
}

void jugador1::keyReleaseEvent(QKeyEvent *event)
{

    if(event->isAutoRepeat()) return;
    switch(event->key()) {
    case Qt::Key_D:
        teclaDerecha = false;
        break;
    case Qt::Key_A:
        teclaIzquierda = false;
        break;
    case Qt::Key_W:
        teclaArriba = false;
        break;
    case Qt::Key_S:
        teclaAbajo = false;
        break;
    }
    actualizarDireccion();
}

void jugador1::setPosicion(int x, int y)
{
    posicion.setX(x);
    posicion.setY(y);
}

QRect jugador1::getRect() const {
    return QRect(posicion.x(), posicion.y(), anchoSprite, altoSprite);
}

void jugador1::quitarVida(){
    vida--;
}

void jugador1::sumarVida(){
    if (vida < 5){
        vida++;
    }
}

void jugador1::sumarItem(){
    items++;
}

void jugador1::quitarItems(int cantidad){
    items -= cantidad;
}
