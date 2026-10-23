#ifndef HACHAGE_H
#define HACHAGE_H



typedef struct Noeud {
    int id;
    int indice;
    struct Noeud *suivant;
} Noeud;

typedef struct TableHachage {
    Noeud **cases;
    int nb_cases;
    int nb_elements;
} TableHachage;

#define CHARGE_MAX 0.75

int  hachage_init(TableHachage *t, int nb_cases_min);
int  hachage_inserer(TableHachage *t, int id, int indice);
int  hachage_rechercher(const TableHachage *t, int id);
void hachage_stats(const TableHachage *t, double *charge, int *plus_longue_liste);
void hachage_liberer(TableHachage *t);

#endif
