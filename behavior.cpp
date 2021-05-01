#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <errno.h>

#include "menu.h"
#include "buff.h"

#define CTRL_KEY(k) ((k) & 0x1f)

using namespace std;

void exitAll();
char readKey();

vector<Window*> listMenus;
struct Window mainMenu;
struct Window firstMenu;
struct Timer tempo;

/* Realiza una división de *text* por cada *del*
 * @return vector<string>
 */
vector<string> split(string text, string del = " ") {
  vector<string> arr;
  int start = 0;
  int end = text.find(del);
  while (end != -1) {
    string s = text.substr(start, end - start);
    start = end + del.size();
    end = text.find(del, start);
    arr.push_back(s);
  }
  arr.push_back(text.substr(start, end - start));
  return arr;
}

void changeMenus(int ind) {
  if (ind < 0) return;
  if (ind > listMenus.size() - 1) return;
  conf.actualMenu = ind;
}

/* Aquí se manejarán las opciones y sus acciones */
void manageMenus(std::string element) {
  std::vector<std::string> arr = split(element, "/");
  if (arr[0] == "main") { // Main Menu
    if (arr[1] == "exit") {
      exitAll();
    } else
    if (arr[1] == "first") {
      changeMenus(1);
    } else
    if (arr[1] == "timer") {
      changeMenus(2);
    }
  } else
  if (arr[0] == "first") { // First Menu
    if (arr[1] == "exit") {
      changeMenus(0);
    }
  } else
  if (arr[0] == "timer") {
    if (arr[1] == "exit") {
      changeMenus(0);
    }
  }
}

/* Renderiza el menú */
void render_Menu(struct Window *menu) {
  string mag = "\x1b[1;95m";
  string clLine = "\x1b[K";
  string neimu = clLine + mag + menu->name + "\x1b[0m" + "\r\n";
  string desci = clLine + menu->desc + "\r\n\x1b[K\r\n";
  abWrite(neimu);
  abWrite(desci);

  std::string symb = "\u001b[1;94m\u276F \u001b[0m";

  for (int i = 0; i < menu->options.capacity(); i++) {
    if (i >= menu->options.size()) {
      abWrite("\x1b[K\r\n");
      continue;
    }
    // if (strcmp(options[i].name, "NULL") == 0) continue;
    abWrite("\x1b[K");
    string inden = string(menu->indentSpace-2, ' ');
    abWrite(inden);
    if (menu->actualPos == i) {
      abWrite(symb);
      abWrite("\u001b[1;94m");
    }
    else abWrite(string(2, ' '));
    abWrite(string(menu->options[i].indentTimes * menu->indentSpace, ' '));
    abWrite(string(menu->options[i].name));
    abWrite("\u001b[0m");
    abWrite("\r\n");
  }
  //abWrite("AAA " + to_string(menu.options.capacity()));
  abWrite("\x1b[J");
}

void runTimer(struct Timer *timer);

/* Renderiza el temporizador */
void render_Temp(struct Timer *menu) {
  // std::future<void> hand = std::async(std::launch::async, &Timer::update, this);
  if (!menu->running) std::make_unique<std::future<void>*>(new auto(std::async(std::launch::async, runTimer, menu))).reset();
  string mag = "\x1b[1;95m";
  string clLine = "\x1b[K";
  string neimu = clLine + mag + menu->name + "\x1b[0m" + "\r\n";
  string desci = clLine + menu->desc + "\r\n\x1b[K\r\n";
  abWrite(neimu);
  abWrite(desci);
  char time[13];
  sprintf(time, "%.2d : %.2d : %.2d", menu->hours, menu->minutes, menu->seconds);
  string sp = string(conf.scols/2 - 7, ' ');
  abWrite("\x1b[K" + sp + string(time) + "\r\n");
  abWrite("\x1b[J");
}

/* Cambia la posición actual del menú actual */
void gotoPos(int pos) {
  auto act = listMenus[conf.actualMenu];
  if (pos < 0) return;
  if (pos > act->options.size() - 1) return;
  act->actualPos = pos;
  conf.cx = act->options[pos].posx;
  conf.cy = act->options[pos].posy;
}
 
/* Mueve el cursor */
void moveCursor(string dir) {
  auto act = listMenus[conf.actualMenu];
  if (dir == "left") {
    if (string(act->id) != "main")
    manageMenus(string(act->id) + "/exit");
  } else if (dir == "right") {
    string element = string(act->id) + "/" + act->options[act->actualPos].id;
    manageMenus(element);
  } else if (dir == "up") {
    gotoPos(listMenus[conf.actualMenu]->actualPos - 1);
  } else if (dir == "down") {
    gotoPos(listMenus[conf.actualMenu]->actualPos + 1);
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
  vector<string> splitted = split(muse, ";");
  if (splitted.size() != 3) {return;}
  b = stoi(splitted[0]);
  mx = stoi(splitted[1]);
  my = stoi(splitted[2]);
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
  /*
  if (iscntrl(c)) {
    printf("%d\r\n", c);
  } else {
    printf("%d ('%c')\r\n", c, c);
  }
  */
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
    auto act = listMenus[conf.actualMenu];
    string element = string(act->id) + "/" + act->options[act->actualPos].id;
    manageMenus(element);
  }
}

/* Aquí se especificaran los menús y sus opciones */
void initMenus() {
  strcpy(mainMenu.name, "Tasky");
  strcpy(mainMenu.id, "main");
  strcpy(mainMenu.desc, "Muévete con las flechas de dirección o con hjkl. Presiona Enter para elegir la opción.");
  mainMenu.options = {
    MenuOption {"Lista de tareas", "first"},
    MenuOption {"Temporizador", "timer"},
    MenuOption {"Salir", "exit"},
  };
  mainMenu.options.reserve(10);

  strcpy(firstMenu.name, "This is the first menu, not the main");
  strcpy(firstMenu.id, "first");
  strcpy(firstMenu.desc, "Just that...");
  firstMenu.options.reserve(10);
  firstMenu.options = {
    MenuOption {"Primera opción", "fa"},
    //MenuOption {"Con sangría!", "ba", 1},
    MenuOption {"Atrás", "exit"},
  };
  mainMenu.options.reserve(10);

  strcpy(tempo.name, "Temporizador");
  strcpy(tempo.id, "timer");
  strcpy(tempo.desc, "Esto es una prueba del temporizador");
  tempo.options.reserve(10);

  listMenus = {
    &mainMenu,
    &firstMenu,
    &tempo,
  };
}

