#ifndef NIVELCOVID_H
#define NIVELCOVID_H

#include "nivel.h"
#include "dron.h"
#include <QPixmap>
#include "BaseDeCarga.h"
#include "pajaro.h"
#include "zonainfeccion.h"

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
    const QSize tamanioVentana = QSize(1100, 650);

    void verificarColisiones();

    int contadorSpawnPajaro;
    int intervaloSpawnPajaro;
    int probabilidadSpawnPajaro;
    QVector<pajaro*> pajaros;
    void spawnPajaro();
    void limpiarEntidades();

    QVector<ZonaInfeccion*> zonasInfeccion;
    int contadorSpawnInfeccion;
    int intervaloSpawnInfeccion;
    int probabilidadSpawnInfeccion;
    void spawnZonaInfeccion();
    void limpiarZonasInfeccion();

    int zonasDesinfectadas;
    int zonasObjetivo;
    bool zonaExcedioLimite;

};

#endif // NIVELCOVID_H
