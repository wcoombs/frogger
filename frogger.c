/**********************************************************************
  Module: frogger.c
  Author: Will Coombs
  Date:   2017 June 13

  Purpose: see frogger.h

  Changes: 

  NOTES: None.
**********************************************************************/

#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <stdbool.h>

#include "console.h"
#include "frogger.h"
#include "linkedlist.h"
#include "threadwrappers.h"
#include "player.h"
#include "log.h"
#include "gameglobals.h"

#define LIVES_Y (GAME_COLS/2)+2

extern char *GAME_BOARD[];

pthread_t screenThread;
pthread_t playerThread;
pthread_t keyboardThread;
pthread_t upkeepThread;
pthread_t logRow1Thread;
pthread_t logRow2Thread;
pthread_t logRow3Thread;
pthread_t logRow4Thread;

void executeGame() {
  initialize();
}

void initialize() {
  lockGameRunning();
  setGameStillRunning(true);
  unlockGameRunning();

  initializeLogList();
  createBays();

  int rc1 = createSafeThread(&screenThread, NULL, refreshScreen, NULL);
  int rc2 = createSafeThread(&playerThread, NULL, animatePlayer, NULL);
  int rc3 = createSafeThread(&keyboardThread, NULL, keyboardInput, NULL);
  int rc4 = createSafeThread(&upkeepThread, NULL, upkeep, NULL);
  int rc5 = createSafeThread(&logRow1Thread, NULL, manageRow1, NULL);
  int rc6 = createSafeThread(&logRow2Thread, NULL, manageRow2, NULL);
  int rc7 = createSafeThread(&logRow3Thread, NULL, manageRow3, NULL);
  int rc8 = createSafeThread(&logRow4Thread, NULL, manageRow4, NULL);

  // put the main program thread to sleep and wait for a signal to cleanup
  lockGameRunning();
  while (gameStillRunning()) {
    conditionWait();
  }
  unlockGameRunning();

  rc2 = joinSafeThread(playerThread, NULL);
  rc4 = joinSafeThread(upkeepThread, NULL);
  rc5 = joinSafeThread(logRow1Thread, NULL);
  rc6 = joinSafeThread(logRow2Thread, NULL);
  rc7 = joinSafeThread(logRow3Thread, NULL);
  rc8 = joinSafeThread(logRow4Thread, NULL);
  rc3 = joinSafeThread(keyboardThread, NULL);  
  rc1 = joinSafeThread(screenThread, NULL);

  exitGame();
  pthread_exit(NULL);
}

void *refreshScreen(void *place_holder) {
  lockScreen();
  bool clean = consoleInit(GAME_ROWS, GAME_COLS, GAME_BOARD);
  unlockScreen();

  if (clean) {
    while (gameStillRunning()) {
      lockScreen();
      consoleRefresh();
      unlockScreen();
      sleepTicks(1);
    }

    lockScreen();  
    consoleFinish();
    unlockScreen();
  }

  pthread_exit(NULL);
}

void *upkeep(void *place_holder) {
  int prevLives = getPlayerLives();
  char *livesAsString = (char *)malloc(sizeof(char));

  sprintf(livesAsString, "%d", getPlayerLives());

  lockScreen();
  putString(livesAsString, 0, LIVES_Y, strlen(livesAsString));
  free(livesAsString);
  unlockScreen();

  while (gameStillRunning()) {
    safeSleep(5);
    lockLogList();
    cleanupDeadLogs();
    unlockLogList();

    if (prevLives != getPlayerLives()) {
      lockScreen();
      lockPlayer();
      prevLives--;
      unlockPlayer();
      unlockScreen();

      char *livesAsString = (char *)malloc(sizeof(char));
      sprintf(livesAsString, "%d", getPlayerLives());

      lockScreen();
      putString(livesAsString, 0, LIVES_Y, strlen(livesAsString));
      unlockScreen();

      free(livesAsString);
    }
  }

  pthread_exit(NULL);
}

void cleanupDeadLogs() {
  LinkedList *log_list = getLogList();
  int i = 0;
  Log *curr_log = firstElement(log_list);
  bool done = false;

  while (i < log_list->number_of_nodes && !done) {
    assert(curr_log != NULL);

    if (curr_log->has_been_joined) {
      bool result = delete(log_list, curr_log);
      if (!result) {
        perror("deleting a log");
        exit(-1);
      }

      done = true;
    }

    i++;
    curr_log = nextElement(log_list);
  }
}

void exitGame() {
  LinkedList *log_list = getLogList();

  destroyList(log_list);
  free(log_list);
  destroyBays();
  destroyMutexes();;

  printf("Done processing.\n");
}
