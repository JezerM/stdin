#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <errno.h>

#include "menu.h"

#define CTRL_KEY(k) ((k) & 0x1f)

using namespace std;

void exitAll();
char readKey();
void enableRawMode(bool t = false);
void disableRawMode();
void clear();

void askTime();
void runTimer();
void viewTime();

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

void menu_Task() {
  while (1) {
    printf("\x1b[H");
    string mag = "\x1b[1;95m";
    string clLine = "\x1b[K";
    string naimu = clLine + mag + "Lista de tareas" + "\x1b[0m\n";
    string desci = clLine + "Esta es la agenda." + "\n";
    printf("%s", naimu.c_str());
    printf("%s", desci.c_str());
    printf("\x1b[K\n");
    printf("\x1b[K[1] - Regresar\n");
    printf("\x1b[K[2] - Visualizar las tareas\n");
    printf("\x1b[K[3] - Añadir una tarea\n");
    printf("\x1b[K[4] - Eliminar una tarea\n");
    int opt;
    printf("\x1b[KSelecciona: ");
    scanf("%d", &opt);

    switch (opt) {
      case 1:
        return;
        break;
      case 2:
        // Ver tareas
        break;
      case 3:
        // Añadir tarea
        break;
      case 4:
        // Eliminar tarea
        break;
      default:
        break;
    }

  }
}

void menu_Timer() {
  while (1) {
    printf("\x1b[H");
    string mag = "\x1b[1;95m";
    string clLine = "\x1b[K";
    string naimu = clLine + mag + "Lista de tareas" + "\x1b[0m\n";
    string desci = clLine + "Esta es la agenda." + "\n";
    printf("%s", naimu.c_str());
    printf("%s", desci.c_str());
    printf("\x1b[K\n");
    printf("\x1b[K[1] - Regresar\n");
    printf("\x1b[K[2] - Visualizar el tiempo\n");
    printf("\x1b[K[3] - Iniciar temporizador\n");
    printf("\x1b[K[4] - Parar temporizador\n");
    printf("\x1b[K[5] - Reiniciar temporizador\n");
    printf("\x1b[K[6] - Cambiar a (cronómetro)\n");
    int opt;
    printf("\x1b[KSelecciona: ");
    scanf("%d", &opt);

    switch (opt) {
      case 1:
        return;
        break;
      case 2:
        viewTime();
        break;
      case 3:
        if (tempo.time == 0) askTime();
        if (!tempo.running && tempo.time > 0) std::make_unique<std::future<void>*>(new auto(std::async(std::launch::async, runTimer))).reset();
        break;
      case 4:
        if (tempo.running) tempo.running = false;
        break;
      case 5:
        tempo.time = 0;
        tempo.hours = 0; tempo.minutes = 0; tempo.seconds = 0;
        tempo.running = false;
        break;
      case 6:
        // Cambiar
        break;
      default:
        break;
    }
  }
}

void menu_Help() {
  clear();
  printf("\x1b[?25l");
  string mag = "\x1b[1;95m";
  string clLine = "\x1b[K";
  string naimu = clLine + mag + "Ayuda" + "\x1b[0m\n";
  string desci = clLine + "Presiona cualquier tecla para salir" + "\n";
  char tab[] = "    ";
  printf("%s", naimu.c_str());
  printf("%s", desci.c_str());
  printf("\x1b[K\n");
  /* Uso */
  printf("\x1b[1mUso\x1b[0m\n");
  printf("%sPara moverse a través de los menús, escribe la opción que deseas.\n", tab);
  printf("%sSi escribes un número no válido, no ocurrirá nada.\n", tab);

  /* Agenda */
  printf("\x1b[1mAgenda\x1b[0m\n");
  printf("%s\x1b[1mAñadir\x1b[0m\n", tab);
  printf("%s%sPuedes añadir una tarea a la vez. Se te solicitará la información correspondiente, como el \x1b[1mNombre\x1b[0m y la \x1b[1mFecha\x1b[0m.\n", tab, tab);

  getch();
  printf("\x1b[?25h");
}

