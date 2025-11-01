#ifndef CONFIG_H
#define CONFIG_H

#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>

#define MAX_MONSTRUOS 100
#define MAX_HEROES 10
#define MAX_LENGTH_RUTA 1000
#define MAX_LENGTH_LINEA 1024

typedef struct {
    int x;
    int y;
} Punto;

typedef struct {
    int monstruos_eliminados;
    int danio_total_causado;
    int danio_total_recibido;
} EstadisticasHeroe;

typedef struct
{
    int id;
    int x;
    int y;
    int hp;
    int hp_inicial;
    int attack_damage;
    int attack_range;
    int vision_range;
    int vivo;
    int alertado;
} Monstruo;

typedef struct
{
    int id;
    int x;
    int y;
    int hp;
    int hp_inicial;
    int attack_damage;
    int attack_range;
    int path_index_actual;
    int path_length;
    Punto *path;
    int vivo;
    int en_combate;
    EstadisticasHeroe stats;
} Heroe;

typedef struct {
    int width;
    int height;
} Grid;

typedef struct {
    Grid grid;
    Heroe heroes[MAX_HEROES];
    int cant_heroes;
    Monstruo monstruos[MAX_MONSTRUOS];
    int cant_monstruos;
} Configuracion;

// Variables globales para sincronización (declaradas en sync.c)
extern pthread_mutex_t mutex_grid;
extern pthread_mutex_t mutex_combate;
extern sem_t turno_combate;
extern sem_t monstruos_activos;

extern atomic_int simulacion_ejecutandose;

// Variables globales compartidas entre threads (declaradas en sync.c)
extern Heroe* heroes_globales;
extern int cant_heroes_global;
extern Monstruo* monstruos_globales;
extern int cant_monstruos_global;

#endif