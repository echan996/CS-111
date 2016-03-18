#define _GNU_SOURCE
#include "SortedList.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

void SortedList_insert(SortedList_t *list, SortedListElement_t *element){
   
    // list is empty --> next = NULL
    int length, compare_cur, compare_next;
	if (list->next == NULL){
		list->next = element;
		element->prev = list;
		element->next = NULL;
	}
	// list is filled --> iterate through the list to find where to place it and connect accordingly
	else {
		SortedListElement_t *it = list->next; // don't want to start comparing to the head's key,
											  // but the key of the actual first element in list
        // CASE 1: before the first element
        if ((compare_cur = strcmp(it->key, element->key)) > 0){
            it = list;
            goto insert;
        }
        
        // CASE 2: between two elements or after last element
		while (it->next != NULL) {
			compare_cur = strcmp(it->key, element->key);
			compare_next = strcmp(it->next->key, element->key);
			if ((compare_cur <= 0) && (compare_next >= 0)){
				break;
			}
			it = it->next;
        }
            
	insert:
		if (opt_yield & INSERT_YIELD){
			pthread_yield();
		}
        element->next = it->next;
        element->prev = it;
        it->next = element;
        if (element->next != NULL){
            element->next->prev = element;
        }
	}
}

int SortedList_delete(SortedListElement_t *element){
   
	if (element == NULL)
        return 1;
    
    SortedListElement_t *p = element->prev;
	SortedListElement_t *n = element->next;
	if (opt_yield & DELETE_YIELD){
		pthread_yield();
	}
    if (p->next != element && n != NULL && n->prev != element){
		return 1;
	}
    
	p->next = n;
    if (n != NULL){
        n->prev = p;
    }
    //free(element);
	return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key){
   
	SortedListElement_t *it = list->next;
    
	while (it != NULL){
		if (strcmp(it->key, key) == 0){
            return it;
		}
		it = it->next;
		if (opt_yield & SEARCH_YIELD){
			pthread_yield();
		}
	}
	return NULL;
}

int SortedList_length(SortedList_t **list){
	int counter = -numlists;
	for (int i = 0; i < numlists; i++){
		SortedListElement_t *it = list[i];
		while (it != NULL){
			counter++;
			it = it->next;
		}
	}
	return counter;
}