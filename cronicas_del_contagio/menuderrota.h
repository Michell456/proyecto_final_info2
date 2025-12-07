#ifndef MENUDERROTA_H
#define MENUDERROTA_H

#include "Menu.h"
#include <QPushButton>
#include <QLabel>
#include <QPixmap>

class MenuDerrota : public Menu
{
    Q_OBJECT
public:
    explicit MenuDerrota(QWidget *parent = nullptr);

protected:
    void setupUI() override;
    void conectarsenales() override;
    void paintEvent(QPaintEvent *event) override;

private:
    QLabel *LabelTitulo;
    QPushButton *btnMenuPrincipal;
    QPushButton *btnSalir;
    QPixmap fondoDerrota;

private slots:
    void onMenuPrincipal();
    void onSalir();
};

#endif // MENUDERROTA_H
