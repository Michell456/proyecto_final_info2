#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "juego.h"
#include "menuvictoria.h"
#include "menuDerrota.h"

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
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void onJuegoNecesitaRedibujar();
    void onEstadoJuegoCambiado(Juego::EstadoJuego nuevoEstado);
    void onJuegoFinalizado(Juego::ResultadoJuego resultado);

private:
    void mostrarMenuSegunEstado();
    void mostrarPantallaVictoria();
    void mostrarPantallaDerrota();

    Ui::MainWindow *ui;
    Juego *juego;

    QWidget *widgetMenuPrincipal;
    QWidget *widgetMenuSeleccion;
    QWidget *widgetMenuPausa;

    MenuVictoria *widgetMenuVictoria;
    MenuDerrota  *widgetMenuDerrota;
};

#endif // MAINWINDOW_H
