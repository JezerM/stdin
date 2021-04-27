#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <future>
#include <ctime>
#include <unistd.h>
#include <thread>

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
    std::string type;
    int actualPos = 0;
    std::vector<MenuOption> options;
    winConfig *confi; // Si ocurren errores al mover el cursor, posiblemente sea por no usar esto

    Window(std::string ida) {
      options.reserve(10);
      id = ida;
      type = "Window";
    }
    ~Window() {}

    virtual void update() {}

    virtual void render() {}

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


/* Clase para la creación de menús */
class Menu: public Window {

  public:
    int indentSpace = 4;

    Menu(std::string ida, struct winConfig *co) : Window(ida) {
      type = "Menu";
      confi = co;
      for (int i = 0; i < options.size() ; i++) {
        options[i] = nullOption;
      }
    }
    ~Menu() {}

    /* Actualiza las opciones y sus datos */
    virtual void update() override {
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
    virtual void render() override {
      update();
      std::string mag = "\x1b[1;95m";
      std::string neimu = "\x1b[K" + mag + name + "\x1b[0m" + "\r\n";
      std::string desci = "\x1b[K" + desc + "\r\n\x1b[K\r\n";
      abWrite(neimu);
      abWrite(desci);

      std::string symb = "\u001b[1;94m\u276F \u001b[0m";

      for (int i = 0; i < options.capacity(); i++) {
        if (i >= options.size()) {
          abWrite("\x1b[K\r\n");
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
};

class Timer: public Window {
  public:
    int time;
    int seconds;
    int minutes;
    int hours;
    bool running = false;

    Timer(std::string ida, struct winConfig *co) : Window(ida) {
      type = "Timer";
      confi = co;
      for (int i = 0; i < options.size() ; i++) {
        options[i] = nullOption;
      }
    }
    ~Timer() {}

    virtual void update() override {
      if (running) return;
      running = true;
      time = 90;
      while (running) {
        seconds = time % 60;
        minutes = (time / 60) % 60;
        hours = (time / 3600) % 60;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (time == 0) {
          break;
        }
        time--;
      }
      running = false;
    }

    virtual void render() override {
      getWindowSize(&conf.srows, &conf.scols);
      // std::future<void> hand = std::async(std::launch::async, &Timer::update, this);
       if (!running) std::make_unique<std::future<void>*>(new auto(std::async(std::launch::async, &Timer::update, this))).reset();
      std::string mag = "\x1b[1;95m";
      std::string neimu = "\x1b[K" + mag + name + "\x1b[0m" + "\r\n";
      std::string desci = "\x1b[K" + desc + "\r\n\x1b[K\r\n";
      abWrite(neimu);
      abWrite(desci);
      char time[13];
      sprintf(time, "%.2d : %.2d : %.2d", hours, minutes, seconds);
      std::string sp = std::string(conf.scols/2 - 6, ' ');
      abWrite("\x1b[K" + sp + std::string(time) + "\r\n");
      abWrite("\x1b[J");
    }
};
extern Window *win;

#endif
