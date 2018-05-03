/**********************************************************************
  Module: threadwrappers.h
  Author: Will Coombs
  Date:   2017 June 13

  Purpose: Provide safe functions for threads to be created in, where
  error checking occurs in this class to simplify the code elsewhere.

  Changes: 

  NOTES: None.
**********************************************************************/

#ifndef THREADWRAPPERS_H
#define THREADWRAPPERS_H

/* Function that is in charge of creating a thread, given the same
 * parameters as pthread_create. This function also handles the error
 * checking from creating a thread, simplifying code elsewhere  */
int createSafeThread(pthread_t *given_thread, const pthread_attr_t *thread_attributes, void *(*target_function) (void *), void *arg);

/* Function that is in charge of joining a thread, given the same
 * parameters as pthread_join. This function also handles the error
 * checming from joining a thread, simplifying code elsewhere */
int joinSafeThread(pthread_t given_thread, void **return_value);

/* Function that sleeps a given thread for the given number of
 * ticks. However, the sleeping is done in a for-loop, and if the
 * game running condition is changed, the loop and thread exits.
 * This assists with closing the game down quickly and cleanly */
void safeSleep(int ticks);

#endif
