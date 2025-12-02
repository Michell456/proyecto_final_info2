#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>

#include "MenuPrincipal.h"
#include "MenuSeleccionNivel.h"
#include "MenuPausa.h"
#include "Nivel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    Ui::MainWindow *ui;

    MenuPrincipal *menuPrincipal;
    MenuSeleccionNivel *menuSeleccionNivel;
    MenuPausa *menuPausa;

    nivel *nivelActual;

    QTimer *timer;

    void cargarMenuPrincipal();
    void cargarNivel1();
    void cargarNivel2();
    void cargarNivel3();
    void cargarMenuPausa();
    void reanudarJuego();
};

#endif // MAINWINDOW_H
