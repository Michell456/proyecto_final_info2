#ifndef MENUPAUSA_H
#define MENUPAUSA_H

#include "Menu.h"
#include <QPushButton>
#include <QLabel>
#include <QPixmap>

class MenuPausa : public Menu
{
    Q_OBJECT
public:
    explicit MenuPausa(QWidget *parent = nullptr);

protected:
    void setupUI() override;
    void connectSignals() override;
    void paintEvent(QPaintEvent *event) override;

private:
    QLabel *titleLabel;
    QPushButton *btnReanudar;
    QPushButton *btnMenuPrincipal;
    QPushButton *btnSalir;
    QPixmap fondoPausa;

private slots:
    void onReanudar();
    void onMenuPrincipal();
    void onSalir();
};

#endif // MENUPAUSA_H
