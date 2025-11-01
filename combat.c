#include "combat.h"
#include "sync.h"
#include <stdio.h>
#include <unistd.h>

// Simula un combate por turnos entre héroe y monstruo
void combate(Heroe *heroe, Monstruo *monstruo)
{
    pthread_mutex_lock(&mutex_combate);

    printf("\n[COMBATE] Héroe %d vs Monstruo %d en (%d,%d)\n", 
           heroe->id, monstruo->id, monstruo->x, monstruo->y);
    printf("          Héroe HP:%d  |  Monstruo HP:%d\n", heroe->hp, monstruo->hp);
    fflush(stdout);

    int turno = 0;
    while (heroe->vivo && monstruo->vivo)
    {
        if (turno == 0)
        {
            // Ataca el héroe
            int hp_antes = monstruo->hp;
            monstruo->hp -= heroe->attack_damage;
            heroe->stats.danio_total_causado += heroe->attack_damage;
            
            printf("   Héroe %d ataca → Monstruo %d [%d→%d HP]\n", 
                   heroe->id, monstruo->id, hp_antes, monstruo->hp);
            fflush(stdout);

            if (monstruo->hp <= 0)
            {
                monstruo->vivo = 0;
                heroe->stats.monstruos_eliminados++;
                printf("   Héroe %d ELIMINÓ a Monstruo %d\n", heroe->id, monstruo->id);
                fflush(stdout);
                break;
            }
        }
        else
        {
            // Ataca el monstruo
            int hp_antes = heroe->hp;
            heroe->hp -= monstruo->attack_damage;
            heroe->stats.danio_total_recibido += monstruo->attack_damage;
            
            printf("   Monstruo %d ataca → Héroe %d [%d→%d HP]\n",
                   monstruo->id, heroe->id, hp_antes, heroe->hp);
            fflush(stdout);

            if (heroe->hp <= 0)
            {
                heroe->vivo = 0;
                printf("   Monstruo %d DERROTÓ a Héroe %d\n", monstruo->id, heroe->id);
                fflush(stdout);
                break;
            }
        }

        turno = 1 - turno;
        usleep(300000);
    }

    printf("[COMBATE] Finalizado\n\n");
    fflush(stdout);

    pthread_mutex_unlock(&mutex_combate);
}