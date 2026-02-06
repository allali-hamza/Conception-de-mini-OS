#include "ecran.h"
#include "cpu.h"
#include <string.h>

/* État interne du module (non exposé) */
static uint32_t curseur_lig = 0;
static uint32_t curseur_col = 0;

/* Pointeur VRAM en volatile */
volatile uint16_t* ptr_mem(uint32_t lig, uint32_t col) {
    return (volatile uint16_t*)(VIDMEM_BASE + 2u * (lig * NB_COLS + col));
}

/* Écriture d’un caractère (blink forcé à 0, masques de sécurité) */
void ecrit_car(uint32_t lig, uint32_t col, char c,
               uint16_t couleur_text, uint16_t couleur_fond) {
    if (lig >= NB_LIGNES || col >= NB_COLS) return;

    uint8_t code = (uint8_t)c;
    uint16_t fg = (uint16_t)(couleur_text & 0x0Fu); /* 4 bits */
    uint16_t bg = (uint16_t)(couleur_fond & 0x07u); /* 3 bits */

    uint16_t mot = (uint16_t)((bg << 12) | (fg << 8) | code);
    mot &= 0x7FFFu; /* bit 15 (blink) = 0 */

    *ptr_mem(lig, col) = mot;
}

void efface_ecran(void) {
    for (uint32_t lig = 0; lig < NB_LIGNES; ++lig) {
        for (uint32_t col = 0; col < NB_COLS; ++col) {
            ecrit_car(lig, col, ' ', FG_DEF, BG_DEF);
        }
    }
    curseur_lig = 0;
    curseur_col = 0;
    place_curseur(curseur_lig, curseur_col);
}

void place_curseur(uint32_t lig, uint32_t col) {
    if (lig >= NB_LIGNES) lig = NB_LIGNES - 1;
    if (col >= NB_COLS)   col = NB_COLS - 1;

    curseur_lig = lig;
    curseur_col = col;

    uint16_t pos = (uint16_t)(curseur_col + curseur_lig * NB_COLS);
    outb(0x0F, CRT_CMD);  outb((uint8_t)(pos & 0xFF), CRT_DATA);
    outb(0x0E, CRT_CMD);  outb((uint8_t)(pos >> 8),   CRT_DATA);
}

void defilement(void) {
    /* Décale les lignes 1..24 vers 0..23 */
    volatile uint16_t* dst = ptr_mem(0, 0);
    volatile uint16_t* src = ptr_mem(1, 0);
    size_t n = (size_t)NB_COLS * (NB_LIGNES - 1);

    /* cast temporaire pour memmove */
    memmove((void*)dst, (const void*)src, n * sizeof(uint16_t));

    /* Efface la dernière ligne */
    for (uint32_t col = 0; col < NB_COLS; ++col) {
        ecrit_car(NB_LIGNES - 1, col, ' ', FG_DEF, BG_DEF);
    }

    curseur_lig = NB_LIGNES - 1;
    /* curseur_col inchangé */
    place_curseur(curseur_lig, curseur_col);
}

void traite_car(char c) {
    uint8_t code = (uint8_t)c;

    if (code == 8) {                   /* BS */
        if (curseur_col > 0) {
            curseur_col--;
            place_curseur(curseur_lig, curseur_col);
        }
        return;
    } else if (code == 9) {            /* HT */
        uint32_t next = ((curseur_col / 8u) + 1u) * 8u;
        if (next > NB_COLS - 1u) next = NB_COLS - 1u; /* stops ...,72,79 */
        curseur_col = next;
        place_curseur(curseur_lig, curseur_col);
        return;
    } else if (code == 10) {           /* LF */
        curseur_col = 0;
        if (curseur_lig < NB_LIGNES - 1) {
            curseur_lig++;
        } else {
            defilement();
        }
        place_curseur(curseur_lig, curseur_col);
        return;
    } else if (code == 12) {           /* FF */
        efface_ecran();
        return;
    } else if (code == 13) {           /* CR */
        curseur_col = 0;
        place_curseur(curseur_lig, curseur_col);
        return;
    } else if (code >= 32 && code <= 126) { /* imprimable */
        ecrit_car(curseur_lig, curseur_col, c, FG_DEF, BG_DEF);
        if (curseur_col == NB_COLS - 1) {
            curseur_col = 0;
            if (curseur_lig < NB_LIGNES - 1) {
                curseur_lig++;
            } else {
                defilement();
            }
        } else {
            curseur_col++;
        }
        place_curseur(curseur_lig, curseur_col);
        return;
    } else {
        /* autres contrôles : ignore */
        return;
    }
}

void console_putbytes(const char *s, int len) {
    if (!s || len <= 0) return;
    for (int i = 0; i < len; ++i) {
        traite_car(s[i]);
    }
}
