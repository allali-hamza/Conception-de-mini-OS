#ifndef GESTION_PROCESSUS_H
#define GESTION_PROCESSUS_H

#include "stdio.h"
#include "stdarg.h"
#include "types.h"
#include "cpu.h"
#include "string.h"
#include "inttypes.h"

// definition de la structure processus

#define TAILLE_MAX 50
#define TAILLE_PILE 512
#define TAILLE_TABLEAU_PROC 8
#define ESP 1


extern uint32_t nombre_processus;

typedef enum etats_proc {
    ELU,
    ACTIVABLE,
    ENDORMI,
    ZOMBIE,
}etats_proc;

typedef struct processus {
    int pid;
    char nom[TAILLE_MAX];
    etats_proc etat;
    int sauvgarde_proc_tab[5];
    int pile_tab[TAILLE_PILE];
    struct processus *suiv;
    uint32_t heure_reveil;
} processus;


extern processus *table_processus[TAILLE_TABLEAU_PROC]; 
extern processus *actif;


void idle(void);

void proc1(void);

void initialiser_processus(void);

extern void ctx_sw(int *ancien_contexte, int *nouveau_contexte);

int32_t mon_pid(void);

char *mon_nom(void);

void ordonnance(void);

int32_t cree_processus(void(*code)(void), char *nom);


void proc_generique(void);

void proc_init(void);

processus* extraire_tete(void);
void inserer_en_queue(processus* p);

void dors(uint32_t nbr_secs);
void reveil(processus* p);
uint32_t nbr_secondes(void);

void fin_processus(void);
void affiche_etats(void);

#endif