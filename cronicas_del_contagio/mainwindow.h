#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QKeyEvent>
#include <QObject>
#include <QPainter>

#include "nivel.h"

namespace Ui {class MainWindow;}

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    //void actualizarAnimacion();
    //void actualizarMovimiento();

protected:
    //void keyPressEvent(QKeyEvent *event) override;
    //void keyReleaseEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

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

