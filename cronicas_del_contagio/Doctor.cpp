#include "doctor.h"
#include <QDebug>
#include <QPainter>

Doctor::Doctor(QObject *parent)
    : QObject(parent)
    , posicion(480, 370) // Posicion inicial
    , frameWidth(64)
    , frameHeight(64)
    , frameActual(0)
    , totalFrames(13)
    , filaLanzamiento(19)
    , lanzando(false)
    , faseLanzamiento(true)
{
    timerAnimacion = new QTimer(this);
    timerAnimacion->setInterval(80);
    connect(timerAnimacion, &QTimer::timeout, this, &Doctor::actualizarFrame);

    cargarSpriteSheet();
}

Doctor::~Doctor()
{
    if (timerAnimacion && timerAnimacion->isActive()) {
        timerAnimacion->stop();
    }
}

void Doctor::cargarSpriteSheet()
{
    // Cargar desde recursos Qt
    spriteSheet.load(":/sprites/Nivel2/doctor.png");

    if (!spriteSheet.isNull()) {
        // Frame inicial (reposo)
        int frameX = 0 * frameWidth;
        int frameY = filaLanzamiento * frameHeight;
        QPixmap frameInicial = spriteSheet.copy(frameX, frameY, frameWidth, frameHeight);
        spriteActual = frameInicial.scaled(frameWidth * 1.6, frameHeight * 1.6,
                                           Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
}

void Doctor::iniciarLanzamiento()
{
    lanzando = true;
    faseLanzamiento = true;
    frameActual = 0;
    if (timerAnimacion) {
        timerAnimacion->start();
    }
}

void Doctor::continuarLanzamiento()
{
    faseLanzamiento = false;
    frameActual = 6; // Empezar desde el frame 6
    if (timerAnimacion) {
        timerAnimacion->start();
    }
}

void Doctor::detenerAnimacion()
{
    lanzando = false;
    if (timerAnimacion) {
        timerAnimacion->stop();
    }

    if (!spriteSheet.isNull()) {
        // Volver al frame de reposo
        int frameX = 0 * frameWidth;
        int frameY = filaLanzamiento * frameHeight;
        QPixmap frameOriginal = spriteSheet.copy(frameX, frameY, frameWidth, frameHeight);
        spriteActual = frameOriginal.scaled(frameWidth * 1.6, frameHeight * 1.6,
                                            Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    emit animacionCompletada();
}

bool Doctor::estaLanzando() const
{
    return lanzando;
}

void Doctor::actualizarAnimacionLanzamiento()
{
    if (!lanzando) return;
    actualizarFrame();
}

void Doctor::actualizarFrame()
{
    if (!lanzando) return;

    if (faseLanzamiento) {
        // Fase 1: frames 0-5
        if (frameActual < 5) {
            frameActual++;
        } else {
            timerAnimacion->stop(); // Pausar hasta que suelten el mouse
            return;
        }
    } else {
        // Fase 2: frames 6-12 (lanzamiento)
        if (frameActual < 12) {
            frameActual++;
        } else {
            detenerAnimacion();
            return;
        }
    }

    if (!spriteSheet.isNull()) {
        int frameX = frameActual * frameWidth;
        int frameY = filaLanzamiento * frameHeight;
        QPixmap frameOriginal = spriteSheet.copy(frameX, frameY, frameWidth, frameHeight);
        spriteActual = frameOriginal.scaled(frameWidth * 1.6, frameHeight * 1.6,
                                            Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
}
