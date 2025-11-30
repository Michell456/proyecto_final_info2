#ifndef NIVELPESTENEGRA_H
#define NIVELPESTENEGRA_H

#include "nivel.h"
#include "jugador1.h"
#include "enfermo.h"
#include "enfermointeligente.h"
#include "item.h"

#include <QVector>
#include <QPixmap>
#include <QObject>
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>

class nivelPesteNegra : public nivel{
    Q_OBJECT
public:
    nivelPesteNegra(QObject *parent = nullptr);

    void update() override;
    void draw(QPainter &p) override;
    void handleInput(QKeyEvent *event) override;
    void handleKeyRelease(QKeyEvent *event) override;

    bool chequearVictoria() override;
    bool chequearDerrota() override;

private:
    jugador1 jugador;
    jugador1* jugadorptr;
    QList<enfermo*> enfermosActivos;
    QVector<item*> items;
    void limpiarEntidades();

    void spawnItem(int tipo);
    int contadorSpawnItem;
    int intervaloSpawnItem;
    int probabilidadSpawnItem1;
    int probabilidadSpawnItem2;
    int probabilidadSpawnItem3;
    void manejarColision(jugador1 &jugador, item *item_);

    // Fondo con scroll
    QPixmap fondo;
    int fondoX1;
    int fondoX2;
    int velocidadFondo;

    // Spawn de enemigos
    int contadorSpawnEnemigo;
    int intervaloSpawnEnemigo;
    int probabilidadSpawnEnemigo;
    void spawnEnemigo();

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

    int contadorTiempo;
    int tiempoTranscurrido;
    float multiplicadorDificultad;
    void aumentarDificultad();
    float getMultiplicadorDificultad() const { return multiplicadorDificultad; }

    int velocidadFondoBase;
    int intervaloSpawnEnemigoBase;
    int intervaloSpawnItemBase;
    int frecuenciaInteligenteBase;

    QSoundEffect sonidoColision;
    QSoundEffect sonidoTos1;
    QSoundEffect sonidoTos2;
    QSoundEffect sonidoTos3;
    QSoundEffect sonidoEstornudo1;
    QSoundEffect sonidoEstornudo2;
    QSoundEffect sonidoEstornudo3;
    QSoundEffect sonidoItem2;
    QSoundEffect sonidoItem3;
    int probabilidadSonido;
    QMediaPlayer* player;
    QAudioOutput *audioOutput;

public slots:
    void borrarItemRecogido(item* itemRecogido);

};

#endif // NIVELPESTENEGRA_H
