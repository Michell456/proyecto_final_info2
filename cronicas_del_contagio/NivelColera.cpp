#include "nivelcolera.h"
#include "doctor.h"
#include "proyectil.h"
#include "obstaculo.h"
#include "balde.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QPainter>
#include <cmath>
#include <QFileInfo>
#include <QTimer>

NivelColera::NivelColera(QObject *parent)
    : nivel(parent)
    , doctor(nullptr)
    , proyectilActual(nullptr)
    , proyectilPiedra(nullptr)
    , proyectilAmpolla(nullptr)
    , arrastrando(false)
    , clickEnDoctor(false)
{
    // Inicializar en orden
    inicializarRecursos();
    inicializarConfiguraciones();

    doctor = new Doctor();
    doctor->setPosicion(QPointF(100, 350));

    proyectilPiedra = new Proyectil(configPiedra.sprite, configPiedra.gravedad,
                                    configPiedra.factorRebote, configPiedra.puedeDestruirObstaculos,
                                    configPiedra.puedeLlenarBaldes, configPiedra.puedeRebotar,
                                    configPiedra.maxColisiones,
                                    this);

    proyectilAmpolla = new Proyectil(configAmpolla.sprite, configAmpolla.gravedad,
                                     configAmpolla.factorRebote, configAmpolla.puedeDestruirObstaculos,
                                     configAmpolla.puedeLlenarBaldes, configAmpolla.puedeRebotar,
                                     configAmpolla.maxColisiones,
                                     this);

    proyectilActual = proyectilPiedra;

    // Crear objetos del nivel
    crearObstaculos();
    crearBaldes();

    // Configurar objetivos de los proyectiles
    proyectilPiedra->setObjetivos(&obstaculos, &baldes);
    proyectilAmpolla->setObjetivos(&obstaculos, &baldes);

    // Cargar sonidos
    sonidoBaldeAmpolla.setSource(QUrl("qrc:/sonido/Nivel2/balde_ampolla.wav"));
    sonidoDestruccionBalde.setSource(QUrl("qrc:/sonido/Nivel2/destruccion_balde.wav"));
    sonidoDestruccionMadera.setSource(QUrl("qrc:/sonido/Nivel2/destruccion_madera.wav"));
    sonidoReboteMadera.setSource(QUrl("qrc:/sonido/Nivel2/rebote_madera.wav"));

    // Configurar volumen
    sonidoBaldeAmpolla.setVolume(0.8f);
    sonidoDestruccionBalde.setVolume(0.7f);
    sonidoDestruccionMadera.setVolume(0.6f);
    sonidoReboteMadera.setVolume(0.5f);
}

NivelColera::~NivelColera()
{
    delete doctor;
    delete proyectilPiedra;
    delete proyectilAmpolla;
    qDeleteAll(obstaculos);
    qDeleteAll(baldes);
}

void NivelColera::update()
{
    tiempoTranscurrido++;

    // Actualizar animacion del doctor
    if (doctor) {
        doctor->actualizarAnimacionLanzamiento();
    }

    // Actualizar proyectil en movimiento
    if (proyectilActual && proyectilActual->estaEnMovimiento()) {
        proyectilActual->actualizarMovimiento();
    }

    // Verificar condiciones de fin de juego
    if (chequearVictoria()) {
        estado = EstadoNivel::ganado;
    } else if (chequearDerrota()) {
        estado = EstadoNivel::perdido;
    }
}

