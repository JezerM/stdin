#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

#include "menu.h"
#include "winConf.h"
#include "buff.h"

#include <vector>

#include <iostream>

#define CTRL_KEY(k) ((k) & 0x1f)

struct winConfig conf;
Menu menu("", &conf);

void initMenus();

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

/* Desabilita el modo "Raw" */
void disableRawMode() {
  write(STDOUT_FILENO, "\e[?1000l", 8);
  write(STDOUT_FILENO, "\e[?1006l", 8);
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &conf.orig_termios) == -1) {
    die("tcsetattr");
  }
}

/* Activa el modo "Raw", en el que no se imprimirán las teclas presionadas, y se pasarán como datos en "crudo", en ASCII */
void enableRawMode() {
  if (tcgetattr(STDIN_FILENO, &conf.orig_termios) == -1) {
    die("tcgetattr");
  }
  atexit(disableRawMode);

  struct termios raw = conf.orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  write(STDOUT_FILENO, "\e[?1000h", 8); // Para detectar el mouse
  write(STDOUT_FILENO, "\e[?1006h", 8); // Para formatearlo como valores decimales
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
    die("tcsetattr");
  } 
}

/* Espera hasta obtener una tecla y la regresa */
char readKey() {
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN) die("read");
  }
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

/*** output ***/

// Esto de aquí es de mero ejemplo, lo cual no nos sirve
/*
void editorDrawRows(struct abuf *ab) {
  int y;
  for (y = 0; y < conf.srows; y++) {
    if (y == conf.srows / 3) {
      char welcome[80];
      int welcomelen = snprintf(welcome, sizeof(welcome),
        "Use arrows, or khjl to move. CTRL+Q to exit");
      if (welcomelen > conf.scols) welcomelen = conf.scols;
      int padding = (conf.scols - welcomelen) / 2;
      if (padding) {
        abAppend(ab, "~", 1);
        padding--;
      }
      while (padding--) abAppend(ab, " ", 1);
      abAppend(ab, welcome, welcomelen);
    } else {
      abAppend(ab, "~", 1);
    }
    abAppend(ab, "\x1b[K", 3);
    if (y < conf.srows - 1) {
      abAppend(ab, "\r\n", 2);
    }
  }
}
*/

/* Actualiza la pantalla */
void refreshScreen() {
  ab = ABUF_INIT;

  abAppend(&ab, "\x1b[?25l", 6);
  abAppend(&ab, "\x1b[H", 3);

  menu.render(&ab);

  char buf[32];
  snprintf(buf, sizeof(buf), "\x1b[%d;%dH", conf.cy+1,conf.cx+1);
  abAppend(&ab, buf, strlen(buf));

  write(STDOUT_FILENO, ab.b, ab.len);
  abFree(&ab);
}

/*** init ***/

void init() {
  conf.cx = 0;
  conf.cy = 0;
  initMenus();
  if (getWindowSize(&conf.srows, &conf.scols) == -1) die("getWindowSize");
}

