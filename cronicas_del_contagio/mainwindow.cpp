#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QDebug>

//#include "nivelPesteNegra.h"
#include "nivelcolera.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(1000, 600);
    cargarNivel2(); // Tu nivel de cólera

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this](){
        if (nivelActual) {
            nivelActual->update();
            update();
        }
    });

    timer->start(16);
}

void MainWindow::cargarMenuPrincipal() {
    if (nivelActual) {
        delete nivelActual;
        nivelActual = nullptr;
    }
    //nivelActual = new menuPrincipal();
}

void MainWindow::cargarNivel1() {
    if (nivelActual) {
        delete nivelActual;
        nivelActual = nullptr;
    }
    //nivelActual = new nivelPesteNegra();
    nivelActual = new NivelColera();  // TEMPORAL
}

void MainWindow::cargarNivel2() {
    if (nivelActual) {
        delete nivelActual;
        nivelActual = nullptr;
    }
    nivelActual = new NivelColera();
}

void MainWindow::cargarNivel3() {
    if (nivelActual) {
        delete nivelActual;
        nivelActual = nullptr;
    }
    //nivelActual = new nivelCovid();
}

void MainWindow::paintEvent(QPaintEvent *) {
    QPainter p(this);
    if (nivelActual) {
        nivelActual->draw(p);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    NivelColera* nivelColera = dynamic_cast<NivelColera*>(nivelActual);
    if (nivelColera) {
        nivelColera->handleMousePress(event);
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    NivelColera* nivelColera = dynamic_cast<NivelColera*>(nivelActual);
    if (nivelColera) {
        nivelColera->handleMouseMove(event);
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    NivelColera* nivelColera = dynamic_cast<NivelColera*>(nivelActual);
    if (nivelColera) {
        nivelColera->handleMouseRelease(event);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    NivelColera* nivelColera = dynamic_cast<NivelColera*>(nivelActual);
    if (nivelColera) {
        nivelColera->handleInput(event);
    }

    /*
    if (event->key() == Qt::Key_1) {
        cargarNivel1();
    } else if (event->key() == Qt::Key_2) {
        cargarNivel2();
    } else if (event->key() == Qt::Key_3) {
        cargarNivel3();
    }
    */
}

MainWindow::~MainWindow()
{
    if (nivelActual) {
        delete nivelActual;
        nivelActual = nullptr;
    }

    if (timer) {
        timer->stop();
    }

    delete ui;
}
