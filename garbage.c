/*
void ordonnance(void) {
    // je dois changer cette fonction aussi 
    uint32_t pid = mon_pid();
    if (pid == 0) {
        // alors le processus 0 est actif
        // on doit changer le contexte
        table_processus[0]->etat = ACTIVABLE;
        table_processus[1]->etat = ELU;
        actif = table_processus[1];

        int *courant = table_processus[0]->sauvgarde_proc_tab;
        int *suivant = table_processus[1]->sauvgarde_proc_tab;
        ctx_sw(courant, suivant);

    } else {
        table_processus[1]->etat = ACTIVABLE;
        table_processus[0]->etat = ELU;
        actif = table_processus[0];

        int *courant = table_processus[1]->sauvgarde_proc_tab;
        int *suivant = table_processus[0]->sauvgarde_proc_tab;
        ctx_sw(courant, suivant);

    }
} 


la fonction ordonnance ancienne
void ordonnance(void) {
    int32_t pid = mon_pid();
    int32_t pid_du_prochain = (pid + 1) % nombre_processus;
    table_processus[pid]->etat = ACTIVABLE;
    table_processus[pid_du_prochain]->etat = ELU;
    actif = table_processus[pid_du_prochain];

    int *courant = table_processus[pid]->sauvgarde_proc_tab;
    int *suivant = table_processus[pid_du_prochain]->sauvgarde_proc_tab;
    ctx_sw(courant, suivant);

}


#include "gestion_processus.h"
#include <tinyalloc.h>
#include "horloge.h"

#define NOMBRE_PROCESSUS_A_CREER 7

processus *tete_activables = NULL;
processus *queue_activables = NULL;

processus *tete_endormis = NULL;
processus *queue_endormis = NULL;


processus *table_processus[TAILLE_TABLEAU_PROC]; 
processus *actif = NULL;            // pointeur vers le processus actif

uint32_t nombre_processus = 0;


uint32_t nbr_secondes(void) {
    return compteur_de_temps / 50;
}
void idle(void) {
    for (;;) {
    printf("[%s] pid = %i\n", mon_nom(), mon_pid());
    sti();
    hlt();
    cli();
    }
}


void initialiser_processus(void) {

    table_processus[0] = malloc(sizeof(processus)); 
    // pour le processus 0
    table_processus[0]->pid = 0;
    strcpy(table_processus[0]->nom, "idle");
    table_processus[0]->etat = ELU;

    actif = table_processus[0];
    nombre_processus = 1;
}

int32_t mon_pid(void) {
    return actif->pid;    
}

char *mon_nom(void) {
    return actif->nom;
}


int32_t cree_processus(void(*code)(void), char *nom) {
    if (nombre_processus >= TAILLE_TABLEAU_PROC) {
        // le nombre de processus cree depasse le nombre maximal, donc erreur
        return -1;
    }
    // si non, on cree un processus
    table_processus[nombre_processus] = malloc(sizeof(processus));
    processus *p = table_processus[nombre_processus];
    p->pid = nombre_processus;
    strcpy(p->nom, nom);
    
    p->etat = ACTIVABLE;  // je ne suis pas sur
    p->sauvgarde_proc_tab[ESP] = (int)(&p->pile_tab[TAILLE_PILE - 1]);
    p->pile_tab[TAILLE_PILE - 1] = (int)code;

    // au final on incremente le nombre de precessus
    nombre_processus++;

    return p->pid;
}


void proc_generique(void) {
    for (;;) {
    printf("[%s] pid = %i\n", mon_nom(), mon_pid());
    sti();
    hlt();
    cli();
    }
}


void proc_init(void) {
    for (int i = 1; i <= NOMBRE_PROCESSUS_A_CREER; i++) {
        char nom[20];
        sprintf(nom, "proc%d", i);
        int pid = cree_processus(proc_generique, nom);
        inserer_en_queue(table_processus[pid]);

    }
}


void ordonnance(void) {
    // on doit parcourir la liste des endormis
    processus* tempo = tete_endormis;
    while (tempo != NULL) {
        if (tempo->heure_reveil <= nbr_secondes()) {
            reveil(tempo);
        }
        tempo = tempo->suiv;
    }


    processus *ancien = actif;
    if (ancien->etat == ACTIVABLE) {
        inserer_en_queue(ancien);
    }
    actif = extraire_tete();


    int *courant = ancien->sauvgarde_proc_tab;

    int *suivant = actif->sauvgarde_proc_tab;

    ctx_sw(courant, suivant);


}


processus* extraire_tete(void) {
    if (tete_activables == NULL) {
        return NULL;
    }
    processus* tete = tete_activables;

    tete_activables = tete_activables->suiv;
    if (tete_activables == NULL){
        queue_activables = NULL;
    }
    tete->etat = ELU;
    tete->suiv = NULL;
    return tete;
}


void inserer_en_queue(processus* p) {
    p->etat = ACTIVABLE;
    p->suiv = NULL;

    if (queue_activables == NULL) {
        tete_activables = queue_activables = p;
    } else {
        queue_activables->suiv = p;
        queue_activables = p;
    }
}


void dors(uint32_t nbr_secs) {
    actif->heure_reveil = nbr_secs + nbr_secondes();  // je dois ajouter qlq choses, je pense. (j'ai ajouté secondes())
    actif->etat = ENDORMI;
    queue_endormis->suiv = actif;
    queue_endormis = actif;
}

void reveil(processus* p) {
    // on dois insere p dans la liste des activables
    // je dois parcourir la liste des endormis pour retirer p
    processus* tempo = tete_endormis;
    while (tempo != NULL) {
        if (tempo->suiv == p) {
            tempo->suiv = tempo->suiv->suiv;
        }
        tempo = tempo->suiv;
    }
    inserer_en_queue(p);
}

void idle() {
    for (;;) {
        sti();
        hlt();
        cli();
    }
}

void proc1(void)
{
    for (;;) {
        printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(),
        mon_nom(), mon_pid());
        dors(2);
    }
}

void proc2(void)
{
    for (;;) {
        printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(),
        mon_nom(), mon_pid());
        dors(3);
    }
}

void proc3(void)
{
    for (;;) {
        printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(),
        mon_nom(), mon_pid());
        dors(5);
    }
}


void initialiser_processus(void) {

    table_processus[0] = malloc(sizeof(processus)); 
    // pour le processus 0
    table_processus[0]->pid = 0;
    strcpy(table_processus[0]->nom, "idle");
    table_processus[0]->etat = ELU;

    actif = table_processus[0];
    nombre_processus = 1;

    // initialiser les 3 processus
    cree_processus(proc1, "proc1");
    cree_processus(proc2, "proc2");
    cree_processus(proc3, "proc3");
}



void reveil(processus* p) {
    // Retirer p de la liste des endormis
    if (tete_endormis == NULL) return;
    
    // Si p est en tête
    if (tete_endormis == p) {
        tete_endormis = tete_endormis->suiv;
    } else {
        // Chercher p dans la liste
        processus* tempo = tete_endormis;
        while (tempo != NULL && tempo->suiv != p) {
            tempo = tempo->suiv;
        }
        
        // Si trouvé, retirer
        if (tempo != NULL && tempo->suiv == p) {
            tempo->suiv = p->suiv;
        }
    }
    
    // Ajouter à la file des activables
    p->suiv = NULL;
    inserer_en_queue(p);
}


void dors(uint32_t nbr_secs) {
    uint32_t heure_de_reveil = nbr_secs + nbr_secondes();
    actif->heure_reveil = heure_de_reveil;
    actif->etat = ENDORMI;
    
    // Liste vide
    if (tete_endormis == NULL) {
        tete_endormis = actif;
        actif->suiv = NULL;
    }
    // Insérer en tête
    else if (heure_de_reveil < tete_endormis->heure_reveil) {
        actif->suiv = tete_endormis;
        tete_endormis = actif;
    }
    // Insérer au milieu ou en fin
    else {
        processus* tempo = tete_endormis;
        while (tempo->suiv != NULL && tempo->suiv->heure_reveil <= heure_de_reveil) {
            tempo = tempo->suiv;
        }
        actif->suiv = tempo->suiv;
        tempo->suiv = actif;
    }
    
    ordonnance();
}


void ordonnance(void) {
    // Réveiller les processus dont l'heure est dépassée
    while (tete_endormis != NULL && tete_endormis->heure_reveil <= nbr_secondes()) {
        reveil(tete_endormis);  // Retire toujours la tête
    }
    
    // Ordonnancement normal
    processus *ancien = actif;
    
    // Ne remettre l'ancien que s'il n'est pas endormi
    if (ancien->etat != ENDORMI) {
        inserer_en_queue(ancien);
    }
    
    actif = extraire_tete();
    
    int *courant = ancien->sauvgarde_proc_tab;
    int *suivant = actif->sauvgarde_proc_tab;
    
    ctx_sw(courant, suivant);
}
*/