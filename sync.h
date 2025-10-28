#ifndef SYNC_H
#define SYNC_H

#include <pthread.h>
#include <semaphore.h>

// Funcion para inicializar la sincronizacion de hilos
void inicializar_sync();

// Funcion para destruir los objetos mutex y semaforos
void destruir_sync();

#endif