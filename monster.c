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

        // Fase 1... Detecta al heroe si no esta alertado
        if (!monstruo->alertado && heroe_global->vivo)
        {
            if (heroe_en_vision(monstruo, heroe_global))
            {
                monstruo->alertado = 1;
                sem_post(&monstruos_activos); // Incrementa el contador de monstruos activos.

                printf("Monstruo %d alertado! Heroe detectado a distancia %d\n", monstruo->id, distancia_manhattan(monstruo->x, monstruo->y, heroe_global->x, heroe_global->y));
            }
        }

        // Fase 2... Si esta alertado moverse hacia el heroe
        if (monstruo->alertado && heroe_global->vivo)
        {
            int distancia = distancia_manhattan(monstruo->x, monstruo->y, heroe_global->x, heroe_global->y);
            // Ahora, si el monstruo NO esta en el rango de ataque, se mueve.
            if (distancia > monstruo->attack_range)
            {
                mover_monstruo_hacia_heroe(monstruo, heroe_global);
                pthread_mutex_unlock(&mutex_grid);
            }
            else // Caso contrario, si no esta en rango de atq, prepararse para combate
            {
                printf("Monstruo %d en rango de ataque (distancia=%d)\n", monstruo->id, distancia);
                pthread_mutex_unlock(&mutex_grid);

                sleep(2);
            }
        }
        else
        {
            pthread_mutex_unlock(&mutex_grid);
        }

        sleep(1);
    }

    if (!monstruo->vivo)
    {
        printf("Monstruo %d fue elmininado\n", monstruo->id);
    }
    else
    {
        printf("Monstruo %d finalizó (simulacion terminada)\n", monstruo->id);
    }

    return NULL;
}