#include <string>
#include <thread>
#include <chrono>

#include "menu.h"

using namespace std;

void enableRawMode(bool t = false);
void disableRawMode();
char readKey();
void clear();

void doNothing() {}

void restartTimer() {
  strcpy(tempo.state, "Detenido");
  tempo.running = false;
  tempo.time = 0;
  tempo.hours = 0;
  tempo.minutes = 0;
  tempo.seconds = 0;
  tempo.shortBreak = 0;
  tempo.longBreak = 0;
  tempo.loop = 0;
  tempo.phase = 0;
}

void viewTime() {
  printf("\x1b[?25l");
  enableRawMode(true);
  while (1) {
    printf("\x1b[H");
    string name;
    string desc;
    switch (tempo.mode) {
      case 0:
        name = "Pomodoro";
        desc = "Estás visualizando el pomodoro";
        break;
      case 1:
        name = "Temporizador";
        desc = "Estás visualizando el temporizador";
        break;
      default:
        name = "Eh... Khe?";
        desc = "Estás visualizando... ¿qué hiciste?";
        break;
    }
    string mag = "\x1b[1;95m";
    string clLine = "\x1b[K";
    string naimu = clLine + mag + name + "\x1b[0m\r\n";
    string desci = clLine + desc + "\r\n";
    printf("\x1b[K%s", naimu.c_str());
    printf("\x1b[K%s", desci.c_str());
    char time[13];
    sprintf(time, "%.2d : %.2d : %.2d", tempo.hours, tempo.minutes, tempo.seconds);
    printf("\x1b[K%s\r\n", time);
    printf("\n");
    printf("\x1b[KEstado: \x1b[1m%s\x1b[0m\r\n", tempo.state);
    if (tempo.mode == 0) {
      printf("\x1b[KVueltas: \x1b[1m%d\x1b[0m\r\n", tempo.loop);
    }
    printf("\r\n\x1b[KPresiona cualquier tecla para salir\r\n");
    printf("\x1b[J");
    char c = readKey();
    if (c != '\0') break;
  }
  disableRawMode();
  printf("\x1b[?25h");
}

void askTime() {
  printf("\x1b[H");
  string mag = "\x1b[1;95m";
  string clLine = "\x1b[K";
  string naimu = clLine + mag + "Temporizador" + "\x1b[0m\n";
  string desci = clLine + "Ingresa el tiempo (horas, minutos, segundos)." + "\n";
  printf("\x1b[K%s", naimu.c_str());
  printf("\x1b[K%s", desci.c_str());
  char c;
  while((c = getchar()) != '\n' && c != EOF) {}

  char h[10], m[10], s[10];
  int hours, minutes, seconds;

  printf("\x1b[KHoras: ");
  fgets(h, 10, stdin);
  printf("\x1b[KMinutos: ");
  fgets(m, 10, stdin);
  printf("\x1b[KSegundos: ");
  fgets(s, 10, stdin);

  if (strcmp(h, "\n") == 0) {
    hours = 1;
  } else {
    strtok(h,"\n");
    hours = stoi(h);
  }
  if (strcmp(m, "\n") == 0) {
    minutes = 30;
  } else {
    strtok(m,"\n");
    minutes = stoi(m);
  }
  if (strcmp(s, "\n") == 0) {
    seconds = 0;
  } else {
    strtok(s,"\n");
    seconds = stoi(s);
  }

  int time = hours*3600 + minutes*60 + seconds;
  tempo.time = time;
  printf("\x1b[KTiempo especificado en \x1b[1m%.2d : %.2d : %.2d\x1b[0m\n", hours, minutes, seconds);
  printf("\n\x1b[KPresiona una tecla para continuar\n");
  printf("\x1b[?25l");
  getch();
  printf("\x1b[?25h");
  tempo.mode = 1;
}

void runTimer() {
  tempo.running = true;
  strcpy(tempo.state, "Trabajando");
  while (tempo.running) {
    tempo.seconds = tempo.time % 60;
    tempo.minutes = (tempo.time / 60) % 60;
    tempo.hours = (tempo.time / 3600) % 60;
    //this_thread::sleep_for(chrono::seconds(1));
    sleep(1);
    if (tempo.time <= 0) {
      break;
    }
    tempo.time--;
  }
  tempo.running = false;
  strcpy(tempo.state, "Detenido");
}

