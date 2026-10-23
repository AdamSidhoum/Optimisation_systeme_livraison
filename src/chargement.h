#ifndef CHARGEMENT_H
#define CHARGEMENT_H

#include "colis.h"

typedef struct Chargement {
    int *indices;
    int nb;
    int capacite;
    int alpha;
    long long poids_total;
    long long valeur_totale;
    long long score_total;
} Chargement;

#define ALPHA_MAX 1000
#define MEMOIRE_MAX_DP (512LL * 1024 * 1024)


long long chargement_score(const Colis *c, int alpha);


int chargement_optimal(const Colis *tab, int n, int capacite, int alpha, Chargement *res);

int chargement_glouton(const Colis *tab, int n, int capacite, Chargement *res);

int chargement_force_brute(const Colis *tab, int n, int capacite, int alpha, Chargement *res);

void chargement_afficher(const Chargement *ch, const Colis *tab);
void chargement_vider(Chargement *ch);

#endif
