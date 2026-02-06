#include "ecran.h"

uint32_t ligne_courante = 0;
uint32_t colonne_courante = 0;
int couleur_du_fond = 0;     
int couleur_du_texte = 15;   

uint16_t *ptr_mem(uint32_t lig, uint32_t col) {
    return (uint16_t *)0xB8000 + (lig * 80 + col);
}

void ecrit_car(uint32_t lig, uint32_t col, char c, int couleur_fond, int couleur_texte, int clignote) {
    uint16_t *case_mem = ptr_mem(lig, col);
    uint16_t caractere;
    caractere = (uint16_t)(clignote << 15) | (uint16_t)(couleur_fond << 12) | (uint16_t)(couleur_texte << 8) | (uint8_t)(c);
    caractere = caractere & ~(1 << 15);
    *case_mem = caractere;

}


void efface_ecran(void) {
    uint32_t lig, col;
    for(lig = 0; lig < NOMBRE_LIGNES; lig ++) {
        for(col = 0; col < NOMBRE_COLONNES; col++) {
            ecrit_car(lig, col, ' ', 0, 15, 0);

        }
    }
}

void place_curseur(uint32_t lig, uint32_t col) {
    uint16_t position = col + lig * 80;
    uint8_t high = (uint8_t)(position >> 8);
    uint8_t low = (uint8_t)position;
    outb(0x0F, 0x3D4);
    outb(low, 0x3D5);
    outb(0x0E, 0x3D4);
    outb(high, 0x3D5);
}


void traite_car(char c) {
    uint8_t code_ascci = (uint8_t)c;
    if (code_ascci > 31 && code_ascci < 127) {
        ecrit_car(ligne_courante, colonne_courante, c,couleur_du_fond, couleur_du_texte, 0);
        colonne_courante++;
        if (colonne_courante >= NOMBRE_COLONNES) {
            colonne_courante = 0;
            ligne_courante++; 
        }
        if (ligne_courante >= NOMBRE_LIGNES) {
            defilement();
            ligne_courante = NOMBRE_LIGNES - 1;
        }
        place_curseur(ligne_courante, colonne_courante);
        
    } else if (code_ascci == 8) {
        if (colonne_courante != 0) {
            colonne_courante--;
            place_curseur(ligne_courante, colonne_courante);
        }
    } else if (code_ascci == 9) {
        // tabulation 
        if (colonne_courante < 72) {
            colonne_courante = ((colonne_courante / 8) + 1) * 8;
            place_curseur(ligne_courante, colonne_courante);
        } else {
            colonne_courante = 79;
            place_curseur(ligne_courante, colonne_courante);

        }

    } else if (code_ascci == 10) {
        if (ligne_courante < 24) {
            ligne_courante ++;
        } else {
            defilement();
            ligne_courante = 24;
        }
        colonne_courante = 0;
        place_curseur(ligne_courante, colonne_courante);
    } else if (code_ascci == 12) {
        efface_ecran();
        ligne_courante = 0;
        colonne_courante = 0;
        place_curseur(ligne_courante, colonne_courante);
    } else if (code_ascci == 13) {
        colonne_courante = 0;
        place_curseur(ligne_courante, colonne_courante);
    }
}


void defilement(void) {
    size_t nb_bytes = (size_t)NOMBRE_COLONNES * (size_t)(NOMBRE_LIGNES - 1) * sizeof(uint16_t);
    uint16_t *case_mem_source = ptr_mem(1, 0);
    uint16_t * case_mem_destination = ptr_mem(0, 0);
    void *source = (void *)case_mem_source;
    void *destination = (void *)case_mem_destination;
    memmove(destination, source, nb_bytes);

    // efacer la derniere colonne
    for (int col = 0; col < NOMBRE_COLONNES; col++) {
        ecrit_car(NOMBRE_LIGNES - 1, col, ' ', 0, 15, 0);
    }
}


void console_putbytes(const char *s, int len) {
    if (!s || len <= 0) return;
    for (int i = 0; i < len; i++) {
        traite_car(s[i]);
    }
}