#include "SortedList.h"
#include <stdio.h>
#include <string.h>

void SortedList_insert(SortedList_t *list, SortedListElement_t *element){
	// list is empty --> next = NULL
	if (list->next == NULL){
		list->next = element;
		element->prev = list;
		element->next = list->next;
	}
	// list is filled --> iterate through the list to find where to place it and connect accordingly
	else {
		SortedListElement_t *it = list->next; // don't want to start comparing to the head's key,
											  // but the key of the actual first element in list
		while (it->next != NULL){
			int compare_prev = strcmp(it->key, element->key);
			int compare_next = strcmp(it->next->key, element->key);
			if ((compare_prev <= 0) && (compare_next >= 0)){
				break;
			}
			it = it->next;
		}
		it->next = element;
		element->prev = it;
		element->next = list->next;
	}
}

int SortedList_delete(SortedListElement_t *element){
	SortedListElement_t *p = element->prev;
	SortedListElement_t *n = element->next;

	// error checking for corrupted pointers
	if (p->next != element && n->prev != element){
		return 1;
	}

	p->next = n;
	n->prev = p;
	return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key){
	SortedListElement_t *it = list->next;
	while (it != NULL){
		if (strcmp(it->key, key) == 0){
			return it;
		}
		it = it->next;
	}
	return NULL;
}

int SortedList_length(SortedList_t *list){
	SortedListElement_t *it = list;
	int counter = -1;
	while (it != NULL){
		counter++;
		it = it->next;
	}
	return counter;
}