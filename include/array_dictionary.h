#ifndef ARRAY_DICTIONARY_H
#define ARRAY_DICTIONARY_H

/**
 * Ajoute une key dans la table de hachage
 *
 * @param  dico le dictionnaire cible
 * @param  key  la cle a ajouter
 * @param  v_nb le nombre de values auquelles elle sera associee
 * @return      l'element ajoute
 */
hash_element_t * addArrayElement(dictionary_t *dico,
    const char *key, int v_nb);

/**
 * Permet de charger un snapshot de table de dictionnaire
 * precedemment creer et d'obtenir une table de hachage read-only
 *
 * @param dico     le dico dans lequel on met les valeurs
 * @param filepath le chemin vers le fichier du snapshot
 */
void loadSnapshot(dictionary_t *dico, char *filepath);

/**
 * Libere l'espace alloue pour le dico
 *
 * @param dico le dictionnaire a liberer
 */
void freeArrayDictionary(dictionary_t *dico);


/**
 * Permet de sauvegarder un snapshot de table de dictionnaire
 *
 * @param dico     le dico que l'on veut sauvegarder
 * @param filepath le chemin vers le fichier
 */
void saveArraySnapshot(dictionary_t *dico, char *filepath);

#endif
