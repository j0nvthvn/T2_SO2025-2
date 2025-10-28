#include "config.h"
#include "parser.h"
#include "simulation.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    if(argc !=2) {
        fprintf(stderr, "Uso: %s <archivo_config>\n", argv[0]);
        return 1;
    }

    Configuracion config;

    if(parsear_archivo(argv[1], &config) !=0) {
        fprintf(stderr, "Error al parsear la configuracion!!\n");
        return 1;
    }

    printf("Test distancia Manhattan...\n");
    int distancia = distancia_manhattan(config.heroe.x, config.heroe.y, config.monstruos[0].x,config.monstruos[0].y);

    printf("Distancia heroe-monstruo 1: %d\n",distancia);

    free(config.heroe.path);

    return 0;
}
