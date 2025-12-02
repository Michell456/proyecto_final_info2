#include "Menu.h"

Menu::Menu(QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this))
{

    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSpacing(20);
}

QPushButton* Menu::createButton(const QString &text, const QString &style)
{
    QPushButton *button = new QPushButton(text);
    QString buttonStyle = style.isEmpty() ?
                              "QPushButton { background-color: #3498db; color: white; border: none; "
                              "padding: 15px 30px; font-size: 18px; border-radius: 10px; }"
                              "QPushButton:hover { background-color: #2980b9; }"
                              "QPushButton:pressed { background-color: #21618c; }" : style;

    button->setStyleSheet(buttonStyle);
    button->setFixedSize(250, 60);
    return button;
}
