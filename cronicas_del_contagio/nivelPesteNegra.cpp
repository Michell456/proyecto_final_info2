#include "nivelPesteNegra.h"
#include <QRandomGenerator>
#include <QPainter>

nivelPesteNegra::nivelPesteNegra(QObject *parent) : nivel(parent) {

    fondo.load(":/sprites/Nivel1/fondo_nivel1.png");

    if (fondo.isNull()){
        qDebug() << "ERROR: fondo NO cargó";
    }
    fondoX1 = 0;
    fondoX2 = fondo.width();

    contadorTiempo = 0;
    tiempoTranscurrido = 0;
    multiplicadorDificultad = 1.0f;

    velocidadFondo = 2;

    jugador.setPosicion(12,300);
    jugadorptr = &jugador;

    // Control spawn enemigos
    contadorSpawnEnemigo = 0;
    intervaloSpawnEnemigo = 30;  // Aprox 0.7 segundos (60 FPS × 0.7)
    probabilidadSpawnEnemigo = 90;

    // Control enemigo inteligente
    inteligenteActual = new enfermoInteligente();
    contadorInteligente = 0;
    dibujarInteligente = false;
    aparicionesInteligente = 0;
    frecuenciaInteligente = 600;

    // señal para items recogidos por el inteligente
    connect(inteligenteActual, &enfermoInteligente::recogeItem, this, &nivelPesteNegra::borrarItemRecogido);
    inteligenteActual->seleccionarSkin();

    // Control spawn items
    contadorSpawnItem = 0;
    intervaloSpawnItem = 240; // aprox 4 segundos 240
    probabilidadSpawnItem1 = 60; // 60% de probabilidad
    probabilidadSpawnItem2 = 20; // 20% de probabilidad
    probabilidadSpawnItem3 = 15; // 15% de probabilidad

    velocidadFondoBase = 2;
    intervaloSpawnEnemigoBase = 30;
    intervaloSpawnItemBase = 240;
    frecuenciaInteligenteBase = 600;

    sonidoColision.setSource(QUrl("qrc:/sonido/Nivel1/colision.wav"));
    sonidoColision.setVolume(1.0);

    sonidoTos1.setSource(QUrl("qrc:/sonido/Nivel1/tos1.wav"));
    sonidoTos1.setVolume(1.0);
    sonidoTos2.setSource(QUrl("qrc:/sonido/Nivel1/tos2.wav"));
    sonidoTos2.setVolume(1.0);
    sonidoTos3.setSource(QUrl("qrc:/sonido/Nivel1/tos3.wav"));
    sonidoTos3.setVolume(1.0);

    sonidoEstornudo1.setSource(QUrl("qrc:/sonido/Nivel1/estornudo1.wav"));
    sonidoEstornudo1.setVolume(1.0);
    sonidoEstornudo2.setSource(QUrl("qrc:/sonido/Nivel1/estornudo2.wav"));
    sonidoEstornudo2.setVolume(1.0);
    sonidoEstornudo3.setSource(QUrl("qrc:/sonido/Nivel1/estornudo3.wav"));
    sonidoEstornudo3.setVolume(1.0);

    probabilidadSonido = 20;

    sonidoItem2.setSource(QUrl("qrc:/sonido/Nivel1/item1.wav"));
    sonidoItem2.setVolume(1.0);
    sonidoItem3.setSource(QUrl("qrc:/sonido/Nivel1/item2.wav"));
    sonidoItem3.setVolume(0.5);

    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    player->setSource(QUrl("qrc:/sonido/Nivel1/musica_fondo.mp3"));
    audioOutput->setVolume(0.3);
    connect(player, &QMediaPlayer::mediaStatusChanged, this, [=](QMediaPlayer::MediaStatus status){
        if (status == QMediaPlayer::EndOfMedia) {
            player->play();
        }
    });
    player->play();
}

