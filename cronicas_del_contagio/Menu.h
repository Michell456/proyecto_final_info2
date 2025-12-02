#ifndef MENU_H
#define MENU_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

// Abstract

class Menu : public QWidget
{
    Q_OBJECT

public:
    explicit Menu(QWidget *parent = nullptr);
    virtual ~Menu() = default;

protected:
    QVBoxLayout *mainLayout;
    QLabel *titleLabel;

    virtual void setupUI() = 0;
    virtual void connectSignals() = 0;

    QPushButton* createButton(const QString &text, const QString &style = "");

signals:
    void gameStarted();
    void levelSelected(int level);
    void gameResumed();
    void gameExited();
    void backToMainMenu();
};

#endif // MENU_H
