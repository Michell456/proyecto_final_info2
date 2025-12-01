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
    intervaloSpawnPajaro = 120;  // Aprox 2 segundos (60 FPS × 0.7)
    probabilidadSpawnPajaro = 70;
}

void NivelCovid::update() {
    if (estado != EstadoNivel::jugando) return;

    tiempoTranscurrido++;
    dron.update(tamanioVentana);

    contadorSpawnPajaro++;
    if(contadorSpawnPajaro >= intervaloSpawnPajaro) {
        contadorSpawnPajaro = 0;
        if(QRandomGenerator::global()->bounded(100) < probabilidadSpawnPajaro) {
            spawnPajaro();
        }
    }
    for(pajaro *pajaroActivo : pajaros) {
        pajaroActivo->update();
    }

    if (baseCarga.estaCargando(dron.getPosition())) {
        dron.cargarBateria(0.8f);
    }
}

void NivelCovid::limpiarEntidades() {

    for(int i = pajaros.size() - 1; i >= 0; i--) {
        pajaro *pajaro = pajaros[i];

        if(pajaro->getPosicion().x() + pajaro->getAncho() < -6
            || pajaro->getPosicion().x() + pajaro->getAncho() > tamanioVentana.width() + 7
            || pajaro->getPosicion().y() + pajaro->getAltura() < -6
            || pajaro->getPosicion().y() + pajaro->getAltura() > tamanioVentana.height() + 7) {
            pajaros.removeAt(i);
            delete pajaro;
            break;
        }
    }

}

void NivelCovid::spawnPajaro() {

    pajaro *nuevoPajaro = new pajaro();
    nuevoPajaro->setParametrosAleatorios();

    int posX = QRandomGenerator::global()->bounded(-5, tamanioVentana.width() + 6);
    int posY = QRandomGenerator::global()->bounded(-5, tamanioVentana.height() + 6);

    nuevoPajaro->setPosicion(posX,posY);
    pajaros.append(nuevoPajaro);
}

void NivelCovid::draw(QPainter &p) {
    // Dibujar el fondo
    p.drawPixmap(0, 0, 1100, 650, fondo);

    // Dibujar base de carga
    baseCarga.draw(p);

    // Dibujar dron
    dron.draw(p);

    for(pajaro *pajaro : pajaros) {
        pajaro->draw(p);
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