void nivelPesteNegra::update(){

    //chequearVictoria();
    //chequearDerrota();

    contadorTiempo++;
    if(contadorTiempo >= 60) { // Cada 60 frames, 1 segundo
        contadorTiempo = 0;
        tiempoTranscurrido++;
        aumentarDificultad();
    }

    fondoX1 -= velocidadFondo;
    fondoX2 -= velocidadFondo;

    if (fondoX1 + fondo.width() < 0) {
        fondoX1 = fondoX2 + fondo.width();
    }
    if (fondoX2 + fondo.width() < 0) {
        fondoX2 = fondoX1 + fondo.width();
    }

    jugador.update(tamanioVentana,velocidadFondo);

    contadorSpawnEnemigo++;
    if(contadorSpawnEnemigo >= intervaloSpawnEnemigo) {
        contadorSpawnEnemigo = 0;
        if(QRandomGenerator::global()->bounded(100) < probabilidadSpawnEnemigo) {
            spawnEnemigo();
        }
    }
    for(enfermo *enfermoActivo : enfermosActivos) {
        enfermoActivo->update();
    }

    contadorSpawnItem++;
    if(contadorSpawnItem >= intervaloSpawnItem) {
        contadorSpawnItem = 0;
        if(QRandomGenerator::global()->bounded(100) < probabilidadSpawnItem1) {
            spawnItem(1);
        }
        if(QRandomGenerator::global()->bounded(100) < probabilidadSpawnItem2) {
            spawnItem(2);
        }
        if(QRandomGenerator::global()->bounded(100) < probabilidadSpawnItem3) {
            spawnItem(3);
        }
    }
    for(item *itemActivo : items) {
        itemActivo->update(velocidadFondo);
    }

    contadorInteligente++;
    if(contadorInteligente >= frecuenciaInteligente && !dibujarInteligente) {
        spawnInteligente();
        contadorInteligente = 0;
    }

    if(dibujarInteligente) {
        inteligenteActual->update(jugadorptr, jugador.getCantidadItems(), items);

        if(inteligenteActual->getPosicion().x() + inteligenteActual->getAncho() < 0) {
            inteligenteActual->verificarEstadoRobo();
            inteligenteActual->resetRobo();
            regenerarInteligente();
        }
    }
    verificarColisiones();
    limpiarEntidades();

}

void nivelPesteNegra::spawnItem(int tipo){
    item *nuevoItem = new item();

    if (tipo == 1){
        nuevoItem->setTipo(1);
    }
    else if (tipo == 2){
        nuevoItem->setTipo(2);
    }
    else{
        nuevoItem->setTipo(3);
    }

    int posX = tamanioVentana.width();
    int pisoMinimo = tamanioVentana.height() - 200;
    int alturaCaidaMin = 50;
    int alturaCaidaMax = 100;
    int posYMin = 160;
    int posYMax = pisoMinimo - alturaCaidaMin;
    int posY = QRandomGenerator::global()->bounded(posYMin, posYMax);
    int alturaCaida = QRandomGenerator::global()->bounded(alturaCaidaMin, alturaCaidaMax);
    int yf = posY + alturaCaida;

    if (yf > pisoMinimo){
        yf = pisoMinimo;
    }

    nuevoItem->setPosicion(posX,posY);
    nuevoItem->setParametrosAleatorios(yf);

    items.append(nuevoItem);
}

void nivelPesteNegra::regenerarInteligente(){

    double posX = tamanioVentana.width()+3.0;
    double minY = 230.0;
    double maxY = tamanioVentana.height() - 200.0;
    double posY = minY + QRandomGenerator::global()->generateDouble() * (maxY - minY);
    inteligenteActual->setPosicion(posX,posY);

    inteligenteActual->estado = enfermoInteligente::Caminando;
    inteligenteActual->itemObjetivo = nullptr;
    inteligenteActual->logroRobar = false;
    inteligenteActual->activo = true;

    inteligenteActual->comenzarActividad(20);

    contadorInteligente = 0;
    aparicionesInteligente++;

}

void nivelPesteNegra::spawnInteligente(){

    double posX = tamanioVentana.width()+3.0;
    double minY = 230.0;
    double maxY = tamanioVentana.height() - 200.0;
    double posY = minY + QRandomGenerator::global()->generateDouble() * (maxY - minY);
    qDebug() << "Posicion X: " << posX;
    qDebug() << "Posicion Y: " << posY;
    inteligenteActual->setPosicion(posX,posY);
    dibujarInteligente = true;
    aparicionesInteligente++;

    inteligenteActual->comenzarActividad(20);

}

void nivelPesteNegra::limpiarEntidades() {

    for(int i = enfermosActivos.size() - 1; i >= 0; i--) {
        enfermo *enfermo = enfermosActivos[i];

        if(enfermo->getPosicion().x() + enfermo->getAncho() < 0) {
            enfermosActivos.removeAt(i);
            delete enfermo;
            break;
        }
    }

    for(int i = items.size() - 1; i >= 0; i--) {
        item *item = items[i];

        if(item->getPosicion().x() + item->getAncho() < 0) {
            items.removeAt(i);
            delete item;
            break;
        }
    }
}

