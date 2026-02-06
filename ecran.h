#ifndef ECRAN_H
#define ECRAN_H

#include "stdio.h"
#include "stdarg.h"
#include "types.h"
#include "cpu.h"
#include "string.h"
#include "inttypes.h"


# define NOMBRE_LIGNES 25
# define NOMBRE_COLONNES 80

extern uint32_t ligne_courante;
extern uint32_t colonne_courante;
extern int couleur_du_fond;     
extern int couleur_du_texte;   



uint16_t *ptr_mem(uint32_t lig, uint32_t col);

void ecrit_car(uint32_t lig, uint32_t col, char c, int couleur_fond, int couleur_texte, int clignote);

void efface_ecran(void);

void place_curseur(uint32_t lig, uint32_t col);

void traite_car(char c);

void defilement(void);

void console_putbytes(const char *s, int len);


#endif