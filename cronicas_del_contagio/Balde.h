#ifndef BALDE_H
#define BALDE_H

#include "objetoestatico.h"
#include <QPixmap>
class NivelColera;

class Balde : public ObjetoEstatico
{
public:
    enum Estado { SUCIO, LLENO, DESTRUIDO };

    Balde(const QPointF &posicion, NivelColera* nivel = nullptr, QGraphicsItem *parent = nullptr);

    bool estaLleno() const;
    bool estaDestruido() const;
    void llenar();
    void destruir();
    Estado getEstado() const;
    QPointF getPosicion() const;
    QPixmap getSprite() const;
    QRectF getAreaColision() const;

private:
    Estado estado;
    QPixmap spriteSucio;
    QPixmap spriteLleno;
    QPixmap spriteDestruido;

    void setNivelColera(NivelColera* nivel);
    void actualizarSprite();
    QPixmap generarBaldeSprite(const QColor &color);
    NivelColera* nivelColera;
};

#endif // BALDE_H