void askPomodoro() {
  printf("\x1b[H");
  string mag = "\x1b[1;95m";
  string clLine = "\x1b[K";
  string naimu = clLine + mag + "Pomodoro" + "\x1b[0m\n";
  string desci = clLine + "Ingresa los datos del temporizador Pomodoro" + "\n";
  printf("\x1b[K%s", naimu.c_str());
  printf("\x1b[K%s", desci.c_str());
  printf("\x1b[K\n");
  char c;
  while((c = getchar()) != '\n' && c != EOF) {}

  char t[10], s[10], l[10];
  int time, shortTime, longTime;

  printf("\x1b[KTiempo total (minutos): ");
  fgets(t, 10, stdin);
  printf("\x1b[KDescanso corto: ");
  fgets(s, 10, stdin);
  printf("\x1b[KDescando largo: ");
  fgets(l, 10, stdin);
  if (strcmp(t, "\n") == 0) {
    time = 25 * 60;
  } else {
    strtok(t,"\n");
    time = stoi(t) * 60;
  }
  if (strcmp(s, "\n") == 0) {
    shortTime = 5 * 60;
  } else {
    strtok(s,"\n");
    shortTime = stoi(s) * 60;
  }
  if (strcmp(l, "\n") == 0) {
    longTime = 10 * 60;
  } else {
    strtok(l,"\n");
    longTime = stoi(l) * 60;
  }
  tempo.time = time;
  tempo.shortBreak = shortTime;
  tempo.longBreak = longTime;
  printf("\n\x1b[KEl pomodoro se estableció en:\n");
  printf("  \x1b[KIntervalos:     \x1b[1m%.2d\x1b[0m minutos\n", time/60);
  printf("  \x1b[KDescanso corto: \x1b[1m%.2d\x1b[0m minutos\n", shortTime/60);
  printf("  \x1b[KDescanso largo: \x1b[1m%.2d\x1b[0m minutos\n", longTime/60);
  printf("\n\x1b[KPresiona una tecla para continuar\n");
  printf("\x1b[?25l");
  getch();
  printf("\x1b[?25h");
  tempo.mode = 0;
}

void runPomodoro() {
  tempo.running = true;
  tempo.loop = 0;
  int time = tempo.time;
  int shortBreak = tempo.shortBreak;
  int longBreak = tempo.longBreak;
  int maxLoops = 4;
  while (tempo.running) {
    if (tempo.phase == 0) {
      strcpy(tempo.state, "Trabajando");
      tempo.seconds = tempo.time % 60;
      tempo.minutes = (tempo.time / 60) % 60;
      tempo.hours = (tempo.time / 3600) % 60;
      this_thread::sleep_for(chrono::seconds(1));
      if (!tempo.running) break;
      tempo.time--;
      if (tempo.time <= 0 && tempo.loop < maxLoops - 1) {
        tempo.loop++;
        tempo.time = time;
        tempo.phase = 1;
      } else if (tempo.time <= 0) {
        tempo.loop = 0;
        tempo.time = time;
        tempo.phase = 2;
      }
    } else
    if (tempo.phase == 1) {
      strcpy(tempo.state, "Descanso corto");
      tempo.seconds = tempo.shortBreak % 60;
      tempo.minutes = (tempo.shortBreak / 60) % 60;
      tempo.hours = (tempo.shortBreak / 3600) % 60;
      this_thread::sleep_for(chrono::seconds(1));
      //this_thread::sleep_for(chrono::milliseconds(200));
      if (!tempo.running) break;
      tempo.shortBreak--;
      if (tempo.shortBreak <= 0) {
        tempo.shortBreak = shortBreak;
        tempo.phase = 0;
      }
    } else
    if (tempo.phase == 2) {
      strcpy(tempo.state, "Descanso largo");
      tempo.seconds = tempo.longBreak % 60;
      tempo.minutes = (tempo.longBreak / 60) % 60;
      tempo.hours = (tempo.longBreak / 3600) % 60;
      this_thread::sleep_for(chrono::seconds(1));
      if (!tempo.running) break;
      tempo.longBreak--;   
      if (tempo.longBreak <= 0) {
        tempo.longBreak = longBreak;
        tempo.phase = 3;
      }
    } else {
      break;
    }
  }
  tempo.running = false;
  strcpy(tempo.state, "Detenido");
}

