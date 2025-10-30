#include "sync.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t mutex_grid;
pthread_mutex_t mutex_combate;
sem_t turno_combate;
sem_t monstruos_activos;
atomic_int simulacion_ejecutandose = 1;

// Variables globales para threads (defniciones)
Heroe* heroes_globales = NULL;
int cant_heroes_global = 0;
Monstruo* monstruos_globales = NULL;
int cant_monstruos_global = 0;

void inicializar_sync()
{
    printf("Inicializando herramientas de sincronización...\n");

    if (pthread_mutex_init(&mutex_grid, NULL) != 0)
    {
        fprintf(stderr, "Error: No se pudo inicializar mutex_grid\n");
        exit(1);
    }

    if (pthread_mutex_init(&mutex_combate, NULL) != 0)
    {
        fprintf(stderr, "Error: No se pudo inicializar mutex_combate\n");
        exit(1);
    }

    if (sem_init(&turno_combate, 0, 1) != 0) // un thread (mosntruo) puede atacar a la vez
    {
        fprintf(stderr, "Error: No se pudo inicializar turno_combate\n");
        exit(1);
    }

    if (sem_init(&monstruos_activos, 0, 0) != 0) // Monstruos en estado pasivo
    {
        fprintf(stderr, "Error: No se pudo inicializar monsters_active\n");
        exit(1);
    }

    printf("Sincronización inicializada correctamente\n");
}

void destruir_sync()
{
    printf("Destruyendo herramientas de sincronización...\n");
    pthread_mutex_destroy(&mutex_grid);
    pthread_mutex_destroy(&mutex_combate);
    sem_destroy(&turno_combate);
    sem_destroy(&monstruos_activos);
}