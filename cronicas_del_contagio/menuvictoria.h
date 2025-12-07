#ifndef MENUVICTORIA_H
#define MENUVICTORIA_H

#include "Menu.h"
#include <QPushButton>
#include <QLabel>
#include <QPixmap>

class MenuVictoria : public Menu
{
    Q_OBJECT
public:
    explicit MenuVictoria(QWidget *parent = nullptr);

protected:
    void setupUI() override;
    void conectarsenales() override;
    void paintEvent(QPaintEvent *event) override;

private:
    QLabel *LabelTitulo;
    QPushButton *btnMenuPrincipal;
    QPushButton *btnSalir;
    QPixmap fondoVictoria;

private slots:
    void onMenuPrincipal();
    void onSalir();
};

#endif // MENUVICTORIA_H
