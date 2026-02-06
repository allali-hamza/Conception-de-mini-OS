#ifndef HORLOGE_H
#define HORLOGE_H


#include "stdio.h"
#include "cpu.h"
#include "inttypes.h"
#include "string.h"
#include "ecran.h"
#include "stdbool.h"
#include "segment.h"

extern uint32_t compteur_de_temps;

void ecrit_temps(const char* s, int len);

void init_traitant_IT(uint32_t num_IT, void (*traitant)(void));

void tic_PIT(void);   // je dois regarder comment utiliser la fonction sprintf

void masque_IRQ(uint32_t num_IRQ, bool masque);

void init_horloge(void);

extern void traitant_IT_32(void);


#endif