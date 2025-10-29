#ifndef COMBAT_H
#define COMBAT_H

#include "config.h"

void iniciar_combate(Heroe* heroe, Monstruo* monstruo);

void heroe_ataca(Heroe* heroe, Monstruo* monstruo);

void monstruo_ataca(Heroe* heroe, Monstruo* monstruo);

#endif