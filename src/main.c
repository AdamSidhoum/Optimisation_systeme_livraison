#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include "colis.h"
#include "collection.h"
#include "hachage.h"
#include "tri.h"
#include "chargement.h"
#include "bench.h"


static int lire_ligne(char *buf, int taille) {
    if (fgets(buf, taille, stdin) == NULL) return 0;
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}
    }
    return 1;
}

static int lire_entier(const char *invite, int min, int max, int *res) {
    char buf[64];
    for (;;) {
        printf("%s", invite);
        if (!lire_ligne(buf, sizeof buf)) return 0;
        char *fin;
        errno = 0;
        long v = strtol(buf, &fin, 10);
        while (*fin == ' ') fin++;
        if (fin != buf && *fin == '\0' && errno == 0 && v >= min && v <= max) {
            *res = (int)v;
            return 1;
        }
        printf("  Saisie invalide (entier entre %d et %d attendu).\n", min, max);
    }
}

/* ---------- Actions du menu ---------- */

static void action_ajouter(Collection *col, TableHachage *t) {
    int id, poids, valeur, prio;
    char dest[TAILLE_DEST + 8];
    if (!lire_entier("Identifiant : ", 1, INT_MAX, &id)) return;
    if (hachage_rechercher(t, id) != -1) {
        printf("Erreur : l'identifiant %d existe deja.\n", id);
        return;
    }
    if (!lire_entier("Poids (kg) : ", 1, 1000000, &poids)) return;
    if (!lire_entier("Valeur : ", 0, 1000000000, &valeur)) return;
    if (!lire_entier("Priorite (1 = faible ... 5 = maximale) : ", PRIO_MIN, PRIO_MAX, &prio)) return;
    printf("Destination : ");
    if (!lire_ligne(dest, sizeof dest)) return;

    int r = systeme_ajouter(col, t, colis_creer(id, poids, valeur, prio, dest));
    if (r == 1)       printf("Colis %d ajoute.\n", id);
    else if (r == 0)  printf("Erreur : identifiant deja utilise.\n");
    else if (r == -1) printf("Erreur : colis invalide.\n");
    else              printf("Erreur : memoire insuffisante.\n");
}

static void action_rechercher(const Collection *col, const TableHachage *t) {
    int id;
    if (!lire_entier("Identifiant recherche : ", 1, INT_MAX, &id)) return;
    int i = hachage_rechercher(t, id);
    if (i == -1) {
        printf("Aucun colis avec l'identifiant %d.\n", id);
        return;
    }
    colis_afficher_entete();
    colis_afficher(&col->tab[i]);
}

static void action_organiser(const Collection *col) {
    if (col->taille == 0) { printf("Aucun colis.\n"); return; }
    int critere, sens, limite;
    printf("Trier selon : 1. Poids  2. Valeur  3. Priorite\n");
    if (!lire_entier("Critere : ", 1, 3, &critere)) return;
    if (!lire_entier("Ordre (1 = croissant, 2 = decroissant) : ", 1, 2, &sens)) return;
    if (!lire_entier("Nombre de colis a afficher (0 = tous) : ", 0, INT_MAX, &limite)) return;

    CmpColis cmp = (critere == 1) ? colis_cmp_poids
                 : (critere == 2) ? colis_cmp_valeur
                 :                  colis_cmp_priorite;
    int *ind = tri_indices_init(col->taille);
    if (ind == NULL || !tri_fusion(col->tab, ind, col->taille, cmp, sens == 1 ? 1 : -1)) {
        printf("Erreur : memoire insuffisante.\n");
        free(ind);
        return;
    }
    int nb = (limite == 0 || limite > col->taille) ? col->taille : limite;
    colis_afficher_entete();
    for (int k = 0; k < nb; k++) colis_afficher(&col->tab[ind[k]]);
    if (nb < col->taille) printf("... (%d colis au total)\n", col->taille);
    free(ind);
}

static void action_preparer(const Collection *col, Chargement *ch) {
    int cap, mode, alpha = 0;
    if (!lire_entier("Capacite du camion (kg) : ", 1, 10000000, &cap)) return;
    printf("Mode : 1. Maximiser la valeur  2. Valeur + priorite\n");
    if (!lire_entier("Choix : ", 1, 2, &mode)) return;
    if (mode == 2) {
        printf("alpha = influence de la priorite (25 conseille : priorite 5 = valeur x2)\n");
        if (!lire_entier("alpha (0 a 1000) : ", 0, ALPHA_MAX, &alpha)) return;
    }
    Chargement nouveau;
    clock_t d = clock();
    int r = chargement_optimal(col->tab, col->taille, cap, alpha, &nouveau);
    double ms = 1000.0 * (double)(clock() - d) / CLOCKS_PER_SEC;
    if (r == -1) {
        printf("Refuse : n x W trop grand pour la memoire (complexite pseudo-polynomiale).\n");
        return;
    }
    if (r == 0) {
        printf("Erreur : memoire insuffisante.\n");
        return;
    }
    chargement_vider(ch);
    *ch = nouveau;
    printf("Chargement optimal calcule en %.2f ms.\n", ms);
    chargement_afficher(ch, col->tab);
}

