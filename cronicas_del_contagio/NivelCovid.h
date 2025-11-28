#ifndef NIVELCOVID_H
#define NIVELCOVID_H

#include "nivel.h"
#include "dron.h"
#include <QPixmap>
#include "BaseDeCarga.h"

class NivelCovid : public nivel
{
public:
    NivelCovid();
    void update() override;
    void draw(QPainter &p) override;
    void handleInput(QKeyEvent *e) override;
    void handleKeyRelease(QKeyEvent *event) override;

    bool chequearVictoria() override;
    bool chequearDerrota() override;

private:
    Dron dron;
    QPixmap fondo;
    BaseDeCarga baseCarga;
};

#endif // NIVELCOVID_H
