#include "hero.h"
#include "simulation.h"
#include "sync.h"
#include "combat.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// Mueve el héroe al siguiente punto de su ruta
void mover_heroe(Heroe *heroe)
{
    if (heroe->path_index_actual >= heroe->path_length)
    {
        return;
    }

    Punto siguiente = heroe->path[heroe->path_index_actual];

    printf("[Héroe %d] Movimiento: (%d,%d) → (%d,%d)\n", 
           heroe->id, heroe->x, heroe->y, siguiente.x, siguiente.y);

    heroe->x = siguiente.x;
    heroe->y = siguiente.y;
    heroe->path_index_actual++;
}

// Verifica si hay monstruos alertados en rango de ataque del héroe
int mounstros_en_rango(Heroe *heroe, Monstruo *monstruos, int cantidad)
{
    for (int i = 0; i < cantidad; i++)
    {
        Monstruo *m = &monstruos[i];

        if (!m->vivo || !m->alertado)
        {
            continue;
        }

        int distancia = distancia_manhattan(heroe->x, heroe->y, m->x, m->y);

        if (distancia <= heroe->attack_range)
        {
            return 1;
        }
    }
    return 0;
}

// Thread principal del héroe
void *heroe_thread(void *arg)
{
    Heroe *heroe = (Heroe *)arg;

    while (1)
    {
        // Verificar estado del héroe
        pthread_mutex_lock(&mutex_combate);
        int vivo = heroe->vivo;
        pthread_mutex_unlock(&mutex_combate);

        if (!vivo || !atomic_load(&simulacion_ejecutandose) || heroe->path_index_actual >= heroe->path_length)
            break;

        pthread_mutex_lock(&mutex_grid);

        if (mounstros_en_rango(heroe, monstruos_globales, cant_monstruos_global))
        {
            heroe->en_combate = 1;

            // Buscar el monstruo más cercano en rango
            Monstruo *monstruo_cercano = NULL;
            int distancia_minima = 9999;

            for (int i = 0; i < cant_monstruos_global; i++)
            {
                Monstruo *m = &monstruos_globales[i];
                if (m->vivo && m->alertado)
                {
                    int distancia = distancia_manhattan(heroe->x, heroe->y, m->x, m->y);
                    if (distancia <= heroe->attack_range && distancia < distancia_minima)
                    {
                        distancia_minima = distancia;
                        monstruo_cercano = m;
                    }
                }
            }

            pthread_mutex_unlock(&mutex_grid);

            // Iniciar combate con el monstruo más cercano
            if (monstruo_cercano != NULL)
            {
                combate(heroe, monstruo_cercano);
            }

            heroe->en_combate = 0;
        }
        else
        {
            heroe->en_combate = 0;
            mover_heroe(heroe);
            pthread_mutex_unlock(&mutex_grid);
        }

        usleep(300000);
    }

    // Mensaje de salida según condición
    if (!heroe->vivo || heroe->hp <= 0)
    {
        printf("[Héroe %d] Eliminado en combate\n", heroe->id);
    }
    else if (heroe->path_index_actual >= heroe->path_length)
    {
        printf("[Héroe %d] Destino alcanzado exitosamente\n", heroe->id);
    }

    return NULL;
}