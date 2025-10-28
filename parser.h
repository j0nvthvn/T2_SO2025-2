#ifndef PARSER_H
#define PARSER_H

#include "config.h"

int parsear_archivo(const char* archivo, Configuracion* config);

int parsear_hero_path(const char* path_str, Punto** path, int* path_length);

#endif