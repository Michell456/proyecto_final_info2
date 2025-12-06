#include "dron.h"
#include <QDebug>
#include <cmath>

Dron::Dron() : posicion(400, 300), frameActual(0), contadorAnimacion(0), inmune(false) {
    velocidad = QPointF(0, 0);
    aceleracion = QPointF(0, 0);
    for (int i = 0; i < 4; i++) teclas[i] = false;
    cargarSprites();

    bateriaMaxima = 100.0f;
    bateria = bateriaMaxima;
    consumoMovimiento = 0.3f;
    consumoBase = 0.03f;

    anchoSprite = 20;
    altoSprite = 30;

    timerInmunidad = new QTimer(this);
    timerInmunidad->setSingleShot(true);
    connect(timerInmunidad, &QTimer::timeout, this, &Dron::finInmunidad);

}

void Dron::activarInmunidad(int milisegundos) {
    inmune = true;
    timerInmunidad->start(milisegundos);
}

void Dron::cargarSprites() {
    sprites.clear();

    for (int i = 1; i <= 6; i++) {
        QString ruta = QString(":/sprites/Nivel3/mov_dron%1.png").arg(i);
        QPixmap spriteOriginal(ruta);
        if (spriteOriginal.isNull()) {
            qDebug() << "Error cargando:" << ruta;
            QPixmap fallback(150, 150);
            fallback.fill(QColor(100, 100, 255));
            sprites.append(fallback);
        } else {
            QPixmap spriteEscalado = spriteOriginal.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            sprites.append(spriteEscalado);
        }
    }

    if (sprites.isEmpty()) {
        QPixmap fallback(150, 150);
        fallback.fill(QColor(255, 0, 0));
        sprites.append(fallback);
    }
}

void Dron::update(QSize tamanioVentana) {

    aceleracion = QPointF(0, 0);

    bateria -= consumoBase;
    bateria = qMax(0.0f, bateria);

    if (bateria <= 0) {
        velocidad *= 0.9f;
        posicion += velocidad;
        return;
    }

    if (aceleracion.manhattanLength() > 0) {
        bateria -= consumoMovimiento;
        bateria = qMax(0.0f, bateria);
    }

    if (teclas[0]) aceleracion.setY(-ACELERACION); // W - Arriba
    if (teclas[1]) aceleracion.setY(ACELERACION);  // S - Abajo
    if (teclas[2]) aceleracion.setX(-ACELERACION); // A - Izquierda
    if (teclas[3]) aceleracion.setX(ACELERACION);  // D - Derecha

    velocidad += aceleracion;

    // Limitar velocidad maxima
    float velocidadActual = sqrt(velocidad.x() * velocidad.x() +
                                 velocidad.y() * velocidad.y());
    if (velocidadActual > VELOCIDAD_MAXIMA) {
        velocidad = (velocidad / velocidadActual) * VELOCIDAD_MAXIMA;
    }

    velocidad *= FRICCION;

    if (velocidadActual < 0.05f) {
        velocidad = QPointF(0, 0);
    }

    posicion += velocidad;

    if (posicion.x() < 20) {
        posicion.setX(20);
        velocidad.setX(-velocidad.x() * 0.5f);
    }
    if (posicion.x() > tamanioVentana.width()-20) {
        posicion.setX(tamanioVentana.width()-20);
        velocidad.setX(-velocidad.x() * 0.5f);
    }
    if (posicion.y() < 40) {
        posicion.setY(40);
        velocidad.setY(-velocidad.y() * 0.5f);
    }
    if (posicion.y() > tamanioVentana.height()-20) {
        posicion.setY(tamanioVentana.height()-20);
        velocidad.setY(-velocidad.y() * 0.5f);
    }

    bool moviendose = teclas[0] || teclas[1] || teclas[2] || teclas[3];
    float velocidadParaAnimacion = velocidadActual / VELOCIDAD_MAXIMA;

    if (moviendose) {
        contadorAnimacion++;
        int velocidadAnimacion = qMax(1, 4 - int(velocidadParaAnimacion * 3));
        if (contadorAnimacion >= velocidadAnimacion) {
            frameActual = (frameActual + 1) % sprites.size();
            contadorAnimacion = 0;
        }
    } else {
        contadorAnimacion++;
        if (contadorAnimacion >= 20) {
            frameActual = (frameActual + 1) % sprites.size();
            contadorAnimacion = 0;
        }
    }
}
void Dron::draw(QPainter &p) {
    if (!sprites.isEmpty() && frameActual < sprites.size()) {
        if(inmune){
            p.setOpacity(0.5);
        }
        else{
            p.setOpacity(1.0);
        }
        p.drawPixmap(posicion.x() - 75, posicion.y() - 75, 150, 150, sprites[frameActual]);
    } else {
        p.setBrush(QColor(0, 100, 200));
        p.setPen(Qt::black);
        p.drawEllipse(posicion, 75, 75);
    }
}

void Dron::handleInput(QKeyEvent *e) {
    switch (e->key()) {
    case Qt::Key_W: teclas[0] = true; break;
    case Qt::Key_S: teclas[1] = true; break;
    case Qt::Key_A: teclas[2] = true; break;
    case Qt::Key_D: teclas[3] = true; break;
    }
}

void Dron::handleKeyRelease(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_W: teclas[0] = false; break;
    case Qt::Key_S: teclas[1] = false; break;
    case Qt::Key_A: teclas[2] = false; break;
    case Qt::Key_D: teclas[3] = false; break;
    }
}

void Dron::cargarBateria(float cantidad) {
    bateria += cantidad;
    bateria = qMin(bateriaMaxima, bateria);
}

QRect Dron::getRect() const {
    return QRect(
        posicion.x() - hitboxW / 2,
        posicion.y() - hitboxH / 2,
        hitboxW,
        hitboxH
        );
}

void Dron::quitarBateria(){
    bateria -= 10.0f;
}
