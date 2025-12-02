#include "balde.h"
#include <QPainter>
#include "nivelcolera.h"

Balde::Balde(const QPointF &posicion, NivelColera* nivel, QGraphicsItem *parent)
    : ObjetoEstatico(parent)
    , estado(SUCIO)
    , nivelColera(nivel)
{
    setPos(posicion);
    setZValue(3);

    // Cargar sprites desde recursos Qt
    spriteSucio = QPixmap(":/sprites/Nivel2/baldeSucio.png");
    spriteLleno = QPixmap(":/sprites/Nivel2/balde_lleno.png");
    spriteDestruido = QPixmap(":/sprites/Nivel2/baldeDestruido.png");

    // Si no se cargan los sprites, generar unos básicos
    if (spriteSucio.isNull()) {
        spriteSucio = generarBaldeSprite(Qt::gray);
    }
    if (spriteLleno.isNull()) {
        spriteLleno = generarBaldeSprite(Qt::blue);
    }
    if (spriteDestruido.isNull()) {
        spriteDestruido = generarBaldeSprite(Qt::red);
    }

    // Escalar sprites
    spriteSucio = spriteSucio.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    spriteLleno = spriteLleno.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    spriteDestruido = spriteDestruido.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    setPixmap(spriteSucio);

    areaColision = QRectF(25, 15, 30, 15); //Relativo a la posicion del balde

}

bool Balde::estaLleno() const
{
    return estado == LLENO;
}

bool Balde::estaDestruido() const
{
    return estado == DESTRUIDO;
}

void Balde::llenar()
{
    if (estado != SUCIO) {
        return;
    }

    estado = LLENO;
    actualizarSprite();

    if (nivelColera) {
        nivelColera->reproducirSonidoBaldeAmpolla();
    }
}

void Balde::destruir()
{
    if (estado != SUCIO) {
        return;
    }

    estado = DESTRUIDO;
    actualizarSprite();

    if (nivelColera) {
        nivelColera->reproducirSonidoDestruccionBalde();
    }
}

Balde::Estado Balde::getEstado() const
{
    return estado;
}

void Balde::actualizarSprite()
{
    switch (estado) {
    case SUCIO:
        setPixmap(spriteSucio);
        break;
    case LLENO:
        setPixmap(spriteLleno);
        break;
    case DESTRUIDO:
        setPixmap(spriteDestruido);
        break;
    }
}

QPixmap Balde::generarBaldeSprite(const QColor &color)
{
    QPixmap balde(80, 80);
    balde.fill(Qt::transparent);

    QPainter painter(&balde);
    painter.setRenderHint(QPainter::Antialiasing);

    // Cuerpo del balde
    painter.setBrush(color);
    painter.setPen(QPen(Qt::black, 2));
    painter.drawEllipse(10, 20, 60, 50);

    // Boca del balde
    painter.drawRect(25, 15, 30, 10);

    // Asa
    painter.setPen(QPen(Qt::black, 3));
    painter.drawArc(45, 25, 20, 20, 0, 180 * 16);

    return balde;
}
QPointF Balde::getPosicion() const
{
    return pos();  // Devuelve la posicion del balde en la escena
}

QPixmap Balde::getSprite() const
{
    return pixmap();  // Devuelve el sprite actual del balde
}

QRectF Balde::getAreaColision() const
{
    if (estado != SUCIO) {
        return QRectF();
    }

    QRectF area = areaColision;
    area.moveTopLeft(pos() + areaColision.topLeft());
    return area;
}

void Balde::setNivelColera(NivelColera* nivel) {
    nivelColera = nivel;
}
