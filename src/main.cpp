/*** includes ***/
#include <ctype.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <locale.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define MVER "1.0.0"

using namespace std;

void enableRawMode();
void disableRawMode();

void die(const char *c);

void processKey();
void refreshScreen();
void init();
void clear();
void exitAll();
void menu_Task();
void menu_Timer();
void menu_Help();

void help() {
  printf("Tasky - versión %s\n\n", MVER);
  printf("Este es un programa para ver y crear tareas, y poder monitorizar el tiempo, usando Sistemas de control del tiempo, como el Pomodoro.\n"); 
}

/*** init ***/

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING  0x0004
#endif

int main(int argc, char *argv[]) {
  char *locale;
  locale = setlocale(LC_ALL, "en_US.utf8");

  if (argc == 2) {
    std::string arg1 = argv[1];
    if (arg1 == "--help") {
      help();
      return 0;
    }
  }

  #ifdef _WIN32
  HANDLE hStdin = GetStdHandle(STD_OUTPUT_HANDLE); 
  DWORD mode = 0;
  GetConsoleMode(hStdin, &mode);
  mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(hStdin, mode);
  #endif
  /* Activa el modo alternativo, para no afectar el output anterior a correr el programa */
  write(STDOUT_FILENO, "\x1b[?1049h", 8);
  //enableRawMode();
  clear();
  //init();

  while (1) {
    printf("\x1b[H");
    string mag = "\x1b[1;95m";
    string clLine = "\x1b[K";
    string naimu = clLine + mag + "Tasky" + "\x1b[0m\n";
    string desci = clLine + "¡Esto es Tasky! Escribe la opción que desees" + "\n";
    printf("%s", naimu.c_str());
    printf("%s", desci.c_str());
    printf("\x1b[K\n");
    printf("\x1b[K[1] - Salir\n");
    printf("\x1b[K[2] - Lista de tareas\n");
    printf("\x1b[K[3] - Temporizador\n");
    printf("\x1b[K[4] - Ayuda\n");
    int opt;
    printf("\x1b[KSelecciona: \x1b[J");
    scanf("%d", &opt);

    switch (opt) {
      case 1:
        exitAll();
        break;
      case 2:
        menu_Task();
        break;
      case 3:
        menu_Timer();
        break;
      case 4:
        menu_Help();
        break;
      default:
        break;
    }
  }

  //disableRawMode();
  /* Desactiva el modo alternativo, regresando el output anterior al programa */
  write(STDOUT_FILENO, "\x1b[?1049l", 8);
  printf("\x1b[?25h");
  return 0;
}
