#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <future>
#include <ctime>
#include <unistd.h>
#include <thread>

#include "winConf.h"
#include "buff.h"

#ifndef MENU_H
#define MENU_H 1 // Estos son guards para evitar múltiples declaraciones

extern struct winConfig conf;

int getWindowSize(int *rows, int *cols);

/*** Menus ***/

/* Estructura para crear opciones de menú */
struct MenuOption {
  char name[50]; // El nombre a mostrar en pantalla
  char id[20]; // El identificador
  int indentTimes = 0; // La cantidad de veces que usará *indentSpace* sobre la base
  int posx = 0; // La posición x en pantalla (No inicializar manualmente)
  int posy = 0; // La posición y en pantalla (No inicializar manualmente)
};

/* Estructura de una Ventana o Menú */
struct Window {
  char name[50];
  char id[50];
  char desc[150];
  int indentSpace = 4;
  int actualPos = 0;
  std::vector<MenuOption> options;
};

/* Estructura del Temporizador */
struct Timer: Window {
  int time;
  int seconds;
  int minutes;
  int hours;
  bool running;
};

static MenuOption nullOption = {"NULL", "none", 0, 0};

extern struct Window mainMenu;
extern struct Window firstMenu;
extern struct Timer tempo;
extern std::vector<Window*> listMenus;

#endif
