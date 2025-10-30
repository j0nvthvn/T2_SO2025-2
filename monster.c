#include "monster.h"
#include "simulation.h"
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>

// Verifica si el heroe esta en rango de VISION del monstruo
int heroe_en_vision(Monstruo *monstruo, Heroe *heroe)
{
    int distancia = distancia_manhattan(monstruo->x, monstruo->y, heroe->x, heroe->y);
    return distancia <= monstruo->vision_range; // retorna solo si la distancia es menor o igual al rango de vision del monstruo
}

// Verifica si el heroe esta en rango de ATAQUE del monstruo
int heroe_en_rango_atq(Monstruo *monstruo, Heroe *heroe)
{
    int distancia = distancia_manhattan(monstruo->x, monstruo->y, heroe->x, heroe->y);
    return distancia <= monstruo->attack_range; // retorna solo si la distancia es menor o igual al rango de ataque del monstruo
}

// Thread principal del monstruo
void *monstruo_thread(void *arg)
{
    Monstruo *monstruo = (Monstruo *)arg;

    printf("Monstruo %d iniciado en posicion (%d,%d)\n", monstruo->id, monstruo->x, monstruo->y);
    printf("Vision: %d, Ataque: %d, HP: %d\n", monstruo->vision_range, monstruo->attack_range, monstruo->hp);

    while (monstruo->vivo && simulacion_ejecutandose)
    {
        pthread_mutex_lock(&mutex_grid);

        // Buscar el héroe más cercano vivo
        Heroe *heroe_cercano = NULL;
        int distancia_minima = 9999;

        for (int i = 0; i < cant_heroes_global; i++)
        {
            if (!heroes_globales[i].vivo)
                continue;

            int distancia = distancia_manhattan(monstruo->x, monstruo->y, 
                                               heroes_globales[i].x, heroes_globales[i].y);

            // Fase 1: Detectar si está en rango de visión
            if (!monstruo->alertado && distancia <= monstruo->vision_range)
            {
                monstruo->alertado = 1;
                sem_post(&monstruos_activos); // Incrementa el contador de monstruos activos
                printf(" Monstruo %d ALERTA! Héroe detectado a distancia %d\n", monstruo->id, distancia);
            }

            // Encontrar el más cercano
            if (distancia < distancia_minima)
            {
                distancia_minima = distancia;
                heroe_cercano = &heroes_globales[i];
            }
        }

        // Fase 2: Si está alertado, moverse hacia el héroe más cercano
        if (monstruo->alertado && heroe_cercano != NULL)
        {
            // Si NO está en rango de ataque, moverse
            if (distancia_minima > monstruo->attack_range)
            {
                mover_monstruo_hacia_heroe(monstruo, heroe_cercano);
                pthread_mutex_unlock(&mutex_grid);
            }
            else
            {
                // Está en rango de ataque, esperar combate iniciado por héroe
                printf("Monstruo %d en rango de ataque (distancia=%d)\n", monstruo->id, distancia_minima);
                pthread_mutex_unlock(&mutex_grid);
                sleep(2);
            }
        }
        else
        {
            pthread_mutex_unlock(&mutex_grid);
        }

        usleep(100000); 
    }

    if (!monstruo->vivo)
    {
        printf("Monstruo %d fue eliminado\n", monstruo->id);
    }
    else
    {
        printf("Monstruo %d finalizó (simulacion terminada)\n", monstruo->id);
    }

    return NULL;
}