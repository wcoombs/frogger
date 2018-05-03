/**********************************************************************
  Module: log.c
  Author: Will Coombs
  Date:   2017 June 13

  Purpose: see log.h

  Changes: 

  NOTES: None.
**********************************************************************/

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdbool.h>

#include "log.h"
#include "console.h"
#include "frogger.h"
#include "linkedlist.h"
#include "threadwrappers.h"
#include "gameglobals.h"
#include "player.h"

#define LOG_ANIM_TILES 8
#define LOG_HEIGHT 4
#define NUM_THREADS_PER_ROW 5

#define LEFT_BOUNDARY (-(strlen(LOG_GRAPHIC[0][0])))-5
#define RIGHT_BOUNDARY (strlen(LOG_GRAPHIC[0][0])+GAME_COLS)+5
#define LEFT_DEAD_BOUNDARY -30
#define RIGHT_DEAD_BOUNDARY 130

static char* LOG_GRAPHIC[LOG_ANIM_TILES][LOG_HEIGHT+1] = {
  {"-=======================\\",
   "|                       |",
   "|                       |",
   "\\=======================+"},

  {"/=======================\\",
   "-   x                   |",
   "| x                     +",
   "\\=======================/"},

  {"/=======================\\",
   "|    x                  +",
   "-      x                |",
   "\\=======================/"},

  {"/=======================+",
   "|         x             |",
   "|       x               |",
   "-=======================/"},

  {"/==================+====\\",
   "|          x            |",
   "|            x          |",
   "\\====-==================/"},

  {"/=============+=========\\",
   "|               x       |",
   "|             x         |",
   "\\=========-=============/"},

  {"/=========+=============\\",
   "|                x      |",
   "|                  x    |",
   "\\=============-=========/"},

  {"/====+==================\\",
   "|                     x |",
   "|                   x   |",
   "\\==================-====/"},
};

extern int playerX;
extern int playerY;

extern char **tile;

pthread_t row1Threads[NUM_THREADS_PER_ROW];
pthread_t row2Threads[NUM_THREADS_PER_ROW];
pthread_t row3Threads[NUM_THREADS_PER_ROW];
pthread_t row4Threads[NUM_THREADS_PER_ROW];

void createLog(char row_number, int array_index) {
  LinkedList *log_list = getLogList();
  Log *new_log = (Log *)malloc(sizeof(Log));
  new_log->contains_frog = false;
  new_log->on_screen = true;
  new_log->curr_log_tile = LOG_GRAPHIC[0];
  new_log->curr_graphic_index = 0;
  new_log->animate_sleep = 0;
  new_log->move_sleep = 0;
  new_log->has_been_joined = false;

  switch (row_number) {
    case '1':
      new_log->row = ROW_1_Y;
      new_log->currX = LEFT_BOUNDARY;
      new_log->displacement = 1;
      new_log->max_animate = 20;
      new_log->max_move = 9;
      new_log->thread_id = row1Threads[array_index];
      break;
    case '2':
      new_log->row = ROW_2_Y;
      new_log->currX = RIGHT_BOUNDARY;
      new_log->displacement = -1;
      new_log->max_animate = 15;
      new_log->max_move = 7;
      new_log->thread_id = row2Threads[array_index];
      break;
    case '3':
      new_log->row = ROW_3_Y;
      new_log->currX = LEFT_BOUNDARY;
      new_log->displacement = 1;
      new_log->max_animate = 10;
      new_log->max_move = 5;
      new_log->thread_id = row3Threads[array_index];
      break;
    case '4':
      new_log->row = ROW_4_Y;
      new_log->currX = RIGHT_BOUNDARY;
      new_log->displacement = -1;
      new_log->max_animate = 5;
      new_log->max_move = 3;
      new_log->thread_id = row4Threads[array_index];
      break;
    default:
      free(new_log);
      new_log = NULL;
      break;
  }

  lockLogList();
  bool result = add(log_list, new_log);
  unlockLogList();

  if (!result) {
    perror("adding to log list");
    exit(-1);
  }
}

void *logHandler(void *args) {
  Thread_Args *the_args = args;
  createLog(the_args->row_number, the_args->array_index);
  LinkedList *log_list = getLogList();

  while (firstElement(log_list) == NULL) {
    // wait for at least one log to be added to the list
  }

  while (gameStillRunning()) {
    lockLogList();
    Log *curr_log = nextElement(log_list);

    assert(curr_log != NULL);
    if (curr_log->on_screen && curr_log->thread_id == pthread_self()) {
      if (curr_log->animate_sleep == 0) {
        animateLog(curr_log);
      }
      if (curr_log->move_sleep == 0) {
        moveLog(curr_log);
      }
      
      curr_log->animate_sleep = (curr_log->animate_sleep+1) % curr_log->max_animate;
      curr_log->move_sleep = (curr_log->move_sleep+1) % curr_log->max_move;

      if (!curr_log->on_screen) {
        unlockLogList();
        pthread_exit(NULL);
      } else {
        unlockLogList();
        sleepTicks(1);
      }
    } else {
      unlockLogList();
    }
  }

  pthread_exit(NULL);
}

