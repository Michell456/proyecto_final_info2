#include "balde.h"
#include <QPainter>

Balde::Balde(const QPointF &posicion, QGraphicsItem *parent)
    : ObjetoEstatico(parent)
    , estado(SUCIO)
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
        qDebug() << "Balde sucio generado";
    }
    if (spriteLleno.isNull()) {
        spriteLleno = generarBaldeSprite(Qt::blue);
        qDebug() << "Balde lleno generado";
    }
    if (spriteDestruido.isNull()) {
        spriteDestruido = generarBaldeSprite(Qt::red);
        qDebug() << "Balde destruido generado";
    }

    // Escalar sprites
    spriteSucio = spriteSucio.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    spriteLleno = spriteLleno.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    spriteDestruido = spriteDestruido.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    setPixmap(spriteSucio);

    // Área de colisión
    areaColision = QRectF(posicion.x() + 15, posicion.y() + 15, 50, 50);
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
    if (estado != SUCIO) return;

    estado = LLENO;
    actualizarSprite();
}

void Balde::destruir()
{
    if (estado != SUCIO) return;

    estado = DESTRUIDO;
    actualizarSprite();
    qDebug() << "Balde destruido - Estado:" << estado;
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
        qDebug() << "Balde cambió a estado: SUCIO";
        break;
    case LLENO:
        setPixmap(spriteLleno);
        qDebug() << "Balde cambió a estado: LLENO";
        break;
    case DESTRUIDO:
        setPixmap(spriteDestruido);
        qDebug() << "Balde cambió a estado: DESTRUIDO";
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
