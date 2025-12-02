#include "item.h"
#include <QRandomGenerator>
#include <QDebug>
#include "random"

item::item(QObject *parent)
    : QObject(parent)
    , estado(rebotando)
    , gravedad(0.35)
    , altoItem(20)
    , anchoItem(20)
{
    cargarTexturas();
}

void item::update(int velocidadFondo) {
    if(estado == posado){
        int nuevaX = posicion.x() + (velocidadFondo * -1);
        posicion.setX(nuevaX);
    }
    else{
        velocidadY += gravedad;
        posicion.setY(posicion.y() + velocidadY);

        if(estado != posado) {
            posicion.setX(posicion.x() + velocidadX);
        }

        if(posicion.y() >= yf) {
            posicion.setY(yf);

            if(rebotesRestantes > 0) {
                velocidadY = -velocidadY * coefRestitucion;
                rebotesRestantes--;
                velocidadX *= friccion;

                estado = rebotando;
            }
            else {
                velocidadY = 0;
                velocidadX = 0;
                estado = posado;
            }
        }
    }
}

void item::setParametrosAleatorios(int yfinal){

    velocidadX = QRandomGenerator::global()->bounded(-6, -3);
    velocidadY = QRandomGenerator::global()->bounded(1, 3);

    yf = yfinal;

    coefRestitucion = 0.4 + (QRandomGenerator::global()->bounded(0, 40) / 100.0);  // entre 0.4 y 0.8

    friccion = 0.8 + (QRandomGenerator::global()->bounded(0, 20) / 100.0);  // entre 0.8 y 0.99

    rebotesRestantes = QRandomGenerator::global()->bounded(3, 7);

}

void item::cargarTexturas(){
    for(int i = 1; i <= 3; i++) {
        QPixmap sprite(QString(":/sprites/Nivel1/items/%1.png").arg(i));
        if(!sprite.isNull()) {
            texturas.append(sprite);
        } else {
            qDebug() << "No se pudo cargar sprite:" << QString("%1.png").arg(i);
        }
    }
}

void item::setTipo(int tipo_){
    tipo = tipo_;
    if(tipo == 1){
        textura = 1;
    }
    else if(tipo == 2){
        textura = 2;
    }
    else{
        textura = 3;
    }
}

void item::draw(QPainter &painter){

    QPixmap texturaActual;
    switch (textura) {
    case 1:
        texturaActual = texturas[0];
        break;
    case 2:
        texturaActual = texturas[1];
        break;
    case 3:
        texturaActual = texturas[2];
        break;
    }
    painter.drawPixmap(posicion.x(), posicion.y(), texturaActual);

}

void item::setPosicion(int x, int y){
    posicion.setX(x);
    posicion.setY(y);
}

QPoint item::getPosicion()const{
    return posicion;
}

QRect item::getRect() const {
    return QRect(posicion.x(), posicion.y(), anchoItem, altoItem);
}
