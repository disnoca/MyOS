#pragma once

#include <stddef.h>
#include <stdbool.h>


#define LIST_INIT(listp) 	({ list_t* lp = ((list_t*) (listp)); *lp = (list_t) {lp, lp}; })

#define LIST_IS_EMPTY(listp) ({ list_t* lp = ((list_t*) (listp)); lp->next == lp; })

/* Returns the first element of a listp or NULL if the listp is empty */
#define LIST_FIRST(listp) 	(LIST_IS_EMPTY(listp) ? NULL : ((list_t*) listp)->next)

/* Clears all entries from a listp */
#define LIST_CLEAR(listp)	({ list_t* lp = ((list_t*) (listp)); lp->next = lp->prev = lp; })


typedef struct list_s {
	struct list_s *prev, *next;
} list_t;


/**
 * Adds a new entry to the end of a list.
 * 
 * @param head the head of the list
 * @param new the new entry to be added
*/
void list_add_last(list_t* head, list_t* new);

/**
 * Removes and returns the first entry of a list.
 * 
 * @param head the head of the list
 * 
 * @return the removed entry or NULL if the list is empty
*/
list_t* list_remove_first(list_t* head);

/**
 * Removes a specific entry from a list.
 * 
 * @param head the head of the list
 * @param entry the entry to be removed
 * 
 * @return the removed entry or NULL if it wasn't in the list
*/
list_t* list_remove(list_t* head, list_t* entry);

/**
 * Runs a function in each element of a list.
 * 
 * The function can be destructive.
 * 
 * If the given list is included in another struct, that struct
 * must contain the list as its first element.
 * 
 * @param head the head of the list
 * @param func the function to be run in each element of the list
*/
void list_for_each(list_t* head, void (*func)(list_t*));

/**
 * Searches the list for an entry that satisfies a condition specified by a function.
 * 
 * @param head the head of the list
 * @param func the function that specifies the condition
 * 
 * @return the first entry that satisfies the condition or NULL if none does
*/
list_t* list_search(list_t* head, bool (*func)(list_t*));
