#include "MenuPausa.h"
#include <QFont>
#include <QPainter>
#include <QPixmap>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

MenuPausa::MenuPausa(QWidget *parent)
    : Menu(parent),
    LabelTitulo(nullptr),
    btnReanudar(nullptr),
    btnMenuPrincipal(nullptr),
    btnSalir(nullptr)
{
    setupUI();
    conectarsenales();
}

void MenuPausa::setupUI()
{
    LabelTitulo = new QLabel(this);

    QPixmap tituloImg(":/sprites/Menu_pausa/Juego_pausado.png");
    if (!tituloImg.isNull()) {
        LabelTitulo->setPixmap(tituloImg);
        LabelTitulo->setScaledContents(true);
        LabelTitulo->setFixedSize(400, 150);
    } else {

        LabelTitulo->setText("JUEGO PAUSADO");
        LabelTitulo->setStyleSheet("font-size: 36px; font-weight: bold; color: #ff3333;");
    }

    LabelTitulo->setAlignment(Qt::AlignCenter);

    btnReanudar = crearBoton("REANUDAR");
    btnMenuPrincipal = crearBoton("MENÚ PRINCIPAL");
    btnSalir = crearBoton("SALIR");

    btnReanudar->setFixedSize(300, 50);
    btnMenuPrincipal->setFixedSize(300, 50);
    btnSalir->setFixedSize(300, 50);

    btnReanudar->setFocusPolicy(Qt::NoFocus);
    btnMenuPrincipal->setFocusPolicy(Qt::NoFocus);
    btnSalir->setFocusPolicy(Qt::NoFocus);

    QFont pixelFont("Monospace", 18, QFont::Bold);
    pixelFont.setStyleHint(QFont::TypeWriter);

    btnReanudar->setFont(pixelFont);
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

    btnReanudar->setStyleSheet(estiloBoton);
    btnMenuPrincipal->setStyleSheet(estiloBoton);
    btnSalir->setStyleSheet(estiloBoton);

    QHBoxLayout *hLayout1 = new QHBoxLayout();
    hLayout1->addStretch();
    hLayout1->addWidget(btnReanudar);
    hLayout1->addStretch();

    QHBoxLayout *hLayout2 = new QHBoxLayout();
    hLayout2->addStretch();
    hLayout2->addWidget(btnMenuPrincipal);
    hLayout2->addStretch();

    QHBoxLayout *hLayout3 = new QHBoxLayout();
    hLayout3->addStretch();
    hLayout3->addWidget(btnSalir);
    hLayout3->addStretch();

    QHBoxLayout *LayoutTitulo = new QHBoxLayout();
    LayoutTitulo->addStretch();
    LayoutTitulo->addWidget(LabelTitulo);
    LayoutTitulo->addStretch();

    QVBoxLayout *LayoutVPrincipal = new QVBoxLayout();
    LayoutVPrincipal->addStretch();
    LayoutVPrincipal->addLayout(LayoutTitulo);
    LayoutVPrincipal->addSpacing(40);
    LayoutVPrincipal->addLayout(hLayout1);
    LayoutVPrincipal->addSpacing(20);
    LayoutVPrincipal->addLayout(hLayout2);
    LayoutVPrincipal->addSpacing(20);
    LayoutVPrincipal->addLayout(hLayout3);
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
}

void MenuPausa::paintEvent(QPaintEvent *event)
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

    QRadialGradient glow(width()/2, height()/2, 200);
    glow.setColorAt(0, QColor(255, 50, 50, 30));
    glow.setColorAt(1, QColor(255, 0, 0, 0));

    painter.setBrush(glow);
    painter.drawRect(rect());
}

void MenuPausa::conectarsenales()
{
    connect(btnReanudar, &QPushButton::clicked, this, &MenuPausa::onReanudar);
    connect(btnMenuPrincipal, &QPushButton::clicked, this, &MenuPausa::onMenuPrincipal);
    connect(btnSalir, &QPushButton::clicked, this, &MenuPausa::onSalir);
}

void MenuPausa::onReanudar()
{
    emit JuegoDespausado();
}

void MenuPausa::onMenuPrincipal()
{
    emit VolverAlMenuPrincipal();
}

void MenuPausa::onSalir()
{
    qDebug() << "MenuPausa: Botón SALIR clickeado";
    emit SalirDelJuego();
}
