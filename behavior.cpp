#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <errno.h>

#include "menu.h"

#define CTRL_KEY(k) ((k) & 0x1f)

void exitAll();
char readKey();

std::vector<Window*> listMenus;
Menu *mainMenu = new Menu("main", &conf);
Menu *firstMenu = new Menu("first", &conf);
Timer *tempo = new Timer("timer", &conf);

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

/* Obtiene el índice del menú actual */
int getActualMenuIndex() {
  int ind = 0;
  for (int i = 0; i < listMenus.size(); i++) {
    if (listMenus[i]->id == win->id) {
      ind = i; break;
    }
  }
  return ind;
}

/* Obtiene el índice del menú según el id */
int getMenuIndex(std::string id) {
  int ind = -1;
  for (int i = 0; i < listMenus.size(); i++) {
    if (listMenus[i]->id == id) {
      ind = i; break;
    }
  }
  return ind;
}

/* Esta cosa sirve para cambiar los menús de forma correcta... Espero... */
void swapMenu(int actualInd, int ind) {
  if (win->type == "Window") {
    listMenus[actualInd] = (Window*) win;
  } else if (win->type == "Menu") {
    listMenus[actualInd] = (Menu*) win;
  } else if (win->type == "Timer") {
    listMenus[actualInd] = (Timer*) win;
  }

  if (listMenus[ind]->type == "Window") {
    win = (Window*) listMenus[ind];
  } else
  if (listMenus[ind]->type == "Menu") {
    win = (Menu*) listMenus[ind];
  } else
  if (listMenus[ind]->type == "Timer") {
    win = (Timer*) listMenus[ind];
  }
}

/* Cambia el menú según el índice.
 * Si el índice excede el tamaño de la lista, no se hará nada.*/
void changeMenus(int ind) {
  if (ind >= listMenus.size()) return;
  int actualInd = getActualMenuIndex();
  swapMenu(actualInd, ind);
}

/* Cambia el menú según el id.
 * Si el id no existe en la lista, no se hará nada.*/
void changeMenus(std::string id) {
  int ind = getMenuIndex(id);
  if (ind == -1) {printf("NOPE!\r\n"); return;};
  int actualInd = getActualMenuIndex();
  swapMenu(actualInd, ind);
}

/* Aquí se manejarán las opciones y sus acciones */
void manageMenus(std::string element) {
  std::vector<std::string> arr = split(element, "/");
  if (arr[0] == "main") { // Main Menu
    if (arr[1] == "exit") {
      exitAll();
    } else
    if (arr[1] == "first") {
      changeMenus("first");
    } else
    if (arr[1] == "timer") {
      changeMenus("timer");
    }
  } else
  if (arr[0] == "first") { // First Menu
    if (arr[1] == "exit") {
      changeMenus("main");
    }
  } else
  if (arr[0] == "timer") {
    if (arr[1] == "exit") {
      changeMenus("main");
    }
  }

}

/* Mueve el cursor */
void moveCursor(std::string dir) {
  if (dir == "left") {
    if (win->id != "main")
    manageMenus(win->id + "/exit");
  } else if (dir == "right") {
    manageMenus(win->id + "/" + win->options[win->actualPos].id);
  } else if (dir == "up") {
      win->gotoPos((win->actualPos) - 1);
  } else if (dir == "down") {
      win->gotoPos((win->actualPos) + 1);
  }
}

std::string muse = "";
bool museRead = false;

/* Obtiene una serie de carácteres y determina si es una interacción con el mouse para así manejar la misma */
void manageMouse(char c) {
  int mx, my; // La posición del evento
  int b; // El número del botón
  bool h;
  if (c == 27) { // Si detecta el código ASCII 27, comienza a captar el mouse
    museRead = true;
    return;
  }
  if (muse.size() > 32) { // Si excede los 32 bytes, se reinicia y detiene
    muse = "";
    museRead = false;
  }
  if (museRead) { // Añade cada carácter a 'muse'
    muse += c;
  }
  if (c != 77 && c != 109) return; // Si no detecta una 'M'(77) ó 'm'(109) no continúa

  museRead = false;
  h = c == 77 ? true : false; // Si es 'M', está siendo presionado. Si es 'm', fue un click.
  muse.back() = '\0'; // Elimina el último carácter
  std::vector<std::string> splitted = split(muse, ";");
  if (splitted.size() != 3) {return;}
  b = std::stoi(splitted[0]);
  mx = std::stoi(splitted[1]);
  my = std::stoi(splitted[2]);
  // printf("%s\r\n", muse.c_str());
  /*
  if (b == 64 || b == 65) {
    printf("%s  on  %dx, %dy\r\n", b == 64 ? "Swipe up" : "Swipe down", mx, my);
  } else {
    printf("%s  on  %dx, %dy  %s\r\n", b == 1 ? "Middle" : b == 0 ? "Left" : "Right click", mx, my, h ? "holding" : "clicked");
  }*/

  if (b == 64) {
    moveCursor("up");
  }
  if (b == 65) {
    moveCursor("down");
  }

  muse = "";
}

/* Procesa las teclas leídas y realiza sus respectivas acciones */
void processKey() {
  char c = readKey();
  if (iscntrl(c)) {
    printf("%d\r\n", c);
  } else {
    printf("%d ('%c')\r\n", c, c);
  }
  manageMouse(c);
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
    exitAll();
  } else
  if (c == 'k') {
    moveCursor("up");
  } else
  if (c == 'j') {
    moveCursor("down");
  } else
  if (c == 'l') {
    moveCursor("right");
  } else
  if (c == 'h') {
    moveCursor("left");
  } else
  if (c == 13) { // "Return" key
    std::string element = win->id + "/" + win->options[win->actualPos].id;
    //manageMenus(element);
  }
  return;
}

/* Aquí se especificaran los menús y sus opciones */
void initMenus() {
  mainMenu->name = "Tasky";
  mainMenu->desc = "Muévete con las flechas de dirección o con hjkl. Presiona Enter para elegir la opción.";
  mainMenu->options = {
    MenuOption {"Lista de tareas", "first"},
    MenuOption {"Temporizador", "timer"},
    MenuOption {"Salir", "exit"},
  };

  mainMenu->gotoPos(0);

  win = mainMenu;

  firstMenu->name = "This is the first menu, not the main";
  firstMenu->desc = "Just that...";
  firstMenu->options = {
    MenuOption {"Primera opción", "fa"},
    //MenuOption {"Con sangría!", "ba", 1},
    MenuOption {"Atrás", "exit"},
  };

  tempo->name = "Temporizador";
  tempo->desc = "Esto es una prueba del temporizador";

  listMenus = {
    mainMenu,
    firstMenu,
    tempo,
  };
}

