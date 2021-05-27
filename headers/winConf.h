
#include <termios.h>

#ifndef WINCONF_H
#define WINCONF_H 1 // Estos son guards para evitar múltiples declaraciones

/*** data ***/

/* Estructura para la configuración de pantalla */
struct winConfig {
  int cx, cy; // Posición [x y] del cursor en pantalla
  int srows; // Filas de la ventana
  int scols; // Columnas de la ventana
  int actualMenu;
  char statusMessage[100];
  int changed; // 0 para 'no cambiado', 1 para 'cambiado'
  struct termios orig_termios; // La estructura termios
};

extern struct winConfig conf;

struct lessConf {
  int pos = 0;
  int cols;
  int rows;
  int lines = 0;
  bool running = false;
  char search[20];
};

#endif
