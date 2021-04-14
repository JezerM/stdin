/*** includes ***/
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
#include "buffer.h"

#include <vector>

#include <iostream>

#define CTRL_KEY(k) ((k) & 0x1f)

struct winConfig conf;

/*** terminal ***/

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

/* Limpia la pantalla */
void clear() {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
}

/* Alerta ante cualquier error de este sistema */
void die(const char *s) {
  clear();
  perror(s);
  exit(1);
}

/* Desabilita el modo "Raw" */
void disableRawMode() {
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

/*** Menus ***/

/* Realiza una división de *text* por cada *del*
 * @return vector<string>
 */
std::vector<std::string> split(std::string text, std::string del = " ") {
  std::vector<std::string> arr;
  int start = 0;
  int end = text.find(del);
  while (end != -1) {
    std::string s = text.substr(start, end - start);
    start = end + del.size();
    end = text.find(del, start);
    arr.push_back(s);
  }
  arr.push_back(text.substr(start, end - start));
  return arr;
}

Menu menu("", &conf);
std::vector<Menu> listMenus;
Menu mainMenu("main", &conf);
Menu firstMenu("first", &conf);

/*** input ***/

/* Obtiene el índice del menú actual */
int getActualMenuIndex() {
  int ind = 0;
  for (int i = 0; i < listMenus.size(); i++) {
    if (listMenus[i].id == menu.id) {
      ind = i; break;
    }
  }
  return ind;
}

/* Obtiene el índice del menú según el id */
int getMenuIndex(std::string id) {
  int ind = -1;
  for (int i = 0; i < listMenus.size(); i++) {
    if (listMenus[i].id == id) {
      ind = i; break;
    }
  }
  return ind;
}

/* Cambia el menú según el índice.
 * Si el índice excede el tamaño de la lista, no se hará nada.*/
void changeMenus(int ind) {
  if (ind >= listMenus.size()) return;
  int actualInd = getActualMenuIndex();
  listMenus[actualInd] = menu;
  menu = listMenus[ind];
}

/* Cambia el menú según el id.
 * Si el id no existe en la lista, no se hará nada.*/
void changeMenus(std::string id) {
  int menuInd = getMenuIndex(id);
  if (menuInd == -1) return;
  int actualInd = getActualMenuIndex();
  listMenus[actualInd] = menu;
  menu = listMenus[menuInd];
}

/* Aquí se manejarán las opciones y sus acciones */
void manageMenus(std::string element) {
  std::vector<std::string> arr = split(element, "/");
  if (arr[0] == "main") { // Main Menu
    if (arr[1] == "exit") {
      clear();
      printf("\x1b[?25h");
      exit(0);
    } else
    if (arr[1] == "first") {
      clear();
      changeMenus("first");
    }
  } else
  if (arr[0] == "first") { // First Menu
    if (arr[1] == "exit") {
      clear();
      changeMenus("main");
    }
  }

}

/* Mueve el cursor */
void moveCursor(std::string dir) {
  if (dir == "left") {
    if (menu.id != "main")
    manageMenus(menu.id + "/exit");
  } else if (dir == "right") {
    manageMenus(menu.id + "/" + menu.options[menu.actualPos].id);
  } else if (dir == "up") {
      menu.gotoPos(menu.actualPos-1);
  } else if (dir == "down") {
      menu.gotoPos(menu.actualPos+1);
  }
}

/* Procesa las teclas leídas y realiza sus respectivas acciones */
void processKey() {
  char c = readKey();

  if (c == '\x1b') {
    char seq[3];
    if (read(STDIN_FILENO, &seq[0], 1) != 1) {}
    else if (read(STDIN_FILENO, &seq[1], 1) != 1) {}
    else if (seq[0] == '[') {
      switch (seq[1]) {
        case 'A': c = 'k'; break;
        case 'B': c = 'j'; break;
        case 'C': c = 'l'; break;
        case 'D': c = 'h'; break;
      }
    }
  }

  if (c == CTRL_KEY('q')) { // CTRL+Q
    clear();
    printf("\x1b[?25h");
    exit(0);
  }
  if (c == 'k') {
    moveCursor("up");
  }
  if (c == 'j') {
    moveCursor("down");
  }
  if (c == 'l') {
    moveCursor("right");
  }
  if (c == 'h') {
    moveCursor("left");
  }
  if (c == 13) { // "Return" key
    std::string element = menu.id + "/" + menu.options[menu.actualPos].id;
    manageMenus(element);
  }
}

/*** output ***/

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

/* Aquí se especificaran los menús y sus opciones */
void initMenus() {
  mainMenu.name = "Testing Interface";
  mainMenu.desc = "Use arrows or hjkl to move. CTRL+Q to close the program. Press Enter to select the option.";

  MenuOption first = {"First option", "first"};
  MenuOption other = {"Other option", "other"};
  MenuOption exi = {"Salir", "exit"};
  mainMenu.options[0] = first;
  mainMenu.options[1] = other;
  mainMenu.options[2] = exi;

  mainMenu.gotoPos(0);

  menu = mainMenu;

  firstMenu.name = "This is the first menu, not the main";
  firstMenu.desc = "Just that...";
  MenuOption fa = {"Fa", "fa"};
  MenuOption ba = {"Ba", "ba", 1};
  MenuOption back = {"Atrás", "exit"};
  firstMenu.options[0] = fa;
  firstMenu.options[1] = ba;
  firstMenu.options[2] = back;

  listMenus.push_back(mainMenu);
  listMenus.push_back(firstMenu);
}

/* Actualiza la pantalla */
void refreshScreen() {
  struct abuf ab = ABUF_INIT;

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

