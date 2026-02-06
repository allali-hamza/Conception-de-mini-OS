#include "gestion_processus.h"
#include <tinyalloc.h>
#include "horloge.h"

#define NOMBRE_PROCESSUS_A_CREER 7

processus *tete_activables = NULL;
processus *queue_activables = NULL;

processus *tete_endormis = NULL;
processus *queue_endormis = NULL;

processus *tete_zombies = NULL;
processus *queue_zombies = NULL;

processus *table_processus[TAILLE_TABLEAU_PROC]; 
processus *actif = NULL;            // pointeur vers le processus actif
processus *precedent_actif = NULL;

uint32_t nombre_processus = 0;   // j'aurai pas besoin de ceci, je pense


uint32_t nbr_secondes(void) {
    return compteur_de_temps / 50;
}

/*
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
*/

int32_t mon_pid(void) {
    return actif->pid;    
}

char *mon_nom(void) {
    return actif->nom;
}




/*
void proc_generique(void) {
    for (;;) {
    printf("[%s] pid = %i\n", mon_nom(), mon_pid());
    sti();
    hlt();
    cli();
    }
}
*/


/*
void proc_init(void) {
    for (int i = 1; i <= NOMBRE_PROCESSUS_A_CREER; i++) {
        char nom[20];
        sprintf(nom, "proc%d", i);
        int pid = cree_processus(proc_generique, nom);
        inserer_en_queue(table_processus[pid]);

    }
}
*/
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

    p->heure_reveil = 0;
    p->etat = ACTIVABLE;  // je ne suis pas sur
    p->sauvgarde_proc_tab[ESP] = (int)(&p->pile_tab[TAILLE_PILE - 1]);
    p->pile_tab[TAILLE_PILE - 1] = (int)code;
    p->suiv = NULL;
    inserer_en_queue(table_processus[nombre_processus]);  // je peux changer ça apres
    // au final on incremente le nombre de precessus
    nombre_processus++;

    return p->pid;
}
/*
void ordonnance(void) {
    // on doit parcourir la liste des endormis et reveiller certains
    processus* tempo = tete_endormis;
    while (tempo != NULL) {
        if (tempo->heure_reveil <= nbr_secondes()) {
            reveil(tempo);
        }
        tempo = tempo->suiv;
    }


    processus *ancien = actif;
    if (ancien->etat != ENDORMI) {
        inserer_en_queue(ancien);
    }
    actif = extraire_tete();

    int *courant = ancien->sauvgarde_proc_tab;

    int *suivant = actif->sauvgarde_proc_tab;

    ctx_sw(courant, suivant);


}
*/



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



/*
void dors(uint32_t nbr_secs) {
    uint32_t heure_de_reveil = nbr_secs + nbr_secondes();
    actif->heure_reveil = heure_de_reveil;   // je dois ajouter qlq choses, je pense. (j'ai ajouté secondes())
    actif->etat = ENDORMI;

    if (tete_endormis == NULL) {
        tete_endormis = actif;
        queue_endormis = actif;
    } else {
        // on insere actif dans la liste des endormis selon l'heure de reveil
        processus* tempo = tete_endormis;
        while(tempo->suiv != NULL) {
            if (tempo->suiv->heure_reveil > heure_de_reveil) {
                tempo->suiv = actif;
                actif->suiv = tempo->suiv->suiv;
            }
            tempo = tempo->suiv;
        }

    }
    ordonnance();
}
*/

static void inserer_liste_endormis(processus **tete, processus *p, processus **queue, uint32_t heure) {
    // liste vide
    if (*tete == NULL) {
        p->suiv = NULL;
        *tete = p;
        *queue = p;
        return;
    }

    if((*tete)->heure_reveil > heure) {
        // insertion en tete
        p->suiv = *tete;
        *tete = p;
        return;
    }

    processus *tempo = *tete;
    while (tempo->suiv != NULL && tempo->suiv->heure_reveil <= heure) {
        tempo = tempo->suiv;
    }
    p->suiv = tempo->suiv;
    tempo->suiv = p;

    // test si on a insere en queue
    if (p->suiv == NULL) {
        *queue = p;
    }
}


   

void dors(uint32_t nbr_secs) {
    uint32_t heure_de_reveil = nbr_secs + nbr_secondes();
    actif->heure_reveil = heure_de_reveil;
    actif ->etat = ENDORMI;
    inserer_liste_endormis(&tete_endormis, actif, &queue_endormis, heure_de_reveil);
    ordonnance();
     
}





void reveil(processus* p) {

    tete_endormis = tete_endormis->suiv;
    
    // on retire p de la liste des endormis
    
    p->etat = ACTIVABLE;
    // on dois insere p dans la liste des activables
    p->suiv = NULL;
    inserer_en_queue(p);

}
void ordonnance(void) {
    // on doit parcourir la liste des endormis et reveiller certains
    processus* tempo = tete_endormis;
    while (tempo != NULL) {
        if (tempo->heure_reveil <= nbr_secondes()) {
            reveil(tempo);
        }
        tempo = tempo->suiv;
    }


    processus *ancien = actif;
    if (ancien->etat != ENDORMI && ancien->etat != ZOMBIE) {
        inserer_en_queue(ancien);
    }
    
    actif = extraire_tete();

    int *courant = ancien->sauvgarde_proc_tab;

    int *suivant = actif->sauvgarde_proc_tab;

    ctx_sw(courant, suivant);

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
    for (int i = 0;i < 3; i++) {
        printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(),
        mon_nom(), mon_pid());
        affiche_etats();
        dors(5);
    }
    fin_processus();
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
        affiche_etats();
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


void fin_processus(void) {
    actif->etat = ZOMBIE;
    // on doit inserer actif dans la liste des zombies
    // on insere simplement en tete
    actif->suiv = tete_zombies;
    tete_zombies = actif;
    ordonnance();

}


static const char* nom_etat(enum etats_proc e) {
    switch(e) {
        case ELU:        return "ELU";
        case ACTIVABLE:  return "ACTIVABLE";
        case ENDORMI:    return "ENDORMI";
        case ZOMBIE:     return "ZOMBIE";
        default:         return "inconnu";  

    }

}

void affiche_etats(void) {
    for (int i =0; i < nombre_processus; i++) {
        printf("processus: %s  pid: %d etat: %s \n", table_processus[i]->nom, table_processus[i]->pid,
             nom_etat(table_processus[i]->etat));
    }
}



