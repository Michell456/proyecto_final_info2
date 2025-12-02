#ifndef OBJETOESTATICO_H
#define OBJETOESTATICO_H

#include <QGraphicsPixmapItem>
#include <QRectF>

class ObjetoEstatico : public QGraphicsPixmapItem
{
public:
    ObjetoEstatico(QGraphicsItem *parent = nullptr);
    virtual ~ObjetoEstatico() = default;

    QRectF getAreaColision() const;
    void setAreaColision(const QRectF &newAreaColision);

protected:
    QRectF areaColision;
};

#endif // OBJETOESTATICO_H
