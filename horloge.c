#include "horloge.h"
#include "cpu.h"     // inb, outb
#include "ecran.h"   // ecrit_car, NB_COLS
#include <stdio.h>   // snprintf
#include <string.h>

/* Compteurs internes */
static uint32_t nb_tics   = 0;   /* Nombre total de tics depuis le démarrage */
static uint32_t secondes  = 0;   /* Temps écoulé en secondes */

/* -------------------------------------------------------------------------- */
/* Affiche une chaîne en haut à droite de l’écran sans déplacer le curseur    */
/* -------------------------------------------------------------------------- */
void ecrit_temps(const char *s, int len)
{
    if (!s || len <= 0) return;

    uint32_t lig = 0;
    uint32_t col = (NB_COLS >= (uint32_t)len) ? (NB_COLS - (uint32_t)len) : 0;

    /* Jaune clair (14) sur fond noir (0) */
    for (int i = 0; i < len; ++i) {
        ecrit_car(lig, col + (uint32_t)i, s[i], 14u, 0u);
    }
}

/* -------------------------------------------------------------------------- */
/* Traitant C appelé par le stub assembleur pour l'interruption PIT (IRQ0)   */
/* -------------------------------------------------------------------------- */
void tic_PIT(void)
{
    outb(0x20, 0x20);  // acquittement PIC

    nb_tics++;
    if (nb_tics % CLOCKFREQ == 0) {
        secondes++;

        uint32_t h = secondes / 3600u;
        uint32_t m = (secondes / 60u) % 60u;
        uint32_t s = secondes % 60u;

        /* Taille 16 = large marge même pour "12345:59:59" */
        char buf[16];

        /* Utilisation prudente : découpage par sprintf partiels */
        int n = snprintf(buf, sizeof(buf), "%u:", h);
        snprintf(buf + n, sizeof(buf) - n, "%02u:%02u", m, s);

        ecrit_temps(buf, strlen(buf));
    }
}


/* -------------------------------------------------------------------------- */
/* Configure la fréquence du PIT pour générer des interruptions à CLOCKFREQ  */
/* -------------------------------------------------------------------------- */
void regle_freq(void)
{
    /* Commande : canal 0, accès low+high, mode 2 (rate generator), binaire */
    outb(0x34, 0x43);

    /* Valeur de chargement sur 16 bits = QUARTZ / CLOCKFREQ */
    uint16_t val = (uint16_t)(QUARTZ / CLOCKFREQ);

    /* Envoie d'abord les 8 bits faibles, puis les 8 bits forts */
    outb((uint8_t)(val & 0xFFu), 0x40);
    outb((uint8_t)(val >> 8),     0x40);
}

/* -------------------------------------------------------------------------- */
/* (Dé)masque l’IRQ num_IRQ (0..7) sur le PIC maître.                        */
/* true  → masque l’IRQ                                                      */
/* false → démasque l’IRQ                                                   */
/* -------------------------------------------------------------------------- */
void masque_IRQ(uint32_t num_IRQ, bool masque)
{
    if (num_IRQ > 7u) return; /* Seulement IRQ0–7 gérées ici */

    uint8_t mask = inb(0x21);  /* Lecture du masque actuel du PIC maître */

    if (masque)
        mask |=  (uint8_t)(1u << num_IRQ);
    else
        mask &= (uint8_t)~(1u << num_IRQ);

    outb(mask, 0x21);
}
