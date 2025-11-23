#include "nivel2.h"
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

Nivel2::Nivel2(QObject *parent)
    : nivel()
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

    // Crear doctor
    doctor = new Doctor(this);

    // Crear proyectiles
    proyectilPiedra = new Proyectil(configPiedra.sprite, configPiedra.gravedad,
                                    configPiedra.factorRebote, configPiedra.puedeDestruirObstaculos,
                                    configPiedra.puedeLlenarBaldes, configPiedra.puedeRebotar,
                                    configPiedra.maxColisiones);

    proyectilAmpolla = new Proyectil(configAmpolla.sprite, configAmpolla.gravedad,
                                     configAmpolla.factorRebote, configAmpolla.puedeDestruirObstaculos,
                                     configAmpolla.puedeLlenarBaldes, configAmpolla.puedeRebotar,
                                     configAmpolla.maxColisiones);

    proyectilActual = proyectilPiedra;

    // Crear objetos del nivel
    crearObstaculos();
    crearBaldes();

    // Configurar objetivos de los proyectiles
    proyectilPiedra->setObjetivos(&obstaculos, &baldes);
    proyectilAmpolla->setObjetivos(&obstaculos, &baldes);

    qDebug() << "Nivel 2 - Colera inicializado";
}

Nivel2::~Nivel2()
{
    delete doctor;
    delete proyectilPiedra;
    delete proyectilAmpolla;
    qDeleteAll(obstaculos);
    qDeleteAll(baldes);
}

void Nivel2::update()
{
    tiempoTranscurrido++;

    // Actualizar animación del doctor
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

void Nivel2::draw(QPainter &p)
{
    // 1. Dibujar fondo
    p.drawPixmap(0, 0, fondo);

    // 2. Dibujar obstáculos
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

    // 6. Dibujar líneas de trayectoria (si estamos arrastrando)
    if (arrastrando) {
        p.setPen(QPen(Qt::red, 3));
        for (const QLineF& linea : lineasTrayectoria) {
            p.drawLine(linea);
        }

        p.setBrush(Qt::yellow);
        p.setPen(Qt::NoPen);
        for (const QRectF& punto : puntosTrayectoria) {
            p.drawEllipse(punto);
        }
    }

    // 7. Dibujar UI
    p.setPen(Qt::white);
    p.setFont(QFont("Arial", 16));
    p.drawText(20, 30, QString("Nivel 2 - Cólera"));
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

void Nivel2::handleInput(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Space) {
        Proyectil* proyectil = obtenerProyectilActivo();
        if (proyectil && !proyectil->estaEnMovimiento() &&
            doctor && !doctor->estaLanzando()) {
            cambiarProyectil();
        }
    }
}

void Nivel2::handleKeyRelease(QKeyEvent *event)
{
    // No se necesita implementación por ahora
    Q_UNUSED(event)
}

bool Nivel2::chequearVictoria()
{
    // Victoria: todos los baldes están llenos
    for (Balde* balde : baldes) {
        if (!balde->estaLleno()) {
            return false;
        }
    }
    return true;
}

bool Nivel2::chequearDerrota()
{
    // Derrota: tiempo agotado (120 segundos)
    return (tiempoTranscurrido / 60) >= 120;
}

void Nivel2::handleMousePress(QMouseEvent *event)
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
            float proyectilX = posDoctor.x() + 50;
            float proyectilY = posDoctor.y() + 80;

            proyectilActual->setPosicion(QPointF(proyectilX, proyectilY));
            proyectilActual->setVisible(true);

        }
    }
}

void Nivel2::handleMouseMove(QMouseEvent *event)
{
    if (arrastrando && clickEnDoctor) {
        puntoActual = event->pos();

        // Dibujar línea de fuerza
        QPointF posProyectil = proyectilActual->getPosicion();
        QPixmap spriteProyectil = proyectilActual->getSprite();
        QPointF centroProyectil(posProyectil.x() + spriteProyectil.width()/2,
                                posProyectil.y() + spriteProyectil.height()/2);

        dibujarLineaFuerza(centroProyectil, puntoActual);

        // Calcular y dibujar trayectoria
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

void Nivel2::handleMouseRelease(QMouseEvent *event)
{
    if (arrastrando && clickEnDoctor) {
        QPointF scenePos = event->pos();

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
            proyectilActual->lanzar(velocidadInicial, posProyectil);
        });

        arrastrando = false;
        clickEnDoctor = false;
        limpiarLineas();
    }
}

