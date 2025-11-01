#include "simulation.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>

// Calcula la distancia Manhattan entre dos puntos
int distancia_manhattan(int x1, int y1, int x2, int y2)
{
    return abs(x1 - x2) + abs(y1 - y2);
}

// Mueve el monstruo un paso hacia el héroe
void mover_monstruo_hacia_heroe(Monstruo *monstruo, Heroe *heroe)
{
    int difx = heroe->x - monstruo->x;
    int dify = heroe->y - monstruo->y;

    int x_prev = monstruo->x;
    int y_prev = monstruo->y;

    // Priorizar movimiento horizontal, luego vertical
    if (difx > 0)
    {
        monstruo->x++;
    }
    else if (difx < 0)
    {
        monstruo->x--;
    }
    else if (dify > 0)
    {
        monstruo->y++;
    }
    else if (dify < 0)
    {
        monstruo->y--;
    }
    
    if (x_prev != monstruo->x || y_prev != monstruo->y)
    {
        printf("[Monstruo %d] Persiguiendo: (%d,%d) → (%d,%d)\n", 
               monstruo->id, x_prev, y_prev, monstruo->x, monstruo->y);
    }
}

// Verifica si el monstruo debe alertarse cuando un héroe entra en su rango de visión
int alertar_monstruos(Monstruo *monstruo, Heroe *heroe)
{
    if (monstruo->alertado)
    { 
        return 1;
    }

    int distancia = distancia_manhattan(monstruo->x, monstruo->y, heroe->x, heroe->y);

    if (distancia <= monstruo->vision_range)
    {
        monstruo->alertado = 1;
        printf("[Monstruo %d] ALERTA - Héroe detectado a distancia %d\n", 
               monstruo->id, distancia);
        return 1;
    }
    return 0;
}