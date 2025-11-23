#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QKeyEvent>

namespace Ui {class MainWindow;}

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void actualizarAnimacion();
    void actualizarMovimiento();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *fig;

    Nivel *nivelActual;
    QTimer *timer;

    void cargarMenuPrincipal();
    void cargarNivel1();
    void cargarNivel2();
    void cargarNivel3();

    // Variables para animación
    QPixmap spriteSheet;
    int frameWidth;
    int frameHeight;
    int frameActual;
    int totalFrames;
    QTimer *timerAnimacion;
    QTimer *timerMovimiento;
    bool moviendoseDerecha;
    bool moviendoseIzquierda;
    bool moviendoseArriba;
    bool moviendoseAbajo;
    int velocidad;
    int filaAnimacion;
};
#endif // MAINWINDOW_H

