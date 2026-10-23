# Optimisation du système de livraison

Programme de gestion de colis et de préparation du chargement d'un camion.

## Langage : C

J'ai choisi le C parce que tout notre cours d'algorithmique est en C et que c'est le langage que je maîtrise le mieux. Il a aussi un intérêt pour ce TP : il ne fournit ni liste dynamique ni dictionnaire. Toutes les structures (tableau dynamique, table de hachage, tri, sac à dos) sont donc écrites à la main, et leurs complexités en temps comme en mémoire sont explicites.

## Compilation

```bash
cd src
make          # produit l'exécutable ./livraison
make clean    # supprime les fichiers compilés
```

Compile avec `gcc -Wall -Wextra -std=c99 -O2`, sans aucun warning.

## Lancement

```bash
./livraison
```

Les 5 colis de l'énoncé (ID 101 à 105) sont chargés au démarrage.


Les choix algorithmiques et les complexités sont détaillés dans `rapport.pdf`.
