QT += core gui multimedia widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Balde.cpp \
    BaseDeCarga.cpp \
    Doctor.cpp \
    Dron.cpp \
    Juego.cpp \
    Menu.cpp \
    MenuPausa.cpp \
    MenuPrincipal.cpp \
    MenuSeleccionNivel.cpp \
    NivelColera.cpp \
    NivelCovid.cpp \
    ObjetoEstatico.cpp \
    Obstaculo.cpp \
    Proyectil.cpp \
    enfermo.cpp \
    enfermointeligente.cpp \
    item.cpp \
    jugador1.cpp \
    main.cpp \
    mainwindow.cpp \
    nivel.cpp \
    nivelPesteNegra.cpp \
    pajaro.cpp \
    zonainfeccion.cpp

HEADERS += \
    Balde.h \
    BaseDeCarga.h \
    Doctor.h \
    Dron.h \
    Juego.h \
    Menu.h \
    MenuPausa.h \
    MenuPrincipal.h \
    MenuSeleccionNivel.h \
    NivelColera.h \
    NivelCovid.h \
    ObjetoEstatico.h \
    Obstaculo.h \
    Proyectil.h \
    enfermo.h \
    enfermointeligente.h \
    item.h \
    jugador1.h \
    mainwindow.h \
    nivel.h \
    nivelPesteNegra.h \
    pajaro.h \
    zonainfeccion.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    recursos.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
