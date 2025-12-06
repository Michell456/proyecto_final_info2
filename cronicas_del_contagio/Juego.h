#ifndef JUEGO_H
#define JUEGO_H

#include <QObject>
#include <QTimer>
#include <QSize>
#include <QKeyEvent>
#include <QMouseEvent>
#include "nivel.h"
#include <QMediaPlayer>
#include <QAudioOutput>

class MenuPrincipal;
class MenuSeleccionNivel;
class MenuPausa;

class Juego : public QObject
{
    Q_OBJECT

public:
    enum class EstadoJuego {
        MenuPrincipal,
        SeleccionNivel,
        Jugando,
        Pausa,
        Terminado
    };

    // Resultados posibles
    enum class ResultadoJuego {
        Victoria,
        Derrota,
        SalirSolicitado
    };

    explicit Juego(QObject *parent = nullptr);
    ~Juego();

    MenuPrincipal* getMenuPrincipal() const { return menuPrincipal; }
    MenuSeleccionNivel* getMenuSeleccionNivel() const { return menuSeleccionNivel; }
    MenuPausa* getMenuPausa() const { return menuPausa; }
    nivel* getNivelActual() const { return nivelActual; }

    EstadoJuego getEstadoActual() const { return estadoActual; }

    void cargarMenuPrincipal();
    void cargarSeleccionNivel();
    void cargarNivel(int numeroNivel);
    void pausarJuego();
    void reanudarJuego();

    void actualizar();

    void manejarKeyPress(QKeyEvent *event);
    void manejarKeyRelease(QKeyEvent *event);
    void manejarMousePress(QMouseEvent *event);
    void manejarMouseMove(QMouseEvent *event);
    void manejarMouseRelease(QMouseEvent *event);

    void setTamanioVentana(const QSize &size);
    QSize getTamanioVentana() const { return tamanioVentana; }

signals:
    void estadoCambiado(EstadoJuego nuevoEstado);
    void necesitaRedibujar();
    void juegoFinalizado(ResultadoJuego resultado);

private slots:
    void onTimerTick();
    void onJuegoIniciado();
    void onNivelSeleccionado(int nivel);
    void onSalirDelJuego();
    void onJuegoDespausado();
    void onVolverAlMenuPrincipal();

private:
    void crearMenus();
    void limpiarNivelActual();
    void verificarEstadoNivel();

    EstadoJuego estadoActual;
    QSize tamanioVentana;

    MenuPrincipal *menuPrincipal;
    MenuSeleccionNivel *menuSeleccionNivel;
    MenuPausa *menuPausa;

    nivel *nivelActual;

    QMediaPlayer* musicaMenus = nullptr;
    QAudioOutput* audioMusicaMenus = nullptr;
    QMediaPlayer* musicaNivel = nullptr;
    QAudioOutput* audioMusicaNivel = nullptr;

    QTimer *timerJuego;
};

#endif // JUEGO_H
