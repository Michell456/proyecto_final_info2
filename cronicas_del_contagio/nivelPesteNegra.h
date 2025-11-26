#ifndef NIVELPESTENEGRA_H
#define NIVELPESTENEGRA_H

#include "nivel.h"
#include "jugador1.h"
#include "enfermo.h"
#include "enfermointeligente.h"
#include "item.h"

#include <QVector>
#include <QPixmap>

class nivelPesteNegra : public nivel{

public:
    nivelPesteNegra();

    void update() override;
    void draw(QPainter &p) override;
    void handleInput(QKeyEvent *event) override;
    void handleKeyRelease(QKeyEvent *event) override;

    bool chequearVictoria() override;
    bool chequearDerrota() override;

private:
    jugador1 jugador;
    QVector<item*> items;

    void spawnItem();
    int contadorSpawn1;
    int contadorSpawn2;
    int intervaloSpawn1;
    int intervaloSpawn1;
    int probabilidadSpawn1;

    // Fondo con scroll
    QPixmap fondo;
    int fondoX1;
    int fondoX2;
    int velocidadFondo;

    // Spawn de enemigos
    int contadorSpawn;
    int intervaloSpawn;
    int probabilidadSpawn;
    QList<enfermo*> enfermosActivos;
    void spawnEnemigo();
    void limpiarEnemigos();

    // Enemigo inteligente
    enfermoInteligente *inteligenteActual;
    int contadorInteligente;
    int aparicionesInteligente;
    int frecuenciaInteligente;
    void spawnInteligente();
    bool dibujarInteligente;
    void regenerarInteligente();

    void verificarColisiones();
    void manejarColision(jugador1 &jugador, enfermo *enfermo);

    const QSize tamanioVentana = QSize(1000, 600);

};

#endif // NIVELPESTENEGRA_H