// Animate and move are in their own separate functions and have their own separate
// max_speed parameters in the Log struct, so that the movespeed on a log has no effect
// on the animation speed of it - they can both be set independantly of one another
void animateLog(Log *curr_log) {
  lockScreen();

  int tile_index = curr_log->curr_graphic_index;
  consoleClearImage(
    curr_log->row,
    curr_log->currX,
    LOG_HEIGHT,
    strlen(curr_log->curr_log_tile[0])
  );
  consoleDrawImage(curr_log->row, curr_log->currX, curr_log->curr_log_tile, LOG_HEIGHT);
  
  if (curr_log->contains_frog) {
    lockPlayer();
    consoleDrawImage(playerY, playerX, tile, PLAYER_HEIGHT);
    unlockPlayer();
  }
  
  curr_log->curr_log_tile = LOG_GRAPHIC[(tile_index+1)%LOG_ANIM_TILES];
  curr_log->curr_graphic_index = (tile_index+1)%LOG_ANIM_TILES;
      
  unlockScreen();
}

void moveLog(Log *curr_log) {
  lockScreen();

  int x = curr_log->currX;
  consoleClearImage(
    curr_log->row,
    curr_log->currX,
    LOG_HEIGHT,
    strlen(curr_log->curr_log_tile[0])
  );
  curr_log->currX = x+curr_log->displacement;
  consoleDrawImage(curr_log->row, curr_log->currX, curr_log->curr_log_tile, LOG_HEIGHT);

  lockPlayer();
  checkForFrogOnLog(curr_log);

  if (validPlayerPosition()) {
    if (curr_log->contains_frog) {
      switch (curr_log->row) {
        case ROW_1_Y:
        case ROW_3_Y:
          if (playerX <= (RIGHT_BOUND-1)) {
	    playerX += curr_log->displacement;
	  }
          break;
        case ROW_2_Y:
        case ROW_4_Y:
          if (playerX >= 1) {
	    playerX += curr_log->displacement;
	  }
          break;
        default:
          break;
      }

      consoleDrawImage(playerY, playerX, tile, PLAYER_HEIGHT);
    }
  } else {
    decrementPlayerLives();
  }

  unlockPlayer();

  switch (curr_log->row) {
    case ROW_1_Y:
    case ROW_3_Y:
      if (curr_log->currX > RIGHT_DEAD_BOUNDARY) {
        curr_log->on_screen = false;
      }
      break;
    case ROW_2_Y:
    case ROW_4_Y:
      if (curr_log->currX < LEFT_DEAD_BOUNDARY) {
        curr_log->on_screen = false;
      }
      break;
    default:
      break;
  }

  unlockScreen();
}

void checkForFrogOnLog(Log *curr_log) {
  int left = curr_log->currX + 1;
  int right = curr_log->currX + strlen(LOG_GRAPHIC[0][1]) - 3;

  if (playerX >= left && playerX <= right && (playerY-1) == curr_log->row) {
    curr_log->contains_frog = true;
  } else {
    curr_log->contains_frog = false;
  }
}

void *manageRow1(void *place_holder) {
  manageRow(300, 450, 0, 0, NUM_THREADS_PER_ROW-1, 0, row1Threads, '1', ROW_1_Y);
  pthread_exit(NULL);
}

void *manageRow2(void *place_holder) {
  manageRow(300, 550, 0, 0, NUM_THREADS_PER_ROW-1, 0, row2Threads, '2', ROW_2_Y);
  pthread_exit(NULL);
}

void *manageRow3(void *place_holder) {
  manageRow(250, 400, 0, 0, NUM_THREADS_PER_ROW-1, 0, row3Threads, '3', ROW_3_Y);
  pthread_exit(NULL);
}

void *manageRow4(void *place_holder) {
  manageRow(125, 250, 0, 0, NUM_THREADS_PER_ROW-1, 0, row4Threads, '4', ROW_4_Y);
  pthread_exit(NULL);
}
 
void manageRow(int sleep_min,
  int sleep_max,
  int i,
  int next_thread_to_delete,
  int end,
  int rc,
  pthread_t rowThreads[],
  char row,
  int row_coordinate
  ) {

  LinkedList *log_list = getLogList();
  while (gameStillRunning()) {
    if (rowThreads[end] == 0) {
      Thread_Args args;
      args.row_number = row;
      args.array_index = i;
      rc = createSafeThread(&rowThreads[i], NULL, logHandler, (void *)&args);
  
      i = (i+1) % NUM_THREADS_PER_ROW;
    }

    safeSleep(getRandomIntegerBetween(sleep_min, sleep_max));

    lockLogList();
    int k = 0;
    bool done = false;
    Log *curr_log = firstElement(log_list);

    while (!done && k < log_list->number_of_nodes) {
      if (!curr_log->on_screen && curr_log->row == row_coordinate) {
	rc = joinSafeThread(rowThreads[next_thread_to_delete], NULL);
	rowThreads[next_thread_to_delete] = 0;
	curr_log->has_been_joined = true;

	next_thread_to_delete = (next_thread_to_delete+1) % NUM_THREADS_PER_ROW;
	end = (end+1) % NUM_THREADS_PER_ROW;
	done = true;
      } 
    
      curr_log = nextElement(log_list);
      k++;
    }

    unlockLogList();
  }
}

int getRandomIntegerBetween(int min, int max) {
  int number = -1;
  int bounds = (max-min)+1;

  srand(time(NULL));
  number = rand() % (bounds) + min;

  return number;
}
