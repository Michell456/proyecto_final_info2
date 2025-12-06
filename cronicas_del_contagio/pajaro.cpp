#include "pajaro.h"
#include <QRandomGenerator>
#include <QDebug>
#include <random>
#include <QtMath>

pajaro::pajaro(QObject *parent)
    : QObject(parent)
    , frameActual(0)
    , velocidad(80.0f)
    , anchoSprite(10)
    , altoSprite(20)
    , tiempo(0.0)
{
    timerAnimacion = new QTimer(this);
    connect(timerAnimacion, &QTimer::timeout, this, &pajaro::cambiarFrame);

    cargarSprites();
    timerAnimacion->start(100);
}

pajaro::~pajaro(){
    if(timerAnimacion->isActive()) {
        timerAnimacion->stop();
    }
}

void pajaro::cambiarFrame(){

    if (frameActual >= 8){
        frameActual = 0;
    }
    frameActual = (frameActual + 1) % 8;

}

void pajaro::cargarSprites(){
    for(int i = 1; i <= 8; i++) {
        QPixmap sprite(QString(":/sprites/Nivel3/pajaro/%1.png").arg(i));
        if(!sprite.isNull()) {
            sprites.append(sprite);
        } else {
            qDebug() << "No se pudo cargar sprite:" << QString("%1.png").arg(i);
        }
    }
}

void pajaro::draw(QPainter &painter){

    QPixmap spriteActual;
    spriteActual = sprites[frameActual];
    int drawX = qRound(posicion.x());
    int drawY = qRound(posicion.y());
    painter.drawPixmap(drawX, drawY, spriteActual);

}

void pajaro::update(float dt, const QSize& tamanioVentana) {

    deltaTime = dt;
    tiempo += deltaTime;

    bool cercaDelBorde = false;

    if (movimiento == 1) {
        if (posicion.x() < 20 ||
            posicion.x() > tamanioVentana.width() - 20 ||
            posicion.y() < 20 ||
            posicion.y() > tamanioVentana.height() - 20) {

            cercaDelBorde = true;
        }
    }

    if (cercaDelBorde) {
        float dirX = (tamanioVentana.width() / 2.0f - posicion.x());
        float dirY = (tamanioVentana.height() / 2.0f - posicion.y());

        float distanciaCentro = qSqrt(dirX * dirX + dirY * dirY);
        if (distanciaCentro > 0) {
            dirX /= distanciaCentro;
            dirY /= distanciaCentro;
        }


        float magnitudActual = qSqrt(vx * vx + vy * vy);

        if (magnitudActual < velocidad * 0.3) {
            magnitudActual = velocidad * 0.6;
        }

        vx = (dirX * 0.7f + (vx / magnitudActual) * 0.3f) * magnitudActual;
        vy = (dirY * 0.7f + (vy / magnitudActual) * 0.3f) * magnitudActual;
    }


    switch (movimiento) {
        case 1: { // Lineal
            posicion.rx() += vx * deltaTime;
            posicion.ry() += vy * deltaTime;
            break;
        }

        case 2: { // Parabolico
            posicion.rx() += vx * deltaTime;
            double t = (posicion.x() - x) / vx;
            if (vx != 0) {
                posicion.ry() = y + vy * t + 0.5 * a * t * t;
            }
            break;
        }

        case 3: { // Senoidal
            posicion.rx() += vx * deltaTime;
            double waveX = (posicion.x() - x);
            posicion.ry() = y + amplitud * qSin(frecuencia * waveX + fase);
            break;
        }

        case 4: { // Curvo
            double radio = amplitud * (1.0 + tiempo * 0.1);
            double angulo = frecuencia * tiempo;
            posicion.rx() = x + radio * qCos(angulo);
            posicion.ry() = y + radio * qSin(angulo);
            break;
        }

        case 5: { // Inestable

            double factorRuido = 30.0;
            double ruidoX = (QRandomGenerator::global()->generateDouble() - 0.5) * factorRuido;
            double ruidoY = (QRandomGenerator::global()->generateDouble() - 0.5) * factorRuido;

            if (QRandomGenerator::global()->bounded(100) < 85) {
                posicion.rx() += (vx + ruidoX) * deltaTime;
                posicion.ry() += (vy + ruidoY) * deltaTime;
            } else {
                posicion.rx() += vx * deltaTime;
                posicion.ry() += vy * deltaTime;
            }

            static int contador = 0;
            contador++;
            if (contador % 30 == 0) {
                double velocidadActual = qSqrt(vx*vx + vy*vy);
                if (velocidadActual < velocidad * 0.4) {
                    double factor = (velocidad * 0.6) / velocidadActual;
                    vx *= factor;
                    vy *= factor;
                }
            }

            break;
        }
    }

}

