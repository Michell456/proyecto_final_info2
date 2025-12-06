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
    QVBoxLayout *LayoutPrincipal;
    QLabel *LabelTitulo;

    virtual void setupUI() = 0;
    virtual void conectarsenales() = 0;

    QPushButton* crearBoton(const QString &texto, const QString &estilo = "");

signals:
    void JuegoIniciado();
    void NivelSeleccionado(int nivel);
    void JuegoDespausado();
    void SalirDelJuego();
    void VolverAlMenuPrincipal();
};

#endif // MENU_H
