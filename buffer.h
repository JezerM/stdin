#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#ifndef BUFFER_H
#define BUFFER_H 1 // Estos son guards para evitar múltiples declaraciones

/*** buffer ***/

/* Estructura para el buffer */
struct abuf {
  char *b; // Carácteres
  int len; // Longitud o tamaño de carácteres
};

#define ABUF_INIT {NULL, 0}

void abAppend(struct abuf *ab, const char *s, int len);

void abFree(struct abuf *ab);

#endif
