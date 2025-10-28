#include "config.h"
#include "parser.h"
#include "simulation.h"
#include "sync.h"
#include <stdio.h>
#include <stdlib.h>

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
        return 1;
    }

    printf("Test distancia Manhattan...\n");
    int distancia = distancia_manhattan(config.heroe.x, config.heroe.y, config.monstruos[0].x, config.monstruos[0].y);

    printf("Distancia heroe-monstruo 1: %d\n", distancia);

    printf("\n=== Test detección de alerta ===\n");

    // Resetear posición del monstruo 1
    config.monstruos[0].x = 8;
    config.monstruos[0].y = 4;
    config.monstruos[0].alertado = 0;

    Monstruo *m1 = &config.monstruos[0];
    printf("Monster 1: pos=(%d,%d) vision_range=%d alerted=%d\n",
           m1->x, m1->y, m1->vision_range, m1->alertado);
    printf("Hero: pos=(%d,%d)\n", config.heroe.x, config.heroe.y);

    // Simular acercamiento del héroe
    printf("\n--- Simulando movimiento del héroe hacia monstruo ---\n");
    for (int i = 0; i < 6; i++)
    {
        printf("\nTurno %d:\n", i + 1);

        // Mover héroe hacia monstruo (simplificado)
        if (config.heroe.x < m1->x)
            config.heroe.x++;

        int dist = distancia_manhattan(m1->x, m1->y, config.heroe.x, config.heroe.y);
        printf("Hero en (%d,%d) - Distancia: %d\n",
               config.heroe.x, config.heroe.y, dist);

        // Detectar alerta
        int alertado = alertar_monstruos(m1, &config.heroe);
        printf("Monstruo alertado: %s\n", alertado ? "SÍ" : "NO");

        if (alertado)
        {
            printf("Monstruo comenzará a perseguir al héroe\n");
            break;
        }
    }

    destruir_sync();
    free(config.heroe.path);

    return 0;
}
