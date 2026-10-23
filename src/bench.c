#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bench.h"
#include "tri.h"
#include "chargement.h"

static const char *VILLES[] = {
    "Paris", "Lille", "Lyon", "Marseille", "Toulouse",
    "Nantes", "Bordeaux", "Strasbourg", "Nice", "Rennes"
};
#define NB_VILLES 10

int systeme_ajouter(Collection *col, TableHachage *t, Colis c) {
    if (!colis_est_valide(&c)) return -1;
    if (hachage_rechercher(t, c.id) != -1) return 0;
    if (!collection_ajouter(col, c)) return -2;
    if (!hachage_inserer(t, c.id, col->taille - 1)) {
        col->taille--;
        return -2;
    }
    return 1;
}

static int alea(int max) {
    unsigned long r = ((unsigned long)rand() << 15) ^ (unsigned long)rand();
    return (int)(r % (unsigned long)max);
}

int bench_generer(Collection *col, TableHachage *t, int n) {
    int ajoutes = 0;
    while (ajoutes < n) {
        Colis c = colis_creer(1 + alea(1000000000),
                              1 + alea(20),
                              1 + alea(100),
                              PRIO_MIN + alea(PRIO_MAX - PRIO_MIN + 1),
                              VILLES[alea(NB_VILLES)]);
        int r = systeme_ajouter(col, t, c);
        if (r == 1) ajoutes++;
        else if (r == -2) break;
    }
    return ajoutes;
}

int recherche_lineaire(const Collection *col, int id) {
    for (int i = 0; i < col->taille; i++) {
        if (col->tab[i].id == id) return i;
    }
    return -1;
}

static double ms_depuis(clock_t debut) {
    return 1000.0 * (double)(clock() - debut) / CLOCKS_PER_SEC;
}

#define NB_RECHERCHES 10000

void bench_lancer(int avec_insertion_100000) {
    const int tailles[] = {100, 1000, 10000, 100000};
    const int nb_tailles = 4;
    srand(42);

    printf("\n=== 1) Ajout, recherche et tri selon n ===\n");
    printf("(recherche : %d recherches d'ID existants ; temps en ms)\n\n", NB_RECHERCHES);
    printf("%-8s | %-10s | %-14s %-14s | %-14s %-14s\n", "n", "ajout",
           "rech. lineaire", "rech. hachage", "tri insertion", "tri fusion");
    printf("---------------------------------------------------------------------------------\n");

    for (int s = 0; s < nb_tailles; s++) {
        int n = tailles[s];
        Collection col;
        TableHachage t;
        if (!collection_init(&col, 16) || !hachage_init(&t, 16)) {
            printf("Erreur memoire\n");
            return;
        }
        clock_t d = clock();
        bench_generer(&col, &t, n);
        double t_ajout = ms_depuis(d);

        int *cibles = malloc(NB_RECHERCHES * sizeof(int));
        for (int k = 0; k < NB_RECHERCHES; k++) cibles[k] = col.tab[alea(n)].id;

        volatile long long controle = 0;
        d = clock();
        for (int k = 0; k < NB_RECHERCHES; k++) controle += recherche_lineaire(&col, cibles[k]);
        double t_lin = ms_depuis(d);

        d = clock();
        for (int k = 0; k < NB_RECHERCHES; k++) controle += hachage_rechercher(&t, cibles[k]);
        double t_hash = ms_depuis(d);

        int *ind = tri_indices_init(n);
        char buf_ins[32];
        if (n < 100000 || avec_insertion_100000) {
            d = clock();
            tri_insertion(col.tab, ind, n, colis_cmp_poids, 1);
            snprintf(buf_ins, sizeof buf_ins, "%.2f", ms_depuis(d));
        } else {
            snprintf(buf_ins, sizeof buf_ins, "(ignore)");
        }
        for (int i = 0; i < n; i++) ind[i] = i;
        d = clock();
        tri_fusion(col.tab, ind, n, colis_cmp_poids, 1);
        double t_fus = ms_depuis(d);

        printf("%-8d | %-10.2f | %-14.2f %-14.3f | %-14s %-14.2f\n",
               n, t_ajout, t_lin, t_hash, buf_ins, t_fus);

        free(ind);
        free(cibles);
        hachage_liberer(&t);
        collection_liberer(&col);
    }

    printf("\n=== 2) Chargement optimal (programmation dynamique) ===\n\n");
    printf("a) W = 50 kg fixe, n varie :\n");
    printf("%-8s | %-10s | %-12s\n", "n", "temps (ms)", "memoire DP");
    for (int s = 0; s < nb_tailles; s++) {
        int n = tailles[s];
        Collection col; TableHachage t;
        collection_init(&col, 16); hachage_init(&t, 16);
        bench_generer(&col, &t, n);
        Chargement ch;
        clock_t d = clock();
        chargement_optimal(col.tab, n, 50, 0, &ch);
        double tm = ms_depuis(d);
        double ko = ((double)n * 51 / 8 + 51 * 8) / 1024.0;
        printf("%-8d | %-10.2f | ~%.0f Ko\n", n, tm, ko);
        chargement_vider(&ch);
        hachage_liberer(&t); collection_liberer(&col);
    }

    printf("\nb) n = 10 000 fixe, W varie :\n");
    printf("%-8s | %-10s | %-12s\n", "W (kg)", "temps (ms)", "memoire DP");
    {
        const int caps[] = {50, 500, 5000, 50000};
        Collection col; TableHachage t;
        collection_init(&col, 16); hachage_init(&t, 16);
        bench_generer(&col, &t, 10000);
        for (int s = 0; s < 4; s++) {
            Chargement ch;
            clock_t d = clock();
            int r = chargement_optimal(col.tab, col.taille, caps[s], 0, &ch);
            double tm = ms_depuis(d);
            double mo = ((double)col.taille * (caps[s] + 1) / 8) / (1024.0 * 1024.0);
            if (r == 1) printf("%-8d | %-10.2f | ~%.1f Mo\n", caps[s], tm, mo);
            else        printf("%-8d | refuse (memoire trop grande)\n", caps[s]);
            chargement_vider(&ch);
        }
        hachage_liberer(&t); collection_liberer(&col);
    }

    printf("\n=== 3) Qualite : DP vs force brute vs glouton ===\n");
    {
        const int essais = 500, n = 15;
        int dp_ok = 0, glouton_opt = 0;
        double ecart_max = 0.0;
        for (int e = 0; e < essais; e++) {
            Collection col; TableHachage t;
            collection_init(&col, 16); hachage_init(&t, 16);
            bench_generer(&col, &t, n);
            int cap = 20 + alea(60);
            Chargement a, b, g;
            chargement_optimal(col.tab, n, cap, 0, &a);
            chargement_force_brute(col.tab, n, cap, 0, &b);
            chargement_glouton(col.tab, n, cap, &g);
            if (a.valeur_totale == b.valeur_totale) dp_ok++;
            if (g.valeur_totale == b.valeur_totale) glouton_opt++;
            double ecart = 100.0 * (double)(b.valeur_totale - g.valeur_totale) / (double)b.valeur_totale;
            if (ecart > ecart_max) ecart_max = ecart;
            chargement_vider(&a); chargement_vider(&b); chargement_vider(&g);
            hachage_liberer(&t); collection_liberer(&col);
        }
        printf("%d instances aleatoires de %d colis :\n", essais, n);
        printf("  DP = force brute (optimal)  : %d / %d\n", dp_ok, essais);
        printf("  Glouton optimal             : %d / %d\n", glouton_opt, essais);
        printf("  Pire perte du glouton       : %.1f %% de valeur\n", ecart_max);
    }
}
