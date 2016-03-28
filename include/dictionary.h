#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <pthread.h>


typedef struct value_list {
    int val;
    struct value_list *next;
} value_list_t;


// un element de la table de hachage
typedef struct hash_element {
    char *key;
    int value_nb;
    struct hash_element *next, *root;

    // un element contient soit une liste soit un tableau
    value_list_t *first_value, *last_value; // list
    int *value_tab; // tableau
} hash_element_t;


/**
 * Structure de la table de hachage
 * Elle permet d'associer des keys (char*) a des values (int)
 *
 * hash_table la table de hachage
 * h_size     la taille de la table de hachage
 * key_nb     le nombre de cle dans la table
 * mutex      le mutex integre pour les operations concurrentes
 */
typedef struct dictionary {
    hash_element_t *hash_table;
    int h_size, key_nb;
    pthread_mutex_t mutex;
} dictionary_t;


 /**
  * Cree un dictionary vide de taille size
  * @param  h_size         taille table de hachage
  * @return                un pointeur sur la table de hachage cree
  */
dictionary_t * initDictionary(int h_size);


/**
 * Calcule et renvoie le hash de key dans dico
 * Pour plus d'info : http://www.cse.yorku.ca/~oz/hash.html
 * Calcul : hash(i) = hash(i-1) * 33 + str[i]
 *
 * @param  dico le dictionary pour lequel on calcule le hash
 * @param  key  la chaine de caractere a hacher
 * @return      le hash
 */
unsigned long hash_djb2(const dictionary_t *dico, const char *key);


// Renvoie l'element correspondant a la key
hash_element_t *getHashElement(dictionary_t *dico, const char *key);


// libere la structure sans vider les values puisqu'on
// ne connait pas l'implementation
void freeDictionary(dictionary_t *dico);


/**
 * Associate a key to another key
 *
 * @param dico              the dictionary to modify
 * @param key_to_redirect   the key to redirect
 * @param root              the root key
 */
void associateWordWithRoot(dictionary_t *dico, const char *key_to_redirect,
    const char *root);

#endif
