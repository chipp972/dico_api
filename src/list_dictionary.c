/**
 * implementation du dictionnaire avec des listes dans
 * les values (utile pour realiser le snapshot de la
 * relation mot-page)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "configuration.h"
#include "utils.h"
#include "dictionary.h"
#include "list_dictionary.h"


hash_element_t * addListElement(dictionary_t *dico, const char *key)
{
    hash_element_t *tmp, *newElement;

    if (key[0] == '\0')
        return NULL;
    if (strlen(key) < 3)
        return NULL;

    tmp = &(dico->hash_table[hash_djb2(dico, key)]);
    if (tmp->key == NULL) { // si le hash n'est pas utilise
        tmp->key = malloc(sizeof(char)*strlen(key)+1);
        TEST_MALLOC(tmp->key, "tmp->key at addListElement");
        strcpy(tmp->key, key);
    } else {
        while (tmp->next != NULL) {
            if (strcmp(tmp->key, key) == 0) // si on trouve la key
                return tmp;
            tmp = tmp->next;
        }
        if (strcmp(tmp->key, key) == 0)
            return tmp;
        newElement = calloc(1, sizeof(hash_element_t));
        TEST_MALLOC(newElement, "newElement at addListElement");
        newElement->key = malloc(sizeof(char)*strlen(key)+1);
        TEST_MALLOC(newElement->key, "newElement->key at addListElement");
        strcpy(newElement->key, key);

        tmp->next = newElement;
        dico->key_nb++;
        return newElement;
    }
    dico->key_nb++;
    return tmp;
}


value_list_t *createValueElement(int val)
{
    value_list_t *new_val;
    new_val = malloc(sizeof(value_list_t));
    TEST_MALLOC(new_val, "new_val at createValueElement");
    new_val->val = val;
    new_val->next = NULL;
    return new_val;
}


void addValueToElement(hash_element_t *e, int val)
{
    value_list_t *tmp_val, *new_val;
    if (e != NULL) {
        if (e->value_nb < MAX_LIST_SIZE) {
            while (e->root != NULL)
                e = e->root;
            if (e->first_value == NULL) {
                new_val = createValueElement(val);
                e->first_value = new_val;
                e->last_value = new_val;
                e->value_nb = 1;
            } else {
                tmp_val = e->last_value;
                if (tmp_val->val != val) {
                    // si la derniere val est similaire
                    // on ne l'ajoute pas
                    new_val = createValueElement(val);
                    e->last_value = new_val;
                    tmp_val->next = new_val;
                    e->value_nb++;
                }
            }
        }
    }
}


void dicoListSnapshot(dictionary_t *dico, char *fp, int destroy)
{
    int i, t, cc;
    hash_element_t *e;
    value_list_t *val;
    FILE *fd = NULL;
    double old_pourcent;

    fd = fopen(fp, "w");
    if (fd == NULL)
        SYSERR_EXIT("fopen fd at saveDicoState");

    // on ecrit le nombre de key sur la premiere ligne
    fprintf(fd, "%d\n", dico->key_nb);

    cc = 0; old_pourcent = 0.0;
    t = dico->h_size - 1;
    for (i = 0; i < dico->h_size; i++) {
        e = &(dico->hash_table[i]);
        while (e != NULL) {
            // si il n'y a pas d'element ou que l'element n'a pas de value
            if (e->key == NULL || e->value_nb == 0 || e->root != NULL) {
                e = e->next;
                continue;
            }

            fprintf(fd, "%s %d\n", e->key, e->value_nb);
            cc++;

            val = e->first_value;
            while (val != NULL) {
                fprintf(fd, "%d\n", val->val);
                val = val->next;
            }
            if (destroy)
                freeValueList(e);
            e = e->next;
        }
        showProgression(i, t, &old_pourcent, "Snapshot Saving");
    }
    fclose(fd);
    printf("\nSnapshot saved with %d keys !\n", cc);
}


dictionary_t *dicoFusion(dictionary_t *dest, dictionary_t *src)
{
    int i, t;
    hash_element_t *src_e, *dest_e;

    pthread_mutex_lock(&(dest->mutex));
    for (i = 0, t = src->h_size; i < t; i++) {
        src_e = &(src->hash_table[i]);
        while (src_e != NULL) {
            // si il n'y a pas d'element ou que l'element n'a pas de value
            if (src_e->key == NULL || src_e->value_nb == 0) {
                src_e = src_e->next;
                continue;
            }

            // on recupere ou cree l'element dans dest
            dest_e = addListElement(dest, src_e->key);

            // on concatene la liste
            if (dest_e->first_value == NULL) {
                dest_e->first_value = src_e->first_value;
                dest_e->last_value = src_e->last_value;
                dest_e->value_nb = src_e->value_nb;
            } else {
                dest_e->last_value->next = src_e->first_value;
                dest_e->last_value = src_e->last_value;
                dest_e->value_nb += src_e->value_nb;
            }
            src_e = src_e->next;
        }
    }

    // on libere la structure mais pas les keys de src
    freeDictionary(src);
    pthread_mutex_unlock(&(dest->mutex));
    return dest;
}


void freeValueList(hash_element_t *e)
{
    value_list_t *tmp_val, *tmp_val2;
    if (e != NULL) {
        while (e->root != NULL)
            e = e->root;
        tmp_val = e->first_value;
        while (tmp_val != NULL) {
            tmp_val2 = tmp_val;
            tmp_val = tmp_val->next;
            free(tmp_val2);
        }
        e->value_nb = 0;
    }
}

void freeListDictionary(dictionary_t *dico)
{
    hash_element_t *e;
    int i, t;
    value_list_t *tmp, *next;
    for (i = 0, t = dico->h_size; i < t; i++) {
        e = &(dico->hash_table[i]);
        while (e != NULL) {
            if (e->value_nb == 0) {
                e = e->next;
                continue;
            }
            tmp = e->first_value;
            while (tmp != NULL) {
                next = tmp->next;
                free(tmp);
                tmp = next;
            }
            e->value_nb = 0;
            e = e->next;
        }
    }
    freeDictionary(dico);
}
