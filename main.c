#include "config.h"
#include "parser.h"
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

    free(config.heroe.path);

    return 0;
}
