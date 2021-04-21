#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "buff.h"

#define ABUF_INIT {NULL, 0}

struct abuf ab = ABUF_INIT;

/* Añade carácteres con una determinada longitud
 * La longitud debe de especificarse correctamente o provocará errores inesperados.
 * El buffer *ab* debe estar precedido de "&". Si se especifica como parámetro será con "*", y entonces su uso no necesitará de "&" ó "*"
 * Ejemplo:
 *     "abAppend(&ab, "ola", 3)"
 *     "int funcion(struct abuf *ab) {
 *         abAppend(ab, "hello", 5);
 *     }"
 */
void abAppend(struct abuf *ab, const char *s, int len) {
  char *ne;
  ne = (char*) realloc(ab->b, ab->len + len);
  if (ne == NULL) return;
  memcpy(&ne[ab->len], s, len);
  ab->b = ne;
  ab->len += len;
}

/* Libera el "caché" del buffer *ab* */
void abFree(struct abuf *ab) {
  free(ab->b);
}

/* Escribe una cadena de carácteres *string* por medio de abAppend
 * Ejemplo:
 *    abWrite("Hola a todos")
 * */
void abWrite(std::string s) {
  abAppend(&ab, s.c_str(), s.size());
}
