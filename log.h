/**********************************************************************
  Module: log.h
  Author: Will Coombs
  Date:   2017 June 13

  Purpose: Maintain log logic and draw/animate the logs on screen.

  Changes: 

  NOTES: None.
**********************************************************************/

#ifndef LOG_H
#define LOG_H

#include <stdbool.h>

#define ROW_1_Y 16
#define ROW_2_Y 12
#define ROW_3_Y 8
#define ROW_4_Y 4

typedef struct LOG Log;
struct LOG {
  bool contains_frog;
  int row;
  int currX;
  int displacement;
  bool on_screen;
  char **curr_log_tile;
  int curr_graphic_index;
  int animate_sleep;
  int max_animate;
  int move_sleep;
  int max_move;
  pthread_t thread_id;
  bool has_been_joined;
};

/* Function that creates a log, given a row number and array index
 * for the thread to use */
void createLog(char row_number, int array_index);

/* Function that handles the various tasks of a log, like moving
 * and animating it */
void *logHandler(void *place_holder);

/* Function that animates a given log */
void animateLog(Log *log);

/* Function that moves a given log */
void moveLog(Log *log);

/* Function that checks if the frog player is on the given log */
void checkForFrogOnLog(Log *log);


/* Functions that create the parameters for each row, then calls the
 * manageRow function with it's unique parameters */
void *manageRow1(void *place_holder);
void *manageRow2(void *place_holder);
void *manageRow3(void *place_holder);
void *manageRow4(void *place_holder);

/* Function that manages each row, and is in charge of spawning logs
 * upon the row and joining the log's thread when it has gone offscreen */
void manageRow(int sleep_min,
  int sleep_max,
  int i,
  int next_thread_to_delete,
  int end,
  int rc,
  pthread_t rowThreads[],
  char row,
  int row_coordinate
);

/* Function that returns a random integer between the given min and max */
int getRandomIntegerBetween(int min, int max);

#endif
