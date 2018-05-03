/**********************************************************************
  Module: player.h
  Author: Will Coombs
  Date:   2017 June 13

  Purpose: Maintain player (frog) logic and draw/animate to screen.

  Changes: 

  NOTES: None.
**********************************************************************/

#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

#define PLAYER_ANIM_TILES 2
#define PLAYER_HEIGHT 2
#define RIGHT_BOUND GAME_COLS-PLAYER_ANIM_TILES

/* Function that creates an array of the bays at the top of the screen
 * that the frog is attempting to reach */
void createBays();

/* Function that destroys the memory used by each bay */
void destroyBays();

/* Function in charge of animating the player */
void *animatePlayer(void *place_holder);

/* Function in charge of moving the player, using input from the
 * keyboard */
void *keyboardInput(void *place_holder);

/* Function that checks to see if a player is below a bay that is
 * unoccupied */
bool playerIsInRangeOfABay();

/* Function that is in charge of moving the player, by clearing the
 * frog's current position, updating it's movement based on the given
 * keypress, then redrawing */
void handlePlayerMove(char key);

/* Function that checks if a player is in a valid position or not. A
 * valid position is specified by if the player is on the starting row,
 * or on a log. Invalid positions would be if the player is in the
 * water (i.e., past the starting row and not on a log */
bool validPlayerPosition();

/* Function that checks whether or not a frog is on any log */
bool frogIsOnLog();

/* Function in charge of decreasing the player's lives. If the player
 * is at 1 life left and this function is called, the handleGameOver
 * function is called */
void decrementPlayerLives();

/* Function that is in charge of the player's positioning, using the
 * validPlayerPosition, and performs certain tasks depending on the
 * outcome */
void handlePositioning();

/* Function that handles unlocking various mutexes, putting the given
 * string to the screen, then signalling the main thread that the
 * game is over */
void handleGameOver(char *msg);

/* Function that is in charge of if the q (for quit) button is pressed,
 * and performs similar but different action than the above function */
void handleQuit();

#endif
