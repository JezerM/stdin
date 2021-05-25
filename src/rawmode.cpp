#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <errno.h>

#include "menu.h"
#include "winConf.h"
#include "global.h"

#include <vector>

#include <iostream>

#define CTRL_KEY(k) ((k) & 0x1f)

struct winConfig conf;

/*** terminal ***/

/* Limpia la pantalla */
void clear() {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
}

/* Alerta ante cualquier error de este sistema */
void die(const char *s) {
  write(STDOUT_FILENO, "\x1b[?1049l", 8);
  printf("\x1b[?25h");
  perror(s);
  exit(1);
}

/* Función para salir del programa */
void exitAll() {
  write(STDOUT_FILENO, "\e[?1049l", 8);
  printf("\x1b[?25h");
  exit(0);
}

#ifdef _WIN32

#include <windows.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_INPUT
#define ENABLE_VIRTUAL_TERMINAL_INPUT 0x0200
#endif

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING  0x0004
#endif

DWORD initialMode;
DWORD initialOMode;

void disableRawMode() {
  //write(STDOUT_FILENO, "\e[?1000l", 8);
  //write(STDOUT_FILENO, "\e[?1006l", 8);
  HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE); 
  HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleMode(hStdin, initialMode);
  SetConsoleMode(hStdout, initialOMode);
}

void enableRawMode() {
  atexit(disableRawMode);
  HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
  HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD mode = 0;
  DWORD oMode = 0;
  GetConsoleMode(hStdin, &mode);
  GetConsoleMode(hStdout, &oMode);
  initialMode = mode;
  initialOMode = oMode;

  //mode |= (ENABLE_VIRTUAL_TERMINAL_INPUT);
  mode &= ~(ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT | ENABLE_ECHO_INPUT | ENABLE_QUICK_EDIT_MODE);
  SetConsoleMode(hStdin, mode);

  oMode |= (DISABLE_NEWLINE_AUTO_RETURN);
  oMode &= ~(ENABLE_PROCESSED_OUTPUT);
  SetConsoleMode(hStdout, oMode);

  //write(STDOUT_FILENO, "\e[?1000h", 8); // Para detectar el mouse
  //write(STDOUT_FILENO, "\e[?1006h", 8); // Para formatearlo como valores decimales
  SetConsoleCP(CP_UTF8);
  SetConsoleOutputCP(CP_UTF8);
}

#else

#include <termios.h>
#include <sys/ioctl.h>

/* Desabilita el modo "Raw" */
void disableRawMode() {
  //write(STDOUT_FILENO, "\e[?1000l", 8);
  //write(STDOUT_FILENO, "\e[?1006l", 8);
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &conf.orig_termios) == -1) {
    die("tcsetattr");
  }
}

/* Activa el modo "Raw", en el que no se imprimirán las teclas presionadas, y se pasarán como datos en "crudo", en ASCII */
void enableRawMode(bool t = false) {
  if (tcgetattr(STDIN_FILENO, &conf.orig_termios) == -1) {
    die("tcgetattr");
  }
  atexit(disableRawMode);

  struct termios raw = conf.orig_termios; // Modifica el modo original
  /* Input modes: no break, no CR to NL, no parity check, no strip char,
   * no start/stop output control*/
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  /* Output modes: disable post processing */
  raw.c_oflag &= ~(OPOST);
  /* Control modes: set 8 bit characters */
  raw.c_cflag |= (CS8);
  /* Local modes: choing off, canonical off, no extended functions,
   * no signal characters (^Z, ^C)*/
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  /* Control chars: set return condition of min number of bytes and timer */
  if (t) {
    raw.c_cc[VMIN] = 0; // Especifica el tamaño a obtener para enviar un resultado a read
    raw.c_cc[VTIME] = 2; // El tiempo en milisegundos a esperar para enviar el resultado a read
  }

  //write(STDOUT_FILENO, "\e[?1000h", 8); // Para detectar el mouse
  //write(STDOUT_FILENO, "\e[?1006h", 8); // Para formatearlo como valores decimales
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
    die("tcsetattr");
  } 
}
#endif

void rawTimer(bool mode) {
  struct termios actual;
  if (tcgetattr(STDIN_FILENO, &actual) == -1) {
    die("tcgetattr");
  }

  struct termios raw = actual; // Modifica el modo original
  if (mode) {
    raw.c_cc[VMIN] = 0; // Especifica el tamaño a obtener para enviar un resultado a read
    raw.c_cc[VTIME] = 2; // El tiempo en milisegundos a esperar para enviar el resultado a read
  } else {
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = -1;
  }

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
    die("tcsetattr");
  } 
}

/* Espera hasta obtener una tecla y la regresa */
char readKey() {
  int nread;
  char c = '\0';
  nread = read(STDIN_FILENO,&c,1) == 0;
    if (nread == -1 && errno != EAGAIN) die("read");
  return c;
}

/* Obtiene la posición del cursor y las regresa por sus argumentos */
int getCursorPosition(int *rows, int *cols) {
  char buf[32];
  unsigned int i = 0;
  if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;
  while (i < sizeof(buf) - 1) {
    if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
    if (buf[i] == 'R') break;
    i++;
  }
  buf[i] = '\0';
  if (buf[0] != '\x1b' || buf[1] != '[') return -1;
  if (sscanf(&buf[2], "%d;%d", rows, cols) != 2) return -1;
  return 0;
}

#ifdef _WIN32
int getWindowSize(int *rows, int *cols) {
  CONSOLE_SCREEN_BUFFER_INFO csbi;

  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  *cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

  return 0;
}

#else
/* Obtiene el tamaño de la ventana y las pasa por sus argumentos */
int getWindowSize(int *rows, int *cols) {
  struct winsize ws;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
    return getCursorPosition(rows, cols);
  } else {
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
  }
}
#endif

void getch() {
  enableRawMode();
  readKey();
  disableRawMode();
}

