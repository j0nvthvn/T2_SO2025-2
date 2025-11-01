#include "monster.h"
#include "simulation.h"
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>

// Verifica si el héroe está en rango de visión del monstruo
int heroe_en_vision(Monstruo *monstruo, Heroe *heroe)
{
    int distancia = distancia_manhattan(monstruo->x, monstruo->y, heroe->x, heroe->y);
    return distancia <= monstruo->vision_range;
}

// Verifica si el héroe está en rango de ataque del monstruo
int heroe_en_rango_atq(Monstruo *monstruo, Heroe *heroe)
{
    int distancia = distancia_manhattan(monstruo->x, monstruo->y, heroe->x, heroe->y);
    return distancia <= monstruo->attack_range;
}

// Thread principal del monstruo
void *monstruo_thread(void *arg)
{
    Monstruo *monstruo = (Monstruo *)arg;

    printf("[Monstruo %d] Iniciado en (%d,%d) - HP:%d Vis:%d Atq:%d\n", 
           monstruo->id, monstruo->x, monstruo->y, monstruo->hp, 
           monstruo->vision_range, monstruo->attack_range);

    while (1)
    {
        // Verificar estado del monstruo
        pthread_mutex_lock(&mutex_combate);
        int vivo = monstruo->vivo;
        pthread_mutex_unlock(&mutex_combate);
        
        if (!vivo || !atomic_load(&simulacion_ejecutandose))
            break;
            
        pthread_mutex_lock(&mutex_grid);

        // Buscar héroe más cercano vivo
        Heroe *heroe_cercano = NULL;
        int distancia_minima = 9999;

        for (int i = 0; i < cant_heroes_global; i++)
        {
            if (!heroes_globales[i].vivo)
                continue;

            int distancia = distancia_manhattan(monstruo->x, monstruo->y, 
                                               heroes_globales[i].x, heroes_globales[i].y);

            // Si está en rango de visión y no está alertado, activarlo
            if (!monstruo->alertado && distancia <= monstruo->vision_range)
            {
                monstruo->alertado = 1;
                sem_post(&monstruos_activos);
                printf("[Monstruo %d] ALERTA - Héroe detectado a distancia %d\n", 
                       monstruo->id, distancia);
                
                // Alertar a otros monstruos cercanos
                for (int j = 0; j < cant_monstruos_global; j++)
                {
                    if (j == (monstruo->id - 1))
                        continue;
                    
                    Monstruo *otro_monstruo = &monstruos_globales[j];
                    
                    if (otro_monstruo->alertado)
                        continue;
                    
                    int dist_monstruo = distancia_manhattan(
                        monstruo->x, monstruo->y,
                        otro_monstruo->x, otro_monstruo->y
                    );
                    
                    if (dist_monstruo <= monstruo->vision_range)
                    {
                        otro_monstruo->alertado = 1;
                        sem_post(&monstruos_activos);
                        printf("   └─ Monstruo %d alerta a Monstruo %d\n", 
                               monstruo->id, otro_monstruo->id);
                    }
                }
            }

            // Encontrar héroe más cercano
            if (distancia < distancia_minima)
            {
                distancia_minima = distancia;
                heroe_cercano = &heroes_globales[i];
            }
        }

        // Si está alertado, moverse hacia el héroe más cercano
        if (monstruo->alertado && heroe_cercano != NULL)
        {
            // Si NO está en rango de ataque, acercarse
            if (distancia_minima > monstruo->attack_range)
            {
                mover_monstruo_hacia_heroe(monstruo, heroe_cercano);
                pthread_mutex_unlock(&mutex_grid);
            }
            else
            {
                // Ya está en rango, esperar ataque del héroe
                pthread_mutex_unlock(&mutex_grid);
                sleep(2);
            }
        }
        else
        {
            pthread_mutex_unlock(&mutex_grid);
        }

        usleep(300000); 
    }

    if (!monstruo->vivo)
    {
        printf("[Monstruo %d] Eliminado\n", monstruo->id);
    }

    return NULL;
}