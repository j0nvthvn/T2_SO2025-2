#ifndef SIMULATION_H
#define SIMULATION_H
#include "config.h"

// Funcion de la distancia Manhattan
int distancia_manhattan(int x1, int y1, int x2, int y2);

// Funcion del movimiento del monstruo
void mover_monstruo_hacia_heroe(Monstruo *monstruo, Heroe *heroe);

// Funcion que alerta a monstruo/s cuando el heroe entra en su rango de vision
int alertar_monstruos(Monstruo* monstruo, Heroe *heroe);

#endif