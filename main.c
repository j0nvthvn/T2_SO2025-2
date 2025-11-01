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

    inicializar_sync();

    Configuracion config;

    if (parsear_archivo(argv[1], &config) != 0)
    {
        fprintf(stderr, "Error al parsear la configuracion!!\n");
        destruir_sync();
        return 1;
    }

    // Setear variables globales para acceso desde threads
    heroes_globales = config.heroes;
    cant_heroes_global = config.cant_heroes;
    monstruos_globales = config.monstruos;
    cant_monstruos_global = config.cant_monstruos;

    printf("\n INICIANDO SIMULACIÓN...\n");
    printf("\n");
    printf("Configuración:\n");
    printf("    - Tamaño Grid: %dx%d\n", config.grid.width, config.grid.height);
    printf("    - Heroes: %d\n", config.cant_heroes);
    printf("    - Monstruos: %d\n", config.cant_monstruos);
    printf("\n");

    // Crear N threads (uno por héroe)
    pthread_t hero_tids[MAX_HEROES];
    for (int i = 0; i < config.cant_heroes; i++)
    {
        if (pthread_create(&hero_tids[i], NULL, heroe_thread, &config.heroes[i]) != 0)
        {
            fprintf(stderr, "Error al crear thread del héroe %d\n", i + 1);
            destruir_sync();
            return 1;
        }
    }

    // Crear M threads (uno por monstruo)
    pthread_t monstruo_tids[MAX_MONSTRUOS];
    for (int i = 0; i < config.cant_monstruos; i++)
    {
        if (pthread_create(&monstruo_tids[i], NULL, monstruo_thread, &config.monstruos[i]) != 0)
        {
            fprintf(stderr, "Error al crear el thread del monstruo %d\n", i + 1);
            atomic_store(&simulacion_ejecutandose, 0);

            // Hacer join de los threads ya creados antes de salir
            for (int j = 0; j < config.cant_heroes; j++)
            {
                pthread_join(hero_tids[j], NULL);
            }

            for (int j = 0; j < i; j++)
            {
                pthread_join(monstruo_tids[j], NULL);
            }
            destruir_sync();
            return 1;
        }
    }

    printf("\n");
    printf("Simulación en progreso (se detendrá al completar rutas o máx 60s)...\n");
    printf("==============================================\n\n");
    
    // Loop de espera hasta que todos completen o se alcance el tiempo máximo
    int tiempo_transcurrido = 0;
    int tiempo_maximo = 60;
    int todos_terminaron = 0;
    
    while (tiempo_transcurrido < tiempo_maximo && !todos_terminaron)
    {
        sleep(1);
        tiempo_transcurrido++;
        
        // Verificar si todos los héroes terminaron (completaron ruta o murieron)
        pthread_mutex_lock(&mutex_combate);
        todos_terminaron = 1;
        for (int i = 0; i < config.cant_heroes; i++)
        {
            Heroe *h = &heroes_globales[i];
            if (h->vivo && h->path_index_actual < h->path_length)
            {
                todos_terminaron = 0;
                break;
            }
        }
        pthread_mutex_unlock(&mutex_combate);
    }
    
    atomic_store(&simulacion_ejecutandose, 0);
    
    if (todos_terminaron)
    {
        printf("\n[!] Todos los héroes completaron o fueron eliminados\n");
    }
    else
    {
        printf("\n[!] Tiempo máximo alcanzado (%d segundos)\n", tiempo_maximo);
    }

    // Join de todos los threads de héroes
    for (int i = 0; i < config.cant_heroes; i++)
    {
        pthread_join(hero_tids[i], NULL);
    }

    // Join de todos los threads de monstruos
    for (int i = 0; i < config.cant_monstruos; i++)
    {
        pthread_join(monstruo_tids[i], NULL);
    }

    printf("\n");
    printf("==============================================\n");
    printf("SIMULACIÓN FINALIZADA\n");
    printf("==============================================\n\n");

    // Resumen final: contar héroes y monstruos vivos
    int heroes_vivos = 0;
    int heroes_exitosos = 0;
    for (int i = 0; i < config.cant_heroes; i++)
    {
        if (heroes_globales[i].vivo)
        {
            heroes_vivos++;
            if (heroes_globales[i].path_index_actual >= heroes_globales[i].path_length)
            {
                heroes_exitosos++;
            }
        }
    }

    int monstruos_vivos = 0;
    for (int i = 0; i < config.cant_monstruos; i++)
    {
        if (monstruos_globales[i].vivo)
            monstruos_vivos++;
    }

    printf("RESUMEN GENERAL:\n");
    printf("  Héroes: %d/%d vivos (%d completaron ruta)\n", 
           heroes_vivos, config.cant_heroes, heroes_exitosos);
    printf("  Monstruos: %d/%d vivos\n", 
           monstruos_vivos, config.cant_monstruos);
    
    int total_kills = 0;
    for (int i = 0; i < config.cant_heroes; i++)
    {
        total_kills += heroes_globales[i].stats.monstruos_eliminados;
    }
    printf("  Total de monstruos eliminados: %d\n\n", total_kills);

    // Detalle de cada héroe
    printf("ESTADÍSTICAS POR HÉROE:\n");
    for (int i = 0; i < config.cant_heroes; i++)
    {
        Heroe *h = &heroes_globales[i];
        printf("  Héroe %d: ", h->id);
        
        if (h->vivo)
        {
            printf("VIVO [%d/%d HP]", h->hp, h->hp_inicial);
            if (h->path_index_actual >= h->path_length)
            {
                printf(" - RUTA COMPLETADA");
            }
        }
        else
        {
            printf("MUERTO");
        }
        
        printf("\n");
        printf("    - Monstruos eliminados: %d\n", h->stats.monstruos_eliminados);
        printf("    - Daño causado: %d\n", h->stats.danio_total_causado);
        printf("    - Daño recibido: %d\n", h->stats.danio_total_recibido);
        printf("    - Posición final: (%d,%d)\n", h->x, h->y);
        printf("    - Progreso ruta: %d/%d puntos\n\n", 
               h->path_index_actual, h->path_length);
    }

    // Estadísticas de monstruos
    printf("ESTADÍSTICAS DE MONSTRUOS:\n");
    for (int i = 0; i < config.cant_monstruos; i++)
    {
        Monstruo *m = &monstruos_globales[i];
        printf("  Monstruo %d: ", m->id);
        
        if (m->vivo)
        {
            printf("VIVO [%d/%d HP]", m->hp, m->hp_inicial);
        }
        else
        {
            printf("ELIMINADO");
        }
        
        printf(" - Posición: (%d,%d)\n", m->x, m->y);
    }
    printf("\n");

    // Liberar memoria de paths
    for (int i = 0; i < config.cant_heroes; i++)
    {
        if (config.heroes[i].path != NULL)
        {
            free(config.heroes[i].path);
        }
    }

    destruir_sync();

    return 0;
}