void nivelPesteNegra::spawnEnemigo() {

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 6);
    int sonido = dist(gen);

    if (QRandomGenerator::global()->bounded(100) < probabilidadSonido){
        switch (sonido) {
        case 1:
            sonidoTos1.play();
            break;
        case 2:
            sonidoTos2.play();
            break;
        case 3:
            sonidoTos3.play();
            break;
        case 4:
            sonidoEstornudo1.play();
            break;
        case 5:
            sonidoEstornudo2.play();
            break;
        case 6:
            sonidoEstornudo3.play();
            break;
        }
    }

    enfermo *nuevoEnfermo = new enfermo();
    nuevoEnfermo->seleccionarSkin();

    int posX = tamanioVentana.width()+2;
    int posY = QRandomGenerator::global()->bounded(230, tamanioVentana.height() -200);

    nuevoEnfermo->setPosicion(posX, posY);
    nuevoEnfermo->setVelocidad(3 * multiplicadorDificultad);
    enfermosActivos.append(nuevoEnfermo);
}

void nivelPesteNegra::draw(QPainter &p){

    p.drawPixmap(fondoX1, 0, fondo);
    p.drawPixmap(fondoX2, 0, fondo);

    p.setOpacity(1.0);

    for(item *item : items) {
        item->draw(p);
    }

    jugador.draw(p);

    p.setOpacity(0.5);
    if (dibujarInteligente){
        p.setOpacity(0.5);
        inteligenteActual->draw(p);
    }
    p.setOpacity(1.0);

    for(enfermo *enfermo : enfermosActivos) {
        enfermo->draw(p);
    }

    p.setPen(Qt::white);
    p.setFont(QFont("Times New Roman", 14, QFont::Bold));

    p.setBrush(QColor(0,0,0,150));
    p.setPen(Qt::NoPen);
    p.drawRect(10, 10, 230, 80);

    p.setPen(Qt::white);
    p.drawText(20, 35, "Vidas: " + QString::number(jugador.consultarVida()));
    p.drawText(20, 55, "Hierbas: " + QString::number(jugador.getCantidadItems()));
    p.drawText(20, 75, QString("Inmunidad: %1").arg(jugador.getInmunidadInteligente() ? "Activada" : "Desactivada"));


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

    for(item *item : items) {
        QRect rectItem = item->getRect();

        if(rectJugador.intersects(rectItem)) {
            manejarColision(jugador,item);
            break;
        }
    }

}

void nivelPesteNegra::manejarColision(jugador1 &jugador, enfermo *enfermo){
    sonidoColision.play();
    jugador.activarInmunidad(1500);
    jugador.quitarVida();

}

void nivelPesteNegra::manejarColision(jugador1 &jugador, item *item_){

    if(item_->getTipo() == 1){
        jugador.sumarItem();
    }
    else if(item_->getTipo() == 2){
        jugador.sumarVida();
        sonidoItem2.play();
    }
    else if (item_->getTipo() == 3 && jugador.getInmunidadInteligente() == false){
        jugador.setInmuneInteligente(15000);
        sonidoItem3.play();
    }

    for(int i = items.size() - 1; i >= 0; i--) {
        item *item = items[i];

        if(item == item_) {
            items.removeAt(i);
            delete item;
            break;
        }
    }
}

void nivelPesteNegra::borrarItemRecogido(item* itemRecogido){
    for(int i = items.size() - 1; i >= 0; i--) {
        item *item = items[i];

        if(item == itemRecogido) {
            items.removeAt(i);
            delete item;
            break;
        }
    }
}

void nivelPesteNegra::handleInput(QKeyEvent *event){
    jugador.keyPressEvent(event);
}

void nivelPesteNegra::handleKeyRelease(QKeyEvent *event){
    jugador.keyReleaseEvent(event);
}

bool nivelPesteNegra::chequearVictoria(){
    return jugador.getCantidadItems() >= 10;
}

bool nivelPesteNegra::chequearDerrota(){
    return jugador.consultarVida() <= 0;
}

void nivelPesteNegra::aumentarDificultad() {

    if(tiempoTranscurrido % 30 == 0 && tiempoTranscurrido > 0) {
        float nuevoMultiplicador = 1.0f + (tiempoTranscurrido / 60.0f) * 0.15f;
        multiplicadorDificultad = qMin(2.5f, nuevoMultiplicador);

        velocidadFondo = qMin(6.0f, velocidadFondoBase * multiplicadorDificultad);

        for(enfermo* e : enfermosActivos) {
            e->setVelocidad(3 * multiplicadorDificultad);
        }

        if(inteligenteActual) {
            inteligenteActual->setVelocidad(3 * multiplicadorDificultad);
        }

        intervaloSpawnEnemigo = qMax(15, (int)(intervaloSpawnEnemigoBase / multiplicadorDificultad));
        intervaloSpawnItem = qMax(80, (int)(intervaloSpawnItemBase / multiplicadorDificultad));
        frecuenciaInteligente = qMax(180, (int)(frecuenciaInteligenteBase / multiplicadorDificultad));

        probabilidadSpawnEnemigo = qMin(95, 70 + (int)(tiempoTranscurrido / 15));
    }

}
