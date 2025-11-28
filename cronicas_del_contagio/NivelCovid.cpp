#include "nivelcovid.h"
#include <QPainter>
#include <cmath>

NivelCovid::NivelCovid() {
    fondo.load("C:/Users/miche/Desktop/tercer_nivel/proyecto_final_info2/cronicas_del_contagio/sprites/Nivel3/blanco.jpg");
    if (fondo.isNull()) {
        fondo = QPixmap(800, 600);
        fondo.fill(Qt::white);
    }
}

void NivelCovid::update() {
    if (estado != EstadoNivel::jugando) return;

    tiempoTranscurrido++;
    dron.update();
}

void NivelCovid::draw(QPainter &p) {
    // Dibujar el fondo
    p.drawPixmap(0, 0, 800, 600, fondo);

    // Dibujar dron
    dron.draw(p);

    // UI con información de física
    p.setPen(Qt::black);
    p.setBrush(QColor(255, 255, 255, 200));
    p.drawRect(10, 10, 250, 100);

    p.drawText(20, 30, "Nivel COVID-19 - WASD para mover");
    p.drawText(20, 90, QString("Tiempo: %1").arg(tiempoTranscurrido / 60));
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
