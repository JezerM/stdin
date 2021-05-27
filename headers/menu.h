#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <ctime>
#include <unistd.h>

#include <thread>
#include <future>

#include "winConf.h"

#ifndef MENU_H
#define MENU_H 1 // Estos son guards para evitar múltiples declaraciones

extern struct winConfig conf;

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
  int stateCode; // 0 "Detenido", 1 "Corriendo", "2" Reiniciado

  bool running;
};

extern struct Timer tempo;

#endif
