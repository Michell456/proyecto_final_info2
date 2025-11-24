#ifndef NIVELCOLERA_H
#define NIVELCOLERA_H

#include "nivel.h"
#include <QTimer>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QVector2D>
#include <QPixmap>
#include <QList>

// Forward declarations
class Doctor;
class Proyectil;
class Obstaculo;
class Balde;

class NivelColera : public nivel  // CAMBIA: Nivel2 → NivelColera
{
public:
    explicit NivelColera(QObject *parent = nullptr);  // CAMBIA: Nivel2 → NivelColera
    ~NivelColera();  // CAMBIA: Nivel2 → NivelColera

    // Métodos virtuales de la clase base
    void update() override;
    void draw(QPainter &p) override;
    void handleInput(QKeyEvent *e) override;
    void handleKeyRelease(QKeyEvent *event) override;
    bool chequearVictoria() override;
    bool chequearDerrota() override;

    // Métodos para manejo de mouse
    void handleMousePress(QMouseEvent *event);
    void handleMouseMove(QMouseEvent *event);
    void handleMouseRelease(QMouseEvent *event);

private:
    // Sprites del nivel
    QPixmap fondo;
    QPixmap spriteObstaculos;

    // Componentes del juego
    Doctor *doctor;
    Proyectil *proyectilActual;
    Proyectil *proyectilPiedra;
    Proyectil *proyectilAmpolla;
    QList<Obstaculo*> obstaculos;
    QList<Balde*> baldes;

    // Variables de lanzamiento
    QPointF puntoInicio;
    QPointF puntoActual;
    bool arrastrando;
    bool clickEnDoctor;

    // Elementos visuales temporales
    QList<QLineF> lineasTrayectoria;
    QList<QRectF> puntosTrayectoria;

    // Configuraciones
    struct ProyectilConfig {
        float gravedad;
        float factorRebote;
        bool puedeDestruirObstaculos;
        bool puedeLlenarBaldes;
        bool puedeRebotar;
        int maxColisiones;
        QPixmap sprite;
    } configPiedra, configAmpolla;

    void inicializarRecursos();
    void inicializarConfiguraciones();
    void crearObstaculos();
    void crearBaldes();
    void dibujarLineaFuerza(const QPointF &inicio, const QPointF &fin);
    void dibujarTrayectoria(const QVector2D& velocidadInicial, const QPointF& posicionInicial);
    void limpiarLineas();
    QVector<QPointF> calcularTrayectoria(const QVector2D& velocidadInicial, const QPointF& posicionInicial);
    bool estaSobreDoctor(const QPointF& punto);
    void cambiarProyectil();
    Proyectil* obtenerProyectilActivo();
    float calcularFuerza(float distancia);

    // Funciones auxiliares para sprites fallback
    QPixmap generarPiedraSprite();
    QPixmap generarAmpollaSprite();
    void generarObstaculosBasicos();
};

#endif // NIVELCOLERA_H  // CAMBIA: NIVEL2_H → NIVELCOLERA_H
