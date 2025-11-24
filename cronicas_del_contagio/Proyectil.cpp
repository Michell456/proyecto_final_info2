#include "proyectil.h"
#include "obstaculo.h"
#include "balde.h"
#include <QDebug>
#include <QPainter>
#include <QElapsedTimer>
#include <cmath>

Proyectil::Proyectil(const QPixmap &sprite, float gravedad, float factorRebote,
                     bool puedeDestruirObstaculos, bool puedeLlenarBaldes,
                     bool puedeRebotar, int maxColisiones, QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
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

    aplicarGravedad();
    actualizarPosicion();

    if (verificarColision()) {
        // La colisión fue manejada en verificarColision()
    }

    bool fueraDePantalla = (posicion.x() > 1100 || posicion.x() < -100 || posicion.y() > 650);
    bool velocidadMuyBaja = (velocidad.length() < 0.5f);

    if (fueraDePantalla || velocidadMuyBaja) {
        qDebug() << "Proyectil detenido - Razón:"
                 << (fueraDePantalla ? "Fuera de pantalla" : "Velocidad muy baja");
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

    qDebug() << "Proyectil detenido - Tipo:" << (config.tipo == PIEDRA ? "PIEDRA" : "AMPOLLA")
             << "Colisiones:" << colisionesRealizadas;

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

void Proyectil::aplicarGravedad()
{
    velocidad.setY(velocidad.y() + config.gravedad);

    float resistenciaAire = 0.995f;
    velocidad *= resistenciaAire;
}

void Proyectil::actualizarPosicion()
{
    static QElapsedTimer timer;
    static bool firstTime = true;

    if (firstTime) {
        timer.start();
        firstTime = false;
    }

    float deltaTime = timer.restart() / 1000.0f;
    deltaTime = qMin(deltaTime, 0.033f);

    posicion.setX(posicion.x() + velocidad.x() * deltaTime * 60.0f);
    posicion.setY(posicion.y() + velocidad.y() * deltaTime * 60.0f);

    setPos(posicion);

    float angulo = std::atan2(velocidad.y(), velocidad.x()) * 180.0f / M_PI;
    setRotation(angulo);
}

bool Proyectil::verificarColision()
{
    if (!enMovimiento) return false;

    QRectF areaProyectil = boundingRect();
    areaProyectil.moveTo(posicion);

    if (config.tipo == PIEDRA) {
        areaProyectil.adjust(15, 15, -15, -15);
    } else {
        areaProyectil.adjust(10, 10, -10, -10);
    }

    if (obstaculos && config.puedeDestruirObstaculos) {
        for (int i = 0; i < obstaculos->size(); ++i) {
            Obstaculo* obstaculo = obstaculos->at(i);
            if (!obstaculo->estaDestruido()) {
                QRectF areaObstaculo = obstaculo->getAreaColision();

                if (areaProyectil.intersects(areaObstaculo)) {
                    if (i == indiceUltimoObstaculoChocado) {
                        continue;
                    }
                    qDebug() << "¡COLISIÓN CON OBSTÁCULO! Tipo:" << obstaculo->getTipo();
                    manejarColisionConObstaculo(i);
                    return true;
                }
            }
        }
    }

    if (baldes) {
        for (int i = 0; i < baldes->size(); ++i) {
            Balde* balde = baldes->at(i);
            QRectF areaBalde = balde->getAreaColision();

            if (areaProyectil.intersects(areaBalde)) {
                qDebug() << "¡COLISIÓN CON BALDE!" << i;
                manejarColisionConBalde(i);
                return true;
            }
        }
    }

    indiceUltimoObstaculoChocado = -1;
    return false;
}

void Proyectil::manejarColisionConObstaculo(int indiceObstaculo)
{
    Obstaculo* obstaculo = obstaculos->at(indiceObstaculo);

    if (config.tipo == PIEDRA) {
        obstaculo->destruir();
        colisionesRealizadas++;

        velocidad.setY(-velocidad.y() * 0.4f);
        velocidad.setX(velocidad.x() * 0.7f);

        qDebug() << "Piedra destruyó obstáculo - Colisiones:" << colisionesRealizadas;

    } else if (config.tipo == AMPOLLA) {
        if (!haRebotado) {
            manejarRebote();
            indiceUltimoObstaculoChocado = indiceObstaculo;
            haRebotado = true;
            colisionesRealizadas++;

            qDebug() << "Ampolla rebotó - Rebotes:" << colisionesRealizadas;
        } else {
            qDebug() << "Ampolla se rompió en segundo impacto";
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
        qDebug() << "Ampolla llenó balde" << (indiceBalde + 1);
    } else if (config.tipo == PIEDRA) {
        if (!balde->estaLleno() && !balde->estaDestruido()) {
            balde->destruir();
            qDebug() << "Piedra destruyó balde" << (indiceBalde + 1);
        }
    }

    colisionesRealizadas++;
    detenerMovimiento();
}

void Proyectil::manejarRebote()
{
    velocidad.setY(-velocidad.y() * config.factorRebote);
    velocidad.setX(velocidad.x() * 0.8f);
    posicion.setY(posicion.y() - 8);

    qDebug() << "Rebote aplicado - Nueva velocidad:" << velocidad.length();
}
