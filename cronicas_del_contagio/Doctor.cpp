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
        spriteActual = frameInicial.scaled(frameWidth * 1.2, frameHeight * 1.2,
                                           Qt::KeepAspectRatio, Qt::SmoothTransformation);
        qDebug() << "Sprite del doctor cargado correctamente desde recursos";
    } else {
        qDebug() << "ERROR: No se pudo cargar el sprite del doctor desde recursos";
        generarSpriteFallback();
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
    qDebug() << "Iniciando animación de lanzamiento - Fase 1 (frames 0-5)";
}

void Doctor::continuarLanzamiento()
{
    faseLanzamiento = false;
    frameActual = 6; // Empezar desde el frame 6
    if (timerAnimacion) {
        timerAnimacion->start();
    }
    qDebug() << "Continuando animación de lanzamiento - Fase 2 (frames 6-12)";
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
        spriteActual = frameOriginal.scaled(frameWidth * 1.2, frameHeight * 1.2,
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
        spriteActual = frameOriginal.scaled(frameWidth * 1.2, frameHeight * 1.2,
                                            Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
}

void Doctor::generarSpriteFallback()
{
    QPixmap fallback(64, 64);
    fallback.fill(Qt::transparent);

    QPainter painter(&fallback);
    painter.setRenderHint(QPainter::Antialiasing);

    // Cabeza
    painter.setBrush(Qt::yellow);
    painter.drawEllipse(20, 5, 24, 24);

    // Cuerpo
    painter.setBrush(Qt::blue);
    painter.drawRect(22, 29, 20, 25);

    // Brazos
    painter.setBrush(Qt::red);
    painter.drawRect(10, 30, 12, 15);
    painter.drawRect(42, 30, 12, 15);

    spriteActual = fallback.scaled(76, 76, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}
