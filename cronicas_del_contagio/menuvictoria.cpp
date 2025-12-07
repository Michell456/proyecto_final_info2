#include "MenuVictoria.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QDebug>
#include <QFont>
#include <QRadialGradient>
#include <QApplication>
#include <QScreen>

MenuVictoria::MenuVictoria(QWidget *parent)
    : Menu(parent), LabelTitulo(nullptr), btnMenuPrincipal(nullptr), btnSalir(nullptr)
{
    setupUI();
    conectarsenales();
}

void MenuVictoria::setupUI()
{
    LabelTitulo = new QLabel(this);
    LabelTitulo->setText("¡VICTORIA!");

    LabelTitulo->setStyleSheet(
        "QLabel {"
        "   font-size: 48px;"
        "   font-weight: bold;"
        "   color: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "                          stop:0 #33ff33,"
        "                          stop:0.5 #66ff66,"
        "                          stop:1 #33ff33);"
        "   text-shadow: 3px 3px 6px rgba(0, 0, 0, 0.8),"
        "                0 0 20px rgba(51, 255, 51, 0.5);"
        "   padding: 10px;"
        "   border: 2px solid rgba(51, 255, 51, 100);"
        "   border-radius: 10px;"
        "   background-color: rgba(0, 30, 0, 100);"
        "}"
        );

    LabelTitulo->setAlignment(Qt::AlignCenter);
    LabelTitulo->setFixedSize(500, 120);

    btnMenuPrincipal = crearBoton("MENÚ PRINCIPAL");
    btnSalir = crearBoton("SALIR");

    btnMenuPrincipal->setFixedSize(300, 50);
    btnSalir->setFixedSize(300, 50);

    // Configurar focus
    btnMenuPrincipal->setFocusPolicy(Qt::NoFocus);
    btnSalir->setFocusPolicy(Qt::NoFocus);

    QFont pixelFont("Monospace", 18, QFont::Bold);
    pixelFont.setStyleHint(QFont::TypeWriter);

    btnMenuPrincipal->setFont(pixelFont);
    btnSalir->setFont(pixelFont);

    QString estiloBoton =
        "QPushButton {"
        "   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,"
        "                                     stop:0 rgba(180, 40, 40, 180),"
        "                                     stop:1 rgba(40, 0, 0, 180));"
        "   color: rgb(255, 220, 220);"
        "   border: 2px solid rgba(255, 50, 50, 220);"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "   text-align: center;"
        "   font-weight: bold;"
        "   text-shadow: 1px 1px 2px rgba(0, 0, 0, 0.7);"
        "}"
        "QPushButton:hover {"
        "   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,"
        "                                     stop:0 rgba(220, 60, 60, 200),"
        "                                     stop:1 rgba(80, 0, 0, 200));"
        "   border: 2px solid rgba(255, 100, 100, 255);"
        "   color: rgb(255, 240, 240);"
        "}"
        "QPushButton:pressed {"
        "   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,"
        "                                     stop:0 rgba(160, 30, 30, 200),"
        "                                     stop:1 rgba(30, 0, 0, 200));"
        "   border: 2px solid rgba(200, 50, 50, 255);"
        "}";

    btnMenuPrincipal->setStyleSheet(estiloBoton);
    btnSalir->setStyleSheet(estiloBoton);

    QHBoxLayout *LayoutTitulo = new QHBoxLayout();
    LayoutTitulo->addStretch();
    LayoutTitulo->addWidget(LabelTitulo);
    LayoutTitulo->addStretch();

    QHBoxLayout *hLayout1 = new QHBoxLayout();
    hLayout1->addStretch();
    hLayout1->addWidget(btnMenuPrincipal);
    hLayout1->addStretch();

    QHBoxLayout *hLayout2 = new QHBoxLayout();
    hLayout2->addStretch();
    hLayout2->addWidget(btnSalir);
    hLayout2->addStretch();

    QVBoxLayout *LayoutVPrincipal = new QVBoxLayout();
    LayoutVPrincipal->addStretch();
    LayoutVPrincipal->addLayout(LayoutTitulo);
    LayoutVPrincipal->addSpacing(60);
    LayoutVPrincipal->addLayout(hLayout1);
    LayoutVPrincipal->addSpacing(30);
    LayoutVPrincipal->addLayout(hLayout2);
    LayoutVPrincipal->addStretch();

    if (LayoutPrincipal) {
        QLayoutItem* item;
        while ((item = LayoutPrincipal->takeAt(0)) != nullptr) {
            if (item->widget()) {
                item->widget()->setParent(nullptr);
            }
            delete item;
        }
        delete LayoutPrincipal;
    }

    LayoutPrincipal = LayoutVPrincipal;
    setLayout(LayoutPrincipal);

    if (this->parentWidget()) {
        setFixedSize(this->parentWidget()->size());
    }
}

void MenuVictoria::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    painter.fillRect(rect(), QColor(0, 0, 0, 180));

    QRadialGradient vignette(width()/2, height()/2, qMax(width(), height())/1.5);
    vignette.setColorAt(0, QColor(0, 0, 0, 100));
    vignette.setColorAt(1, QColor(0, 0, 0, 200));

    painter.setBrush(vignette);
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());

    QRadialGradient glow(width()/2, height()/2, 300);
    glow.setColorAt(0, QColor(51, 255, 51, 40));
    glow.setColorAt(0.5, QColor(0, 150, 0, 20));
    glow.setColorAt(1, QColor(0, 50, 0, 0));

    painter.setBrush(glow);
    painter.drawRect(rect());

    painter.setPen(QPen(QColor(100, 255, 100, 30), 1));
    for (int i = 0; i < 20; ++i) {
        int x = rand() % width();
        int y = rand() % height();
        painter.drawPoint(x, y);
    }
}

void MenuVictoria::conectarsenales()
{
    connect(btnMenuPrincipal, &QPushButton::clicked, this, &MenuVictoria::onMenuPrincipal);
    connect(btnSalir, &QPushButton::clicked, this, &MenuVictoria::onSalir);
}

void MenuVictoria::onMenuPrincipal()
{
    emit VolverAlMenuPrincipal();
}

void MenuVictoria::onSalir()
{
    emit SalirDelJuego();
}
