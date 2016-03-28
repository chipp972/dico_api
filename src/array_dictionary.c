/**
 * implementation du dictionnaire avec des tableaux d'entiers dans
 * les values (utile pour charger le snapshot de la
 * relation mot-page et faire des recherches)
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "configuration.h"
#include "utils.h"
#include "dictionary.h"
#include "array_dictionary.h"


hash_element_t * addArrayElement(dictionary_t *dico,
    const char *key, int v_nb)
{
    hash_element_t *tmp, *newElement;

    tmp = &(dico->hash_table[hash_djb2(dico, key)]);
    if (tmp->key == NULL) { // si le hash n'est pas utilise
        tmp->key = malloc(sizeof(char)*strlen(key)+1);
        TEST_MALLOC(tmp->key, "tmp->key at addHashElement");
        strcpy(tmp->key, key);
        tmp->value_tab = calloc(v_nb, sizeof(int));
        TEST_MALLOC(tmp->value_tab, "tmp->value_tab at addArrayElement");
        tmp->value_nb = v_nb;

        dico->key_nb++;
        return tmp;
    } else {
        while(1) {
            if (strcmp(tmp->key, key) == 0) {
                if (tmp->value_tab == NULL) {
                    tmp->value_tab = calloc(v_nb, sizeof(int));
                    TEST_MALLOC(tmp->value_tab,
                        "tmp->value_tab at addArrayElement");
                    tmp->value_nb = v_nb;
                }
                return tmp;
            }
            if (tmp->next != NULL)
                tmp = tmp->next;
            else
                break;
        }
        newElement = calloc(1, sizeof(hash_element_t));
        TEST_MALLOC(newElement, "newElement at addHashElement");
        newElement->key = malloc(sizeof(char)*strlen(key)+1);
        TEST_MALLOC(newElement->key, "newElement->key at addHashElement");
        strcpy(newElement->key, key);
        newElement->value_tab = calloc(v_nb, sizeof(int));
        TEST_MALLOC(newElement->value_tab,
            "newElement->value_tab at addArrayElement");
        newElement->value_nb = v_nb;

        // on le rajoute a la fin
        tmp->next = newElement;

        dico->key_nb++;
        return newElement;
    }
}


void loadSnapshot(dictionary_t *dico, char *filepath)
{
    int t, total, n, i, j;
    double progression;
    hash_element_t *e;
    FILE *fd = NULL;
    char str[BUFFER_SIZE];

    fd = fopen(filepath, "r");
    if (fd == NULL)
        SYSERR_EXIT("fopen fd at loadSnapshot");

    // recuperation du nombre de keys
    fscanf(fd, "%d\n", &total);

    // boucle de chargement
    for (j = 1; j <= total; j++) {
        if (fscanf(fd, "%s %d\n", str, &t) != EOF) {
            e = addArrayElement(dico, str, t);
            for (i = 0; i < t; i++) {
                fscanf(fd, "%d\n", &n);
                e->value_tab[i] = n;
            }
        }
        showProgression(j, total, &progression, "Snapshot Loading");
    }
    printf("\nSnapshot loaded with %d keys!\n", total);
}


void saveArraySnapshot(dictionary_t *dico, char *filepath)
{
    int hash, v;
    hash_element_t *element;
    FILE *fd;
    double prog = 0.0;

    if (filepath == NULL || dico == NULL) {
        fprintf(stderr, "Trying to save empty dico to %s\n", filepath);
        return;
    }

    fd = fopen(filepath, "w");
    TEST_MALLOC(fd, "fopen fd at saveArraySnapshot");

    fprintf(fd, "%d\n", dico->key_nb);

    for (hash=0; hash < dico->h_size; hash++) {
        showProgression(hash, dico->h_size-1, &prog, "Snapshot Saving");
        element = dico->hash_table + hash;
        while (element != NULL) {

            if (element->value_nb > 0) {
                fprintf(fd, "%s %d\n", element->key, element->value_nb);

                for (v=0; v<element->value_nb; v++)
                    fprintf(fd, "%d\n", element->value_tab[v]);
	       }
        	element = element->next;
        }
    }
    fclose(fd);
}


void freeArrayDictionary(dictionary_t *dico)
{
    hash_element_t *e;
    int i;
    for (i = 0; i < dico->h_size; i++) {
        e = &(dico->hash_table[i]);
        while (e != NULL) {
            if (e->value_tab != NULL)
                free(e->value_tab);
            e = e->next;
        }
    }
    freeDictionary(dico);
}
