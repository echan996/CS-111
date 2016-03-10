#include "SortedList.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

void SortedList_insert(SortedList_t *list, SortedListElement_t *element){
	// list is empty --> next = NULL
    int length, compare_cur, compare_next;
    //fprintf(stderr, "inside insert function\n");
	if (list->next == NULL){
        //fprintf(stderr, "inside if statement\n");
		list->next = element;
		element->prev = list;
		element->next = NULL;
        //fprintf(stderr, "added node\n");
	}
	// list is filled --> iterate through the list to find where to place it and connect accordingly
	else {
        //fprintf(stderr, "inside else statement\n");
		SortedListElement_t *it = list->next; // don't want to start comparing to the head's key,
											  // but the key of the actual first element in list
		while (it->next != NULL) {
            //fprintf(stderr, "inside do while\n");
            //fprintf(stderr, "current key is %c\n", *(it->key));
			int compare_cur = strcmp(it->key, element->key);
            
			int compare_next = strcmp(it->next->key, element->key);
			if ((compare_cur <= 0) && (compare_next >= 0)){
				break;
			}
			it = it->next;
        }
        
        element->next = it->next;
        element->prev = it;
        it->next = element;
	}
    //fprintf(stderr, "about to call length\n");
    length = SortedList_length(list);
    //fprintf(stderr, "length after insert is %d\n", length);
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
	free(element);
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

int main(){
    SortedList_t *list = (SortedList_t *)malloc(sizeof(SortedList_t));
    list->next = NULL;
    list->prev = NULL;
    list->key = NULL;
    int length = SortedList_length(list);
    //fprintf(stderr, "the INTITIAL length is %d\n", length);
    int i;
    for (i = 0; i < 5; i++){
        SortedListElement_t *add = (SortedListElement_t *)malloc(sizeof(SortedListElement_t));
        switch (i) {
            case 0:
                add->key = "1";
                break;
                
            case 1:
                add->key = "3";
                break;
                
            case 2:
                add->key = "2";
                break;
                
            case 3:
                add->key = "4";
                break;
                
            case 4:
                add->key = "5";
                break;
                
                
            default:
                break;
        }
        //fprintf(stderr, "INSERT key is %c\n", *(add->key));
        //fprintf(stderr, "i is %d\n", i);
        if (add == NULL){
            //fprintf(stderr, "Error: memory not allocated\n");
            exit(1);
        }
        SortedList_insert(list, add);
    }
    
    assert(SortedList_length(list) == 5);
    SortedListElement_t *removeThis = SortedList_lookup(list, "1");
    assert(SortedList_delete(removeThis) == 0);
    assert(SortedList_length(list) == 4);
    SortedListElement_t *it = list->next;
    for (i = 0; i < 4; i++){ // check that 1 is correctly removed
        //fprintf(stderr, "key %d ", i + 1);
        //fprintf(stderr, "is %c\n", *(it->key));
        it = it->next;
    }

}