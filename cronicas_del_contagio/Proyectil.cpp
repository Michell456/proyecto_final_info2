#include "proyectil.h"
#include "obstaculo.h"
#include "balde.h"
#include <QDebug>
#include <QPainter>
#include <QElapsedTimer>
#include <cmath>
#include <QVector2D>
#include "nivelcolera.h"

Proyectil::Proyectil(const QPixmap &sprite, float gravedad, float factorRebote,
                     bool puedeDestruirObstaculos, bool puedeLlenarBaldes,
                     bool puedeRebotar, int maxColisiones,
                     NivelColera* nivel, QGraphicsItem *parent)  // ← AÑADE NivelColera* nivel
    : QGraphicsPixmapItem(parent)
    , nivelColera(nivel)
    , enMovimiento(false)
    , visible(false)
    , obstaculos(nullptr)
    , baldes(nullptr)
    , colisionesRealizadas(0)
    , haRebotado(false)
    , indiceUltimoObstaculoChocado(-1)
{
    if (puedeLlenarBaldes) {
        config.tipo = AMPOLLA;
    } else {
        config.tipo = PIEDRA;
    }

    config.gravedad = gravedad;
    config.factorRebote = factorRebote;
    config.puedeDestruirObstaculos = puedeDestruirObstaculos;
    config.puedeLlenarBaldes = puedeLlenarBaldes;
    config.puedeRebotar = puedeRebotar;
    config.maxColisiones = maxColisiones;
    config.sprite = sprite;

    timerMovimiento = new QTimer(this);
    timerMovimiento->setInterval(16);

    connect(timerMovimiento, &QTimer::timeout, this, &Proyectil::onTimerMovimiento);

    setPixmap(config.sprite);

    if (config.tipo == PIEDRA) {
        setZValue(10);
    } else {
        setZValue(9);
    }
}

