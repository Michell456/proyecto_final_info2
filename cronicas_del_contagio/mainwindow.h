#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QKeyEvent>
#include <QObject>
#include <QPainter>
#include <QMouseEvent>

#include "nivel.h"

class NivelColera;

class nivel;
//class menuPrincipal;
//class NivelPesteNegra;
//class NivelColera;
class NivelCovid;

namespace Ui {class MainWindow;}

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

    // AÑADIR métodos de mouse (NivelColera)
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::MainWindow *ui;

    nivel *nivelActual = nullptr;
    QTimer *timer;

    void cargarMenuPrincipal();
    void cargarNivel1();
    void cargarNivel2();
    void cargarNivel3();

};
#endif // MAINWINDOW_H
