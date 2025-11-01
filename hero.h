#ifndef HERO_H
#define HERO_H

#include <pthread.h>
#include "config.h"

void *heroe_thread(void *arg);

int mounstros_en_rango(Heroe *heroe, Monstruo *monstruos, int cantidad);

void mover_heroe(Heroe *heroe);

#endif