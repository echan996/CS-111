#include "SortedList.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

void SortedList_insert(SortedList_t *list, SortedListElement_t *element){
    /*if (opt_yield & INSERT_YIELD){
        pthread_yield();
    }*/
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
        element->next = it->next;
        element->prev = it;
        it->next = element;
        if (element->next != NULL){
            element->next->prev = element;
        }
	}
}

int SortedList_delete(SortedListElement_t *element){
    /*if (opt_yield & DELETE_YIELD){
        pthread_yield();
    }*/
	if (element == NULL)
        return 1;
    
    SortedListElement_t *p = element->prev;
	SortedListElement_t *n = element->next;
    
    if (p->next != element && n != NULL && n->prev != element){
		return 1;
	}
    
	p->next = n;
    if (n != NULL){
        n->prev = p;
    }
	free(element);
	return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key){
    /*if (opt_yield & SEARCH_YIELD){
        pthread_yield();
    }*/
	SortedListElement_t *it = list->next;
    int count = 1;
	while (it != NULL){
		if (strcmp(it->key, key) == 0){
            return it;
		}
		it = it->next;
        count++;
	}
	return NULL;
}

int SortedList_length(SortedList_t *list){
    /*if (opt_yield & SEARCH_YIELD){
        pthread_yield();
    }*/
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
                add->key = "4";
                break;
                
            case 1:
                add->key = "1";
                break;
                
            case 2:
                add->key = "2";
                break;
                
            case 3:
                add->key = "3";
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
    SortedListElement_t *it = list->next;
    i = 0;
    while (it != NULL){ // check the order of insertion
        //fprintf(stderr, "key %d ", i + 1);
        //fprintf(stderr, "is %c\n", *(it->key));
        it = it->next;
        i++;
    }
    
    //fprintf(stderr, "Testing when removing first element\n");
    /*SortedListElement_t *removeThis = SortedList_lookup(list, "1");
    assert(SortedList_delete(removeThis) != 0);
    assert(SortedList_length(list) == 4);
    it = list->next;
    i = 0;
    while (it != NULL){ // check that 1 is correctly removed
        //fprintf(stderr, "key %d ", i + 1);
        //fprintf(stderr, "is %c\n", *(it->key));
        it = it->next;
        i++;
    }
    assert(SortedList_length(list) == 4);
    
    //fprintf(stderr, "\n");
    //fprintf(stderr, "Testing when removing last element\n");
    removeThis = SortedList_lookup(list, "5");
    assert(SortedList_length(list) == 4);
    assert(SortedList_delete(removeThis) == 0);
    assert(SortedList_length(list) == 3);
    it = list->next;
    i = 0;
    while (it != NULL){ // check that 5 is correctly removed
        //fprintf(stderr, "key %d ", i + 1);
        //fprintf(stderr, "is %c\n", *(it->key));
        it = it->next;
        i++;
    }
    
    removeThis = SortedList_lookup(list, "1");
    assert(removeThis == NULL);
    
    //fprintf(stderr, "\n");
    //fprintf(stderr, "Testing when removing middle element\n");
    removeThis = SortedList_lookup(list, "3");
    assert(SortedList_length(list) == 3);
    assert(SortedList_delete(removeThis) == 0);
    assert(SortedList_length(list) == 2);
    it = list->next;
    i = 0;
    while (it != NULL){ // check that 5 is correctly removed
        //fprintf(stderr, "key %d ", i + 1);
        //fprintf(stderr, "is %c\n", *(it->key));
        it = it->next;
        i++;
    }
     */
    
    static const char alphanum[] =     "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    
    SortedList_t *newlist = (SortedList_t *)malloc(sizeof(SortedList_t));
    list->next = NULL;
    list->prev = NULL;
    list->key = NULL;
    for (int e = 0; e < 1000; e++){
        int len = (rand() % 49) + 1; // random length from 1 to 50
        char *s = (char *)malloc(len);
        for (int j = 0; j < len; ++j) {
            s[j] = alphanum[rand() % (sizeof(alphanum) - 1)];
        }
        s[len] = 0;
        SortedListElement_t *add = (SortedListElement_t *)malloc(sizeof(SortedListElement_t));
        if (add == NULL){
            fprintf(stderr, "Error: memory not allocated\n");
            exit(1);
        }
        add->key = s;
        SortedList_insert(newlist, add);
    }
}