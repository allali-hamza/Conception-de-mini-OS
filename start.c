#include <cpu.h>
#include <inttypes.h>
#include "ecran.h"
#include "horloge.h"

extern void traitant_IT_32(void);                     // label ASM défini dans traitants.S
void init_traitant_IT(uint32_t num_IT, void (*traitant)(void)); // écriture IDT[32]
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
    uint32_t x = fact(5);
    console_putbytes("Hello world", 11);
    console_putbytes("\n", 1);
    console_putbytes("jj", 2);

    init_traitant_IT(32u, traitant_IT_32);

    regle_freq();
    masque_IRQ(0u, false);   // false = démasqué

    sti();
    // quand on saura gerer l'ecran, on pourra afficher x
    (void)x;
    // on ne doit jamais sortir de kernel_start
    while (1) {
        // cette fonction arrete le processeur
        hlt();
    }
}