Proyectil::Proyectil(const Configuracion &config, QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
    , config(config)
    , enMovimiento(false)
    , visible(false)
    , obstaculos(nullptr)
    , baldes(nullptr)
    , colisionesRealizadas(0)
    , haRebotado(false)
    , indiceUltimoObstaculoChocado(-1)
{
    timerMovimiento = new QTimer(this);
    timerMovimiento->setInterval(16);

    connect(timerMovimiento, &QTimer::timeout, this, &Proyectil::onTimerMovimiento);

    setPixmap(config.sprite);

    if (config.tipo == PIEDRA) {
        setZValue(10);
    } else {
        setZValue(9);
    }
}

Proyectil::~Proyectil()
{
    if (timerMovimiento && timerMovimiento->isActive()) {
        timerMovimiento->stop();
    }
}

void Proyectil::setPosicion(const QPointF &posicion)
{
    this->posicion = posicion;
    setPos(posicion);
}

void Proyectil::setVisible(bool visible)
{
    this->visible = visible;
    QGraphicsPixmapItem::setVisible(visible);
}

bool Proyectil::esVisible() const
{
    return visible;
}

QPointF Proyectil::getPosicion() const
{
    return posicion;
}

QPixmap Proyectil::getSprite() const
{
    return pixmap();
}

void Proyectil::lanzar(const QVector2D &velocidad, const QPointF &posicion)
{
    this->velocidad = velocidad;
    this->posicion = posicion;
    this->enMovimiento = true;
    this->visible = true;
    this->colisionesRealizadas = 0;
    this->haRebotado = false;
    this->indiceUltimoObstaculoChocado = -1;

    setPos(posicion);
    setVisible(true);
    setRotation(0);

    if (config.tipo == PIEDRA) {
        setZValue(12);
    }

    timerMovimiento->start();

    qDebug() << "Proyectil lanzado - Tipo:" << (config.tipo == PIEDRA ? "PIEDRA" : "AMPOLLA")
             << "Velocidad:" << velocidad.length()
             << "Ángulo:" << std::atan2(velocidad.y(), velocidad.x()) * 180 / M_PI << "°";
}

void Proyectil::onTimerMovimiento()
{
    if (!enMovimiento) return;

    //Verificar colisiones antes de actualizar posicion
    if (verificarColision()) {
        return; //Si hay colision, salir
    }

    actualizarPosicion();

    bool fueraDePantalla = (posicion.x() > 1500 || posicion.x() < -300 || posicion.y() > 900);
    bool velocidadMuyBaja = (velocidad.length() < 0.5f);

    if (fueraDePantalla || velocidadMuyBaja) {
        detenerMovimiento();
    }
}

void Proyectil::actualizarMovimiento()
{
    onTimerMovimiento();
}

void Proyectil::detenerMovimiento()
{
    enMovimiento = false;
    visible = false;

    if (timerMovimiento) {
        timerMovimiento->stop();
    }

    setVisible(false);

    if (config.tipo == PIEDRA) {
        setZValue(10);
    } else {
        setZValue(9);
    }

    emit proyectilDetenido();
}

bool Proyectil::estaEnMovimiento() const
{
    return enMovimiento;
}

Proyectil::Tipo Proyectil::getTipo() const
{
    return config.tipo;
}

void Proyectil::setObjetivos(QList<Obstaculo*> *obstaculos, QList<Balde*> *baldes)
{
    this->obstaculos = obstaculos;
    this->baldes = baldes;
}

bool Proyectil::verificarColision()
{
    if (!enMovimiento) return false;

    //area del proyectil
    QRectF areaProyectil = boundingRect();
    areaProyectil.moveTo(posicion);

    if (config.tipo == PIEDRA) {
        areaProyectil.adjust(15, 15, -15, -15);
    } else {
        areaProyectil.adjust(5, 5, -5, -5);
    }

    // Primero verificar colisiones con obstaculos
    if (obstaculos) {
        for (int i = 0; i < obstaculos->size(); ++i) {
            Obstaculo* obstaculo = obstaculos->at(i);
            if (!obstaculo || obstaculo->estaDestruido()) continue;

            QRectF areaObstaculo = obstaculo->getAreaColision();
            QPointF posObstaculo = obstaculo->pos();
            areaObstaculo.moveTo(posObstaculo);

            if (areaProyectil.intersects(areaObstaculo)) {
                // Verificar que no sea la misma colision consecutiva
                if (i != indiceUltimoObstaculoChocado || !haRebotado) {
                    manejarColisionConObstaculo(i);
                    return true;
                }
            }
        }
    }

    // Luego verificar colisiones con baldes
    if (baldes) {
        for (int i = 0; i < baldes->size(); ++i) {
            Balde* balde = baldes->at(i);
            if (!balde) continue;

            QRectF areaBalde = balde->getAreaColision();
            QPointF posBalde = balde->pos();
            areaBalde.moveTo(posBalde);

            if (areaProyectil.intersects(areaBalde)) {
                manejarColisionConBalde(i);
                return true;
            }
        }
    }

    if (indiceUltimoObstaculoChocado != -1) {
        QRectF areaUltimoObstaculo = obstaculos->at(indiceUltimoObstaculoChocado)->getAreaColision();
        areaUltimoObstaculo.moveTo(obstaculos->at(indiceUltimoObstaculoChocado)->pos());

        if (!areaProyectil.intersects(areaUltimoObstaculo)) {
            indiceUltimoObstaculoChocado = -1;
        }
    }

    return false;
}

void Proyectil::manejarColisionConObstaculo(int indiceObstaculo)
{

    Obstaculo* obstaculo = obstaculos->at(indiceObstaculo);
    if (!obstaculo || obstaculo->estaDestruido()) return;

    QPointF posObstaculo = obstaculo->getPosicion();

    if (config.tipo == PIEDRA && config.puedeDestruirObstaculos) {
        obstaculo->destruir();
        colisionesRealizadas++;

        // Aplicar pequeño rebote
        velocidad.setY(-velocidad.y() * 0.3f);
        velocidad.setX(velocidad.x() * 0.6f);

        if (nivelColera) {
            nivelColera->reproducirSonidoDestruccionMadera();
        }

    } else if (config.tipo == AMPOLLA) {
        if (!haRebotado) {
            manejarRebote();
            indiceUltimoObstaculoChocado = indiceObstaculo;
            haRebotado = true;
            colisionesRealizadas++;

            if (nivelColera) {
                nivelColera->reproducirSonidoReboteMadera();
            }

            // Asegurarse de que se separe del obstaculo
            QRectF areaObstaculo = obstaculo->getAreaColision();
            areaObstaculo.moveTo(obstaculo->pos());

            // Mover la ampolla fuera del obstaculo
            if (posicion.y() < areaObstaculo.top()) {
                posicion.setY(areaObstaculo.top() - 20);
            } else {
                posicion.setY(areaObstaculo.bottom() + 20);
            }

        } else {
            detenerMovimiento();
            return;
        }
    }

    if (colisionesRealizadas >= config.maxColisiones) {
        detenerMovimiento();
    }
}

void Proyectil::manejarColisionConBalde(int indiceBalde)
{
    Balde* balde = baldes->at(indiceBalde);

    if (config.tipo == AMPOLLA && config.puedeLlenarBaldes) {
        balde->llenar();
    } else if (config.tipo == PIEDRA) {
        if (!balde->estaLleno() && !balde->estaDestruido()) {
            balde->destruir();
        }
    }

    colisionesRealizadas++;
    detenerMovimiento();
}

void Proyectil::manejarRebote()
{

    float factorVertical = 0.7f;
    float factorHorizontal = 0.8f;

    velocidad.setY(-velocidad.y() * factorVertical);
    velocidad.setX(velocidad.x() * factorHorizontal);

    //velocidad minima despues del rebote
    float velocidadMinima = 3.0f;
    if (velocidad.length() < velocidadMinima) {
        velocidad = velocidad.normalized() * velocidadMinima;
    }

    // Ajuste de posicion para evitar que se quede pegado
    posicion.setY(posicion.y() - 15);

}

void Proyectil::actualizarPosicion()
{
    const float deltaTime = 0.01f;

    QPointF nuevaPos = NivelColera::calcularPosicionFisica(
        posicion,
        velocidad,
        config.gravedad,
        deltaTime
        );

    QVector2D nuevaVelocidad(nuevaPos.x() - posicion.x(), nuevaPos.y() - posicion.y());
    nuevaVelocidad /= (deltaTime * 60.0f);

    posicion = nuevaPos;
    velocidad = nuevaVelocidad;

    setPos(posicion);

    float angulo = std::atan2(velocidad.y(), velocidad.x()) * 180.0f / M_PI;
    setRotation(angulo);
}
