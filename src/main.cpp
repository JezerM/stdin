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

void enableRawMode(bool t = false);
void disableRawMode();

void die(const char *c);

void processKey();
void refreshScreen();
void init();
void clear();

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
  enableRawMode(true);
  clear();
  init();

  while (1) {
    refreshScreen();
    processKey();
  }

  disableRawMode();
  /* Desactiva el modo alternativo, regresando el output anterior al programa */
  write(STDOUT_FILENO, "\x1b[?1049l", 8);
  printf("\x1b[?25h");
  return 0;
}
