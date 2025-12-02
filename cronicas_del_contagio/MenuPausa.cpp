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
    titleLabel(nullptr),
    btnReanudar(nullptr),
    btnMenuPrincipal(nullptr),
    btnSalir(nullptr)
{
    setupUI();
    connectSignals();
}

void MenuPausa::setupUI()
{
    // titulo
    titleLabel = new QLabel(this);

    // cargar imagen
    QPixmap tituloImg(":/sprites/Menu_pausa/Juego_pausado.png");
    if (!tituloImg.isNull()) {
        titleLabel->setPixmap(tituloImg);
        titleLabel->setScaledContents(true);
        titleLabel->setFixedSize(400, 150); // ajustar imagen
    } else {
        // Si no carga la imagen, usar texto
        titleLabel->setText("JUEGO PAUSADO");
        titleLabel->setStyleSheet("font-size: 36px; font-weight: bold; color: #ff3333;");
    }

    titleLabel->setAlignment(Qt::AlignCenter);

    // Botones
    btnReanudar = createButton("REANUDAR");
    btnMenuPrincipal = createButton("MENÚ PRINCIPAL");
    btnSalir = createButton("SALIR");

    // Tamaño de botones
    btnReanudar->setFixedSize(300, 50);
    btnMenuPrincipal->setFixedSize(300, 50);
    btnSalir->setFixedSize(300, 50);

    // Evita que se seleccionen con TAB
    btnReanudar->setFocusPolicy(Qt::NoFocus);
    btnMenuPrincipal->setFocusPolicy(Qt::NoFocus);
    btnSalir->setFocusPolicy(Qt::NoFocus);

    //Fuente
    QFont pixelFont("Monospace", 18, QFont::Bold);
    pixelFont.setStyleHint(QFont::TypeWriter);

    btnReanudar->setFont(pixelFont);
    btnMenuPrincipal->setFont(pixelFont);
    btnSalir->setFont(pixelFont);

    // Estilo Botones
    QString buttonStyle =
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

    btnReanudar->setStyleSheet(buttonStyle);
    btnMenuPrincipal->setStyleSheet(buttonStyle);
    btnSalir->setStyleSheet(buttonStyle);


    // Crear layouts horizontales para centrar
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

    // Layout para titulo
    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->addStretch();
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();

    // Layout principal
    QVBoxLayout *mainVLayout = new QVBoxLayout();
    mainVLayout->addStretch();
    mainVLayout->addLayout(titleLayout);
    mainVLayout->addSpacing(40);
    mainVLayout->addLayout(hLayout1);
    mainVLayout->addSpacing(20);
    mainVLayout->addLayout(hLayout2);
    mainVLayout->addSpacing(20);
    mainVLayout->addLayout(hLayout3);
    mainVLayout->addStretch();

    // Limpiar layout anterior si existe
    if (mainLayout) {
        // Limpiar widgets del layout anterior
        QLayoutItem* item;
        while ((item = mainLayout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                item->widget()->setParent(nullptr);
            }
            delete item;
        }
        delete mainLayout;
    }

    mainLayout = mainVLayout;
    setLayout(mainLayout);
}

void MenuPausa::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    // fondo oscuro
    painter.fillRect(rect(), QColor(0, 0, 0, 180));

    // Efecto viñeta (para oscurecer bordes)
    QRadialGradient vignette(width()/2, height()/2, qMax(width(), height())/1.5);
    vignette.setColorAt(0, QColor(0, 0, 0, 100));
    vignette.setColorAt(1, QColor(0, 0, 0, 200));

    painter.setBrush(vignette);
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());

    // resplandor en el centro rojo
    QRadialGradient glow(width()/2, height()/2, 200);
    glow.setColorAt(0, QColor(255, 50, 50, 30));
    glow.setColorAt(1, QColor(255, 0, 0, 0));

    painter.setBrush(glow);
    painter.drawRect(rect());
}

void MenuPausa::connectSignals()
{
    qDebug() << "MenuPausa: Conectando señales";

    connect(btnReanudar, &QPushButton::clicked, this, &MenuPausa::onReanudar);
    connect(btnMenuPrincipal, &QPushButton::clicked, this, &MenuPausa::onMenuPrincipal);
    connect(btnSalir, &QPushButton::clicked, this, &MenuPausa::onSalir);

    qDebug() << "MenuPausa: Señales conectadas";
}

void MenuPausa::onReanudar()
{
    qDebug() << "MenuPausa: Botón REANUDAR clickeado";
    emit gameResumed();
}

void MenuPausa::onMenuPrincipal()
{
    qDebug() << "MenuPausa: Botón MENÚ PRINCIPAL clickeado";
    emit backToMainMenu();
}

void MenuPausa::onSalir()
{
    qDebug() << "MenuPausa: Botón SALIR clickeado";
    emit gameExited();
}
