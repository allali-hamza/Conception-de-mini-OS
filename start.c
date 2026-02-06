#include <cpu.h>
#include <inttypes.h>
#include "ecran.h"
#include "horloge.h"
#include "gestion_processus.h"

// on peut s'entrainer a utiliser GDB avec ce code de base
// par exemple afficher les valeurs de x, n et res avec la commande display

// une fonction bien connue
uint32_t fact(uint32_t n)
{
    uint32_t res;
    if (n <= 1) {
        res = 1;
    } else {
        res = fact(n - 1) * n;
    }
    return res;
}

void kernel_start(void)
{
    
    efface_ecran();

    init_traitant_IT(32, traitant_IT_32);

    init_horloge();

    masque_IRQ(0, false);

    // sti(); 
    //printf("game over\n");
    //printf("time to go\t");
    //printf("I need to sleep rn");
    initialiser_processus(); // 
    idle();
   

    // on ne doit jamais sortir de kernel_start
    while (1) {
        // cette fonction arrete le processeur
        hlt();
    }
}

