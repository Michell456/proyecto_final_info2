#ifndef NIVELCOLERA_H
#define NIVELCOLERA_H

#include "nivel.h"
#include <QTimer>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QVector2D>
#include <QPixmap>
#include <QList>
#include <QSoundEffect>

class Doctor;
class Proyectil;
class Obstaculo;
class Balde;

class NivelColera : public nivel
{
public:
    explicit NivelColera(QObject *parent = nullptr);
    ~NivelColera();

    // Metodos virtuales de la clase base
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

    QVector<QPointF> calcularTrayectoria(const QVector2D& velocidadInicial, const QPointF& posicionInicial);
    float getGravedadActual() const;
    static QPointF calcularPosicionFisica(const QPointF& posInicial, const QVector2D& velInicial, float gravedad, float tiempo, bool conResistenciaAire = true);

    void reproducirSonidoBaldeAmpolla();
    void reproducirSonidoDestruccionBalde();
    void reproducirSonidoDestruccionMadera();
    void reproducirSonidoReboteMadera();

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

    // Efectos de sonido
    QSoundEffect sonidoBaldeAmpolla;
    QSoundEffect sonidoDestruccionBalde;
    QSoundEffect sonidoDestruccionMadera;
    QSoundEffect sonidoReboteMadera;

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
    void dibujarTrayectoria(const QVector2D& velocidadInicial, const QPointF& posicionInicial);
    void limpiarLineas();
    bool estaSobreDoctor(const QPointF& punto);
    void cambiarProyectil();
    Proyectil* obtenerProyectilActivo();
    float calcularFuerza(float distancia);

};

#endif // NIVELCOLERA_H