void pajaro::inicializarMovimientoDesdeBorde(const QSize& tamanioVentana) {
    int borde = QRandomGenerator::global()->bounded(4);
    int screenWidth = tamanioVentana.width();
    int screenHeight = tamanioVentana.height();

    int margenFuera = 5;
    int margenBorde = 5;

    margenBorde = qMin(margenBorde, screenWidth / 4);
    margenBorde = qMin(margenBorde, screenHeight / 4);

    float minComponente = velocidad * 0.3f;

    switch(borde) {
    case 0: // ARRIBA - Movimiento hacia ABAJO con variación horizontal
        posicion.setX(QRandomGenerator::global()->bounded(margenBorde, screenWidth - margenBorde));
        posicion.setY(-altoSprite - margenFuera);

        vx = (QRandomGenerator::global()->generateDouble() - 0.5) * velocidad * 0.6;
        vy = QRandomGenerator::global()->generateDouble() * velocidad * 0.4 + velocidad * 0.6;

        if (qAbs(vx) > qAbs(vy) * 0.8) {
            vx *= 0.5;
        }
        if (vy < velocidad * 0.4) {
            vy = velocidad * 0.4;
        }
        if (qAbs(vx) < minComponente) {
            vx = (vx >= 0) ? minComponente : -minComponente;
        }
        if (qAbs(vy) < minComponente) {
            vy = (vy >= 0) ? minComponente : -minComponente;
        }
        break;

    case 1: // DERECHA - Movimiento hacia IZQUIERDA con variación vertical
        posicion.setX(screenWidth + anchoSprite + margenFuera);
        posicion.setY(QRandomGenerator::global()->bounded(margenBorde, screenHeight - margenBorde));

        vx = -(QRandomGenerator::global()->generateDouble() * velocidad * 0.6 + velocidad * 0.8);
        vy = (QRandomGenerator::global()->generateDouble() - 0.5) * velocidad * 0.8;

        if (qAbs(vy) > qAbs(vx) * 0.8) {
            vy *= 0.5;
        }
        if (vx > -velocidad * 0.4) {
            vx = -velocidad * 0.4;
        }
        if (qAbs(vx) < minComponente) {
            vx = (vx >= 0) ? minComponente : -minComponente;
        }
        if (qAbs(vy) < minComponente) {
            vy = (vy >= 0) ? minComponente : -minComponente;
        }
        break;

    case 2: // ABAJO - Movimiento hacia ARRIBA con variación horizontal
        posicion.setX(QRandomGenerator::global()->bounded(margenBorde, screenWidth - margenBorde));
        posicion.setY(screenHeight + altoSprite + margenFuera);

        vx = (QRandomGenerator::global()->generateDouble() - 0.5) * velocidad * 0.8;
        vy = -(QRandomGenerator::global()->generateDouble() * velocidad * 0.6 + velocidad * 0.8);

        if (qAbs(vx) > qAbs(vy) * 0.8) {
            vx *= 0.5;
        }
        if (vy > -velocidad * 0.4) {
            vy = -velocidad * 0.4;
        }
        if (qAbs(vx) < minComponente) {
            vx = (vx >= 0) ? minComponente : -minComponente;
        }
        if (qAbs(vy) < minComponente) {
            vy = (vy >= 0) ? minComponente : -minComponente;
        }
        break;

    case 3: // IZQUIERDA - Movimiento hacia DERECHA con variación vertical
        posicion.setX(-anchoSprite - margenFuera);
        posicion.setY(QRandomGenerator::global()->bounded(margenBorde, screenHeight - margenBorde));

        vx = QRandomGenerator::global()->generateDouble() * velocidad * 0.6 + velocidad * 0.8;
        vy = (QRandomGenerator::global()->generateDouble() - 0.5) * velocidad * 0.8;

        if (qAbs(vy) > qAbs(vx) * 0.8) {
            vy *= 0.5;
        }
        if (vx < velocidad * 0.4) {
            vx = velocidad * 0.4;
        }
        if (qAbs(vx) < minComponente) {
            vx = (vx >= 0) ? minComponente : -minComponente;
        }
        if (qAbs(vy) < minComponente) {
            vy = (vy >= 0) ? minComponente : -minComponente;
        }
        break;
    }

    // Garantizar velocidad mínima
    double velocidadActual = qSqrt(vx * vx + vy * vy);
    double velocidadMinimaTotal = velocidad * 0.6;  // AUMENTAR A 60% como mínimo total

    if (velocidadActual < velocidadMinimaTotal) {
        if (velocidadActual > 0) {
            double factor = velocidadMinimaTotal / velocidadActual;
            vx *= factor;
            vy *= factor;
        } else {
            double angulo = QRandomGenerator::global()->generateDouble() * M_PI * 2;
            vx = velocidadMinimaTotal * qCos(angulo);
            vy = velocidadMinimaTotal * qSin(angulo);
        }
    }

    double angulo = qAtan2(vy, vx);
    double minAng = qDegreesToRadians(20.0); // mínimo 20° hacia adentro

    switch(borde) {
    case 0: // Arriba
        if (fabs(angulo - M_PI/2) < minAng)
            vy += velocidad * 0.5;
        break;

    case 2: // Abajo
        if (fabs(angulo + M_PI/2) < minAng)
            vy -= velocidad * 0.5;
        break;

    case 1: // Derecha
        if (fabs(angulo - M_PI) < minAng)
            vx -= velocidad * 0.5;
        break;

    case 3: // Izquierda
        if (fabs(angulo) < minAng)
            vx += velocidad * 0.5;
        break;
    }


    x = posicion.x();
    y = posicion.y();
}

