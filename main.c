#include "config.h"
#include "parser.h"
#include "simulation.h"
#include "sync.h"
#include "hero.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Definir variables globales (declaradas como extern en config.h)
Heroe* heroe_global = NULL;
Monstruo* monstruos_globales = NULL;
int cant_monstruos_global = 0;

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
    heroe_global = &config.heroe;
    monstruos_globales = config.monstruos;
    cant_monstruos_global = config.cont_monstruos;

    printf("\nINICIANDO SIMULACIÓN...\n\n");

    // Crear thread del héroe
    pthread_t heroe_tid;
    if (pthread_create(&heroe_tid, NULL, heroe_thread, &config.heroe) != 0) {
        fprintf(stderr, "Error al crear thread del héroe\n");
        destruir_sync();
        return 1;
    }

    printf("Thread del héroe creado\n\n");

    // Esperar a que el héroe termine (máximo 15 segundos para testing)
    sleep(15);
    
    // Detener simulación
    simulacion_ejecutandose = 0;
    
    // Esperar que el thread termine
    pthread_join(heroe_tid, NULL);

    printf("\nSIMULACIÓN TERMINADA...\n");

    // Liberar recursos
    destruir_sync();
    free(config.heroe.path);

    return 0;
}