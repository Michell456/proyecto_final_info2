#ifndef ENFERMOINTELIGENTE_H
#define ENFERMOINTELIGENTE_H

#include "enfermo.h"
#include <QPainter>
#include <QRect>
#include <QPixmap>
#include <QKeyEvent>
#include <QObject>
#include <QTimer>
#include "item.h"
#include "jugador1.h"

class enfermoInteligente : public enfermo  {
    Q_OBJECT
public:

    enum Estado {
        Caminando,
        PersiguiendoJugador,
        YendoAItem,
        Huyendo,
        Autonomo
    };

    enfermoInteligente(QObject *parent = nullptr);
    void setPosicion(double x, double y);
    void setActivo(bool activo_){ activo = activo_; }
    void update(jugador1* jugador, int cantidadItemsJugador, const QList<item*>& itemsEnSuelo);
    void comenzarActividad(int segundos);
    bool logroRobar;
    void resetRobo();
    void verificarEstadoRobo();
    bool activo;
    item* itemObjetivo;          // Puntero al item que quiere tomar

    Estado estado;

private:
    QTimer *timerDuracion;

    // DECISIÓN y MEMORIA
    int umbralRobo;              // Cantidad mínima de items para perseguir
    int robosExitosos;           // Contador de robos exitosos
    int robosFallidos;           // Contador de robos fallidos
    float boostVelocidad;        // Multiplicador de velocidad

    // OBJETIVOS
    QPoint objetivoActual;       // Jugador o Item más cercano

    // TIEMPOS
    QTimer* timerDecision;       // Re-evaluar cada X tiempo
    QTimer* timerRobo;           // Tiempo máximo de robo

    void comportamientoYendoAItem();
    void comportamientoHuyendo();
    item* encontrarItemMasCercano(const QList<item*>& itemsEnSuelo);
    void iniciarRobo();
    void tomarItem();
    void aplicarBoostVelocidad();
    void reducirBoostVelocidad();
    void comportamientoCaminando(jugador1* jugador, int cantidadItemsJugador, const QList<item*>& itemsEnSuelo);
    void comportamientoPersiguiendo(jugador1* jugador);

    QPointF posicionF;

signals:
    void recogeItem(item* itemRecogido);
};

#endif // ENFERMOINTELIGENTE_H
