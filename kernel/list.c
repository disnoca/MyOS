/**
 * Simple doubly-linked list implementation.
 * 
 * @author Samuel Pires
*/

#include <kernel/list.h>


void list_add_last(list_t* head, list_t* new)
{
	new->next = head;
	new->prev = head->prev;
	head->prev->next = new;
	head->prev = new;
}


list_t* list_remove_first(list_t* head)
{
	if(head->next == head)
		return NULL;

	list_t* entry = head->next;
	head->next = entry->next;
	entry->next->prev = head;

	return entry;
}


list_t* list_remove(list_t* head, list_t* entry)
{
	list_t* curr_entry = head->next;

	while(curr_entry != head)
	{
		if(curr_entry == entry) {
			curr_entry->prev->next = curr_entry->next;
			curr_entry->next->prev = curr_entry->prev;
			return curr_entry;
		}
		
		curr_entry = curr_entry->next;
	}

	return NULL;
}


void list_for_each(list_t* head, void (*func)(list_t*))
{
	list_t* curr_entry = head->next;
	list_t* next_entry = curr_entry->next;

	while(curr_entry != head) {
		func(curr_entry);
		curr_entry = next_entry;
		next_entry = next_entry->next;
	}
}


list_t* list_search(list_t* head, bool (*func)(list_t*))
{
	list_t* curr_entry = head->next;

	while(curr_entry != head) {
		if(func(curr_entry))
			return curr_entry;
		
		curr_entry = curr_entry->next;
	}

	return NULL;
}
