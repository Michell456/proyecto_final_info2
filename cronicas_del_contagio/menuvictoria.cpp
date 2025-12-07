#include "MenuVictoria.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QDebug>

MenuVictoria::MenuVictoria(QWidget *parent)
    : Menu(parent), LabelTitulo(nullptr), btnMenuPrincipal(nullptr), btnSalir(nullptr)
{
    setupUI();
    conectarsenales();
}

void MenuVictoria::setupUI()
{
    LabelTitulo = new QLabel(this);
    QPixmap tituloImg(":/sprites/Menu_victoria/Victoria.png");
    if (!tituloImg.isNull()) {
        LabelTitulo->setPixmap(tituloImg);
        LabelTitulo->setScaledContents(true);
        LabelTitulo->setFixedSize(400, 150);
    } else {
        LabelTitulo->setText("¡VICTORIA!");
        LabelTitulo->setStyleSheet("font-size: 36px; font-weight: bold; color: #33ff33;");
    }
    LabelTitulo->setAlignment(Qt::AlignCenter);

    btnMenuPrincipal = crearBoton("MENÚ PRINCIPAL");
    btnSalir = crearBoton("SALIR");

    btnMenuPrincipal->setFixedSize(300, 50);
    btnSalir->setFixedSize(300, 50);

    QFont pixelFont("Monospace", 18, QFont::Bold);
    pixelFont.setStyleHint(QFont::TypeWriter);

    btnMenuPrincipal->setFont(pixelFont);
    btnSalir->setFont(pixelFont);

    // Layout
    QHBoxLayout *hLayout1 = new QHBoxLayout();
    hLayout1->addStretch(); hLayout1->addWidget(btnMenuPrincipal); hLayout1->addStretch();

    QHBoxLayout *hLayout2 = new QHBoxLayout();
    hLayout2->addStretch(); hLayout2->addWidget(btnSalir); hLayout2->addStretch();

    QHBoxLayout *LayoutTitulo = new QHBoxLayout();
    LayoutTitulo->addStretch(); LayoutTitulo->addWidget(LabelTitulo); LayoutTitulo->addStretch();

    QVBoxLayout *LayoutVPrincipal = new QVBoxLayout();
    LayoutVPrincipal->addStretch();
    LayoutVPrincipal->addLayout(LayoutTitulo);
    LayoutVPrincipal->addSpacing(40);
    LayoutVPrincipal->addLayout(hLayout1);
    LayoutVPrincipal->addSpacing(20);
    LayoutVPrincipal->addLayout(hLayout2);
    LayoutVPrincipal->addStretch();

    if (LayoutPrincipal) {
        QLayoutItem* item;
        while ((item = LayoutPrincipal->takeAt(0)) != nullptr) {
            if (item->widget()) item->widget()->setParent(nullptr);
            delete item;
        }
        delete LayoutPrincipal;
    }

    LayoutPrincipal = LayoutVPrincipal;
    setLayout(LayoutPrincipal);

}

void MenuVictoria::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0, 0, 0, 180)); // fondo semitransparente
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