void Nivel2::inicializarRecursos()
{
    // Cargar sprites desde recursos Qt
    fondo.load(":/sprites/Nivel2/fondoNivel2.png");
    spriteObstaculos.load(":/sprites/Nivel2/obsaculos.png");

    if (fondo.isNull()) {
        qDebug() << "ERROR: No se pudo cargar el fondo desde recursos";
        fondo = QPixmap(1000, 600);
        fondo.fill(QColor(100, 150, 255));
    }

    if (spriteObstaculos.isNull()) {
        qDebug() << "ERROR: No se pudo cargar sprites de obstaculos desde recursos";
    }
}

void Nivel2::inicializarConfiguraciones()
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
        configPiedra.sprite = generarPiedraSprite();
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
        configAmpolla.sprite = generarAmpollaSprite();
    }
}

void Nivel2::crearObstaculos()
{
    // Coordenadas de los obstáculos en el spritesheet
    struct CoordenadaObstaculo {
        int x, y, ancho, alto;
        QPointF posicionEscena;
    };

    QList<CoordenadaObstaculo> coordenadas = {
        {18, 13, 235, 37, QPointF(100, 100)},
        {19, 70, 320, 37, QPointF(150, 200)},
        {21, 127, 403, 38, QPointF(50, 300)},
        {21, 189, 154, 38, QPointF(400, 150)},
        {341, 196, 37, 230, QPointF(700, 100)},
        {268, 197, 38, 160, QPointF(600, 150)},
        {213, 198, 38, 69, QPointF(500, 250)},
        {14, 249, 107, 38, QPointF(300, 350)},
        {213, 292, 38, 37, QPointF(200, 400)},
        {11, 322, 62, 38, QPointF(350, 450)}
    };

    if (spriteObstaculos.isNull()) {
        generarObstaculosBasicos();
        return;
    }

    for (const auto& coord : coordenadas) {
        QPixmap spriteObstaculo = spriteObstaculos.copy(coord.x, coord.y, coord.ancho, coord.alto);

        if (!spriteObstaculo.isNull()) {
            Obstaculo *nuevoObstaculo = new Obstaculo(coord.posicionEscena, spriteObstaculo);
            obstaculos.append(nuevoObstaculo);
        }
    }

}

void Nivel2::crearBaldes()
{
    QList<QPointF> posicionesBaldes = {
        QPointF(800, 450),
        QPointF(200, 450),
        QPointF(500, 300)
    };

    for (int i = 0; i < posicionesBaldes.size(); ++i) {
        Balde *nuevoBalde = new Balde(posicionesBaldes[i]);
        baldes.append(nuevoBalde);
    }

}

void Nivel2::dibujarLineaFuerza(const QPointF &inicio, const QPointF &fin)
{
    limpiarLineas();
    lineasTrayectoria.append(QLineF(inicio, fin));
}

void Nivel2::dibujarTrayectoria(const QVector2D& velocidadInicial, const QPointF& posicionInicial)
{
    limpiarLineas();

    QVector<QPointF> puntos = calcularTrayectoria(velocidadInicial, posicionInicial);

    if (puntos.size() > 1) {
        // Dibujar líneas entre puntos
        for (int i = 0; i < puntos.size() - 1; i++) {
            lineasTrayectoria.append(QLineF(puntos[i], puntos[i+1]));
        }

        // Dibujar puntos de referencia
        for (int i = 0; i < puntos.size(); i += 3) {
            puntosTrayectoria.append(QRectF(puntos[i].x() - 2, puntos[i].y() - 2, 4, 4));
        }
    }
}

