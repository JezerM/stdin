#ifndef WINCONF_H
#define WINCONF_H 1 // Estos son guards para evitar múltiples declaraciones

/*** data ***/
#ifdef _WIN32

struct winConfig {
  int cx, cy;
  int srows;
  int scols;
  int actualMenu;
  char statusMessage[100];
};

extern struct winConfig conf;

#else

#include <termios.h>
/* Estructura para la configuración de pantalla */
struct winConfig {
  int cx, cy; // Posición [x y] del cursor en pantalla
  int srows; // Filas de la ventana
  int scols; // Columnas de la ventana
  int actualMenu;
  char statusMessage[100];
  struct termios orig_termios; // La estructura termios
};

extern struct winConfig conf;

#endif

#endif
