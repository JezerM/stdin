#include <stdexcept>
#include <string>
#include <thread>
#include <chrono>

#include "menu.h"
#include "winConf.h"
#include "global.h"

using namespace std;

void doNothing() {}

void alertBeep() {
  playBeep(440.f, 300);
  playBeep(400.f, 400);
  playBeep(460.f, 600);
}

void restartTimer() {
  tempo.stateCode = 2;
  strcpy(conf.statusMessage, "Temporizador reiniciado");
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
  printf("\e[?25l");
  enableRawMode(true);
  while (1) {
    printf("\e[H");
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
    string mag = "\e[1;95m";
    string clLine = "\e[K";
    string naimu = clLine + mag + name + "\e[0m\r\n";
    string desci = clLine + desc + "\r\n";
    printf("\e[K%s", naimu.c_str());
    printf("\e[K%s", desci.c_str());
    char time[13];
    sprintf(time, "%.2d : %.2d : %.2d", tempo.hours, tempo.minutes, tempo.seconds);
    printf("\e[K%s\r\n", time);
    printf("\n");
    printf("\e[KEstado: \e[1m%s\e[0m\r\n", tempo.state);
    if (tempo.mode == 0) {
      printf("\e[KVueltas: \e[1m%d\e[0m\r\n", tempo.loop);
    }
    printf("\r\n\e[KPresiona cualquier tecla para salir\r\n");
    printf("\e[J");
    char c = readKey();
    if (c != '\0') break;
  }
  disableRawMode();
  printf("\e[?25h");
}

void askTime() {
  printf("\e[H");
  string mag = "\e[1;95m";
  string clLine = "\e[K";
  string naimu = clLine + mag + "Temporizador" + "\e[0m\n";
  string desci = clLine + "Ingresa el tiempo (horas, minutos, segundos)." + "\n";
  printf("\e[K%s", naimu.c_str());
  printf("\e[K%s", desci.c_str());

  char h[10], m[10], s[10];
  int hours, minutes, seconds;

  printf("\e[KHoras: ");
  fgets(h, 10, stdin);
  printf("\e[KMinutos: ");
  fgets(m, 10, stdin);
  printf("\e[KSegundos: ");
  fgets(s, 10, stdin);

  try {
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
  } catch (invalid_argument) {
    printf("\e[91;1mError:\e[0m Argumentos inválidos. Ingresa correctamente los datos.\n");
    getch();
    return;
  }
  int time = hours*3600 + minutes*60 + seconds;
  tempo.time = time;
  printf("\e[KTiempo especificado en \e[1m%.2d : %.2d : %.2d\e[0m\n", hours, minutes, seconds);
  printf("\n\e[KPresiona una tecla para continuar\n");
  printf("\e[?25l");
  getch();
  printf("\e[?25h");
  tempo.mode = 1;
}

void runTimer() {
  tempo.running = true;
  tempo.stateCode = 1;
  strcpy(tempo.state, "Trabajando");
  while (tempo.running) {
    tempo.seconds = tempo.time % 60;
    tempo.minutes = (tempo.time / 60) % 60;
    tempo.hours = (tempo.time / 3600) % 60;
    //this_thread::sleep_for(chrono::seconds(1));
    sleep(1);
    if (tempo.time <= 0) {
      if (tempo.stateCode == 1) {
        alertBeep();
        tempo.stateCode = 0;
      }
      break;
    }
    tempo.time--;
  }
  tempo.running = false;
  strcpy(tempo.state, "Detenido");
}

void askPomodoro() {
  printf("\e[H");
  string mag = "\e[1;95m";
  string clLine = "\e[K";
  string naimu = clLine + mag + "Pomodoro" + "\e[0m\n";
  string desci = clLine + "Ingresa los datos del temporizador Pomodoro" + "\n";
  printf("\e[K%s", naimu.c_str());
  printf("\e[K%s", desci.c_str());
  printf("\e[K\n");

  char t[10], s[10], l[10];
  int time, shortTime, longTime;

  printf("\e[KTiempo total (minutos): ");
  fgets(t, 10, stdin);
  printf("\e[KDescanso corto: ");
  fgets(s, 10, stdin);
  printf("\e[KDescando largo: ");
  fgets(l, 10, stdin);
  try {
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
  } catch (invalid_argument) {
    printf("\e[91;1mError:\e[0m Argumentos inválidos. Ingresa correctamente los datos.\n");
    getch();
    return;
  }
  tempo.time = time;
  tempo.shortBreak = shortTime;
  tempo.longBreak = longTime;
  printf("\n\e[KEl pomodoro se estableció en:\n");
  printf("  \e[KIntervalos:     \e[1m%.2d\e[0m minutos\n", time/60);
  printf("  \e[KDescanso corto: \e[1m%.2d\e[0m minutos\n", shortTime/60);
  printf("  \e[KDescanso largo: \e[1m%.2d\e[0m minutos\n", longTime/60);
  printf("\n\e[KPresiona una tecla para continuar\n");
  printf("\e[?25l");
  getch();
  printf("\e[?25h");
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
      if (tempo.stateCode == 1) {
        alertBeep();
        tempo.stateCode = 0;
      }
      break;
    }
  }
  tempo.running = false;
  strcpy(tempo.state, "Detenido");
}

