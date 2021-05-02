#include <thread>
#include <chrono>

#include "menu.h"

using namespace std;

void enableRawMode(bool t = false);
void disableRawMode();
char readKey();
void clear();

void viewTime() {
  printf("\x1b[?25l");
  enableRawMode(true);
  while (1) {
    clear();
    string mag = "\x1b[1;95m";
    string clLine = "\x1b[K";
    string naimu = clLine + mag + "Temporizador" + "\x1b[0m\r\n";
    string desci = clLine + "Estás visualizando el temporizador" + "\r\n";
    printf("%s", naimu.c_str());
    printf("%s", desci.c_str());
    char time[13];
    sprintf(time, "%.2d : %.2d : %.2d", tempo.hours, tempo.minutes, tempo.seconds);
    printf("%s\r\n", time);
    printf("\r\nPresiona cualquier tecla para salir\r\n");
    printf("\x1b[J");
    char c = readKey();
    if (c != '\0') break;
  }
  disableRawMode();
  printf("\x1b[?25h");
}

void askTime() {
  clear();
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
  tempo.time = time;
  printf("Tiempo especificado en \x1b[1m%.2d : %.2d : %.2d\x1b[0m\n", h, m, s);
  printf("Presiona una tecla para continuar\n");
  printf("\x1b[?25l");
  getch();
  printf("\x1b[?25h");
}

void runTimer() {
  tempo.running = true;
  while (tempo.running) {
    tempo.seconds = tempo.time % 60;
    tempo.minutes = (tempo.time / 60) % 60;
    tempo.hours = (tempo.time / 3600) % 60;
    this_thread::sleep_for(chrono::seconds(1));
    if (tempo.time <= 0) {
      break;
    }
    tempo.time--;
  }
  tempo.running = false;
}

