# Utilisation de l'API Dico #

## Introduction ##

L'API Dico permet d'utiliser une structure de donnée de type
table de hachage pour associer des chaînes de caractères à des
entiers (index dans une table de correspondance).
On peut au choix remplir une table de hachage de type liste
(dont on ne connait pas le nombre de valeurs à l'avance) ou
bien charger un snapshot du table précédemment crée et dans ce
cas on utilisera une table de hachage "read-only" implémentée
avec des tableaux pour des accès plus rapides et une emprunte
mémoire plus légère.

## Utilisation dans un programme ##

Pour utiliser la structure de donnée dans un programme il faut :
*   inclure les .h de l'API (0)
*   initialiser un dictionnaire (1)

Ensuite 2 possibilités d'utilisation :
*   Mode read-write
    *   ajouter une key (2a)
    *   récuperer un élement (3)
    *   ajouter une value a une key (3a)
    *   fusionner un dico temporaire dans un dico plus gros (thread-safe) et faire un snapshot (4a)
    *   liberer les ressources (5a)

*   Mode read-only
    *   charger un snapshot (2b)
    *   récuperer un élement (3)
    *   ajouter un element et des values (3b)
    *   liberer les ressources (4b)

Exemple :
```
// (0)
#include "dictionary.h" // obligatoire
#include "list_dictionary.h" // pour le dico de type liste (write)
#include "french_dictionary.h" // pour avoir un dico pré-rempli avec les mots du dictionnaire francais
#include "array_dictionary.h" // pour avoir le dico de type table (read-only)


[...]

// (1)
dictionary_t *dico;

// pour avoir un dico vierge
dico = initDictionnary(size);
// pour avoir un dico pré-rempli avec les mots francais
dico = buildFrenchDico(size);

// (2a)
addListElement(dico, "nouveau_mot");
// (2b)
loadSnapshot(dico, "data/dico.sav");

// (3)
hash_element_t *e;
e = getHashElement(dico, "blabla");

// (3a)
addValueToElement(e, 42);

// (3b)
hash_element_t *tmp;
tmp = addArrayElement(dico, "nouveau_mot", 20); // 20 = tableau de 20 values
tmp->value_tab[0] = 42;
...
tmp->value_tab[19] = 24;

// (4a)
dicoFusion(dico, tmpDico); // typiquement realise par plusieurs threads

dicoListSnapshot(dico, "data/dico.sav", 0); // realise par la derniere thread ou le main

// (4b)
freeArrayDictionary(dico);

// (5a)
freeListDictionary(dico);
```
