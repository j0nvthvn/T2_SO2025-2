#include "simulation.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>

// Calcular distancia Manhattan entre dos puntos (|x1-x2|+|y1-y2|)
int distancia_manhattan(int x1, int y1, int x2, int y2)
{
    return abs(x1 - x2) + abs(y1 - y2);
}

// Mover monstruo un paso hacia el heroe
void mover_monstruo_hacia_heroe(Monstruo *monstruo, Heroe *heroe)
{
    int difx = heroe->x - monstruo->x; // Diferencia en coord X
    int dify = heroe->y - monstruo->y; // Diferencia en coord Y

    // DERECHA | heroe es + y monstruo - (x)
    if (difx > 0)
    {
        monstruo->x++;
        printf("Monstruo %d se mueve hacia DERECHA (%d,%d)", monstruo->id, monstruo->x, monstruo->y);
    }
    // IZQUIERDA | heroe es - y monstruo + (x)
    else if (difx < 0)
    {
        monstruo->x--;
        printf("Monstruo %d se mueve hacia IZQUIERDA (%d,%d)", monstruo->id, monstruo->x, monstruo->y);
    }
    // ARRIBA | heroe es + y monstruo - (y)
    else if (dify > 0)
    {
        monstruo->y++;
        printf("Monstruo %d se mueve hacia ARRIBA (%d,%d)", monstruo->id, monstruo->x, monstruo->y);
    }
    // ABAJO | heroe es - y monstruo + (y)
    else if (dify < 0)
    {
        monstruo->y--;
        printf("Monstruo %d se mueve hacia ABAJO (%d,%d)", monstruo->id, monstruo->x, monstruo->y);
    }
    /*
    c.o.c. no hace nada 0.0, osea que difx=dify=0, por lo tanto el mounstro
    llegó donde el heroe
    */
}

// Detectar si el monstruo debe alertarse, osea heroe entre en rango de vision de monstruo.
int alertar_monstruos(Monstruo *monstruo, Heroe *heroe)
{
    if (monstruo->alertado)
    { // Esta alertado?
        return 1;
    }

    int distancia = distancia_manhattan(monstruo->x, monstruo->y, heroe->x, heroe->y); // Distancia actual monstruo del heroe

    if (distancia <= monstruo->vision_range)
    {
        monstruo->alertado = 1;
        //printf del monstruo alertado...
        printf("Monstruo %d alertado!! Heroe detectado a distancia %d (Rango de vision: %d)\n", monstruo->id,distancia,monstruo->vision_range);
        return 1;
    }
    return 0;
}