void NivelColera::draw(QPainter &p)
{
    // 1. Dibujar fondo
    p.drawPixmap(0, 0, fondo);

    // 2. Dibujar obstaculos
    for (Obstaculo* obstaculo : obstaculos) {
        if (!obstaculo->estaDestruido()) {
            QPointF pos = obstaculo->getPosicion();
            QPixmap sprite = obstaculo->getSprite();
            p.drawPixmap(pos.x(), pos.y(), sprite);
        }
    }

    // 3. Dibujar baldes
    for (Balde* balde : baldes) {
        QPointF pos = balde->getPosicion();
        QPixmap sprite = balde->getSprite();
        p.drawPixmap(pos.x(), pos.y(), sprite);
    }

    // 4. Dibujar doctor
    if (doctor) {
        QPointF pos = doctor->getPosicion();
        QPixmap sprite = doctor->getSpriteActual();
        p.drawPixmap(pos.x(), pos.y(), sprite);
    }

    // 5. Dibujar proyectiles
    if (proyectilPiedra && proyectilPiedra->esVisible()) {
        QPointF pos = proyectilPiedra->getPosicion();
        QPixmap sprite = proyectilPiedra->getSprite();
        p.drawPixmap(pos.x(), pos.y(), sprite);
    }

    if (proyectilAmpolla && proyectilAmpolla->esVisible()) {
        QPointF pos = proyectilAmpolla->getPosicion();
        QPixmap sprite = proyectilAmpolla->getSprite();
        p.drawPixmap(pos.x(), pos.y(), sprite);
    }

    // 6. Dibujar lineas de trayectoria
    if (arrastrando) {
        // Puntos blancos
        p.setBrush(Qt::white);
        p.setPen(Qt::NoPen);

        for (const QRectF& punto : puntosTrayectoria) {
            p.drawEllipse(punto);
        }
    }

    // 7. Dibujar UI
    p.setPen(Qt::white);
    p.setFont(QFont("Arial", 16));
    p.drawText(20, 30, QString("Nivel - Cólera"));
    p.drawText(20, 60, QString("Tiempo: %1").arg(tiempoTranscurrido / 60));

    // Mostrar proyectil actual
    QString proyectilTexto = (proyectilActual == proyectilPiedra) ? "PIEDRA" : "AMPOLLA";
    p.drawText(20, 90, QString("Proyectil: %1").arg(proyectilTexto));

    // Mostrar baldes llenos
    int baldesLlenos = 0;
    for (Balde* balde : baldes) {
        if (balde->estaLleno()) baldesLlenos++;
    }
    p.drawText(20, 120, QString("Baldes llenos: %1/%2").arg(baldesLlenos).arg(baldes.size()));

    // 8. Dibujar estado del juego
    if (estado == EstadoNivel::ganado) {
        p.setPen(Qt::green);
        p.setFont(QFont("Arial", 32, QFont::Bold));
        p.drawText(350, 300, "¡VICTORIA!");
        p.setPen(Qt::white);
        p.setFont(QFont("Arial", 16));
        p.drawText(400, 350, "Todos los baldes están llenos");
    } else if (estado == EstadoNivel::perdido) {
        p.setPen(Qt::red);
        p.setFont(QFont("Arial", 32, QFont::Bold));
        p.drawText(350, 300, "DERROTA");
    }
}

void NivelColera::handleKeyRelease(QKeyEvent *event)
{
    Q_UNUSED(event)
}

void NivelColera::handleInput(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Space) {
        Proyectil* proyectil = obtenerProyectilActivo();
        if (proyectil && !proyectil->estaEnMovimiento() &&
            doctor && !doctor->estaLanzando()) {
            cambiarProyectil();
        }
    }
}

bool NivelColera::chequearVictoria()
{
    // Victoria: todos los baldes están llenos
    for (Balde* balde : baldes) {
        if (!balde->estaLleno()) {
            return false;
        }
    }
    return true;
}

bool NivelColera::chequearDerrota()
{
    // Derrota: tiempo agotado (120 segundos)
    return (tiempoTranscurrido / 60) >= 120;
}

void NivelColera::handleMousePress(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && estado == EstadoNivel::jugando) {
        Proyectil* proyectil = obtenerProyectilActivo();
        if (proyectil && proyectil->estaEnMovimiento()) return;
        if (doctor && doctor->estaLanzando()) return;
        if (arrastrando) return;

        QPointF scenePos = event->pos();

        if (estaSobreDoctor(scenePos)) {
            puntoInicio = scenePos;
            puntoActual = scenePos;
            arrastrando = true;
            clickEnDoctor = true;

            doctor->iniciarLanzamiento();

            // Posicionar proyectil detrás del doctor
            QPointF posDoctor = doctor->getPosicion();
            QPixmap spriteDoctor = doctor->getSpriteActual();
            float proyectilX = posDoctor.x() + 40;
            float proyectilY = posDoctor.y() + 30;

            if (proyectilActual) {
                proyectilActual->setPosicion(QPointF(proyectilX, proyectilY));
                proyectilActual->setVisible(true);
            }
        }
    }
}

