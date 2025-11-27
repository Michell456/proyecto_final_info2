#ifndef PROYECTIL_H
#define PROYECTIL_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QVector2D>
#include <QTimer>
#include <QList>
#include <QPixmap>
#include <NivelColera.h>

class Obstaculo;
class Balde;

class Proyectil : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    enum Tipo { PIEDRA, AMPOLLA };

    struct Configuracion {
        Tipo tipo;
        float gravedad;
        float factorRebote;
        bool puedeDestruirObstaculos;
        bool puedeLlenarBaldes;
        bool puedeRebotar;
        int maxColisiones;
        QPixmap sprite;

        Configuracion()
            : tipo(PIEDRA), gravedad(0.8f), factorRebote(0.4f),
            puedeDestruirObstaculos(true), puedeLlenarBaldes(false),
            puedeRebotar(false), maxColisiones(1) {}
    };

    Proyectil(const QPixmap &sprite, float gravedad, float factorRebote,
              bool puedeDestruirObstaculos, bool puedeLlenarBaldes,
              bool puedeRebotar, int maxColisiones,
              NivelColera* nivel = nullptr,
              QGraphicsItem *parent = nullptr);

    Proyectil(const Configuracion &config, QGraphicsItem *parent = nullptr);
    ~Proyectil();

    void lanzar(const QVector2D &velocidad, const QPointF &posicion);
    void actualizarMovimiento();
    void detenerMovimiento();
    bool estaEnMovimiento() const;
    bool esVisible() const;
    Tipo getTipo() const;

    void setPosicion(const QPointF &posicion);
    void setVisible(bool visible);
    QPointF getPosicion() const;
    QPixmap getSprite() const;

    void setObjetivos(QList<Obstaculo*> *obstaculos, QList<Balde*> *baldes);
    void setNivelColera(NivelColera* nivel);

signals:
    void proyectilDetenido();

private slots:
    void onTimerMovimiento();

private:
    Configuracion config;
    QVector2D velocidad;
    QPointF posicion;
    bool enMovimiento;
    bool visible;
    QTimer *timerMovimiento;

    QList<Obstaculo*> *obstaculos;
    QList<Balde*> *baldes;

    int colisionesRealizadas;
    bool haRebotado;
    int indiceUltimoObstaculoChocado;

    void actualizarPosicion();
    bool verificarColision();
    void manejarColisionConObstaculo(int indiceObstaculo);
    void manejarColisionConBalde(int indiceBalde);
    void manejarRebote();
    NivelColera* nivelColera;
};

#endif // PROYECTIL_H
