#pragma once

#include <stddef.h>
#include <stdbool.h>


#define LIST_INIT(list) 	({ (list) = (list_t) {&(list), &(list)}; })

#define LIST_IS_NULL(list)	(((list_t) (list)).prev == 0 && ((list_t) (list)).next == 0)

#define LIST_IS_EMPTY(list) (((list_t) (list)).next == &(list))

/* Returns the first element of a list or NULL if the list is empty */
#define LIST_FIRST(list) 	(LIST_IS_EMPTY(list) ? NULL : ((list_t) (list)).next)

/* Clears all entries from a list */
#define LIST_CLEAR(list)	({ ((list_t) (list)).next = ((list_t) (list)).prev = &(list); })


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
