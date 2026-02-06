#pragma once
#include <inttypes.h>

/* Constantes une seule fois ici */
#define NB_LIGNES    25u
#define NB_COLS      80u
#define VIDMEM_BASE  0xB8000u
#define CRT_CMD      0x3D4u
#define CRT_DATA     0x3D5u

/* Couleurs par défaut */
#define FG_DEF  15u  /* blanc */
#define BG_DEF  0u   /* noir */

/* NE PAS déclarer/initialiser de globales ici.
   Si besoin d’exposer la position du curseur, fais des getters. */

/* Prototypes — types cohérents avec l’implémentation */
volatile uint16_t* ptr_mem(uint32_t lig, uint32_t col);

void ecrit_car(uint32_t lig, uint32_t col, char c,
               uint16_t couleur_text, uint16_t couleur_fond);

void efface_ecran(void);
void place_curseur(uint32_t lig, uint32_t col);
void defilement(void);
void traite_car(char c);
void console_putbytes(const char *s, int len);
