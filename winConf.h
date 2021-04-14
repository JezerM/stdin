#include <termios.h>

#ifndef WINCONF_H
#define WINCONF_H 1 // Estos son guards para evitar múltiples declaraciones

/*** data ***/

/* Estructura para la configuración de pantalla */
struct winConfig {
  int cx, cy; // Posición [x y] del cursor en pantalla
  int srows; // Filas de la ventana
  int scols; // Columnas de la ventana
  struct termios orig_termios; // La estructura termios
};

extern struct winConfig conf;

#endif
