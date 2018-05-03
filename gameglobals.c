/**********************************************************************
  Module: gameglobals.c
  Author: Will Coombs
  Date:   2017 June 13

  Purpose: see gameglobals.h

  Changes: 

  NOTES: None.
**********************************************************************/

#include <pthread.h>
#include <stdbool.h>

#include "gameglobals.h"
#include "frogger.h"
#include "player.h"
#include "threadwrappers.h"
#include "linkedlist.h"

char *GAME_BOARD[] = {
"                                   Lives:",
"/------\\          /------\\          /------\\          /------\\          /------\\",
"|      |          |      |          |      |          |      |          |      |",
"+      +----------+      +----------+      +----------+      +----------+      +",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"",
"",
"",
"" };

pthread_mutexattr_t attributes;

pthread_mutex_t gameRunningMutex;
pthread_mutex_t screenMutex;
pthread_mutex_t playerMutex;
pthread_mutex_t keyboardMutex;
pthread_mutex_t logManagersMutex;
pthread_mutex_t logListMutex;

pthread_cond_t gameRunningCond;
pthread_condattr_t gameRunningCondAttributes;

pthread_t screenThread;
pthread_t playerThread;
pthread_t keyboardThread;
pthread_t upkeepThread;
pthread_t logRow1Thread;
pthread_t logRow2Thread;
pthread_t logRow3Thread;
pthread_t logRow4Thread;

bool game_still_running;
int player_lives;
LinkedList *log_list;

void initializeLogList() {
  log_list = createList();
}

LinkedList *getLogList() {
  return log_list;
}

void setGameStillRunning(bool result) {
  game_still_running = result;
}

bool gameStillRunning() {
  return game_still_running;
}

void setPlayerLives(int amount) {
  player_lives += amount;
}

int getPlayerLives() {
  return player_lives;
}

void initializeMutexes() {
  pthread_mutexattr_init(&attributes);
  pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_NORMAL);
 
  pthread_cond_init(&gameRunningCond, NULL);
  
  pthread_mutex_init(&gameRunningMutex, &attributes);
  pthread_mutex_init(&screenMutex, &attributes);
  pthread_mutex_init(&playerMutex, &attributes);
  pthread_mutex_init(&keyboardMutex, &attributes);
  pthread_mutex_init(&logManagersMutex, &attributes);
  pthread_mutex_init(&logListMutex, &attributes);
}

void conditionWait() {
  pthread_cond_wait(&gameRunningCond, &gameRunningMutex);
}

void conditionSignal() {
  pthread_cond_signal(&gameRunningCond);
}

void destroyMutexes() {
  pthread_mutex_destroy(&gameRunningMutex);
  pthread_mutex_destroy(&screenMutex);
  pthread_mutex_destroy(&playerMutex);
  pthread_mutex_destroy(&keyboardMutex);
  pthread_mutex_destroy(&logManagersMutex);
  pthread_mutex_destroy(&logListMutex);
}

void lockGameRunning() {
  pthread_mutex_lock(&gameRunningMutex);
}

void lockScreen() {
  pthread_mutex_lock(&screenMutex);
}

void lockPlayer() {
  pthread_mutex_lock(&playerMutex);
}

void lockKeyboard() {
  pthread_mutex_lock(&keyboardMutex);
}

void lockLogManagers() {
  pthread_mutex_lock(&logManagersMutex);
}

void lockLogList() {
  pthread_mutex_lock(&logListMutex);
}

void unlockGameRunning() {
  pthread_mutex_unlock(&gameRunningMutex);
}

void unlockScreen() {
  pthread_mutex_unlock(&screenMutex);
}

void unlockPlayer() {
  pthread_mutex_unlock(&playerMutex);
}

void unlockKeyboard() {
  pthread_mutex_unlock(&keyboardMutex);
}

void unlockLogManagers() {
  pthread_mutex_unlock(&logManagersMutex);
}

void unlockLogList() {
  pthread_mutex_unlock(&logListMutex);
}