void NivelColera::handleMouseMove(QMouseEvent *event)
{
    if (arrastrando && clickEnDoctor) {
        puntoActual = event->pos();

        if (!proyectilActual) return;

        QPointF posProyectil = proyectilActual->getPosicion();
        QPixmap spriteProyectil = proyectilActual->getSprite();
        QPointF centroProyectil(posProyectil.x() + spriteProyectil.width()/2,
                                posProyectil.y() + spriteProyectil.height()/2);

        QPointF diferencia = puntoInicio - puntoActual;
        float distancia = QVector2D(diferencia).length();
        float fuerza = calcularFuerza(distancia);

        QVector2D direccion(diferencia);
        if (direccion.length() > 0) {
            direccion.normalize();
        }

        QVector2D velocidadInicial = direccion * fuerza;
        dibujarTrayectoria(velocidadInicial, centroProyectil);
    }
}

void NivelColera::handleMouseRelease(QMouseEvent *event)
{
    if (arrastrando && clickEnDoctor) {
        QPointF scenePos = event->pos();

        if (!proyectilActual) {
            arrastrando = false;
            clickEnDoctor = false;
            return;
        }

        QPointF posProyectil = proyectilActual->getPosicion();
        QPixmap spriteProyectil = proyectilActual->getSprite();
        QPointF centroProyectil(posProyectil.x() + spriteProyectil.width()/2,
                                posProyectil.y() + spriteProyectil.height()/2);

        QPointF diferencia = puntoInicio - scenePos;
        float distancia = QVector2D(diferencia).length();
        float fuerza = calcularFuerza(distancia);

        QVector2D direccion(diferencia);
        if (direccion.length() > 0) {
            direccion.normalize();
        } else {
            direccion = QVector2D(1, 0);
        }

        QVector2D velocidadInicial = direccion * fuerza;

        // Continuar animación y lanzar proyectil
        doctor->continuarLanzamiento();

        QTimer::singleShot(200, [this, velocidadInicial, posProyectil]() {
            if (proyectilActual) {
                proyectilActual->lanzar(velocidadInicial, posProyectil);
            }
        });

        arrastrando = false;
        clickEnDoctor = false;
        limpiarLineas();
    }
}

void NivelColera::inicializarRecursos()
{
    // Cargar sprites desde recursos Qt
    fondo.load(":/sprites/Nivel2/fondoNivel2.png");
    spriteObstaculos.load(":/sprites/Nivel2/obstaculos.png");

    if (fondo.isNull()) {
        qDebug() << "ERROR: No se pudo cargar el fondo desde recursos";
        fondo = QPixmap(1000, 600);
        fondo.fill(QColor(100, 150, 255));
    }

    if (spriteObstaculos.isNull()) {
        qDebug() << "ERROR: No se pudo cargar sprites de obstaculos desde recursos";
    }
}

