#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "nivelPesteNegra.h"
#include "nivelcolera.h"
#include <QDebug>

#include "nivelcolera.h"
#include "nivelPesteNegra.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , nivelActual(nullptr)
{
    ui->setupUi(this);

    this->showFullScreen();

    // Crear menus
    menuPrincipal = new MenuPrincipal(this);
    menuSeleccionNivel = new MenuSeleccionNivel(this);
    menuPausa = new MenuPausa(this);

    // Ajustar a pantalla completa dinamica
    menuPrincipal->setGeometry(0, 0, width(), height());
    menuSeleccionNivel->setGeometry(0, 0, width(), height());
    menuPausa->setGeometry(0, 0, width(), height());

    menuSeleccionNivel->hide();
    menuPausa->hide();

    // Conexiones menu principal
    connect(menuPrincipal, &MenuPrincipal::levelSelected, this, [this](int nivel) {
        qDebug() << "MainWindow: Señal levelSelected recibida para nivel" << nivel;
        menuPrincipal->hide();

        if(nivel == 1) cargarNivel1();
        else if(nivel == 2) cargarNivel2();
        else if(nivel == 3) cargarNivel3();
        else {
            qDebug() << "Nivel no reconocido:" << nivel << "- Cargando nivel 1 por defecto";
            cargarNivel1();
        }
    });

    connect(menuPrincipal, &MenuPrincipal::gameStarted, this, [this]() {
        qDebug() << "MainWindow: Mostrando selección de nivel";
        menuPrincipal->hide();
        menuSeleccionNivel->show();
    });

    connect(menuPrincipal, &MenuPrincipal::gameExited, this, &MainWindow::close);

    // conexiones del menu seleccion nivel
    connect(menuSeleccionNivel, &MenuSeleccionNivel::nivelSeleccionado,
            this, [this](int nivel){
                qDebug() << "MainWindow: Nivel seleccionado desde menuSeleccionNivel:" << nivel;
                menuSeleccionNivel->hide();
                if(nivel == 1) cargarNivel1();
                else if(nivel == 2) cargarNivel2();
                else if(nivel == 3) cargarNivel3();
            });

    // conexiones menu pausa
    connect(menuPausa, &MenuPausa::gameResumed, this, &MainWindow::reanudarJuego);
    connect(menuPausa, &MenuPausa::backToMainMenu, this, &MainWindow::cargarMenuPrincipal);
    connect(menuPausa, &MenuPausa::gameExited, this, &MainWindow::close);

    // Timer del juego
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        if(nivelActual) {
            nivelActual->update();
            if(nivelActual->chequearVictoria() || nivelActual->chequearDerrota()){
                timer->stop();
                cargarMenuPrincipal();
            }
            update();
        }
    });

    qDebug() << "MainWindow: Inicialización completada";
    cargarMenuPrincipal();
}

void MainWindow::cargarMenuPrincipal()
{
    if(nivelActual){
        delete nivelActual;
        nivelActual = nullptr;
    }

    if(timer->isActive())
        timer->stop();

    menuPrincipal->show();
    menuSeleccionNivel->hide();
    menuPausa->hide();
}

void MainWindow::cargarNivel1()
{
    if(nivelActual){ delete nivelActual; nivelActual = nullptr; }
    nivelActual = new nivelPesteNegra(this);

    menuPrincipal->hide();
    menuSeleccionNivel->hide();
    menuPausa->hide();

    timer->start(16);
}

void MainWindow::cargarNivel2()
{
    if(nivelActual){ delete nivelActual; nivelActual = nullptr; }
    nivelActual = new NivelColera(this);

    menuPrincipal->hide();
    menuSeleccionNivel->hide();
    menuPausa->hide();

    timer->start(16);
}

void MainWindow::cargarNivel3()
{
    if(nivelActual){ delete nivelActual; nivelActual = nullptr; }
    // Usa temporalmente peste negra
    nivelActual = new nivelPesteNegra(this);

    menuPrincipal->hide();
    menuSeleccionNivel->hide();
    menuPausa->hide();

    timer->start(16);
}

void MainWindow::cargarMenuPausa()
{
    if(timer->isActive())
        timer->stop();

    menuPausa->show();
}

void MainWindow::reanudarJuego()
{
    menuPausa->hide();
    if(nivelActual)
        timer->start(16);
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    if(nivelActual)
        nivelActual->draw(p);
    else
        p.fillRect(rect(), Qt::black);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape){
        if(nivelActual && !menuPausa->isVisible() && !menuPrincipal->isVisible())
            cargarMenuPausa();
        else if(menuPausa->isVisible())
            reanudarJuego();
        return;
    }

    if(nivelActual && !menuPausa->isVisible() && !menuPrincipal->isVisible())
        nivelActual->handleInput(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(nivelActual && !menuPausa->isVisible() && !menuPrincipal->isVisible())
        nivelActual->handleKeyRelease(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(nivelActual && !menuPausa->isVisible() && !menuPrincipal->isVisible()){
        if(auto n = dynamic_cast<NivelColera*>(nivelActual))
            n->handleMousePress(event);
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(nivelActual && !menuPausa->isVisible() && !menuPrincipal->isVisible()){
        if(auto n = dynamic_cast<NivelColera*>(nivelActual))
            n->handleMouseMove(event);
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if(nivelActual && !menuPausa->isVisible() && !menuPrincipal->isVisible()){
        if(auto n = dynamic_cast<NivelColera*>(nivelActual))
            n->handleMouseRelease(event);
    }
}

MainWindow::~MainWindow()
{
    if(timer && timer->isActive())
        timer->stop();

    delete nivelActual;
    delete menuPrincipal;
    delete menuSeleccionNivel;
    delete menuPausa;
    delete timer;
    delete ui;
}
