
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
  int pos = 0; // La posición del cursor.
  int cols; // la cantidad de columnas máximas, determinada por el tamaño de la terminal.
  int rows; // la cantidad de filas máximas, determinada por el tamaño de la terminal.
  int lines = 0; // La cantidad de líneas del texto
  bool running = false; // Indica si la función está o no corriendo.
  char search[20]; // Usado para buscar por términos. 
};

#endif
