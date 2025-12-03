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
    qDebug() << "=== CONSTRUCTOR JUEGO ===";

    timerJuego = new QTimer(this);
    connect(timerJuego, &QTimer::timeout, this, &Juego::onTimerTick);

    crearMenus();
    cargarMenuPrincipal();

    qDebug() << "Juego inicializado correctamente";
}

Juego::~Juego()
{
    qDebug() << "=== DESTRUCTOR JUEGO ===";

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
    qDebug() << "=== CREANDO MENÚS ===";

    // Crear instancias de los menús
    menuPrincipal = new MenuPrincipal();
    menuSeleccionNivel = new MenuSeleccionNivel();
    menuPausa = new MenuPausa();

    qDebug() << "Menús creados:"
             << "Principal:" << menuPrincipal
             << "Selección:" << menuSeleccionNivel
             << "Pausa:" << menuPausa;

    qDebug() << "Conectando señales...";

    // ===== CONEXIONES MENU PRINCIPAL =====
    bool conn1 = connect(menuPrincipal, SIGNAL(gameStarted()),
                         this, SLOT(onGameStarted()));
    qDebug() << "gameStarted -> onGameStarted:" << (conn1 ? "OK" : "FALLÓ");

    bool conn2 = connect(menuPrincipal, SIGNAL(levelSelected(int)),
                         this, SLOT(onLevelSelected(int)));
    qDebug() << "levelSelected -> onLevelSelected:" << (conn2 ? "OK" : "FALLÓ");

    bool conn3 = connect(menuPrincipal, SIGNAL(gameExited()),
                         this, SLOT(onGameExited()));
    qDebug() << "gameExited -> onGameExited:" << (conn3 ? "OK" : "FALLÓ");

    // ===== CONEXIONES MENU SELECCIÓN NIVEL =====
    bool conn4 = connect(menuSeleccionNivel, SIGNAL(nivelSeleccionado(int)),
                         this, SLOT(onNivelSeleccionado(int)));
    qDebug() << "nivelSeleccionado -> onNivelSeleccionado:" << (conn4 ? "OK" : "FALLÓ");

    // ===== CONEXIONES MENU PAUSA =====
    bool conn5 = connect(menuPausa, SIGNAL(gameResumed()),
                         this, SLOT(onGameResumed()));
    qDebug() << "gameResumed -> onGameResumed:" << (conn5 ? "OK" : "FALLÓ");

    bool conn6 = connect(menuPausa, SIGNAL(backToMainMenu()),
                         this, SLOT(onBackToMainMenu()));
    qDebug() << "backToMainMenu -> onBackToMainMenu:" << (conn6 ? "OK" : "FALLÓ");

    bool conn7 = connect(menuPausa, SIGNAL(gameExited()),
                         this, SLOT(onGameExited()));
    qDebug() << "gameExited (pausa) -> onGameExited:" << (conn7 ? "OK" : "FALLÓ");

    qDebug() << "=== CONEXIONES COMPLETADAS ===";
}

void Juego::cargarMenuPrincipal()
{
    qDebug() << "cargarMenuPrincipal()";

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
    qDebug() << "cargarSeleccionNivel()";

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

// ===== SLOTS PARA SEÑALES DE MENÚS =====

void Juego::onGameStarted()
{
    qDebug() << "SLOT: onGameStarted()";
    cargarSeleccionNivel();
}

void Juego::onLevelSelected(int nivel)
{
    qDebug() << "SLOT: onLevelSelected(" << nivel << ")";
    cargarNivel(nivel);
}

void Juego::onGameExited()
{
    qDebug() << "SLOT: onGameExited() - EMITIENDO SalirSolicitado";
    emit juegoFinalizado(ResultadoJuego::SalirSolicitado);
}

void Juego::onNivelSeleccionado(int nivel)
{
    qDebug() << "SLOT: onNivelSeleccionado(" << nivel << ")";
    cargarNivel(nivel);
}

void Juego::onGameResumed()
{
    qDebug() << "SLOT: onGameResumed()";
    reanudarJuego();
}

void Juego::onBackToMainMenu()
{
    qDebug() << "SLOT: onBackToMainMenu()";
    cargarMenuPrincipal();
}

// ===== MANEJO DE INPUT =====

void Juego::manejarKeyPress(QKeyEvent *event)
{
    // Manejo de tecla ESC global
    if (event->key() == Qt::Key_Escape) {
        if (estadoActual == EstadoJuego::Jugando) {
            pausarJuego();
        } else if (estadoActual == EstadoJuego::Pausa) {
            reanudarJuego();
        }
        return;
    }

    // Pasar input al nivel actual
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

// ===== MÉTODOS PRIVADOS =====

void Juego::limpiarNivelActual()
{
    if (nivelActual) {
        qDebug() << "limpiarNivelActual() - eliminando nivel anterior";
        delete nivelActual;
        nivelActual = nullptr;
    }
}

void Juego::verificarEstadoNivel()
{
    if (!nivelActual) return;

    if (nivelActual->chequearVictoria()) {
        qDebug() << "¡VICTORIA DETECTADA!";
        timerJuego->stop();
        emit juegoFinalizado(ResultadoJuego::Victoria);
    } else if (nivelActual->chequearDerrota()) {
        qDebug() << "¡DERROTA DETECTADA!";
        timerJuego->stop();
        emit juegoFinalizado(ResultadoJuego::Derrota);
    }
}
