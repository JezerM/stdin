#include <vector>
#include <string>
#include <string.h>

#include "winConf.h"
#include "buffer.h"

extern struct winConfig conf;

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

/* Clase para la creación de menús */
class Menu {

  public:
    std::string id;
    std::string name;
    std::string desc;
    int indentSpace = 4;
    int actualPos = 0;
    static const int numberOptions = 10;
    MenuOption options[numberOptions];
    winConfig *confi; // Si ocurren errores al mover el cursor, posiblemente sea por no usar esto

    Menu(std::string ida, struct winConfig *co) {
      id = ida;
      confi = co;
      for (int i = 0; i < numberOptions ; i++) {
        options[i] = nullOption;
      }
    }

    /* Actualiza las opciones y sus datos */
    void update() {
      int posx = indentSpace;
      int posy = 3;
      for (int i = 0; i < numberOptions; i++) {
        if (strcmp(options[i].name, "NULL") == 0) continue;
        options[i].posx = posx;
        options[i].posy = posy;
        posy++;
      }
    }

    /* Renderiza el menú en pantalla */
    void render(struct abuf *ab) {
      update();
      abAppend(ab, name.c_str(), name.length());
      abAppend(ab, "\r\n", 2);
      abAppend(ab, desc.c_str(), desc.length());
      abAppend(ab, "\r\n\r\n", 4);

      for (int i = 0; i < numberOptions; i++) {
        if (strcmp(options[i].name, "NULL") == 0) continue;
        abAppend(ab, std::string(indentSpace, ' ').c_str(), indentSpace);
        abAppend(ab, std::string(options[i].indentTimes*indentSpace, ' ').c_str(), options[i].indentTimes*indentSpace);
        if (actualPos == i) abAppend(ab, "\u001b[107;30m", 9);
        abAppend(ab, options[i].name, sizeof(options[i].name));
        if (actualPos == i) abAppend(ab, "\u001b[0m", 4);
        abAppend(ab, "\r\n", 2);
      }
    }

    /* Mueve el cursor entre las opciones del menú */
    void gotoPos(int pos) {
      if (pos < 0) return;
      if (pos > numberOptions - 1) return;
      if (strcmp(options[pos].name, "NULL") == 0) return;
      actualPos = pos;
      conf.cx = options[pos].posx;
      conf.cy = options[pos].posy;
    }
};
