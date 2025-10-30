#include "hero.h"
#include "simulation.h"
#include "sync.h"
#include "combat.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void mover_heroe(Heroe *heroe)
{
    if (heroe->path_index_actual >= heroe->path_length)
    {
        printf("Heroe llego al final de la ruta definida!!\n");
        return;
    }

    Punto siguiente = heroe->path[heroe->path_index_actual];

    printf("Heroe se mueve de (%d,%d) a (%d,%d)\n", heroe->x, heroe->y, siguiente.x, siguiente.y);

    heroe->x = siguiente.x;     // Avanza a la siguiente posicion x
    heroe->y = siguiente.y;     // Avanza a la siguiente posicion y
    heroe->path_index_actual++; // Siguiente posicion del path
}

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
            printf("Monstruo %d detectado en rango (dist=%d)\n", m->id, distancia);
            return 1;
        }
    }
    return 0;
}

void *heroe_thread(void *arg)
{
    Heroe *heroe = (Heroe *)arg;

    printf("Heroe iniciado en posicion (%d,%d)\n", heroe->x, heroe->y);
    printf("Cantidad de puntos: %d\n", heroe->path_length);

    while (1)
    {
        // Verificar estado del héroe
        pthread_mutex_lock(&mutex_combate);
        int vivo = heroe->vivo;
        pthread_mutex_unlock(&mutex_combate);
        
        if (!vivo || !atomic_load(&simulacion_ejecutandose) || heroe->path_index_actual >= heroe->path_length)
            break;
            
        pthread_mutex_lock(&mutex_grid);
        // SECCION CRITICA
        if (mounstros_en_rango(heroe, monstruos_globales, cant_monstruos_global))
        {
            heroe->en_combate = 1;

            // Se encuentra el monstruo más cercano en rango
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
                iniciar_combate(heroe, monstruo_cercano);
            }

            heroe->en_combate = 0;
        }
        else
        {
            heroe->en_combate = 0;
            mover_heroe(heroe);
            pthread_mutex_unlock(&mutex_grid);
        }

        usleep(100000);
    }

    // Al salir del loop, se determina la razón
    if (!heroe->vivo || heroe->hp <= 0)
    {
        printf("Heroe murio en combate :c\n");
    }
    else if (heroe->path_index_actual >= heroe->path_length)
    {
        printf("Heroe completo su viaje exitosamente!\n");
    }
    else
    {
        printf("Simulacion detenida\n");
    }

    return NULL;
}