#include "MenuSeleccionNivel.h"
#include <QPixmap>
#include <QEvent>
#include <QMouseEvent>

MenuSeleccionNivel::MenuSeleccionNivel(QWidget *parent)
    : Menu(parent)
{
    setupUI();
    conectarsenales();
}

void MenuSeleccionNivel::setupUI()
{
    // remover el layout vertical base
    delete LayoutPrincipal;

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    pesteBN     = ":/sprites/Menu_seleccion_nivel/nivel_peste_bn.png";
    pesteColor  = ":/sprites/Menu_seleccion_nivel/nivel_peste_color.png";
    coleraBN    = ":/sprites/Menu_seleccion_nivel/nivel_colera_bn.png";
    coleraColor = ":/sprites/Menu_seleccion_nivel/nivel_colera_color.png";
    covidBN     = ":/sprites/Menu_seleccion_nivel/nivel_covid_bn.png";
    covidColor  = ":/sprites/Menu_seleccion_nivel/nivel_covid_color.png";

    pnlPeste  = new QLabel(this);
    pnlColera = new QLabel(this);
    pnlCovid  = new QLabel(this);

    QList<QLabel*> panels = { pnlPeste, pnlColera, pnlCovid };

    for (auto *p : panels)
    {
        p->setScaledContents(true);
        p->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        p->installEventFilter(this);
        p->setCursor(Qt::PointingHandCursor);
    }

    pnlPeste->setPixmap(QPixmap(pesteBN));
    pnlColera->setPixmap(QPixmap(coleraBN));
    pnlCovid->setPixmap(QPixmap(covidBN));

    layout->addWidget(pnlPeste);
    layout->addWidget(pnlColera);
    layout->addWidget(pnlCovid);
}

void MenuSeleccionNivel::conectarsenales()
{
    // clicks
    pnlPeste->installEventFilter(this);
    pnlColera->installEventFilter(this);
    pnlCovid->installEventFilter(this);
}

bool MenuSeleccionNivel::eventFilter(QObject *obj, QEvent *event)
{
    QLabel *panel = qobject_cast<QLabel*>(obj);
    if (!panel) return false;

    // HOVER -> COLOR
    if (event->type() == QEvent::Enter)
    {
        if (panel == pnlPeste)  panel->setPixmap(QPixmap(pesteColor));
        if (panel == pnlColera) panel->setPixmap(QPixmap(coleraColor));
        if (panel == pnlCovid)  panel->setPixmap(QPixmap(covidColor));
    }

    // LEAVE -> BN
    if (event->type() == QEvent::Leave)
    {
        if (panel == pnlPeste)  panel->setPixmap(QPixmap(pesteBN));
        if (panel == pnlColera) panel->setPixmap(QPixmap(coleraBN));
        if (panel == pnlCovid)  panel->setPixmap(QPixmap(covidBN));
    }

    // CLICK -> señal nivel
    if (event->type() == QEvent::MouseButtonPress)
    {
        if (panel == pnlPeste)  emit nivelSeleccionado(1);
        if (panel == pnlColera) emit nivelSeleccionado(2);
        if (panel == pnlCovid)  emit nivelSeleccionado(3);
    }

    return false;
}
