/**********************************************************************
  Module: threadwrappers.c
  Author: Will Coombs
  Date:   2017 June 13

  Purpose: see threadwrappers.h

  Changes: 

  NOTES: None.
**********************************************************************/

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

#include "threadwrappers.h"
#include "console.h"
#include "gameglobals.h"

int createSafeThread(pthread_t *given_thread, const pthread_attr_t *thread_attributes, void *(*target_function) (void *), void *arg) {
  int rc = pthread_create(given_thread, thread_attributes, target_function, arg);
  
  if (rc != 0) {
    printf("ERROR - return code from pthread_create() is %d\n", rc);
    exit(-1);
  }

  return rc;
}

int joinSafeThread(pthread_t given_thread, void **return_value) {
  int rc = pthread_join(given_thread, return_value);

  if (rc != 0) {
    perror("Thread join");
    printf("%d\n", rc);
    exit(EXIT_FAILURE);
  }

  return rc;
}

void safeSleep(int ticks) {
  int i;
  for (i = 0; i < ticks && gameStillRunning(); i++) {
    sleepTicks(1);
  }
}
