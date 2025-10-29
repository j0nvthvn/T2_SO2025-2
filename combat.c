#include "combat.h"
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>

// Heroe ataca al monstruo
void heroe_ataca(Heroe *heroe, Monstruo *monstruo)
{
    if (!heroe->vivo || !monstruo->vivo)
    {
        return;
    }

    printf("Heroe ataca a monstruo %d con %d de daño\n", monstruo->id, heroe->attack_damage);

    monstruo->hp -= heroe->attack_damage;

    printf("Monstruo %d HP: %d -> %d\n", monstruo->id, monstruo->hp + heroe->attack_damage, monstruo->hp);

    if (monstruo->hp <= 0)
    {
        monstruo->vivo = 0;
        monstruo->hp = 0;
        printf("Monstruo %d ha sido eliminado\n", monstruo->id);
    }
}

// Monstruo ataca al heroe
void monstruo_ataca(Heroe *heroe, Monstruo *monstruo)
{
    if (!heroe->vivo || !monstruo->vivo)
    {
        return;
    }

    printf("Monstruo %d ataca al heroe con %d de daño\n", monstruo->id, monstruo->attack_damage);

    heroe->hp -= monstruo->attack_damage;

    printf("Heroe HP: %d -> %d\n", heroe->hp + monstruo->attack_damage, heroe->hp);

    if (heroe->hp <= 0)
    {
        heroe->vivo = 0;
        heroe->hp = 0;
        printf("Heroe ha sido eliminado\n");
    }
}

// Iniciar combate entre heroe y monstruo
void iniciar_combate(Heroe *heroe, Monstruo *monstruo)
{
    printf("\n COMBATE INICIADO. Heroe vs Monstruo %d\n", monstruo->id);

    int turno = 1;

    while (heroe->vivo && monstruo->vivo)
    {
        printf("Turno %d...\n", turno);

        // Turno del heroe
        pthread_mutex_lock(&mutex_combate);

        if (heroe->vivo && monstruo->vivo)
        {
            heroe_ataca(heroe, monstruo);
        }

        pthread_mutex_unlock(&mutex_combate);

        if (!monstruo->vivo) // si el monstruo murio, termina el combate
        {
            break;
        }

        sleep(1);

        // Turno del monstruo
        pthread_mutex_lock(&mutex_combate);

        if (heroe->vivo && monstruo->vivo)
        {
            monstruo_ataca(heroe, monstruo);
        }

        pthread_mutex_unlock(&mutex_combate);

        if (!heroe->vivo)
        { // si el heroe murio, termina el combate
            break;
        }

        turno++;
        sleep(1);
    }

    printf("\nCombate terminado...\n");
    if (heroe->vivo)
    {
        printf("Heroe victorioso!\n");
    }
    else
    {
        printf("Heroe derrotado.\n");
    }

    if (monstruo->vivo)
    {
        printf("Monstruo victorioso\n");
    }
    else
    {
        printf("Monstruo derrotado\n");
    }
    printf("\n");
}