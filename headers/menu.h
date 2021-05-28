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
  int time = 0; // Tiempo total en cualquiera de los temporizadores.
  int seconds; // Los segundos a mostrar en pantalla.
  int minutes; // Los minutos a mostrar en pantalla.
  int hours; // Las horas a mostrar en pantalla.

  int shortBreak; // El tiempo de descanso corto (Pomodoro).
  int longBreak; // El tiempo de descanso largo (Pomodoro).

  int mode; // El modo, Temporizador o Pomodoro.
  int loop; // La cantidad de veces que se ha hecho un descanso corto.
  int phase = 0;  // La fase del Pomodoro. 0 para trabajo, 1 para descanso corto, 2 para descanso largo
  char state[50] = "Detenido"; // El estado a mostrar en pantalla.
  int stateCode; // El código del estado. 0 "Detenido", 1 "Corriendo", 2 "Reiniciado".

  bool running; // Si el temporizador está corriendo o no.
};

extern struct Timer tempo;

#endif
