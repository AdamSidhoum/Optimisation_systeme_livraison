#ifndef COLIS_H
#define COLIS_H

#define TAILLE_DEST 32

/* Convention : la priorite va de 1 (faible) a 5 (la plus importante). */
#define PRIO_MIN 1
#define PRIO_MAX 5

typedef struct Colis {
    int id;
    int poids;      /* en kg, entier strictement positif */
    int valeur;
    int priorite;   /* entre PRIO_MIN et PRIO_MAX */
    char destination[TAILLE_DEST];
} Colis;

/* Pointeur de fonction de comparaison : < 0, 0 ou > 0 */
typedef int (*CmpColis)(const Colis *a, const Colis *b);

Colis colis_creer(int id, int poids, int valeur, int priorite, const char *destination);
int   colis_est_valide(const Colis *c);

void  colis_afficher_entete(void);
void  colis_afficher(const Colis *c);

int   colis_cmp_poids(const Colis *a, const Colis *b);
int   colis_cmp_valeur(const Colis *a, const Colis *b);
int   colis_cmp_priorite(const Colis *a, const Colis *b);

#endif
