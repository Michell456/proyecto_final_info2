#ifndef MENU_SELECCION_NIVEL_H
#define MENU_SELECCION_NIVEL_H

#include "Menu.h"
#include <QLabel>
#include <QHBoxLayout>

class MenuSeleccionNivel : public Menu
{
    Q_OBJECT

public:
    explicit MenuSeleccionNivel(QWidget *parent = nullptr);

signals:
    void nivelSeleccionado(int nivel);

protected:
    void setupUI() override;
    void conectarsenales() override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QLabel *pnlPeste;
    QLabel *pnlColera;
    QLabel *pnlCovid;

    QString pesteBN, pesteColor;
    QString coleraBN, coleraColor;
    QString covidBN, covidColor;
};

#endif // MENU_SELECCION_NIVEL_H
