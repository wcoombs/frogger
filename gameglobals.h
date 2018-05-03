/**********************************************************************
  Module: gameglobals.h
  Author: Will Coombs
  Date:   2017 June 13

  Purpose: Maintain the various globals used by threads in the program.

  Changes: 

  NOTES: All mutexes are accessed via this class.
**********************************************************************/

#ifndef GAMEGLOBALS_H
#define GAMEGLOBALS_H

#include "linkedlist.h"
#include <stdbool.h>

#define NUMBER_OF_BAYS 5

typedef struct THREADARGS Thread_Args;
struct THREADARGS {
  char row_number;
  int array_index;
};

typedef struct BAY Bay;
struct BAY {
  int left_border;
  int right_border;
  bool contains_frog;
};

/* Function that initializes the linked list used for logs */
void initializeLogList();

/* Function that returns a pointer to the log list, used in functions
 * that require the log list, not used as a global in each file */
LinkedList *getLogList();


/* Updates the overarching boolean that specifies that the game is
 * running or not */
void setGameStillRunning(bool result);

/* Returns whether or not the game is still running based on the above
 * variable */
bool gameStillRunning();

/* Function that updates the player's lives */
void setPlayerLives(int amount);

/* Function that returns how many lives the player has */
int getPlayerLives();


/* Function that initializes all the mutexes used by the threads */
void initializeMutexes();

/* Tells the calling thread to wait on the game running condition */
void conditionWait();

/* Function that signals when the game is over, which is then used by
 * the main thread to wake up and begin cleanup */
void conditionSignal();

/* Function that destroys all mutexes used by the threads */
void destroyMutexes();

/* Functions that lock a specific mutex */
void lockGameRunning();
void lockScreen();
void lockPlayer();
void lockKeyboard();
void lockUpkeep();
void lockLogManagers();
void lockLogList();

/* Functions that unlock a specific mutex */
void unlockGameRunning();
void unlockScreen();
void unlockPlayer();
void unlockKeyboard();
void unlockUpkeep();
void unlockLogManagers();
void unlockLogList();

#endif
