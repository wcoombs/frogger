/**********************************************************************
  Module: linkedlist.c
  Author: Will Coombs
  Date:   2017 June 13

  Purpose: see linkedlist.h

  Changes: 

  NOTES: None.
**********************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "linkedlist.h"
#include "gameglobals.h"

LinkedList *createList() {
  LinkedList *new_list = (LinkedList *)malloc(sizeof(LinkedList));
  new_list->top = NULL;
  new_list->tail = NULL;
  new_list->traversal_node = NULL;
  new_list->number_of_nodes = 0;

  return new_list;
}

void validateLinkedList(LinkedList *list) {
  if (list->number_of_nodes == 0) {
    assert(list->top == NULL);
  } else if (list->number_of_nodes == 1) {
    assert(list->tail == list->top);
  } else {
    assert(list->top != NULL && list->top->next != NULL);
  }
}

bool add(LinkedList *list, void *new_element) {
  bool result = false;
  Node *new_node = NULL;

  validateLinkedList(list);

  assert(new_element != NULL);
  if (new_element != NULL) {
    new_node = (Node *)malloc(sizeof(Node));
    
    if (new_node != NULL) {
      new_node->element = (void *)malloc(sizeof(new_element));

      if (new_node->element != NULL) {
        new_node->element = new_element;
	
	if (list->top == NULL) {
	  new_node->next = list->top;
          list->top = new_node;
	  list->tail = new_node;
	} else {
	  list->tail->next = new_node;
	  list->tail = new_node;
	  new_node->next = list->top;
	}

        list->number_of_nodes++;
        result = true;
      } else {
        free(new_node->element);
	free(new_node);
      }
    } else {
      free(new_node);
    }
  }

  return result;
}

bool delete(LinkedList *list, void *target) {
  bool result = 0;
  bool found = 0;
  int i = 0;
  Node *curr = list->top;
  Node *prev = NULL;

  validateLinkedList(list);

  assert(target != NULL);
  if (target != NULL) {
    if (list->top->element == target) {
      found = true;
    }
    
    while (i < list->number_of_nodes && !found) { 
      prev = curr;
      curr = curr->next;
      
      if (curr->element == target) {
        found = true;
      }

      i++;
    }

    if (found && curr != NULL) {
      if (prev != NULL) {
        if (list->tail == curr) {
	  list->tail = prev;
	}

	if (list->traversal_node == curr) {
	  list->traversal_node = prev;
	}

        prev->next = curr->next;
      } else {
	if (list->traversal_node == curr) {
	  list->traversal_node = list->tail;
	  list->tail->next = curr->next;
	}

        // deleting the first element in the list. check to see if it is the only
	// one in the list. if it is, the logic is setup such that top->next will
	// point to itself, as this is a circular list. in this case, top and tail
	// will need to explicitly be set to null
        if (list->number_of_nodes == 1) {
	  list->top = NULL;
	  list->tail = NULL;
	} else {
          list->top = curr->next;
	  list->tail->next = list->top;
	}
      }
      
      free(curr->element);
      curr->element = NULL;
      free(curr);
      curr = NULL;

      result = true;
      list->number_of_nodes--;

      assert(curr == NULL);
      validateLinkedList(list);
    }
  }

  return result;
}

void destroyList(LinkedList *list) {
  void *the_element = firstElement(list);

  while (list->number_of_nodes != 0) {
    bool deleted  = delete(list, the_element);
    if (!deleted) {
      perror("deleting a node");
      exit(-1);
    }

    the_element = nextElement(list);
  }

  assert(list->number_of_nodes == 0);
}

void *firstElement(LinkedList *list) {
  void *the_element = NULL;

  if (list->top != NULL) {
    list->traversal_node = list->top->next;
    the_element = list->top->element;
  }

  return the_element;
}

void *nextElement(LinkedList *list) {
  void *the_element = NULL;

  if (list->traversal_node != NULL) {
    the_element = list->traversal_node->element;
    list->traversal_node = list->traversal_node->next;
  }

  if (list->traversal_node == NULL) {
    list->traversal_node = list->top;
    the_element = list->top->element;
  }

  return the_element;
}
