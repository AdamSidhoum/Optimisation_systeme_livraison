#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chargement.h"
#include "tri.h"

long long chargement_score(const Colis *c, int alpha) {
    return (long long)c->valeur * (100LL + (long long)alpha * (c->priorite - 1));
}

void chargement_vider(Chargement *ch) {
    if (ch == NULL) return;
    free(ch->indices);
    memset(ch, 0, sizeof(*ch));
}

static void calculer_totaux(Chargement *res, const Colis *tab) {
    res->poids_total = 0;
    res->valeur_totale = 0;
    res->score_total = 0;
    for (int k = 0; k < res->nb; k++) {
        const Colis *c = &tab[res->indices[k]];
        res->poids_total += c->poids;
        res->valeur_totale += c->valeur;
        res->score_total += chargement_score(c, res->alpha);
    }
}

int chargement_optimal(const Colis *tab, int n, int capacite, int alpha, Chargement *res) {
    memset(res, 0, sizeof(*res));
    res->capacite = capacite;
    res->alpha = alpha;
    if (capacite <= 0 || n <= 0) {
        res->indices = malloc(sizeof(int));
        return res->indices != NULL;
    }

    int *elig = malloc((size_t)n * sizeof(int));
    if (elig == NULL) return 0;
    int m = 0;
    for (int i = 0; i < n; i++) {
        if (tab[i].poids > 0 && tab[i].poids <= capacite) elig[m++] = i;
    }

    size_t largeur = (size_t)capacite + 1;
    size_t nb_bits = (size_t)m * largeur;
    size_t nb_octets = (nb_bits + 7) / 8;
    if ((long long)nb_octets + (long long)largeur * 8 > MEMOIRE_MAX_DP) {
        free(elig);
        return -1;
    }
    long long *dp = calloc(largeur, sizeof(long long));
    unsigned char *pris = calloc(nb_octets > 0 ? nb_octets : 1, 1);
    if (dp == NULL || pris == NULL) {
        free(dp); free(pris); free(elig);
        return 0;
    }


    for (int k = 0; k < m; k++) {
        const Colis *c = &tab[elig[k]];
        int p = c->poids;
        long long s = chargement_score(c, alpha);
        for (int w = capacite; w >= p; w--) {
            if (dp[w - p] + s > dp[w]) {
                dp[w] = dp[w - p] + s;
                size_t bit = (size_t)k * largeur + (size_t)w;
                pris[bit / 8] |= (unsigned char)(1u << (bit % 8));
            }
        }
    }

    res->indices = malloc((size_t)(m > 0 ? m : 1) * sizeof(int));
    if (res->indices == NULL) {
        free(dp); free(pris); free(elig);
        return 0;
    }
    int w = capacite;
    for (int k = m - 1; k >= 0; k--) {
        size_t bit = (size_t)k * largeur + (size_t)w;
        if (pris[bit / 8] & (1u << (bit % 8))) {
            res->indices[res->nb++] = elig[k];
            w -= tab[elig[k]].poids;
        }
    }
    calculer_totaux(res, tab);

    free(dp);
    free(pris);
    free(elig);
    return 1;
}


static int cmp_ratio(const Colis *a, const Colis *b) {
    long long g = (long long)a->valeur * b->poids;
    long long d = (long long)b->valeur * a->poids;
    return (g > d) - (g < d);
}

int chargement_glouton(const Colis *tab, int n, int capacite, Chargement *res) {
    memset(res, 0, sizeof(*res));
    res->capacite = capacite;
    int *ind = tri_indices_init(n);
    res->indices = malloc((size_t)(n > 0 ? n : 1) * sizeof(int));
    if (ind == NULL || res->indices == NULL || !tri_fusion(tab, ind, n, cmp_ratio, -1)) {
        free(ind);
        chargement_vider(res);
        return 0;
    }
    int reste = capacite;
    for (int k = 0; k < n; k++) {
        const Colis *c = &tab[ind[k]];
        if (c->poids > 0 && c->poids <= reste) {
            res->indices[res->nb++] = ind[k];
            reste -= c->poids;
        }
    }
    free(ind);
    calculer_totaux(res, tab);
    return 1;
}

int chargement_force_brute(const Colis *tab, int n, int capacite, int alpha, Chargement *res) {
    memset(res, 0, sizeof(*res));
    res->capacite = capacite;
    res->alpha = alpha;
    if (n > 25) return -1;
    res->indices = malloc((size_t)(n > 0 ? n : 1) * sizeof(int));
    if (res->indices == NULL) return 0;

    long long meilleur = -1;
    unsigned long meilleur_masque = 0;
    for (unsigned long masque = 0; masque < (1UL << n); masque++) {
        long long poids = 0, score = 0;
        for (int i = 0; i < n; i++) {
            if (masque & (1UL << i)) {
                poids += tab[i].poids;
                score += chargement_score(&tab[i], alpha);
            }
        }
        if (poids <= capacite && score > meilleur) {
            meilleur = score;
            meilleur_masque = masque;
        }
    }
    for (int i = 0; i < n; i++) {
        if (meilleur_masque & (1UL << i)) res->indices[res->nb++] = i;
    }
    calculer_totaux(res, tab);
    return 1;
}

void chargement_afficher(const Chargement *ch, const Colis *tab) {
    if (ch == NULL || ch->indices == NULL) {
        printf("Aucun chargement prepare.\n");
        return;
    }
    printf("Capacite du camion : %d kg | alpha (priorite) : %d\n", ch->capacite, ch->alpha);
    if (ch->nb == 0) {
        printf("Aucun colis ne peut etre charge.\n");
        return;
    }
    colis_afficher_entete();
    for (int k = ch->nb - 1; k >= 0; k--) {
        colis_afficher(&tab[ch->indices[k]]);
    }
    printf("----------------------------------------------\n");
    printf("%d colis | poids total : %lld / %d kg | valeur totale : %lld\n",
           ch->nb, ch->poids_total, ch->capacite, ch->valeur_totale);
}