static void action_statistiques(const Collection *col, const TableHachage *t, const Chargement *ch) {
    long long poids = 0, valeur = 0;
    int par_prio[PRIO_MAX + 1] = {0};
    for (int i = 0; i < col->taille; i++) {
        poids += col->tab[i].poids;
        valeur += col->tab[i].valeur;
        par_prio[col->tab[i].priorite]++;
    }
    double charge;
    int plus_longue;
    hachage_stats(t, &charge, &plus_longue);

    printf("Nombre de colis       : %d (capacite allouee : %d)\n", col->taille, col->capacite);
    printf("Poids total           : %lld kg\n", poids);
    printf("Valeur totale         : %lld\n", valeur);
    if (col->taille > 0)
        printf("Poids / valeur moyens : %.2f kg / %.2f\n",
               (double)poids / col->taille, (double)valeur / col->taille);
    printf("Repartition priorites :");
    for (int p = PRIO_MIN; p <= PRIO_MAX; p++) printf("  P%d=%d", p, par_prio[p]);
    printf("\nTable de hachage      : %d cases, facteur de charge %.2f, plus longue liste %d\n",
           t->nb_cases, charge, plus_longue);
    if (ch->indices != NULL)
        printf("Dernier chargement    : %d colis, %lld / %d kg, valeur %lld\n",
               ch->nb, ch->poids_total, ch->capacite, ch->valeur_totale);
}

static void charger_exemple(Collection *col, TableHachage *t) {
    systeme_ajouter(col, t, colis_creer(101, 8, 20, 2, "Paris"));
    systeme_ajouter(col, t, colis_creer(102, 3, 10, 5, "Lille"));
    systeme_ajouter(col, t, colis_creer(103, 12, 40, 1, "Lyon"));
    systeme_ajouter(col, t, colis_creer(104, 5, 25, 4, "Paris"));
    systeme_ajouter(col, t, colis_creer(105, 2, 15, 3, "Lille"));
}

int main(void) {
    Collection col;
    TableHachage table;
    Chargement ch;
    memset(&ch, 0, sizeof ch);

    if (!collection_init(&col, 16) || !hachage_init(&table, 16)) {
        fprintf(stderr, "Erreur memoire au demarrage.\n");
        return 1;
    }
    srand((unsigned)time(NULL));
    charger_exemple(&col, &table);

    int choix = -1;
    while (choix != 0) {
        printf("\n===== GESTION DES LIVRAISONS =====  (%d colis)\n", col.taille);
        printf("1. Afficher les colis\n");
        printf("2. Ajouter un colis\n");
        printf("3. Rechercher un colis\n");
        printf("4. Organiser les colis (tri)\n");
        printf("5. Preparer un camion\n");
        printf("6. Afficher le chargement\n");
        printf("7. Afficher les statistiques\n");
        printf("8. Generer des colis aleatoires\n");
        printf("9. Lancer les mesures de performance\n");
        printf("0. Quitter\n");
        if (!lire_entier("Choix : ", 0, 9, &choix)) break;
        printf("\n");

        switch (choix) {
        case 1: collection_afficher(&col); break;
        case 2: action_ajouter(&col, &table); break;
        case 3: action_rechercher(&col, &table); break;
        case 4: action_organiser(&col); break;
        case 5: action_preparer(&col, &ch); break;
        case 6: chargement_afficher(&ch, col.tab); break;
        case 7: action_statistiques(&col, &table, &ch); break;
        case 8: {
            int n;
            if (lire_entier("Combien de colis generer ? ", 1, 1000000, &n)) {
                int a = bench_generer(&col, &table, n);
                printf("%d colis ajoutes.\n", a);
            }
            break;
        }
        case 9: {
            int avec;
            printf("Le tri par insertion O(n^2) sur 100 000 colis peut prendre plusieurs secondes.\n");
            if (lire_entier("L'inclure ? (1 = oui, 0 = non) : ", 0, 1, &avec))
                bench_lancer(avec);
            break;
        }
        case 0: printf("Au revoir.\n"); break;
        }
    }

    chargement_vider(&ch);
    hachage_liberer(&table);
    collection_liberer(&col);
    return 0;
}
