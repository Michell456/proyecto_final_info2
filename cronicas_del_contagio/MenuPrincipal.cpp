#include "MenuPrincipal.h"
#include <QFont>
#include <QPainter>
#include <QPixmap>
#include <QLabel>
#include <QSpacerItem>
#include <QResizeEvent>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QDebug>

MenuPrincipal::MenuPrincipal(QWidget *parent)
    : Menu(parent),
    btnIniciar(nullptr),
    btnSeleccionarNivel(nullptr),
    btnSalir(nullptr),
    verticalLayout(nullptr),
    buttonLayout1(nullptr),
    buttonLayout2(nullptr),
    buttonLayout3(nullptr)
{

    colorRojoPrincipal = QColor(180, 40, 40);
    colorNegroPrincipal = QColor(20, 20, 20);
    colorBordeBotones = QColor(255, 50, 50, 200);
    colorTextoBotones = QColor(255, 220, 220);

    fondo.load(":/sprites/Menu_principal/fondo_menu_principal.jpg");

    if (fondo.isNull()) {
        fondo = QPixmap(800, 600);
        fondo.fill(Qt::black);
        QPainter tempPainter(&fondo);
        QLinearGradient gradient(0, 0, 800, 600);
        gradient.setColorAt(0, QColor(50, 0, 0));
        gradient.setColorAt(1, QColor(20, 0, 0));
        tempPainter.fillRect(0, 0, 800, 600, gradient);
    }

    setupUI();
    conectarsenales();

}

void MenuPrincipal::setupUI()
{
    btnIniciar = crearBoton("INICIAR");
    if (!btnIniciar) {
        btnIniciar = new QPushButton("INICIAR", this);
    }

    btnSeleccionarNivel = crearBoton("NIVELES");
    if (!btnSeleccionarNivel) {
        btnSeleccionarNivel = new QPushButton("NIVELES", this);
    }

    btnSalir = crearBoton("SALIR");
    if (!btnSalir) {
        btnSalir = new QPushButton("SALIR", this);
    }

    btnIniciar->setFixedSize(250, 45);
    btnSeleccionarNivel->setFixedSize(250, 45);
    btnSalir->setFixedSize(250, 45);

    btnIniciar->setFocusPolicy(Qt::NoFocus);
    btnSeleccionarNivel->setFocusPolicy(Qt::NoFocus);
    btnSalir->setFocusPolicy(Qt::NoFocus);

    QFont pixelFont("Monospace", 16, QFont::Bold);
    pixelFont.setStyleHint(QFont::TypeWriter);

    btnIniciar->setFont(pixelFont);
    btnSeleccionarNivel->setFont(pixelFont);
    btnSalir->setFont(pixelFont);

    QString buttonStyle = QString(
                              "QPushButton {"
                              "   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,"
                              "                                     stop:0 rgba(%1, %2, %3, 180),"
                              "                                     stop:1 rgba(40, 0, 0, 180));"
                              "   color: rgb(%4, %5, %6);"
                              "   border: 2px solid rgba(%7, %8, %9, 220);"
                              "   border-radius: 5px;"
                              "   padding: 5px;"
                              "   text-align: center;"
                              "   font-weight: bold;"
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
                              "}"
                              ).arg(colorRojoPrincipal.red())
                              .arg(colorRojoPrincipal.green())
                              .arg(colorRojoPrincipal.blue())
                              .arg(colorTextoBotones.red())
                              .arg(colorTextoBotones.green())
                              .arg(colorTextoBotones.blue())
                              .arg(colorBordeBotones.red())
                              .arg(colorBordeBotones.green())
                              .arg(colorBordeBotones.blue());

    btnIniciar->setStyleSheet(buttonStyle);
    btnSeleccionarNivel->setStyleSheet(buttonStyle);
    btnSalir->setStyleSheet(buttonStyle);

    buttonLayout1 = new QHBoxLayout();
    buttonLayout1->addStretch();
    buttonLayout1->addWidget(btnIniciar);
    buttonLayout1->addStretch();

    buttonLayout2 = new QHBoxLayout();
    buttonLayout2->addStretch();
    buttonLayout2->addWidget(btnSeleccionarNivel);
    buttonLayout2->addStretch();

    buttonLayout3 = new QHBoxLayout();
    buttonLayout3->addStretch();
    buttonLayout3->addWidget(btnSalir);
    buttonLayout3->addStretch();

    verticalLayout = new QVBoxLayout(this);
    verticalLayout->setSpacing(0);
    verticalLayout->setContentsMargins(0, 0, 0, 0);

    verticalLayout->addStretch();
    verticalLayout->addStretch();
    verticalLayout->addStretch();
    verticalLayout->addStretch();

    verticalLayout->addLayout(buttonLayout1);
    verticalLayout->addSpacing(10);

    verticalLayout->addLayout(buttonLayout2);
    verticalLayout->addSpacing(10);

    verticalLayout->addLayout(buttonLayout3);

    verticalLayout->addStretch();

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

    LayoutPrincipal = verticalLayout;
    setLayout(LayoutPrincipal);

    setMinimumSize(800, 600);

}

void MenuPrincipal::conectarsenales()
{

    connect(btnIniciar, &QPushButton::clicked,
            this, &MenuPrincipal::onIniciarPartida);

    connect(btnSeleccionarNivel, &QPushButton::clicked,
            this, &MenuPrincipal::onSeleccionarNivel);

    connect(btnSalir, &QPushButton::clicked,
            this, &MenuPrincipal::onSalir);

}

void MenuPrincipal::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    if (!fondo.isNull()) {
        QPixmap scaledFondo = fondo.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(0, 0, scaledFondo);
    }

    painter.setBrush(QColor(0, 0, 0, 50));
    painter.setPen(Qt::NoPen);
    painter.drawRect(0, 0, width(), height());
}

void MenuPrincipal::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    const int MIN_WIDTH = 200;
    const int MAX_WIDTH = 300;
    const int MIN_HEIGHT = 40;
    const int MAX_HEIGHT = 50;

    int windowWidth = width();
    int newWidth = windowWidth * 0.25;

    if (newWidth < MIN_WIDTH) newWidth = MIN_WIDTH;
    if (newWidth > MAX_WIDTH) newWidth = MAX_WIDTH;

    int newHeight = newWidth * 0.18;
    if (newHeight < MIN_HEIGHT) newHeight = MIN_HEIGHT;
    if (newHeight > MAX_HEIGHT) newHeight = MAX_HEIGHT;

    if (btnIniciar) btnIniciar->setFixedSize(newWidth, newHeight);
    if (btnSeleccionarNivel) btnSeleccionarNivel->setFixedSize(newWidth, newHeight);
    if (btnSalir) btnSalir->setFixedSize(newWidth, newHeight);

    int fontSize = newWidth / 15;
    if (fontSize < 14) fontSize = 14;
    if (fontSize > 20) fontSize = 20;

    QFont pixelFont("Monospace", fontSize, QFont::Bold);
    pixelFont.setStyleHint(QFont::TypeWriter);

    if (btnIniciar) btnIniciar->setFont(pixelFont);
    if (btnSeleccionarNivel) btnSeleccionarNivel->setFont(pixelFont);
    if (btnSalir) btnSalir->setFont(pixelFont);

    update();
}

void MenuPrincipal::onIniciarPartida()
{
    emit NivelSeleccionado(1);
}

void MenuPrincipal::onSeleccionarNivel()
{
    emit JuegoIniciado();
}

void MenuPrincipal::onSalir()
{
    emit SalirDelJuego();
}
