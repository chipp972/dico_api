/**
 * Fournit une API simple pour construire une table de hachage
 * et ne rentre pas dans les details d'implementation des values
 * qui peuvent etre des struct, des tableaux, des types simples etc.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "configuration.h"
#include "utils.h"
#include "dictionary.h"


unsigned long hash_djb2(const dictionary_t *dico, const char *str)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return (hash % dico->h_size);
}


dictionary_t * initDictionary(int h_size)
{
    dictionary_t *new_dico = malloc(sizeof(dictionary_t));
    TEST_MALLOC(new_dico, "initDictionary at new_dictionary");

    new_dico->hash_table = calloc(h_size, sizeof(hash_element_t));
    TEST_MALLOC(new_dico->hash_table, "new_dico->hash_table at initDictionary");

    new_dico->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;

    new_dico->h_size = h_size;
    new_dico->key_nb = 0;
    return new_dico;
}


hash_element_t *getHashElement(dictionary_t *dico, const char *key)
{

    hash_element_t *e = NULL;
    e = &(dico->hash_table[hash_djb2(dico, key)]);
    while (e != NULL) {
        if (e->key != NULL) {
            if (strcmp(e->key, key) == 0)
                break;
        }
        e = e->next;
    }
    return e;
}

void freeDictionary(dictionary_t *dico)
{
    int i;
    hash_element_t *tmp, *next;

    // on prend pour acquis que la/es value/s sont free
    for (i = 0; i < dico->h_size; i++) {
        tmp = &(dico->hash_table[i]);
        next = tmp->next;
        if (tmp->key != NULL)
            free(tmp->key);
        while (next != NULL) {
            // on vide les autres elements de meme hash
            tmp = next;
            next = tmp->next;
            if (tmp->key != NULL)
                free(tmp->key);
            free(tmp);
        }
    }
    free(dico->hash_table);

    free(dico);
    dico = NULL;
}


void associateWordWithRoot(dictionary_t *dico, const char *key1, const char *key2)
{
    hash_element_t *e1 = NULL, *e2 = NULL;
    e1 = getHashElement(dico, key1);
    e2 = getHashElement(dico, key2);
    if(e1 != NULL && e2 != NULL)
        e1->root = e2;
}
