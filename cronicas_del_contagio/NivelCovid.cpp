#include "nivelcovid.h"
#include <QPainter>
#include <cmath>
#include <QRandomGenerator>
#include "BaseDeCarga.h"

NivelCovid::NivelCovid()
    : baseCarga(QPointF(1050, 50))
{

    fondo.load("sprites/Nivel3/fondo3.1.png");
    if (fondo.isNull()) {
        fondo = QPixmap(800, 600);
        fondo.fill(Qt::white);
    }

    contadorSpawnPajaro = 0;
    intervaloSpawnPajaro = 120;
    probabilidadSpawnPajaro = 90;

    contadorSpawnInfeccion = 0;
    intervaloSpawnInfeccion = 200;
    probabilidadSpawnInfeccion = 90;
}

void NivelCovid::update() {
    if (estado != EstadoNivel::jugando) return;

    tiempoTranscurrido++;
    dron.update(tamanioVentana);

    static QElapsedTimer timer;
    static bool firstTime = true;

    if (firstTime) {
        timer.start();
        firstTime = false;
    }

    float deltaTime = timer.elapsed() / 1000.0f;
    timer.restart();

    if (deltaTime > 0.1f) deltaTime = 0.1f;

    contadorSpawnPajaro++;
    if(contadorSpawnPajaro >= intervaloSpawnPajaro) {
        contadorSpawnPajaro = 0;
        if(QRandomGenerator::global()->bounded(100) < probabilidadSpawnPajaro) {
            spawnPajaro();
        }
    }
    for(pajaro *pajaroActivo : pajaros) {
        pajaroActivo->update(deltaTime, tamanioVentana);
    }

    contadorSpawnInfeccion++;
    if(contadorSpawnInfeccion >= intervaloSpawnInfeccion) {
        contadorSpawnInfeccion = 0;
        if(QRandomGenerator::global()->bounded(100) < probabilidadSpawnInfeccion) {
            spawnZonaInfeccion();
        }
    }
    for(ZonaInfeccion *zona : zonasInfeccion) {
        zona->update(deltaTime);
    }

    limpiarEntidades();

    if (baseCarga.estaCargando(dron.getPosition())) {
        dron.cargarBateria(0.8f);
    }

    verificarColisiones();
}

void NivelCovid::limpiarEntidades() {

    for(int i = pajaros.size() - 1; i >= 0; i--) {
        pajaro *p = pajaros[i];
        QPointF pos = p->getPosicion();

        int margen = 100;

        if(pos.x() + p->getAncho() < -margen
            || pos.x() > tamanioVentana.width() + margen
            || pos.y() + p->getAltura() < -margen
            || pos.y() > tamanioVentana.height() + margen) {
            pajaros.removeAt(i);
            delete p;
        }
    }

}

void NivelCovid::spawnZonaInfeccion() {
    if (zonasInfeccion.size() >= 7) return;

    ZonaInfeccion* nuevaZona = new ZonaInfeccion(tamanioVentana);
    bool demasiadoCerca = false;

    for (ZonaInfeccion* zonaExistente : zonasInfeccion) {
        if (nuevaZona->estaDemasiadoCercaDe(zonaExistente->getPosicion(),
                                            zonaExistente->getRadio())) {
            demasiadoCerca = true;
            break;
        }
    }

    if (demasiadoCerca) {
        delete nuevaZona;
        return;
    }

    zonasInfeccion.append(nuevaZona);
}


void NivelCovid::spawnPajaro() {

    pajaro *nuevoPajaro = new pajaro();
    nuevoPajaro->setParametrosAleatorios(tamanioVentana);
    pajaros.append(nuevoPajaro);
}

void NivelCovid::draw(QPainter &p) {
    // Dibujar el fondo
    p.drawPixmap(0, 0, 1100, 650, fondo);

    // Dibujar base de carga
    baseCarga.draw(p);

    // Dibujar dron
    dron.draw(p);
    p.setOpacity(1.0);

    for(pajaro *pajaro : pajaros) {
        pajaro->draw(p);
    }

    for(ZonaInfeccion *zona : zonasInfeccion) {
        zona->draw(p);
    }

    // UI
    p.setPen(Qt::black);
    p.setBrush(QColor(255, 255, 255, 200));
    p.drawRect(10, 10, 250, 120);

    p.drawText(20, 30, "Nivel COVID-19 - WASD para mover");

    // Barra de bateria
    p.drawText(20, 50, "Batería:");
    QRectF bateriaRect(80, 40, 100, 10);
    p.drawRect(bateriaRect);
    float porcentajeBateria = dron.getBateria() / dron.getBateriaMaxima();
    p.fillRect(QRectF(bateriaRect.left(), bateriaRect.top(),
                      bateriaRect.width() * porcentajeBateria, bateriaRect.height()),
               dron.getBateria() > 20 ? Qt::green : Qt::red);

    // Estado
    if (baseCarga.estaCargando(dron.getPosition())) {
        p.drawText(20, 70, "Estado: CARGANDO...");
    } else if (dron.getBateria() <= 0) {
        p.drawText(20, 70, "Estado: SIN BATERÍA!");
    } else {
        p.drawText(20, 70, "Estado: VOLANDO");
    }

    p.drawText(20, 90, QString("Batería: %1%").arg(dron.getBateria(), 0, 'f', 0));
    p.drawText(20, 110, QString("Tiempo: %1").arg(tiempoTranscurrido / 60));

    // Mensajes de estado del juego
    if (estado == EstadoNivel::ganado) {
        p.setFont(QFont("Arial", 24, QFont::Bold));
        p.drawText(300, 300, "¡VICTORIA!");
    } else if (estado == EstadoNivel::perdido) {
        p.setFont(QFont("Arial", 24, QFont::Bold));
        p.drawText(300, 300, "¡DERROTA!");
    }
}

void NivelCovid::verificarColisiones(){

    QRect rectDron = dron.getRect();

    for(pajaro *pajaro : pajaros) {
        QRect rectPajaro = pajaro->getRect();

        if(rectDron.intersects(rectPajaro)) {
            if(!dron.estaInmune()) {
                dron.activarInmunidad(1500);
                dron.quitarBateria();
            }
            break;
        }
    }

    for (ZonaInfeccion* zona : zonasInfeccion) {
        if (zona && zona->estaActiva()) {
            QRect rectZona = zona->getRect();

            if (rectDron.intersects(rectZona)) {
                if (!zona->estaSiendoDesinfectada()) {
                    zona->iniciarDesinfeccion();
                }
            } else {
                if (zona->estaSiendoDesinfectada()) {
                    zona->detenerDesinfeccion();
                }
            }
        }
    }

}

void NivelCovid::handleInput(QKeyEvent *e) {
    if (estado != EstadoNivel::jugando) return;
    dron.handleInput(e);
}

void NivelCovid::handleKeyRelease(QKeyEvent *event) {
    dron.handleKeyRelease(event);
}

bool NivelCovid::chequearVictoria() {
    return tiempoTranscurrido > 600;
}

bool NivelCovid::chequearDerrota() {
    QPointF pos = dron.getPosition();
    return (pos.x() <= 0 || pos.x() >= 800 || pos.y() <= 0 || pos.y() >= 600);
}
