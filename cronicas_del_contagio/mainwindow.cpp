#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "nivel.h"
#include "nivelcovid.h"
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , nivelActual(nullptr)
{
    ui->setupUi(this);

    // Para probar, cargar directamente el nivel 3
    cargarNivel3();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this](){
        if (nivelActual) {
            nivelActual->update();
            update();   // vuelve a dibujar
        }
    });

    timer->start(16); // 60 FPS
}

void MainWindow::cargarNivel3() {
    if (nivelActual) {
        delete nivelActual;
        nivelActual = nullptr;
    }
    nivelActual = new NivelCovid();
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter p(this);
    if (nivelActual) {
        nivelActual->draw(p);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (nivelActual) {
        nivelActual->handleInput(event);
    }
    update(); // Forzar redibujado cuando se presiona tecla
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if (nivelActual) {
        nivelActual->handleKeyRelease(event);
    }
}

void MainWindow::actualizarAnimacion() {
    // Puedes implementar esto si necesitas animaciones específicas
}

void MainWindow::actualizarMovimiento() {
    // Puedes implementar esto si necesitas movimiento específico
}

MainWindow::~MainWindow()
{
    delete nivelActual;
    delete ui;
}
