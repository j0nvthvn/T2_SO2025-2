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
    pthread_mutex_t lock;
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
    pthread_mutex_t lock;
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

#endif