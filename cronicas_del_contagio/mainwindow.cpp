#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MenuPrincipal.h"
#include "MenuSeleccionNivel.h"
#include "MenuPausa.h"
#include <QPainter>
#include <QDebug>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , juego(nullptr)
    , widgetMenuPrincipal(nullptr)
    , widgetMenuSeleccion(nullptr)
    , widgetMenuPausa(nullptr)
{
    ui->setupUi(this);

    juego = new Juego(this);

    // Conectar señales
    connect(juego, &Juego::necesitaRedibujar,
            this, &MainWindow::onJuegoNecesitaRedibujar);
    connect(juego, &Juego::estadoCambiado,
            this, &MainWindow::onEstadoJuegoCambiado);
    connect(juego, &Juego::juegoFinalizado,
            this, &MainWindow::onJuegoFinalizado);

    connect(juego, &Juego::juegoFinalizado,
            this, &MainWindow::onJuegoFinalizado);

    widgetMenuPrincipal = dynamic_cast<QWidget*>(juego->getMenuPrincipal());
    widgetMenuSeleccion = dynamic_cast<QWidget*>(juego->getMenuSeleccionNivel());
    widgetMenuPausa = dynamic_cast<QWidget*>(juego->getMenuPausa());

    if (widgetMenuPrincipal) {
        widgetMenuPrincipal->setParent(this);
        widgetMenuPrincipal->setGeometry(0, 0, width(), height());
    }
    if (widgetMenuSeleccion) {
        widgetMenuSeleccion->setParent(this);
        widgetMenuSeleccion->setGeometry(0, 0, width(), height());
    }
    if (widgetMenuPausa) {
        widgetMenuPausa->setParent(this);
        widgetMenuPausa->setGeometry(0, 0, width(), height());
    }

    // Pantalla completa
    this->showFullScreen();

    juego->setTamanioVentana(size());

    mostrarMenuSegunEstado();
}

MainWindow::~MainWindow()
{
    delete juego;
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    painter.fillRect(rect(), Qt::black);

    if (nivel *nivelActual = juego->getNivelActual()) {
        nivelActual->draw(painter);
    }

}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    juego->setTamanioVentana(event->size());

    if (widgetMenuPrincipal) {
        widgetMenuPrincipal->setGeometry(0, 0, width(), height());
    }
    if (widgetMenuSeleccion) {
        widgetMenuSeleccion->setGeometry(0, 0, width(), height());
    }
    if (widgetMenuPausa) {
        widgetMenuPausa->setGeometry(0, 0, width(), height());
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    juego->manejarKeyPress(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    juego->manejarKeyRelease(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    juego->manejarMousePress(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    juego->manejarMouseMove(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    juego->manejarMouseRelease(event);
}

void MainWindow::onJuegoNecesitaRedibujar()
{
    update();
}

void MainWindow::onEstadoJuegoCambiado(Juego::EstadoJuego nuevoEstado)
{
    mostrarMenuSegunEstado();
    update();
}

void MainWindow::onJuegoFinalizado(Juego::ResultadoJuego resultado)
{
    qDebug() << "Resultado del juego:" << static_cast<int>(resultado);

    switch (resultado) {
    case Juego::ResultadoJuego::Victoria:
        qDebug() << "¡VICTORIA!";
        mostrarPantallaVictoria();
        QTimer::singleShot(3000, this, [this]() {
            juego->cargarMenuPrincipal();
        });
        break;

    case Juego::ResultadoJuego::Derrota:
        qDebug() << "Derrota...";
        mostrarPantallaDerrota();
        QTimer::singleShot(3000, this, [this]() {
            juego->cargarMenuPrincipal();
        });
        break;

    case Juego::ResultadoJuego::SalirSolicitado:
        qDebug() << "Saliendo de la aplicación...";
        QTimer::singleShot(100, this, &MainWindow::close);
        break;
    }
}

void MainWindow::mostrarMenuSegunEstado()
{
    // Ocultar los menus
    if (widgetMenuPrincipal) widgetMenuPrincipal->hide();
    if (widgetMenuSeleccion) widgetMenuSeleccion->hide();
    if (widgetMenuPausa) widgetMenuPausa->hide();

    switch (juego->getEstadoActual()) {
    case Juego::EstadoJuego::MenuPrincipal:
        if (widgetMenuPrincipal) widgetMenuPrincipal->show();
        break;

    case Juego::EstadoJuego::SeleccionNivel:
        if (widgetMenuSeleccion) widgetMenuSeleccion->show();
        break;

    case Juego::EstadoJuego::Pausa:
        if (widgetMenuPausa) widgetMenuPausa->show();
        break;

    case Juego::EstadoJuego::Jugando:
        // No mostrar menú
        break;

    case Juego::EstadoJuego::Terminado:
        // Pantalla fin del juego
        break;
    }
}

void MainWindow::mostrarPantallaVictoria()
{
    // Implementar pantalla de victoria
    qDebug() << "Mostrando pantalla de victoria";
}

void MainWindow::mostrarPantallaDerrota()
{
    // Implementar pantalla de derrota
    qDebug() << "Mostrando pantalla de derrota";
}
