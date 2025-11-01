#ifndef SIMULATION_H
#define SIMULATION_H
#include "config.h"

int distancia_manhattan(int x1, int y1, int x2, int y2);

void mover_monstruo_hacia_heroe(Monstruo *monstruo, Heroe *heroe);

int alertar_monstruos(Monstruo* monstruo, Heroe *heroe);

#endif