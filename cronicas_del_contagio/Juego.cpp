#include "juego.h"
#include "nivelPesteNegra.h"
#include "nivelcolera.h"
#include "nivelcovid.h"
#include "MenuPrincipal.h"
#include "MenuSeleccionNivel.h"
#include "MenuPausa.h"
#include <QDebug>
#include <QPainter>
#include <QApplication>

Juego::Juego(QObject *parent)
    : QObject(parent)
    , estadoActual(EstadoJuego::MenuPrincipal)
    , tamanioVentana(800, 600)
    , menuPrincipal(nullptr)
    , menuSeleccionNivel(nullptr)
    , menuPausa(nullptr)
    , nivelActual(nullptr)
{

    timerJuego = new QTimer(this);
    connect(timerJuego, &QTimer::timeout, this, &Juego::onTimerTick);

    crearMenus();
    cargarMenuPrincipal();

}

Juego::~Juego()
{

    if (timerJuego->isActive()) {
        timerJuego->stop();
    }

    limpiarNivelActual();

    delete menuPrincipal;
    delete menuSeleccionNivel;
    delete menuPausa;
}

void Juego::setTamanioVentana(const QSize &size)
{
    if (tamanioVentana != size) {
        tamanioVentana = size;
        emit necesitaRedibujar();
    }
}

void Juego::crearMenus()
{

    menuPrincipal = new MenuPrincipal();
    menuSeleccionNivel = new MenuSeleccionNivel();
    menuPausa = new MenuPausa();

    bool conn1 = connect(menuPrincipal, SIGNAL(JuegoIniciado()),
                         this, SLOT(onJuegoIniciado()));

    bool conn2 = connect(menuPrincipal, SIGNAL(NivelSeleccionado(int)),
                         this, SLOT(onNivelSeleccionado(int)));

    bool conn3 = connect(menuPrincipal, SIGNAL(SalirDelJuego()),
                         this, SLOT(onSalirDelJuego()));

    bool conn4 = connect(menuSeleccionNivel, SIGNAL(nivelSeleccionado(int)),
                         this, SLOT(onNivelSeleccionado(int)));

    bool conn5 = connect(menuPausa, SIGNAL(JuegoDespausado()),
                         this, SLOT(onJuegoDespausado()));

    bool conn6 = connect(menuPausa, SIGNAL(VolverAlMenuPrincipal()),
                         this, SLOT(onVolverAlMenuPrincipal()));

    bool conn7 = connect(menuPausa, SIGNAL(SalirDelJuego()),
                         this, SLOT(onSalirDelJuego()));

}

void Juego::cargarMenuPrincipal()
{
    limpiarNivelActual();

    if (timerJuego->isActive()) {
        timerJuego->stop();
    }

    if (musicaNivel) {
        musicaNivel->stop();
    }

    if (!musicaMenus) {
        musicaMenus = new QMediaPlayer(this);
        audioMusicaMenus = new QAudioOutput(this);
        musicaMenus->setAudioOutput(audioMusicaMenus);

        audioMusicaMenus->setVolume(0.4);
        musicaMenus->setSource(QUrl("qrc:/sonido/Menu/Musica_fondo.mp3"));
        musicaMenus->setLoops(QMediaPlayer::Infinite);
        musicaMenus->play();
    } else {
        musicaMenus->play();
    }

    estadoActual = EstadoJuego::MenuPrincipal;
    emit estadoCambiado(estadoActual);
    emit necesitaRedibujar();
}

void Juego::cargarSeleccionNivel()
{
    if (musicaMenus) {
        musicaMenus->stop();
    }

    if (musicaMenus) {
        musicaMenus->play();
    }

    estadoActual = EstadoJuego::SeleccionNivel;
    emit estadoCambiado(estadoActual);
    emit necesitaRedibujar();

    estadoActual = EstadoJuego::SeleccionNivel;
    emit estadoCambiado(estadoActual);
    emit necesitaRedibujar();
}