void Nivel2::limpiarLineas()
{
    lineasTrayectoria.clear();
    puntosTrayectoria.clear();
}

QVector<QPointF> Nivel2::calcularTrayectoria(const QVector2D& velocidadInicial, const QPointF& posicionInicial)
{
    QVector<QPointF> puntos;
    const float deltaTime = 0.05f;
    const int maxSteps = 50;

    QVector2D pos(posicionInicial.x(), posicionInicial.y());
    QVector2D vel = velocidadInicial;

    float gravedad = (proyectilActual == proyectilPiedra) ? 0.8f : 0.5f;
    float resistenciaAire = 0.995f;

    for (int i = 0; i < maxSteps; i++) {
        puntos.append(QPointF(pos.x(), pos.y()));

        // Aplicar gravedad
        vel.setY(vel.y() + gravedad);

        // Aplicar resistencia del aire
        vel *= resistenciaAire;

        // Actualizar posición
        pos += vel * deltaTime * 60.0f;

        // Condiciones de parada
        if (pos.x() > 1200 || pos.x() < -200 || pos.y() > 800 || vel.length() < 0.5f) {
            break;
        }
    }

    return puntos;
}

bool Nivel2::estaSobreDoctor(const QPointF& punto)
{
    if (!doctor) return false;

    QPointF posDoctor = doctor->getPosicion();
    QPixmap spriteDoctor = doctor->getSpriteActual();
    QRectF areaDoctor(posDoctor.x(), posDoctor.y(), spriteDoctor.width(), spriteDoctor.height());

    return areaDoctor.contains(punto);
}

void Nivel2::cambiarProyectil()
{
    if (proyectilActual == proyectilPiedra) {
        proyectilActual = proyectilAmpolla;
    } else {
        proyectilActual = proyectilPiedra;
    }
}

Proyectil* Nivel2::obtenerProyectilActivo()
{
    return proyectilActual;
}

float Nivel2::calcularFuerza(float distancia)
{
    float distanciaMin = 20;
    float distanciaMax = 200;
    float fuerzaMin = 8.0f;
    float fuerzaMax = 25.0f;

    distancia = qMax(distanciaMin, qMin(distancia, distanciaMax));
    return ((distancia - distanciaMin) / (distanciaMax - distanciaMin)) * (fuerzaMax - fuerzaMin) + fuerzaMin;
}

QPixmap Nivel2::generarPiedraSprite()
{
    QPixmap piedra(80, 80);
    piedra.fill(Qt::transparent);
    QPainter painter(&piedra);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::darkGray);
    painter.setPen(Qt::black);
    painter.drawEllipse(10, 10, 60, 60);
    return piedra;
}

QPixmap Nivel2::generarAmpollaSprite()
{
    QPixmap ampolla(40, 55);
    ampolla.fill(Qt::transparent);
    QPainter painter(&ampolla);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor(173, 216, 230, 180));
    painter.setPen(Qt::blue);
    painter.drawRect(10, 5, 20, 40);
    painter.drawText(12, 25, "Amp");
    return ampolla;
}

void Nivel2::generarObstaculosBasicos()
{
    QList<QRectF> formas = {
        QRectF(100, 100, 235, 37),
        QRectF(150, 200, 320, 37),
        QRectF(50, 300, 403, 38),
        QRectF(400, 150, 154, 38),
        QRectF(700, 100, 37, 230),
        QRectF(600, 150, 38, 160),
        QRectF(500, 250, 38, 69),
        QRectF(300, 350, 107, 38),
        QRectF(200, 400, 38, 37),
        QRectF(350, 450, 62, 38)
    };

    for (int i = 0; i < formas.size(); ++i) {
        QPixmap sprite(formas[i].width(), formas[i].height());
        sprite.fill(QColor(139, 69, 19));

        QPainter painter(&sprite);
        painter.setPen(Qt::black);
        painter.drawText(10, 20, QString("Obs %1").arg(i + 1));

        Obstaculo *obstaculo = new Obstaculo(formas[i].topLeft(), sprite);
        obstaculos.append(obstaculo);
    }

}