void pajaro::setParametrosAleatorios(const QSize& tamanioVentana){

    static std::random_device rd;  // Fuente de entropía (para la semilla)
    static std::mt19937 gen(rd());  // Genera numero
    std::uniform_int_distribution<> dist(1, 5);  // 1-5
    movimiento = dist(gen);

    tiempo = 0.0;

    std::uniform_real_distribution<> distVel(100.0, 150.0);
    velocidad = distVel(gen);

    inicializarMovimientoDesdeBorde(tamanioVentana);

    std::uniform_real_distribution<> distA(0.001, 0.01);
    a = distA(gen);

    std::uniform_real_distribution<> distAmp(20.0, 80.0);
    amplitud = distAmp(gen);

    std::uniform_real_distribution<> distFreq(0.02, 0.1);
    frecuencia = distFreq(gen);

    std::uniform_real_distribution<> distfase(0.0, M_PI * 2.0);
    fase = distfase(gen);

}

void pajaro::setPosicion(float x, float y){
    posicion.setX(x);
    posicion.setY(y);
    this->x = x;
    this->y = y;
}

QPointF pajaro::getPosicion()const{
    return posicion;
}

QRect pajaro::getRect() const {    
    if (!sprites.isEmpty() && frameActual < sprites.size()) {
        QPixmap currentSprite = sprites[frameActual];
        return QRect(posicion.x(), posicion.y(),
                     currentSprite.width(), currentSprite.height());
    }

    return QRect(posicion.x(), posicion.y(), 50, 60);

}
