#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/*
 * SortedList (and SortedListElement)
 *
 *	A doubly linked list, kept sorted by a specified key.
 *	This structure is used for a list head, and each element
 *	of the list begins with this structure.
 *
 *	The list head is in the list, and an empty list contains
 *	only a list head.  The list head is also recognizable because
 *	it has a NULL key pointer.
 */
struct SortedListElement {
	struct SortedListElement *prev;
	struct SortedListElement *next;
	const char *key;
};
typedef struct SortedListElement SortedList_t;
typedef struct SortedListElement SortedListElement_t;

/**
 * SortedList_insert ... insert an element into a sorted list
 *
 *	The specified element will be inserted in to
 *	the specified list, which will be kept sorted
 *	in ascending order based on associated keys
 *
 * @param SortedList_t *list ... header for the list
 * @param SortedListElement_t *element ... element to be added to the list
 *
 * Note: if (opt_yield & INSERT_YIELD)
 *		call pthread_yield in middle of critical section
 */
void SortedList_insert(SortedList_t *list, SortedListElement_t *element);

/**
 * SortedList_delete ... remove an element from a sorted list
 *
 *	The specified element will be removed from whatever
 *	list it is currently in.
 *
 *	Before doing the deletion, we check to make sure that
 *	next->prev and prev->next both point to this node
 *
 * @param SortedListElement_t *element ... element to be removed
 *
 * @return 0: element deleted successfully, 1: corrtuped prev/next pointers
 *
 * Note: if (opt_yield & DELETE_YIELD)
 *		call pthread_yield in middle of critical section
 */
int SortedList_delete( SortedListElement_t *element);

/**
 * SortedList_lookup ... search sorted list for a key
 *
 *	The specified list will be searched for an
 *	element with the specified key.
 *
 * @param SortedList_t *list ... header for the list
 * @param const char * key ... the desired key
 *
 * @return pointer to matching element, or NULL if none is found
 *
 * Note: if (opt_yield & SEARCH_YIELD)
 *		call pthread_yield in middle of critical section
 */
SortedListElement_t* SortedList_lookup(SortedList_t *list, const char *key);

/**
 * SortedList_length ... count elements in a sorted list
 *	While enumeratign list, it checks all prev/next pointers
 *
 * @param SortedList_t *list ... header for the list
 *
 * @return int number of elements in list (excluding head)
 *	   -1 if the list is corrupted
 *
 * Note: if (opt_yield & SEARCH_YIELD)
 *		call pthread_yield in middle of critical section
 */
int SortedList_length(SortedList_t *list);

/**
 * variable to enable diagnositc calls to pthread_yield
 */
extern int opt_yield;
#define	INSERT_YIELD	0x01	// yield in insert critical section
#define	DELETE_YIELD	0x02	// yield in delete critical section
#define	SEARCH_YIELD	0x04	// yield in lookup/length critical section

void SortedList_insert(SortedList_t *list, SortedListElement_t *element){
    if (opt_yield & INSERT_YIELD){
        pthread_yield();
    }
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
    if (opt_yield & DELETE_YIELD){
        pthread_yield();
    }
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
    if (opt_yield & SEARCH_YIELD){
        pthread_yield();
    }
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
    if (opt_yield & SEARCH_YIELD){
        pthread_yield();
    }
    SortedListElement_t *it = list;
    int counter = -1;
    while (it != NULL){
        counter++;
        it = it->next;
    }
    return counter;
}
