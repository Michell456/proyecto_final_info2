#include "nivel.h"

nivel::nivel(QObject *parent)
    : QObject(parent),
    estado(EstadoNivel::jugando),
    tiempoTranscurrido(0)
{
}

void nivel::setTamanioVentana(QSize tamanio){
    tamanioVentana = tamanio;
}
