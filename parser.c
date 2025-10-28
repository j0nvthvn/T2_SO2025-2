#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Parsea el HERO_PATH
int parsear_hero_path(const char *path_str, Punto **path, int *path_length)
{

    // Se cuenta cuantos puntos hay -> si existe un '(' es porque hay un punto, por lo tanto se suma 1 al contador
    int count = 0;
    const char *p = path_str;
    while (*p)
    {
        if (*p == '(')
            count++;
        p++;
    }

    if (count == 0)
    {
        fprintf(stderr, "Error: Path vacio!!\n");
        return -1;
    }

    // Asignar memoria para los puntos
    *path = (Punto *)malloc(count * sizeof(Punto));
    if (*path == NULL)
    { // Comprueba si la asignación fue exitosa
        fprintf(stderr, "Error: no se pudo asignar memoria.\n");
        return -1;
    }

    // Ahora se parsea cada punto
    int index = 0;
    p = path_str;
    while (*p && index < count)
    {
        if (*p == '(')
        {
            int x, y;
            if (sscanf(p, "(%d,%d)", &x, &y) == 2)
            {
                (*path)[index].x = x;
                (*path)[index].y = y;
                index++;
            }
            while (*p && *p != ')') // Avanza hasta el siguiente parentesis
                p++;
            if (*p)
                p++;
        }
        else
        {
            p++;
        }
    }
    *path_length = index;
    return 0;
}

int parsear_archivo(const char *archivo, Configuracion *config)
{
    FILE *file = fopen(archivo, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error: No se pudo abrir el archivo %s\n", archivo);
        return -1;
    }

    char linea[MAX_LENGTH_LINEA];
    config->cont_monstruos = 0;
    int monstruos_posibles;

    // Arrays temporales para almacenar datos de monstruos
    int monstruo_hp[MAX_MONSTRUOS] = {0};
    int monstruo_atack[MAX_MONSTRUOS] = {0};
    int monstruo_vision[MAX_MONSTRUOS] = {0};
    int monstruo_range[MAX_MONSTRUOS] = {0};
    int monstruo_x[MAX_MONSTRUOS] = {0};
    int monstruo_y[MAX_MONSTRUOS] = {0};
    int monstruo_count_data[MAX_MONSTRUOS] = {0}; // Un contador de datos por monstruo

    while (fgets(linea, sizeof(linea), file))
    {
        if (linea[0] == '\n')
            continue;

        // GRID_SIZE
        if (sscanf(linea, "GRID_SIZE %d %d", &config->grid.width, &config->grid.height) == 2)
        {
            printf("Grid: %d x %d\n", config->grid.width, config->grid.height);
        }

        // HERO_HP
        if (sscanf(linea, "HERO_HP %d", &config->heroe.hp) == 1)
        {
            printf("HP Heroe: %d\n", config->heroe.hp);
        }

        // HERO_ATTACK_DAMAGE
        if (sscanf(linea, "HERO_ATTACK_DAMAGE %d", &config->heroe.attack_damage) == 1)
        {
            printf("Hero attack damage: %d\n", config->heroe.attack_damage);
            continue;
        }

        // HERO_ATTACK_RANGE
        if (sscanf(linea, "HERO_ATTACK_RANGE %d", &config->heroe.attack_range) == 1)
        {
            printf("Hero attack range: %d\n", config->heroe.attack_range);
            continue;
        }

        // HERO_START
        if (sscanf(linea, "HERO_START %d %d", &config->heroe.x, &config->heroe.y) == 2)
        {
            printf("Hero start: (%d, %d)\n", config->heroe.x, config->heroe.y);
            continue;
        }

        // HERO_PATH
        if (strncmp(linea, "HERO_PATH ", 10) == 0)
        {
            char *path_str = linea + 10; // Saltar "HERO_PATH "
            if (parsear_hero_path(path_str, &config->heroe.path, &config->heroe.path_length) == 0)
            {
                printf("Hero path: %d puntos\n", config->heroe.path_length);
            }
            continue;
        }

        // MONSTER_COUNT
        if (sscanf(linea, "MONSTER_COUNT %d", &monstruos_posibles) == 1)
        {
            printf("Monstruos esperados: %d\n", monstruos_posibles);
            continue;
        }

        // MONSTER_X_HP
        int monster_id, value;
        if (sscanf(linea, "MONSTER_%d_HP %d", &monster_id, &value) == 2)
        {
            if (monster_id > 0 && monster_id <= MAX_MONSTRUOS)
            {
                monstruo_hp[monster_id - 1] = value;
                monstruo_count_data[monster_id - 1]++;
            }
            continue;
        }

        // MONSTER_X_ATTACK_DAMAGE
        if (sscanf(linea, "MONSTER_%d_ATTACK_DAMAGE %d", &monster_id, &value) == 2)
        {
            if (monster_id > 0 && monster_id <= MAX_MONSTRUOS)
            {
                monstruo_atack[monster_id - 1] = value;
                monstruo_count_data[monster_id - 1]++;
            }
            continue;
        }

        // MONSTER_X_VISION_RANGE
        if (sscanf(linea, "MONSTER_%d_VISION_RANGE %d", &monster_id, &value) == 2)
        {
            if (monster_id > 0 && monster_id <= MAX_MONSTRUOS)
            {
                monstruo_vision[monster_id - 1] = value;
                monstruo_count_data[monster_id - 1]++;
            }
            continue;
        }

        // MONSTER_X_ATTACK_RANGE
        if (sscanf(linea, "MONSTER_%d_ATTACK_RANGE %d", &monster_id, &value) == 2)
        {
            if (monster_id > 0 && monster_id <= MAX_MONSTRUOS)
            {
                monstruo_range[monster_id - 1] = value;
                monstruo_count_data[monster_id - 1]++;
            }
            continue;
        }

        // MONSTER_N_COORDS
        int x, y;
        if (sscanf(linea, "MONSTER_%d_COORDS %d %d", &monster_id, &x, &y) == 3)
        {
            if (monster_id > 0 && monster_id <= MAX_MONSTRUOS)
            {
                monstruo_x[monster_id - 1] = x;
                monstruo_y[monster_id - 1] = y;
                monstruo_count_data[monster_id - 1]++;
            }
            continue;
        }
    }
    fclose(file);

    // Se construye las estructuras de los monstruos
    for (int i = 0; i < monstruos_posibles; i++)
    {
        if (monstruo_count_data[i] == 5)
        {
            Monstruo *m = &config->monstruos[config->cont_monstruos];
            m->id = i + 1;
            m->hp = monstruo_hp[i];
            m->attack_damage = monstruo_atack[i];
            m->vision_range = monstruo_vision[i];
            m->attack_range = monstruo_range[i];
            m->x = monstruo_x[i];
            m->y = monstruo_y[i];
            m->alertado = 0; // default
            m->vivo = 1;     // default

            printf("Monstruo %d: pos=(%d,%d) hp=%d atq=%d vis=%d ran=%d\n", m->id, m->x, m->y, m->hp, m->attack_damage, m->vision_range, m->attack_range);

            config->cont_monstruos++;
        }
        else
        {
            fprintf(stderr, "Advertencia: Monster %d tiene datos incompletos\n", i + 1);
        }
    }

    /*
    También se inicializan los demás valores adicionales del heroe
    path_index_atual, vivo y en_combate con valores por default 
    */
    config->heroe.path_index_actual = 0;
    config->heroe.vivo = 1;
    config->heroe.en_combate = 0;

    printf("\nConfiguracion Cargada...\n");
    printf("Monstruos totales: %d\n", config->cont_monstruos);

    return 0;
}