void Juego::cargarNivel(int numeroNivel)
{

    limpiarNivelActual();

    if (musicaMenus) {
        musicaMenus->stop();
    }

    if (!musicaNivel) {
        musicaNivel = new QMediaPlayer(this);
        audioMusicaNivel = new QAudioOutput(this);
        musicaNivel->setAudioOutput(audioMusicaNivel);
        audioMusicaNivel->setVolume(0.4);
    }

    switch (numeroNivel) {
    case 1:
        nivelActual = new nivelPesteNegra(this);
        nivelActual->setTamanioVentana(tamanioVentana);
        musicaNivel->setSource(QUrl("qrc:/sonido/Nivel1/musica_fondo.mp3"));
        break;
    case 2:
        nivelActual = new NivelColera(this);
        musicaNivel->setSource(QUrl("qrc:/sonido/Nivel2/Musica_fondo_nivel2.mp3"));
        break;
    case 3:
        nivelActual = new NivelCovid(this);
        nivelActual->setTamanioVentana(tamanioVentana);
        musicaNivel->setSource(QUrl("qrc:/sonido/Nivel3/Musica_fondo_nivel3.mp3"));
        break;
    default:
        qDebug() << "Nivel no reconocido, cargando nivel 1 por defecto";
        musicaNivel->setSource(QUrl("qrc:/sonido/Nivel1/musica_fondo.mp3"));
        break;
    }

    musicaNivel->setLoops(QMediaPlayer::Infinite);
    musicaNivel->play();

    estadoActual = EstadoJuego::Jugando;
    timerJuego->start(16); // ~60 FPS

    emit estadoCambiado(estadoActual);
    emit necesitaRedibujar();
}

void Juego::pausarJuego()
{
    if (estadoActual == EstadoJuego::Jugando) {


        estadoActual = EstadoJuego::Pausa;
        timerJuego->stop();
        if (musicaNivel) musicaNivel->pause();

        emit estadoCambiado(estadoActual);
        emit necesitaRedibujar();
    }
}

void Juego::reanudarJuego()
{
    if (estadoActual == EstadoJuego::Pausa && nivelActual) {

        estadoActual = EstadoJuego::Jugando;
        timerJuego->start(16);
        if (musicaNivel) musicaNivel->play();

        emit estadoCambiado(estadoActual);
        emit necesitaRedibujar();
    }
}

void Juego::actualizar()
{
    if (estadoActual == EstadoJuego::Jugando && nivelActual) {
        nivelActual->update();
        verificarEstadoNivel();
    }
}

void Juego::onTimerTick()
{
    actualizar();
    emit necesitaRedibujar();
}


void Juego::onJuegoIniciado()
{
    cargarSeleccionNivel();
}

void Juego::onNivelSeleccionado(int nivel)
{
    cargarNivel(nivel);
}

void Juego::onSalirDelJuego()
{
    emit juegoFinalizado(ResultadoJuego::SalirSolicitado);
}

void Juego::onJuegoDespausado()
{
    reanudarJuego();
}

void Juego::onVolverAlMenuPrincipal()
{
    cargarMenuPrincipal();
}


void Juego::manejarKeyPress(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        if (estadoActual == EstadoJuego::Jugando) {
            pausarJuego();
        } else if (estadoActual == EstadoJuego::Pausa) {
            reanudarJuego();
        }
        return;
    }

    if (estadoActual == EstadoJuego::Jugando && nivelActual) {
        nivelActual->handleInput(event);
    }
}

void Juego::manejarKeyRelease(QKeyEvent *event)
{
    if (estadoActual == EstadoJuego::Jugando && nivelActual) {
        nivelActual->handleKeyRelease(event);
    }
}

void Juego::manejarMousePress(QMouseEvent *event)
{
    if (estadoActual == EstadoJuego::Jugando && nivelActual) {
        if (auto nivelColera = dynamic_cast<NivelColera*>(nivelActual)) {
            nivelColera->handleMousePress(event);
        }
    }
}

void Juego::manejarMouseMove(QMouseEvent *event)
{
    if (estadoActual == EstadoJuego::Jugando && nivelActual) {
        if (auto nivelColera = dynamic_cast<NivelColera*>(nivelActual)) {
            nivelColera->handleMouseMove(event);
        }
    }
}

void Juego::manejarMouseRelease(QMouseEvent *event)
{
    if (estadoActual == EstadoJuego::Jugando && nivelActual) {
        if (auto nivelColera = dynamic_cast<NivelColera*>(nivelActual)) {
            nivelColera->handleMouseRelease(event);
        }
    }
}

void Juego::limpiarNivelActual()
{
    if (nivelActual) {
        delete nivelActual;
        nivelActual = nullptr;
    }
}

void Juego::verificarEstadoNivel()
{
    if (!nivelActual) return;

    if (nivelActual->chequearVictoria()) {
        timerJuego->stop();
        if (musicaNivel) musicaNivel->stop();
        emit juegoFinalizado(ResultadoJuego::Victoria);
    } else if (nivelActual->chequearDerrota()) {
        timerJuego->stop();
        if (musicaNivel) musicaNivel->stop();
        emit juegoFinalizado(ResultadoJuego::Derrota);
    }
}
