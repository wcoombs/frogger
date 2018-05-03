/**********************************************************************
  Module: frogger.h
  Author: Will Coombs
  Date:   2017 June 13

  Purpose: Create threads used in the game program, cleanup dead logs
  (logs that are offscreen), and cleanup the game when it is over.

  Changes: 

  NOTES: None.
**********************************************************************/

#ifndef FROGGER_H
#define FROGGER_H

#define GAME_ROWS 24
#define GAME_COLS 80

/* Executes the game, function is called from main */
void executeGame();

/* Initializes the various threads, lists, etc. used by the game */
void initialize();

/* Function that calls the screen refresh function from console, while
 * the game is running */
void *refreshScreen(void *place_holder);

/* Function that performs regular upkeep of the game, like putting
 * player lives to the screen and calling the dead log cleanup */
void *upkeep(void *place_holder);

/* Function that frees the memory used by dead logs */
void cleanupDeadLogs();

/* Function that cleans up mutexes, list memory, etc. */
void exitGame();

#endif
