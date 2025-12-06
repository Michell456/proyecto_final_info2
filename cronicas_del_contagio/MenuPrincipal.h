#ifndef MENUPRINCIPAL_H
#define MENUPRINCIPAL_H

#include "Menu.h"
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

class MenuPrincipal : public Menu
{
    Q_OBJECT
public:
    explicit MenuPrincipal(QWidget *parent = nullptr);

protected:
    void setupUI() override;
    void conectarsenales() override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QPushButton *btnIniciar;
    QPushButton *btnSeleccionarNivel;
    QPushButton *btnSalir;
    QPixmap fondo;

    QVBoxLayout *verticalLayout;
    QHBoxLayout *buttonLayout1;
    QHBoxLayout *buttonLayout2;
    QHBoxLayout *buttonLayout3;

    QColor colorRojoPrincipal;
    QColor colorNegroPrincipal;
    QColor colorBordeBotones;
    QColor colorTextoBotones;

private slots:
    void onIniciarPartida();
    void onSeleccionarNivel();
    void onSalir();
};

#endif // MENUPRINCIPAL_H