void NivelColera::inicializarConfiguraciones()
{
    // Configuración para piedra
    configPiedra.gravedad = 0.8f;
    configPiedra.factorRebote = 0.4f;
    configPiedra.puedeDestruirObstaculos = true;
    configPiedra.puedeLlenarBaldes = false;
    configPiedra.puedeRebotar = false;
    configPiedra.maxColisiones = 1;

    // Cargar sprite de piedra desde recursos
    QPixmap piedraSprite(":/sprites/Nivel2/piedra.png");
    if (!piedraSprite.isNull()) {
        QPixmap piedraRecortada = piedraSprite.copy(55, 28, 326, 378);
        configPiedra.sprite = piedraRecortada.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    } else {
        qDebug() << "error al cargar sprite de piedra";
    }

    // Configuración para ampolla
    configAmpolla.gravedad = 0.5f;
    configAmpolla.factorRebote = 0.6f;
    configAmpolla.puedeDestruirObstaculos = true;
    configAmpolla.puedeLlenarBaldes = true;
    configAmpolla.puedeRebotar = true;
    configAmpolla.maxColisiones = 2;

    // Cargar sprite de ampolla desde recursos
    QPixmap ampollaSprite(":/sprites/Nivel2/ampolla.png");
    if (!ampollaSprite.isNull()) {
        configAmpolla.sprite = ampollaSprite.scaled(40, 55, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    } else {
        qDebug() << "Error al generar sprite de ampolla";
    }
}

void NivelColera::crearObstaculos()
{
    // Coordenadas de los obstaculos en el spritesheet
    struct CoordenadaObstaculo {
        int x, y, ancho, alto;
        QPointF posicionEscena;
    };

    QList<CoordenadaObstaculo> coordenadas = {

        // sprites maderas
        //{18, 13, 235, 37, QPointF(100, 100)}, //1
        //{19, 70, 320, 37, QPointF(150, 200)}, //2
        //{21, 127, 403, 38, QPointF(50, 300)}, //3
        //{21, 189, 154, 38, QPointF(400, 150)}, //4
        //{341, 196, 37, 230, QPointF(700, 100)}, //5
        //{268, 197, 38, 160, QPointF(600, 150)}, //6
        //{213, 198, 38, 69, QPointF(750, 200)}, //7
        //{14, 249, 107, 38, QPointF(300, 350)}, //8
        //{213, 292, 38, 37, QPointF(200, 400)}, //9
        //{11, 322, 62, 38, QPointF(350, 450)} //10

        //base plataforma

        {213, 198, 38, 69, QPointF(1500, 800)}, //7
        {213, 198, 38, 69, QPointF(800, 800)}, //7
        {21, 127, 403, 38, QPointF(1150, 770)}, //3
        {21, 127, 403, 38, QPointF(800, 770)}, //3
        {21, 189, 154, 38, QPointF(800, 770)}, //4

        //primer piso

        {268, 197, 38, 160, QPointF(950, 615)}, //6
        {268, 197, 38, 160, QPointF(1150, 615)}, //6
        {268, 197, 38, 160, QPointF(1350, 615)}, //6
        {19, 70, 320, 37, QPointF(1300, 600)}, //2
        {19, 70, 320, 37, QPointF(900, 600)}, //2
        {14, 249, 107, 38, QPointF(1200, 600)}, //8

        //segundo piso

        {213, 198, 38, 69, QPointF(1200, 535)}, //7
        {213, 198, 38, 69, QPointF(900, 535)}, //7
        {213, 198, 38, 69, QPointF(1500, 535)}, //7
        {21, 127, 403, 38, QPointF(1150, 500)}, //3
        {21, 127, 403, 38, QPointF(800, 500)}, //3
        {14, 249, 107, 38, QPointF(700, 500)}, //8

        //tercer piso

        {268, 197, 38, 160, QPointF(1350, 350)}, //6
        {268, 197, 38, 160, QPointF(950, 350)}, //6
        {21, 127, 403, 38, QPointF(1150, 370)}, //3
        {21, 127, 403, 38, QPointF(800, 370)}, //3

    };

    if (spriteObstaculos.isNull()) {
        qDebug() << "Error al generar sprite de obstaculos";
        return;
    }

    for (const auto& coord : coordenadas) {
        QPixmap spriteObstaculo = spriteObstaculos.copy(coord.x, coord.y, coord.ancho, coord.alto);

        if (!spriteObstaculo.isNull()) {
            Obstaculo *nuevoObstaculo = new Obstaculo(coord.posicionEscena, spriteObstaculo);
            obstaculos.append(nuevoObstaculo);
        }
    }

    QPixmap plataformaSprite(":/sprites/Nivel2/plataforma_dr.png");
    if (!plataformaSprite.isNull()) {
        // Escalar la plataforma (ajusta el tamaño según necesites)
        plataformaSprite = plataformaSprite.scaled(2800, 520, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // Posicionar la plataforma - COORDENADAS ACTUALES DEL DOCTOR:
        // El doctor está en (100, 350), así que la plataforma debe estar debajo
        QPointF posPlataforma(10, 370);

        Obstaculo *plataformaDoctor = new Obstaculo(posPlataforma, plataformaSprite);
        obstaculos.append(plataformaDoctor);
        plataformaDoctor->setAreaColision(QRectF(0, 0, 0, 0));

    }

}

void NivelColera::crearBaldes()
{
    QList<QPointF> posicionesBaldes = {

        QPointF(850, 710),
        QPointF(1225, 710),
        QPointF(1000, 540),
        //QPointF(1300, 540),
        QPointF(1150, 440),
        QPointF(1000, 440),
        QPointF(1275, 440),
        QPointF(1150, 310),
        QPointF(1400, 310),
        QPointF(850, 310)

    };

    for (int i = 0; i < posicionesBaldes.size(); ++i) {
        Balde *nuevoBalde = new Balde(posicionesBaldes[i],this);
        baldes.append(nuevoBalde);

        // Verificar area de colision inmediatamente
        QRectF area = nuevoBalde->getAreaColision();
        area.moveTo(posicionesBaldes[i]);
    }

}

void NivelColera::dibujarTrayectoria(const QVector2D& velocidadInicial, const QPointF& posicionInicial)
{
    limpiarLineas();

    QVector<QPointF> puntos = calcularTrayectoria(velocidadInicial, posicionInicial);

    // SOLO PUNTOS - sin líneas
    for (int i = 0; i < puntos.size(); i += 2) {  // Cada 2 puntos
        puntosTrayectoria.append(QRectF(puntos[i].x() - 3, puntos[i].y() - 3, 6, 6));
    }
}

void NivelColera::limpiarLineas()
{
    lineasTrayectoria.clear();
    puntosTrayectoria.clear();
}

QVector<QPointF> NivelColera::calcularTrayectoria(const QVector2D& velocidadInicial, const QPointF& posicionInicial)
{
    QVector<QPointF> puntos;
    const float deltaTime = 0.01f;
    const int maxSteps = 50;

    QVector2D pos(posicionInicial.x(), posicionInicial.y());
    QVector2D vel = velocidadInicial;
    float gravedad = getGravedadActual();

    for (int i = 0; i < maxSteps; i++) {
        puntos.append(QPointF(pos.x(), pos.y()));

        // Usar la misma función física
        QPointF nuevaPos = calcularPosicionFisica(QPointF(pos.x(), pos.y()), vel, gravedad, deltaTime);

        // Actualizar posición y velocidad para el siguiente paso
        QVector2D nuevaVel(nuevaPos.x() - pos.x(), nuevaPos.y() - pos.y());
        nuevaVel /= (deltaTime * 60.0f);

        pos = QVector2D(nuevaPos.x(), nuevaPos.y());
        vel = nuevaVel;

        if (pos.x() > 1200 || pos.x() < -200 || pos.y() > 800 || vel.length() < 0.5f) {
            break;
        }
    }

    return puntos;
}

float NivelColera::getGravedadActual() const
{
    return (proyectilActual == proyectilPiedra) ? configPiedra.gravedad : configAmpolla.gravedad;
}

bool NivelColera::estaSobreDoctor(const QPointF& punto)
{
    if (!doctor) return false;

    QPointF posDoctor = doctor->getPosicion();
    QPixmap spriteDoctor = doctor->getSpriteActual();
    QRectF areaDoctor(posDoctor.x(), posDoctor.y(), spriteDoctor.width(), spriteDoctor.height());

    return areaDoctor.contains(punto);
}

void NivelColera::cambiarProyectil()
{
    if (proyectilActual == proyectilPiedra) {
        proyectilActual = proyectilAmpolla;
    } else {
        proyectilActual = proyectilPiedra;
    }
}

Proyectil* NivelColera::obtenerProyectilActivo()
{
    return proyectilActual;
}

float NivelColera::calcularFuerza(float distancia)
{
    float distanciaMin = 20;
    float distanciaMax = 200;
    float fuerzaMin = 16.0f;
    float fuerzaMax = 50.0f;

    distancia = qMax(distanciaMin, qMin(distancia, distanciaMax));
    return ((distancia - distanciaMin) / (distanciaMax - distanciaMin)) * (fuerzaMax - fuerzaMin) + fuerzaMin;
}

QPointF NivelColera::calcularPosicionFisica(const QPointF& posInicial, const QVector2D& velInicial, float gravedad, float tiempo, bool conResistenciaAire)
{
    if (!conResistenciaAire) {
        // Física simple sin resistencia (para predicción más limpia)
        float x = posInicial.x() + velInicial.x() * tiempo;
        float y = posInicial.y() + velInicial.y() * tiempo + 0.5f * gravedad * tiempo * tiempo;
        return QPointF(x, y);
    } else {
        // Física con resistencia (igual que tu Proyectil::actualizarPosicion)
        const float deltaTime = 0.01f;
        const float resistenciaAire = 0.995f;

        QVector2D pos(posInicial.x(), posInicial.y());
        QVector2D vel = velInicial;
        int steps = tiempo / deltaTime;

        for (int i = 0; i < steps; i++) {
            vel.setY(vel.y() + gravedad);
            vel *= resistenciaAire;
            pos += vel * deltaTime * 60.0f;
        }

        return QPointF(pos.x(), pos.y());
    }
}

void NivelColera::reproducirSonidoBaldeAmpolla() {
    sonidoBaldeAmpolla.play();
}

void NivelColera::reproducirSonidoDestruccionBalde() {
    sonidoDestruccionBalde.play();
}

void NivelColera::reproducirSonidoDestruccionMadera() {
    sonidoDestruccionMadera.play();
}

void NivelColera::reproducirSonidoReboteMadera() {
    sonidoReboteMadera.play();
}
