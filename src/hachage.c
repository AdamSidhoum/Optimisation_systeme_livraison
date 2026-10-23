#include <stdlib.h>
#include "hachage.h"

static int est_premier(int n) {
    if (n < 2) return 0;
    if (n % 2 == 0) return n == 2;
    for (int d = 3; (long long)d * d <= n; d += 2) {
        if (n % d == 0) return 0;
    }
    return 1;
}

static int premier_suivant(int n) {
    while (!est_premier(n)) {
        n++;
    }
    return n;
}


static int hacher(int id, int nb_cases) {
    return (int)((unsigned int)id % (unsigned int)nb_cases);
}

int hachage_init(TableHachage *t, int nb_cases_min) {
    if (t == NULL) return 0;
    if (nb_cases_min < 7) nb_cases_min = 7;
    t->nb_cases = premier_suivant(nb_cases_min);
    t->nb_elements = 0;
    t->cases = calloc((size_t)t->nb_cases, sizeof(Noeud *));
    return t->cases != NULL;
}


static int redimensionner(TableHachage *t) {
    int nouv_nb = premier_suivant(t->nb_cases * 2 + 1);
    Noeud **nouv = calloc((size_t)nouv_nb, sizeof(Noeud *));
    if (nouv == NULL) return 0;

    for (int i = 0; i < t->nb_cases; i++) {
        Noeud *n = t->cases[i];
        while (n != NULL) {
            Noeud *suiv = n->suivant;
            int h = hacher(n->id, nouv_nb);
            n->suivant = nouv[h];
            nouv[h] = n;
            n = suiv;
        }
    }
    free(t->cases);
    t->cases = nouv;
    t->nb_cases = nouv_nb;
    return 1;
}

int hachage_inserer(TableHachage *t, int id, int indice) {
    if (t == NULL) return 0;
    if (hachage_rechercher(t, id) != -1) return 0;

    if ((double)(t->nb_elements + 1) / t->nb_cases > CHARGE_MAX) {
        if (!redimensionner(t)) return 0;
    }
    Noeud *n = malloc(sizeof(Noeud));
    if (n == NULL) return 0;
    int h = hacher(id, t->nb_cases);
    n->id = id;
    n->indice = indice;
    n->suivant = t->cases[h];
    t->cases[h] = n;
    t->nb_elements++;
    return 1;
}

int hachage_rechercher(const TableHachage *t, int id) {
    if (t == NULL || t->cases == NULL) return -1;
    Noeud *n = t->cases[hacher(id, t->nb_cases)];
    while (n != NULL) {
        if (n->id == id) return n->indice;
        n = n->suivant;
    }
    return -1;
}

void hachage_stats(const TableHachage *t, double *charge, int *plus_longue_liste) {
    int max = 0;
    for (int i = 0; i < t->nb_cases; i++) {
        int len = 0;
        for (Noeud *n = t->cases[i]; n != NULL; n = n->suivant) len++;
        if (len > max) max = len;
    }
    if (charge) *charge = (double)t->nb_elements / t->nb_cases;
    if (plus_longue_liste) *plus_longue_liste = max;
}

void hachage_liberer(TableHachage *t) {
    if (t == NULL || t->cases == NULL) return;
    for (int i = 0; i < t->nb_cases; i++) {
        Noeud *n = t->cases[i];
        while (n != NULL) {
            Noeud *suiv = n->suivant;
            free(n);
            n = suiv;
        }
    }
    free(t->cases);
    t->cases = NULL;
    t->nb_cases = 0;
    t->nb_elements = 0;
}
