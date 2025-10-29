#ifndef MONSTER_H
#define MONSTER_H

#include <pthread.h>
#include "config.h"

// Funcion principal
void* monstruo_thread(void* arg);

int heroe_en_vision(Monstruo* monstruo, Heroe* heroe);

int heroe_en_rango_atq(Monstruo* monstruo, Heroe* heroe);

#endif