#ifndef JUEGO_H
#define JUEGO_H

#include <QObject>
#include <QTimer>
#include <QSize>
#include <QKeyEvent>
#include <QMouseEvent>
#include "nivel.h"

// Declaraciones forward
class MenuPrincipal;
class MenuSeleccionNivel;
class MenuPausa;

class Juego : public QObject
{
    Q_OBJECT

public:
    // Estados del juego
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

    // Métodos para obtener los menús
    MenuPrincipal* getMenuPrincipal() const { return menuPrincipal; }
    MenuSeleccionNivel* getMenuSeleccionNivel() const { return menuSeleccionNivel; }
    MenuPausa* getMenuPausa() const { return menuPausa; }
    nivel* getNivelActual() const { return nivelActual; }

    EstadoJuego getEstadoActual() const { return estadoActual; }

    // Control del juego
    void cargarMenuPrincipal();
    void cargarSeleccionNivel();
    void cargarNivel(int numeroNivel);
    void pausarJuego();
    void reanudarJuego();

    // Actualización
    void actualizar();

    // Manejo de input
    void manejarKeyPress(QKeyEvent *event);
    void manejarKeyRelease(QKeyEvent *event);
    void manejarMousePress(QMouseEvent *event);
    void manejarMouseMove(QMouseEvent *event);
    void manejarMouseRelease(QMouseEvent *event);

    // Configuración
    void setTamanioVentana(const QSize &size);
    QSize getTamanioVentana() const { return tamanioVentana; }

signals:
    void estadoCambiado(EstadoJuego nuevoEstado);
    void necesitaRedibujar();
    void juegoFinalizado(ResultadoJuego resultado);

private slots:
    void onTimerTick();
    void onGameStarted();
    void onLevelSelected(int nivel);
    void onGameExited();
    void onNivelSeleccionado(int nivel);
    void onGameResumed();
    void onBackToMainMenu();

private:
    void crearMenus();
    void limpiarNivelActual();
    void verificarEstadoNivel();

    EstadoJuego estadoActual;
    QSize tamanioVentana;

    // Menus
    MenuPrincipal *menuPrincipal;
    MenuSeleccionNivel *menuSeleccionNivel;
    MenuPausa *menuPausa;

    // Nivel actual
    nivel *nivelActual;

    // Timer del juego
    QTimer *timerJuego;
};

#endif // JUEGO_H
