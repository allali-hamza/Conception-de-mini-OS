#pragma once
#include <inttypes.h>
#include <stdbool.h>

/* Fréquence du quartz du PIT (≈ 1.193182 MHz) */
#define QUARTZ     0x1234DDu

/* Fréquence des interruptions (ticks/seconde) */
#define CLOCKFREQ  50u   // 50 Hz → 20 ms entre interruptions

/* Affiche une chaîne (HH:MM:SS) en haut à droite de l’écran */
void ecrit_temps(const char *s, int len);

/* Traitant C appelé par le stub assembleur (traitant_IT_32) */
void tic_PIT(void);

/* Configure la fréquence du PIT */
void regle_freq(void);

/* (Dé)masque une IRQ du PIC maître (0..7)
   - masque = true  → masque l’IRQ
   - masque = false → démasque l’IRQ */
void masque_IRQ(uint32_t num_IRQ, bool masque);
