#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Parsea una cadena de puntos (x,y) y retorna un arreglo de puntos
int parsear_hero_path(const char *path_str, Punto **path, int *path_length)
{

    // Contar cuántos puntos hay (cada '(' indica un punto)
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

    // Asignar memoria para el arreglo de puntos
    *path = (Punto *)malloc(count * sizeof(Punto));
    if (*path == NULL)
    {
        fprintf(stderr, "Error: no se pudo asignar memoria.\n");
        return -1;
    }

    // Parsear cada punto
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
            while (*p && *p != ')')
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

// Parsea el archivo de configuración y carga héroes, monstruos y grid
int parsear_archivo(const char *archivo, Configuracion *config)
{
    FILE *file = fopen(archivo, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error: No se pudo abrir el archivo %s\n", archivo);
        return -1;
    }

    char linea[MAX_LENGTH_LINEA];
    config->cant_monstruos = 0;
    config->cant_heroes = 0;
    int monstruos_posibles = 0;

    // Arrays temporales para almacenar datos de monstruos
    int monstruo_hp[MAX_MONSTRUOS] = {0};
    int monstruo_atack[MAX_MONSTRUOS] = {0};
    int monstruo_vision[MAX_MONSTRUOS] = {0};
    int monstruo_range[MAX_MONSTRUOS] = {0};
    int monstruo_x[MAX_MONSTRUOS] = {0};
    int monstruo_y[MAX_MONSTRUOS] = {0};
    int monstruo_count_data[MAX_MONSTRUOS] = {0};

    // Arrays temporales para almacenar datos de héroes
    int hero_hp[MAX_HEROES] = {0};
    int hero_damage[MAX_HEROES] = {0};
    int hero_range[MAX_HEROES] = {0};
    int hero_x[MAX_HEROES] = {0};
    int hero_y[MAX_HEROES] = {0};
    char hero_path_str[MAX_HEROES][MAX_LENGTH_LINEA * 10] = {0};
    int hero_count_data[MAX_HEROES] = {0};
    int max_hero_id = 0;
    int formato_nuevo = 0;

    int leyendo_path_de_hero = -1;

    while (fgets(linea, sizeof(linea), file))
    {
        // Ignorar líneas vacías y comentarios
        if (linea[0] == '\n' || linea[0] == '#')
            continue;

        // GRID_SIZE
        if (sscanf(linea, "GRID_SIZE %d %d", &config->grid.width, &config->grid.height) == 2)
        {
            printf("Grid: %d x %d\n", config->grid.width, config->grid.height);
            continue;
        }

        // Detectar formato HERO_X_* (múltiples héroes)
        int hero_id, value, x, y;

        // HERO_X_HP
        if (sscanf(linea, "HERO_%d_HP %d", &hero_id, &value) == 2)
        {
            formato_nuevo = 1;
            if (hero_id > 0 && hero_id <= MAX_HEROES)
            {
                hero_hp[hero_id - 1] = value;
                hero_count_data[hero_id - 1]++;
                if (hero_id > max_hero_id)
                    max_hero_id = hero_id;
            }
            continue;
        }

        // HERO_X_ATTACK_DAMAGE
        if (sscanf(linea, "HERO_%d_ATTACK_DAMAGE %d", &hero_id, &value) == 2)
        {
            formato_nuevo = 1;
            if (hero_id > 0 && hero_id <= MAX_HEROES)
            {
                hero_damage[hero_id - 1] = value;
                hero_count_data[hero_id - 1]++;
                if (hero_id > max_hero_id)
                    max_hero_id = hero_id;
            }
            continue;
        }

        // HERO_X_ATTACK_RANGE
        if (sscanf(linea, "HERO_%d_ATTACK_RANGE %d", &hero_id, &value) == 2)
        {
            formato_nuevo = 1;
            if (hero_id > 0 && hero_id <= MAX_HEROES)
            {
                hero_range[hero_id - 1] = value;
                hero_count_data[hero_id - 1]++;
                if (hero_id > max_hero_id)
                    max_hero_id = hero_id;
            }
            continue;
        }

        // HERO_X_START
        if (sscanf(linea, "HERO_%d_START %d %d", &hero_id, &x, &y) == 3)
        {
            formato_nuevo = 1;
            if (hero_id > 0 && hero_id <= MAX_HEROES)
            {
                hero_x[hero_id - 1] = x;
                hero_y[hero_id - 1] = y;
                hero_count_data[hero_id - 1]++;
                if (hero_id > max_hero_id)
                    max_hero_id = hero_id;
            }
            continue;
        }

        // HERO_x_PATH
        char pattern[50];
        for (int i = 1; i <= MAX_HEROES; i++)
        {
            sprintf(pattern, "HERO_%d_PATH", i);
            if (strstr(linea, pattern) != NULL)
            {
                formato_nuevo = 1;
                leyendo_path_de_hero = i - 1;
                char *path_start = strchr(linea, '(');
                if (path_start != NULL)
                {
                    strcat(hero_path_str[leyendo_path_de_hero], path_start);
                }
                hero_count_data[i - 1]++;
                if (i > max_hero_id)
                    max_hero_id = i;
                break;
            }
        }
        if (leyendo_path_de_hero >= 0 && linea[0] == '(')
        {
            strcat(hero_path_str[leyendo_path_de_hero], linea);
            continue;
        }

        // Formato HERO_* (un solo héroe)
        if (!formato_nuevo)
        {
            if (sscanf(linea, "HERO_HP %d", &hero_hp[0]) == 1)
            {
                hero_count_data[0]++;
                max_hero_id = 1;
                continue;
            }
            if (sscanf(linea, "HERO_ATTACK_DAMAGE %d", &hero_damage[0]) == 1)
            {
                hero_count_data[0]++;
                continue;
            }
            if (sscanf(linea, "HERO_ATTACK_RANGE %d", &hero_range[0]) == 1)
            {
                hero_count_data[0]++;
                continue;
            }
            if (sscanf(linea, "HERO_START %d %d", &hero_x[0], &hero_y[0]) == 2)
            {
                hero_count_data[0]++;
                continue;
            }
            if (strncmp(linea, "HERO_PATH ", 10) == 0)
            {
                leyendo_path_de_hero = 0;
                char *path_start = strchr(linea, '(');
                if (path_start != NULL)
                {
                    strcat(hero_path_str[0], path_start);
                }
                hero_count_data[0]++;
                max_hero_id = 1;
                continue;
            }
        }

        // MONSTER_X_HP
        int monster_id;
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

        // MONSTER_X_COORDS
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

    // Construir arreglo de héroes desde los datos parseados
    config->cant_heroes = max_hero_id;
    printf("\nHÉROES DETECTADOS: %d\n", config->cant_heroes);

    for (int i = 0; i < config->cant_heroes; i++)
    {
        if (hero_count_data[i] >= 5)
        {
            Heroe *h = &config->heroes[i];
            h->id = i + 1;
            h->hp = hero_hp[i];
            h->hp_inicial = hero_hp[i];
            h->attack_damage = hero_damage[i];
            h->attack_range = hero_range[i];
            h->x = hero_x[i];
            h->y = hero_y[i];

            // Parsear path del héroe
            if (strlen(hero_path_str[i]) > 0)
            {
                parsear_hero_path(hero_path_str[i], &h->path, &h->path_length);
            }
            else
            {
                h->path = NULL;
                h->path_length = 0;
            }

            h->path_index_actual = 0;
            h->vivo = 1;
            h->en_combate = 0;
            h->stats.monstruos_eliminados = 0;
            h->stats.danio_total_causado = 0;
            h->stats.danio_total_recibido = 0;

            printf("Héroe %d: pos=(%d,%d) hp=%d atq=%d rango=%d path=%d puntos\n",
                   i + 1, h->x, h->y, h->hp, h->attack_damage, h->attack_range, h->path_length);
        }
        else
        {
            fprintf(stderr, "Advertencia: Héroe %d tiene datos incompletos (%d/5)\n",
                    i + 1, hero_count_data[i]);
        }
    }

    // Construir arreglo de monstruos desde los datos parseados
    for (int i = 0; i < MAX_MONSTRUOS; i++)
    {
        if (monstruo_count_data[i] > 0)
        {
            monstruos_posibles = i + 1;
        }
    }
    
    printf("\nMONSTRUOS DETECTADOS: %d\n", monstruos_posibles);
    for (int i = 0; i < monstruos_posibles; i++)
    {
        if (monstruo_count_data[i] == 5)
        {
            Monstruo *m = &config->monstruos[config->cant_monstruos];
            m->id = i + 1;
            m->hp = monstruo_hp[i];
            m->hp_inicial = monstruo_hp[i];
            m->attack_damage = monstruo_atack[i];
            m->vision_range = monstruo_vision[i];
            m->attack_range = monstruo_range[i];
            m->x = monstruo_x[i];
            m->y = monstruo_y[i];
            m->alertado = 0;
            m->vivo = 1;

            printf("Monstruo %d: pos=(%d,%d) hp=%d atq=%d vis=%d ran=%d\n",
                   m->id, m->x, m->y, m->hp, m->attack_damage, m->vision_range, m->attack_range);

            config->cant_monstruos++;
        }
        else if (monstruo_count_data[i] > 0)
        {
            fprintf(stderr, "Advertencia: Monstruo %d tiene datos incompletos (%d/5)\n",
                    i + 1, monstruo_count_data[i]);
        }
    }
    return 0;
}