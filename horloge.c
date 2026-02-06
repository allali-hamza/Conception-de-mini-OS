#include "horloge.h"
#include "gestion_processus.h"

#define QUARTZ    0x1234DD
#define CLOCKFREQ 50

uint32_t compteur_de_temps = 0;

uint32_t *table_vecteur_interruption = (uint32_t *)0x1000;

void ecrit_temps(const char* s, int len) {
    uint32_t old_ligne = ligne_courante;
    uint32_t old_colonne = colonne_courante;

    ligne_courante = 0;
    colonne_courante = (uint32_t)NOMBRE_COLONNES - len;

    place_curseur(ligne_courante, colonne_courante);
    console_putbytes(s, len);

    ligne_courante = old_ligne;
    colonne_courante = old_colonne;

    place_curseur(ligne_courante, colonne_courante);
}


void tic_PIT(void) {
    outb(0x20, 0x20);   
    
    compteur_de_temps++;
    ordonnance();
    /*
    uint32_t nombre_de_seconds =      
    uint32_t heures = nombre_de_seconds / 3600;
    uint32_t minutes = (nombre_de_seconds % 3600) / 60;
    uint32_t secodes = (nombre_de_seconds) % 60;
    char buffer[50];
    int len = sprintf(buffer, "%02d : %02d : %02d", heures, minutes, secodes);
    ecrit_temps(buffer, len);
    */
}


void init_traitant_IT(uint32_t num_IT, void (*traitant)(void)) {
    uint32_t handler_adress = (uint32_t)traitant;
    uint32_t premier_mot = ((uint32_t)KERNEL_CS << 16) | (handler_adress & 0x0000FFFF);
    uint32_t deuxieme_mot = (handler_adress & 0xFFFF0000) | (uint32_t)(0x8E00);
    table_vecteur_interruption[num_IT * 2] = premier_mot;
    table_vecteur_interruption[num_IT * 2 + 1] = deuxieme_mot;
}


void masque_IRQ(uint32_t num_IRQ, bool masque) {
    // false si on veut demasquer
    uint8_t mask_value = inb(0x21);
    if (masque) {
        mask_value = mask_value | (1 << num_IRQ);
    } else {
        mask_value = mask_value & ~(1 << num_IRQ);
    }
    outb(mask_value, 0x21);
}


void init_horloge(void) {
    outb(0x34, 0x43);
    uint16_t valeur_reglage = (uint16_t)(QUARTZ / CLOCKFREQ);
    uint8_t low = (uint8_t)valeur_reglage;
    uint8_t high = (uint8_t)(valeur_reglage >> 8);
    outb(low, 0x40);
    outb(high, 0x40);
}
