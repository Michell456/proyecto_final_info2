#include "nivelPesteNegra.h"
#include <QRandomGenerator>
#include <QPainter>

nivelPesteNegra::nivelPesteNegra() {

    fondo.load("sprites/nivel_1/fondo_nivel1.png");

    if (fondo.isNull())
        qDebug() << "ERROR: fondo NO cargó";

    fondoX1 = 0;
    fondoX2 = fondo.width();

    velocidadFondo = 2;

    jugador.setPosicion(12,300);

    // Control spawn enemigos
    contadorSpawn = 0;
    intervaloSpawn = 20;  // Aprox 0.5 segundos (60 FPS × 0.5)
    probabilidadSpawn = 80;  // 80% de probabilidad
    oleadaActual = 1;
    contadorOleadas = 0;
    intervaloEntreOleadas = 300; // 5 segundos

    //spawnItems();

}

void nivelPesteNegra::update(){

    chequearDerrota();

    fondoX1 -= velocidadFondo;
    fondoX2 -= velocidadFondo;

    if (fondoX1 + fondo.width() < 0) {
        fondoX1 = fondoX2 + fondo.width();
    }
    if (fondoX2 + fondo.width() < 0) {
        fondoX2 = fondoX1 + fondo.width();
    }

    jugador.update(tamanioVentana,velocidadFondo);

    contadorSpawn++;
    if(contadorSpawn >= intervaloSpawn) {
        contadorSpawn = 0;
        if(QRandomGenerator::global()->bounded(100) < probabilidadSpawn) {
            spawnEnemigo();
        }
    }
    for(enfermo *enfermoActivo : enfermosActivos) {
        enfermoActivo->update();
    }
    verificarColisiones();
    limpiarEnemigos();

}

void nivelPesteNegra::limpiarEnemigos() {

    for(int i = enfermosActivos.size() - 1; i >= 0; i--) {
        enfermo *enfermo = enfermosActivos[i];

        if(enfermo->getPosicion().x() + enfermo->getAncho() < 0) {
            enfermosActivos.removeAt(i);
            delete enfermo;
        }
    }
}

void nivelPesteNegra::spawnEnemigo() {
    enfermo *nuevoEnfermo = new enfermo();
    nuevoEnfermo->seleccionarSkin();

    int posX = tamanioVentana.width()+3;
    int posY = QRandomGenerator::global()->bounded(230, tamanioVentana.height() -200);

    nuevoEnfermo->setPosicion(posX, posY);
    enfermosActivos.append(nuevoEnfermo);
}

void nivelPesteNegra::draw(QPainter &p){

    p.drawPixmap(fondoX1, 0, fondo);
    p.drawPixmap(fondoX2, 0, fondo);

    jugador.draw(p);
    p.setOpacity(1.0);

    for(enfermo *enfermo : enfermosActivos) {
        enfermo->draw(p);
    }

    p.setPen(Qt::white);
    p.setFont(QFont("Times New Roman", 14, QFont::Bold));

    QString textoStats;
    textoStats += "Vidas: " + QString::number(jugador.consultarVida()) + "\n";

    p.setBrush(QColor(0,0,0,150));
    p.setPen(Qt::NoPen);
    p.drawRect(10, 10, 100, 40);

    p.setPen(Qt::white);
    p.drawText(20, 35, textoStats);

}

void nivelPesteNegra::verificarColisiones(){

    QRect rectJugador = jugador.getRect();

    for(enfermo *enfermo : enfermosActivos) {
        QRect rectEnfermo = enfermo->getRect();

        if(rectJugador.intersects(rectEnfermo)) {
            if(!jugador.estaInmune()) {
                manejarColision(jugador, enfermo);
            }
            break;
        }
    }

}
void nivelPesteNegra::manejarColision(jugador1 &jugador, enfermo *enfermo){

    jugador.activarInmunidad(1500);
    jugador.quitarVida();

}

void nivelPesteNegra::handleInput(QKeyEvent *event){
    jugador.keyPressEvent(event);
}

void nivelPesteNegra::handleKeyRelease(QKeyEvent *event){
    jugador.keyReleaseEvent(event);
}

bool nivelPesteNegra::chequearVictoria(){

}
bool nivelPesteNegra::chequearDerrota(){
    if(jugador.consultarVida() == 0){

    }
}
