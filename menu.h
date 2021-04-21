#include <vector>
#include <string>
#include <string.h>

#include "winConf.h"
#include "buff.h"

#ifndef MENU_H
#define MENU_H 1 // Estos son guards para evitar múltiples declaraciones

extern struct winConfig conf;

int getWindowSize(int *rows, int *cols);

/*** Menus ***/

/* Estructura para crear opciones de menú */
struct MenuOption {
  char name[50]; // El nombre a mostrar en pantalla
  char id[20]; // El identificador
  int indentTimes = 0; // La cantidad de veces que usará *indentSpace* sobre la base
  int posx = 0; // La posición x en pantalla (No inicializar manualmente)
  int posy = 0; // La posición y en pantalla (No inicializar manualmente)
};


static MenuOption nullOption = {"NULL", "none", 0, 0};

class Window {
  public:
    std::string id;
    std::string name;
    std::string desc;
    int actualPos = 0;
    std::vector<MenuOption> options;

    Window(std::string ida) {
      options.reserve(10);
      id = ida;
    }

    void update();

    void render();

};


/* Clase para la creación de menús */
class Menu: public Window {

  public:
    std::string name;
    std::string desc;
    int indentSpace = 4;
    int actualPos = 0;
    winConfig *confi; // Si ocurren errores al mover el cursor, posiblemente sea por no usar esto

    Menu(std::string ida, struct winConfig *co) : Window(ida) {
      confi = co;
      for (int i = 0; i < options.size() ; i++) {
        options[i] = nullOption;
      }
    }

    /* Actualiza las opciones y sus datos */
    void update() {
      getWindowSize(&conf.srows, &conf.scols);
      int posx = indentSpace;
      int posy = 3;

      posy += desc.size() / conf.scols;

      for (int i = 0; i < options.size(); i++) {
        options[i].posx = posx;
        options[i].posy = posy;
        posy++;
      }
      gotoPos(actualPos);
    }

    /* Renderiza el menú en pantalla */
    void render(struct abuf *ab) {
      update();
      std::string mag = "\x1b[1;95m";
      std::string neimu = "\x1b[K" + mag + name + "\x1b[0m" + "\r\n";
      std::string desci = "\x1b[K" + desc + "\r\n\x1b[K\r\n";
      abWrite(neimu);
      abWrite(desci);

      std::string symb = "\u001b[1;94m❯ \u001b[0m";

      for (int i = 0; i < options.capacity(); i++) {
        if (i >= options.size()) {
          abWrite("\x1b[K");
          continue;
        }
        // if (strcmp(options[i].name, "NULL") == 0) continue;
        abWrite("\x1b[K");
        std::string inden = std::string(indentSpace-2, ' ');
        abWrite(inden);
        if (actualPos == i) {
          abWrite(symb);
          abWrite("\u001b[1;94m");
        }
        else abWrite(std::string(2, ' '));
        abWrite(std::string(options[i].indentTimes * indentSpace, ' '));
        abWrite(std::string(options[i].name));
        if (actualPos == i) abWrite("\u001b[0m");
        abWrite("\r\n");
      }
    }

    /* Mueve el cursor entre las opciones del menú */
    void gotoPos(int pos) {
      if (pos < 0) return;
      if (pos > options.size() - 1) return;
      if (strcmp(options[pos].name, "NULL") == 0) return;
      actualPos = pos;
      conf.cx = options[pos].posx;
      conf.cy = options[pos].posy;
    }
};

class Timer: public Window {
  public:
    time_t time;

};
extern Menu menu;

#endif
