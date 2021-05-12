#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <ctime>
#include <unistd.h>

#ifdef _WIN32

#include "mingw.thread.h"
#include "mingw.future.h"

#else

#include <thread>
#include <future>

#endif

#include "winConf.h"

#ifndef MENU_H
#define MENU_H 1 // Estos son guards para evitar múltiples declaraciones

extern struct winConfig conf;

int getWindowSize(int *rows, int *cols);

struct Timer {
  int time = 0;
  int seconds;
  int minutes;
  int hours;

  int shortBreak;
  int longBreak;

  int mode;
  int loop;
  int phase = 0;
  char state[50] = "Detenido";

  bool running;
};

extern struct Timer tempo;

void getch();
void restartTimer();

#endif
