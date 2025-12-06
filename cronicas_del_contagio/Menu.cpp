#include "Menu.h"

Menu::Menu(QWidget *parent)
    : QWidget(parent), LayoutPrincipal(new QVBoxLayout(this))
{

    LayoutPrincipal->setAlignment(Qt::AlignCenter);
    LayoutPrincipal->setSpacing(20);
}

QPushButton* Menu::crearBoton(const QString &texto, const QString &estilo)
{
    QPushButton *boton = new QPushButton(texto);
    QString EstiloBoton = estilo.isEmpty() ?
                              "QPushButton { background-color: #3498db; color: white; border: none; "
                              "padding: 15px 30px; font-size: 18px; border-radius: 10px; }"
                              "QPushButton:hover { background-color: #2980b9; }"
                              "QPushButton:pressed { background-color: #21618c; }" : estilo;

    boton->setStyleSheet(EstiloBoton);
    boton->setFixedSize(250, 60);
    return boton;
}
