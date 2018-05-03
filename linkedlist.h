/**********************************************************************
  Module: linkedlist.h
  Author: Will Coombs
  Date:   2017 June 13

  Purpose: Maintain a generic, circular linked list and provides
  validations upon the list.

  Changes: 

  NOTES: None of these functions are thread-safe.
**********************************************************************/

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdbool.h>

typedef struct NODE Node;
struct NODE {
  void *element;
  Node *next;
};

typedef struct LINKEDLIST LinkedList;
struct LINKEDLIST {
  Node *top;
  Node *tail;
  Node *traversal_node;
  int number_of_nodes;
};

/* Creates a new Linked List and returns it to the caller */
LinkedList *createList();

/* Validates a given linked list with various assertions */
void validateLinkedList(LinkedList *list);

/* Function that adds a given element to a given list, at the end
 * of the given list */
bool add(LinkedList *list, void *new_element);

/* Function that deletes a given target element from the given list */
bool delete(LinkedList *list, void *target);


/* Function that deletes a given list's elements */
void destroyList(LinkedList *list);

/* Function that returns the first element in a given list */
void *firstElement(LinkedList *list);

/* Function that returns the next element in a given list, and updates
 * the traversal node appropriately */
void *nextElement(LinkedList *list);

#endif
