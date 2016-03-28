#ifndef LIST_DICTIONARY_H
#define LIST_DICTIONARY_H


// taille maximum d'une liste de values
#define MAX_LIST_SIZE 2000000000


value_list_t *createValueElement(int val);

/**
 * ajoute un element avec la cle key, sans value et le retourne
 * l'element contiendra une liste de value (le tableau n'est pas initialise)
 * (si l'element existe deja, renvoie l'element existant)
 *
 * @param d   le dictionary dans lequel on ajoute la cle
 * @param key la cle que l'on veut ajouter
 * @return    l'element ajoute
 */
hash_element_t * addListElement(dictionary_t *dico, const char *key);


// ajoute une value a la liste de l'element
void addValueToElement(hash_element_t *e, int val);

/**
 * Fais un snapshot du dictionnaire dans un fichier
 * si on met destroy a 1 on libere les elements au fur
 * et a mesure de la completion du snapshot
 *
 * @param di             le dico a sauvegarder
 * @param fp             le fichier dans lequel on sauvegarde
 * @param destroy        switch pour liberer l'espace au fur et a mesure
 */
 void dicoListSnapshot(dictionary_t *dico, char *fp, int destroy);

/**
 * concatene la table de hachage src dans dest
 * la table de hachage src est videe puis free
 *
 * @param  dest la table de hachage qui va recevoir toutes les values
 * @param  src  la table de hachage a vider
 * @return      la table de hachage de destination
 */
dictionary_t *dicoFusion(dictionary_t *dest, dictionary_t *src);


/**
 * Libere la liste de values d'un element
 * @param e   l'element de la table dont on libere les values
 */
 void freeValueList(hash_element_t *e);

/**
 * Libere toutes les listes de values du dico puis
 * passe la main a freeDictionary pour liberer le reste
 * de la structure
 *
 * @param d le dico duquel on veut supprimer les values
 */
void freeListDictionary(dictionary_t *d);

#endif
