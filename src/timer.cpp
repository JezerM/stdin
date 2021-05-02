#include <cstring>
#include <string>

#include "menu.h"

void clear();
void enableRawMode(bool t = false);
void disableRawMode();
char readKey();

using namespace std;

void askTime() {
  disableRawMode();
  clear();
  printf("\x1b[?25h");
  string mag = "\x1b[1;95m";
  string clLine = "\x1b[K";
  string naimu = clLine + mag + "Temporizador" + "\x1b[0m\n";
  string desci = clLine + "Ingresa el tiempo (horas, minutos, segundos)." + "\n";
  printf("%s", naimu.c_str());
  printf("%s", desci.c_str());
  int h, m, s;
  printf("Horas: ");
  scanf("%d", &h);
  printf("Minutos: ");
  scanf("%d", &m);
  printf("Segundos: ");
  scanf("%d", &s);
  int time = h*3600 + m*60 + s;
  tempo->time = time;
  enableRawMode(true);
}

void runTimer() {
  tempo->running = true;
  while (tempo->running) {
    tempo->seconds = tempo->time % 60;
    tempo->minutes = (tempo->time / 60) % 60;
    tempo->hours = (tempo->time / 3600) % 60;
    this_thread::sleep_for(chrono::seconds(1));
    if (tempo->time <= 0) {
      break;
    }
    tempo->time--;
  }
  tempo->running = false;
}
