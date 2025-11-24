#include "proyectil.h"
#include "obstaculo.h"
#include "balde.h"
#include <QDebug>
#include <QPainter>
#include <QElapsedTimer>
#include <cmath>
#include "nivelcolera.h"

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

    // Verificar colisiones ANTES de actualizar posición
    if (verificarColision()) {
        return; // Si hay colisión, salir
    }

    actualizarPosicion();

    bool fueraDePantalla = (posicion.x() > 1100 || posicion.x() < -100 || posicion.y() > 650);
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

bool Proyectil::verificarColision()
{
    if (!enMovimiento) return false;

    // Área del proyectil basada en el sprite real
    QRectF areaProyectil = boundingRect();
    areaProyectil.moveTo(posicion);

    // Ajustar área para que sea más precisa (opcional, según necesidad)
    areaProyectil.adjust(5, 5, -5, -5);

    qDebug() << "\n=== VERIFICACIÓN DE COLISIONES ===";
    qDebug() << "📍 Proyectil - Pos:" << posicion << "Área:" << areaProyectil;

    // Primero verificar colisiones con obstáculos
    if (obstaculos) {
        for (int i = 0; i < obstaculos->size(); ++i) {
            Obstaculo* obstaculo = obstaculos->at(i);
            if (!obstaculo || obstaculo->estaDestruido()) continue;

            QRectF areaObstaculo = obstaculo->getAreaColision();
            QPointF posObstaculo = obstaculo->pos();
            areaObstaculo.moveTo(posObstaculo);

            if (areaProyectil.intersects(areaObstaculo)) {
                qDebug() << "💥 COLISIÓN CON OBSTÁCULO" << i;
                manejarColisionConObstaculo(i);
                return true;
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
                qDebug() << "💥 COLISIÓN CON BALDE" << i;
                manejarColisionConBalde(i);
                return true;
            }
        }
    }

    return false;
}

void Proyectil::manejarColisionConObstaculo(int indiceObstaculo)
{
    if (indiceUltimoObstaculoChocado == indiceObstaculo) {
        return; // Evitar colisiones múltiples con el mismo obstáculo
    }

    Obstaculo* obstaculo = obstaculos->at(indiceObstaculo);
    if (!obstaculo || obstaculo->estaDestruido()) return;

    qDebug() << "🔄 Manejando colisión con obstáculo" << indiceObstaculo;

    if (config.tipo == PIEDRA && config.puedeDestruirObstaculos) {
        obstaculo->destruir();
        colisionesRealizadas++;
        qDebug() << "Piedra destruyó obstáculo - Colisiones:" << colisionesRealizadas;

        // Aplicar pequeño rebote
        velocidad.setY(-velocidad.y() * 0.3f);
        velocidad.setX(velocidad.x() * 0.6f);

    } else if (config.tipo == AMPOLLA && config.puedeRebotar) {
        manejarRebote();
        indiceUltimoObstaculoChocado = indiceObstaculo;
        colisionesRealizadas++;
        qDebug() << "Ampolla rebotó - Rebotes:" << colisionesRealizadas;
    }

    // Verificar si debe detenerse
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
    // Rebote más realista
    velocidad.setY(-velocidad.y() * config.factorRebote);
    velocidad.setX(velocidad.x() * 0.9f); // Reducir velocidad horizontal también

    // Pequeño ajuste de posición para evitar que se quede pegado
    posicion.setY(posicion.y() - 5);

    qDebug() << "🔁 Rebote aplicado - Nueva velocidad:" << velocidad.length()
             << "Factor:" << config.factorRebote;
}

void Proyectil::actualizarPosicion()
{
    // USAR LOS MISMOS VALORES que la predicción
    const float deltaTime = 0.01f;           // MISMO que NivelColera::calcularTrayectoria()
    const float resistenciaAire = 0.995f;    // MISMO que NivelColera::calcularTrayectoria()

    // FÍSICA IDÉNTICA a la predicción
    velocidad.setY(velocidad.y() + config.gravedad);
    velocidad *= resistenciaAire;

    // MOVIMIENTO IDÉNTICO a la predicción
    posicion.setX(posicion.x() + velocidad.x() * deltaTime * 60.0f);
    posicion.setY(posicion.y() + velocidad.y() * deltaTime * 60.0f);

    setPos(posicion);

    // Rotación natural basada en la dirección
    float angulo = std::atan2(velocidad.y(), velocidad.x()) * 180.0f / M_PI;
    setRotation(angulo);

    // Debug opcional (puedes comentarlo después)
    // qDebug() << "Proyectil actualizado - Pos:" << posicion << "Vel:" << velocidad.length();
}
