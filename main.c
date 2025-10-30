#include "config.h"
#include "parser.h"
#include "simulation.h"
#include "sync.h"
#include "hero.h"
#include "monster.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Uso: %s <archivo_config>\n", argv[0]);
        return 1;
    }

    // Inicializar sincronización
    inicializar_sync();

    Configuracion config;

    if (parsear_archivo(argv[1], &config) != 0)
    {
        fprintf(stderr, "Error al parsear la configuracion!!\n");
        destruir_sync();
        return 1;
    }

    // Configurar variables globales
    heroes_globales = config.heroes;
    cant_heroes_global = config.cant_heroes;
    monstruos_globales = config.monstruos;
    cant_monstruos_global = config.cant_monstruos;

    printf("\n INICIANDO SIMULACIÓN...\n");
    printf("Héroes: %d | Monstruos: %d\n\n", config.cant_heroes, config.cant_monstruos);

    // Crear threads de héroes (N threads)
    pthread_t hero_tids[MAX_HEROES];
    for (int i = 0; i < config.cant_heroes; i++)
    {
        if (pthread_create(&hero_tids[i], NULL, heroe_thread, &config.heroes[i]) != 0)
        {
            fprintf(stderr, "Error al crear thread del héroe %d\n", i + 1);
            destruir_sync();
            return 1;
        }
        printf("Thread del héroe %d creado\n", i + 1);
    }
    printf("\n");

    // Crear threads de monstruos
    pthread_t monstruo_tids[MAX_MONSTRUOS];
    for (int i = 0; i < config.cant_monstruos; i++)
    {
        if (pthread_create(&monstruo_tids[i], NULL, monstruo_thread, &config.monstruos[i]) != 0)
        {
            fprintf(stderr, "Error al crear el thread del monstruo %d\n", i + 1);
            atomic_store(&simulacion_ejecutandose, 0);
            
            // Join héroes creados
            for (int j = 0; j < config.cant_heroes; j++)
            {
                pthread_join(hero_tids[j], NULL);
            }
            
            // Join monstruos creados
            for (int j = 0; j < i; j++)
            {
                pthread_join(monstruo_tids[j], NULL);
            }
            destruir_sync();
            return 1;
        }
        printf("Thread del monstruo %d creado\n", i + 1);
    }

    // Espera tiempo de simulación
    printf("\n Simulación ejecutándose por 10 segundos...\n\n");
    sleep(10);

    // Detener simulación
    atomic_store(&simulacion_ejecutandose, 0);

    // Esperar que todos los threads de héroes terminen
    for (int i = 0; i < config.cant_heroes; i++)
    {
        pthread_join(hero_tids[i], NULL);
    }

    // Esperar que todos los threads de monstruos terminen
    for (int i = 0; i < config.cant_monstruos; i++)
    {
        pthread_join(monstruo_tids[i], NULL);
    }

    printf("\nSIMULACIÓN TERMINADA...\n");

    // Liberar recursos (paths de todos los héroes)
    for (int i = 0; i < config.cant_heroes; i++)
    {
        if (config.heroes[i].path != NULL)
        {
            free(config.heroes[i].path);
        }
    }
    
    // Destruir sincronización
    destruir_sync();

    return 0;
}