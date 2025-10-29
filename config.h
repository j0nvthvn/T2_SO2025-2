#ifndef CONFIG_H
#define CONFIG_H

#include <pthread.h>
#include <semaphore.h>

#define MAX_MONSTRUOS 100
#define MAX_LENGTH_RUTA 1000
#define MAX_LENGTH_LINEA 1024

// Estructura para punto (x,y)
typedef struct {
    int x;
    int y;
} Punto;

// Estructura para mounstro
typedef struct {
    int id;
    int x;
    int y;
    int hp;
    int attack_damage;
    int vision_range;
    int attack_range;
    int alertado;
    int vivo;
} Monstruo;

// Estructura para heroe
typedef struct {
    int x;
    int y;
    int hp;
    int attack_damage;
    int attack_range;
    Punto* path;
    int path_length;
    int path_index_actual;
    int vivo;
    int en_combate;
} Heroe;

// Estructura para grid
typedef struct {
    int width;  // <- ancho
    int height; // <- alto
} Grid;

typedef struct {
    Grid grid;
    Heroe heroe;
    Monstruo monstruos[MAX_MONSTRUOS];
    int cont_monstruos;
} Configuracion;

// variables extern, osea que su definicion esta en otro archivo (https://stackoverflow.com/questions/1433204/how-do-i-use-extern-to-share-variables-between-source-files)

// variables para sincronizacion (globales)
extern pthread_mutex_t mutex_grid;
extern pthread_mutex_t mutex_combate;
extern sem_t turno_combate;
extern sem_t monstruos_activos;

extern volatile int simulacion_ejecutandose;

// variables para threads (globales)
extern Heroe* heroe_global;
extern Monstruo* monstruos_globales;
extern int cant_monstruos_global;

#endif