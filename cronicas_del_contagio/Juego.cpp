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

    estadoActual = EstadoJuego::MenuPrincipal;
    emit estadoCambiado(estadoActual);
    emit necesitaRedibujar();
}

void Juego::cargarSeleccionNivel()
{

    estadoActual = EstadoJuego::SeleccionNivel;
    emit estadoCambiado(estadoActual);
    emit necesitaRedibujar();
}

void Juego::cargarNivel(int numeroNivel)
{
    qDebug() << "cargarNivel(" << numeroNivel << ")";

    limpiarNivelActual();

    // Crear el nivel apropiado
    switch (numeroNivel) {
    case 1:
        nivelActual = new nivelPesteNegra(this);
        qDebug() << "Nivel 1 (Peste Negra) creado";
        break;
    case 2:
        nivelActual = new NivelColera(this);
        qDebug() << "Nivel 2 (Cólera) creado";
        break;
    case 3:
        nivelActual = new NivelCovid(this);
        qDebug() << "Nivel 3 (COVID-19) creado";
        break;
    default:
        qDebug() << "Nivel no reconocido, cargando nivel 1 por defecto";
        nivelActual = new nivelPesteNegra(this);
        break;
    }

    estadoActual = EstadoJuego::Jugando;
    timerJuego->start(16); // ~60 FPS

    emit estadoCambiado(estadoActual);
    emit necesitaRedibujar();
}

void Juego::pausarJuego()
{
    if (estadoActual == EstadoJuego::Jugando) {
        qDebug() << "pausarJuego()";

        estadoActual = EstadoJuego::Pausa;
        timerJuego->stop();

        emit estadoCambiado(estadoActual);
        emit necesitaRedibujar();
    }
}

void Juego::reanudarJuego()
{
    if (estadoActual == EstadoJuego::Pausa && nivelActual) {
        qDebug() << "reanudarJuego()";

        estadoActual = EstadoJuego::Jugando;
        timerJuego->start(16);

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
        emit juegoFinalizado(ResultadoJuego::Victoria);
    } else if (nivelActual->chequearDerrota()) {
        timerJuego->stop();
        emit juegoFinalizado(ResultadoJuego::Derrota);
    }
}
