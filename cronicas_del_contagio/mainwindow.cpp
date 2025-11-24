#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QKeyEvent>
#include <QDebug>

#include "nivelPesteNegra.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(1000, 600);
    cargarNivel1();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this](){
        nivelActual->update();
        update();   // vuelve a dibujar
    });

    timer->start(16); // 60 FPS
}

void MainWindow::cargarMenuPrincipal() {
    delete nivelActual;
    //nivelActual = new menuPrincipal();
}

void MainWindow::cargarNivel1() {
    delete nivelActual;
    nivelActual = new nivelPesteNegra();
}

void MainWindow::cargarNivel2() {
    delete nivelActual;
    //nivelActual = new nivelColera();
}

void MainWindow::cargarNivel3() {
    delete nivelActual;
    //nivelActual = new nivelCovid();
}

void MainWindow::paintEvent(QPaintEvent *) {
    QPainter p(this);
    nivelActual->draw(p);
}
/*
void MainWindow::keyPressEvent(QKeyEvent *event) {
    nivelActual->handleInput(event);
}
*/
MainWindow::~MainWindow()
{
    delete ui;
}
