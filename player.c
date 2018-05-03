/**********************************************************************
  Module: player.c
  Author: Will Coombs
  Date:   2017 June 13

  Purpose: see player.h

  Changes: 

  NOTES: None.
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/select.h>
#include <assert.h>
#include <stdbool.h>

#include "player.h"
#include "console.h"
#include "frogger.h"
#include "threadwrappers.h"
#include "linkedlist.h"
#include "log.h"
#include "gameglobals.h"

Bay *bays[NUMBER_OF_BAYS];

static char* PLAYER_GRAPHIC[PLAYER_ANIM_TILES][PLAYER_HEIGHT+1] = {
  {"@@",
   "<>"},
  {"--",
   "<>"}
};

#define PLAYER_STARTING_LOCATION_X (GAME_COLS/2)-1
#define PLAYER_STARTING_LOCATION_Y GAME_ROWS-PLAYER_HEIGHT-1
#define JUMP_LENGTH PLAYER_HEIGHT*2

#define GOAL_FROGS_AT_HOME 5

#define BAY_1_LEFT 0
#define BAY_1_RIGHT 8
#define BAY_2_LEFT 20
#define BAY_2_RIGHT 26
#define BAY_3_LEFT 37
#define BAY_3_RIGHT 44
#define BAY_4_LEFT 55
#define BAY_4_RIGHT 62
#define BAY_5_LEFT 73
#define BAY_5_RIGHT 80

#define QUITTER_MSG "Quitter..."
#define NO_LIVES_MSG "Game over. Do, or do not... there is no try!"
#define WIN_MSG "All frogs are home. You win!"

bool playerAlive;
int playerX;
int playerY;
int frogs_at_home = 0;

char **tile;

void createBays() {
  Bay *bay1 = (Bay *)malloc(sizeof(Bay));
  bay1->left_border = BAY_1_LEFT;
  bay1->right_border = BAY_1_RIGHT;
  bay1->contains_frog = false;
  bays[0] = bay1;

  Bay *bay2 = (Bay *)malloc(sizeof(Bay));
  bay2->left_border = BAY_2_LEFT;
  bay2->right_border = BAY_2_RIGHT;
  bay2->contains_frog = false;
  bays[1] = bay2;

  Bay *bay3 = (Bay *)malloc(sizeof(Bay));
  bay3->left_border = BAY_3_LEFT;
  bay3->right_border = BAY_3_RIGHT;
  bay3->contains_frog = false;
  bays[2] = bay3;

  Bay *bay4 = (Bay *)malloc(sizeof(Bay));
  bay4->left_border = BAY_4_LEFT;
  bay4->right_border = BAY_4_RIGHT;
  bay4->contains_frog = false;
  bays[3] = bay4;

  Bay *bay5 = (Bay *)malloc(sizeof(Bay));
  bay5->left_border = BAY_5_LEFT;
  bay5->right_border = BAY_5_RIGHT;
  bay5->contains_frog = false;
  bays[4] = bay5; 
}

void destroyBays() {
  int i;
  for (i = 0; i < NUMBER_OF_BAYS; i++) {
    free(bays[i]);
  }
}

void *animatePlayer(void *place_holder) {
  playerAlive = 1;
  setPlayerLives(4);
  playerX = PLAYER_STARTING_LOCATION_X;
  playerY = PLAYER_STARTING_LOCATION_Y;
  
  while (playerAlive) {
    int i;
    
    for (i = 0; i<PLAYER_ANIM_TILES; i++) {
      lockScreen();
      lockPlayer();

      tile = PLAYER_GRAPHIC[i];
      consoleClearImage(playerY, playerX, PLAYER_HEIGHT, strlen(tile[0]));
      consoleDrawImage(playerY, playerX, tile, PLAYER_HEIGHT);
      
      unlockPlayer();
      unlockScreen();
        
      safeSleep(50);
    }
  }
 
  pthread_exit(NULL);
}

void *keyboardInput(void *place_holder) {
  // sleep until the player is alive, otherwise, the thread running through
  // this function will hit the while (playerAlive) loop, evaluate false,
  // then exit the function. So this while loop exists to allow the player
  // thread to catch up and establish itself
  while (!playerAlive) {
    // wait
  }
  
  while (gameStillRunning() && playerAlive) {
    int return_code = 0;
    
    while (gameStillRunning() && playerAlive && return_code == 0) {
      fd_set set;
      FD_ZERO(&set);
      FD_SET(STDIN_FILENO, &set);
      struct timespec timeout = getTimeout(1);
      int return_code = pselect(FD_SETSIZE, &set, NULL, NULL, &timeout, NULL);

      if (return_code == -1) {
        perror("select()");
	exit(-1);
      } else if (playerAlive && return_code == 1) {
        char buf[1];
        read(0, buf, 1);
	char key = buf[0];

	if (key == 'q') {
	  handleQuit();
	} else {
	  handlePlayerMove(key);
	}
      }
    }
  }

  pthread_exit(NULL);
}

void handlePlayerMove(char key) {
  int moveX = 0;
  int moveY = 0;

  switch (key) {
    case 'w':
      if ((playerY-1) == ROW_4_Y && playerIsInRangeOfABay()) {
        moveY = -(JUMP_LENGTH-1);
      } else if (playerY > ROW_3_Y) {
        moveY = -JUMP_LENGTH;
      }
      break;
    case 'a':
      if (playerX > 0) {
        moveX = -1;
      }
      break;
    case 's':
      if (playerY < PLAYER_STARTING_LOCATION_Y) {
        moveY = JUMP_LENGTH;
      }
      break;
    case 'd':
      if (playerX < RIGHT_BOUND) {
        moveX = 1;
      }
      break;
    default:
      break;
  }

  if (moveX != 0 || moveY != 0) {
    lockScreen();
    lockPlayer();
	  
    consoleClearImage(playerY, playerX, PLAYER_HEIGHT, strlen(tile[0]));
    playerX += moveX;
    playerY += moveY;
    consoleDrawImage(playerY, playerX, tile, PLAYER_HEIGHT);

    handlePositioning();

    unlockPlayer();
    unlockScreen();
  }
}

bool playerIsInRangeOfABay() {
  bool in_range = false;
  int i;

  for (i = 0; i < NUMBER_OF_BAYS && !in_range; i++) {
    if (!bays[i]->contains_frog) {
      if (playerX > bays[i]->left_border && playerX < bays[i]->right_border) {
        in_range = true;
      }
    }
  }

  return in_range;
}

void handlePositioning() {
  unlockPlayer();
  unlockScreen();
  lockLogList();
  lockScreen();
  lockPlayer();
  if (!validPlayerPosition()) {
    decrementPlayerLives();
  }

  if (playerY < ROW_4_Y) {
    consoleDrawImage(playerY, playerX, tile, PLAYER_HEIGHT);
    
    bool set = 0;
    int i;
    for (i = 0; i < NUMBER_OF_BAYS && !set; i++) {
      if (playerX > bays[i]->left_border && playerX < bays[i]->right_border) {
        bays[i]->contains_frog = true;
	set = true;
      }
    }

    assert(set);

    playerX = PLAYER_STARTING_LOCATION_X;
    playerY = PLAYER_STARTING_LOCATION_Y;

    frogs_at_home++;

    if (frogs_at_home == GOAL_FROGS_AT_HOME) {
      handleGameOver(WIN_MSG);
    }
  }
  
  unlockLogList();
}

void decrementPlayerLives() {
  if (getPlayerLives() > 1) {
    setPlayerLives(-1);
    consoleDrawImage(playerY, playerX, tile, PLAYER_HEIGHT);
    playerX = PLAYER_STARTING_LOCATION_X;
    playerY = PLAYER_STARTING_LOCATION_Y;

    // let the screen catch up in order to display the dead frog graphic
    unlockLogList();
    unlockScreen();
    unlockPlayer();
    sleepTicks(1);

    lockLogList();
    lockScreen();
    lockPlayer();

    safeSleep(50);
  } else {
    handleGameOver(NO_LIVES_MSG);
  }
}

bool validPlayerPosition() {
  bool valid = true;

  if (playerY < PLAYER_STARTING_LOCATION_Y) {
    if (playerY >= ROW_4_Y) {
      valid = frogIsOnLog();
    }
  }

  return valid;
}

bool frogIsOnLog() {
  LinkedList *log_list = getLogList();
  int on_log = false;
  int i = 0;
  int done = false;
  Log *curr_log = firstElement(log_list);
  
  while (!done && i < log_list->number_of_nodes) {
    assert(curr_log != NULL);

    // there's a chance there hasn't been a context switch yet, updating the contains_frog
    // parameter in the log class. so check here as well before determining if the player
    // is on the log or not
    checkForFrogOnLog(curr_log);

    if (curr_log->contains_frog && curr_log->on_screen) {
      on_log = true;
      done = true;
    }

    i++;
    curr_log = nextElement(log_list);
  }

  return on_log;
}

void handleGameOver(char *msg) {
  lockGameRunning();
  setGameStillRunning(false);
  conditionSignal();
  unlockGameRunning();
  
  putString(msg,
    PLAYER_STARTING_LOCATION_Y,
    PLAYER_STARTING_LOCATION_X-(strlen(msg)/2),
    strlen(msg));
  finalKeypress();

  playerAlive = false;
  
  unlockLogList();
  unlockPlayer();
  unlockScreen();
}

void handleQuit() {
  lockScreen();	
  putString(QUITTER_MSG,
    PLAYER_STARTING_LOCATION_Y,
    PLAYER_STARTING_LOCATION_X-(strlen(QUITTER_MSG)/2),
    strlen(QUITTER_MSG));
  finalKeypress();
  unlockScreen();

  lockPlayer();
  playerAlive = false;
  unlockPlayer();

  lockGameRunning();
  setGameStillRunning(false);
  conditionSignal();
  unlockGameRunning();
}
