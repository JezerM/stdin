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
void askPomodoro();
void runPomodoro();

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
    printf("\x1b[K\x1b[1m[1]\x1b[0m - Regresar\n");
    printf("\x1b[K\x1b[1m[2]\x1b[0m - \x1b[90mVisualizar las tareas\x1b[0m\n");
    printf("\x1b[K\x1b[1m[3]\x1b[0m - \x1b[90mAñadir una tarea\x1b[0m\n");
    printf("\x1b[K\x1b[1m[4]\x1b[0m - \x1b[90mEliminar una tarea\x1b[0m\n");
    int opt;
    printf("\x1b[K\x1b[92;1mSelecciona:\x1b[0m \x1b[J");
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

void menu_SelectTimer() {
  while (1) {
    printf("\x1b[H");
    string mag = "\x1b[1;95m";
    string clLine = "\x1b[K";
    string naimu = clLine + mag + "Temporizador" + "\x1b[0m\n";
    string desci = clLine + "Selección del método de temporizador." + "\n";
    printf("%s", naimu.c_str());
    printf("%s", desci.c_str());
    printf("\x1b[K\n");
    printf("\x1b[K\x1b[1m[1]\x1b[0m - Regresar\n");
    printf("\x1b[K\x1b[1m[2]\x1b[0m - Pomodoro\n");
    printf("\x1b[K\x1b[1m[3]\x1b[0m - Temporizador\n");
    printf("\x1b[K\x1b[1m[4]\x1b[0m - \e[90mCronómetro\e[0m\n");
    int opt;
    printf("\x1b[K\x1b[92;1mSelecciona:\x1b[0m \x1b[J");
    scanf("%d", &opt);

    switch (opt) {
      case 1:
        return;
        break;
      case 2:
        clear();
        return askPomodoro();
        break;
      case 3:
        clear();
        return askTime();
        break;
      case 4:
        // Cronómetro
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
    string naimu = clLine + mag + "Temporizador" + "\x1b[0m\n";
    string desci = clLine + "Este es el temporizador." + "\n";
    printf("%s", naimu.c_str());
    printf("%s", desci.c_str());
    printf("\x1b[K\n");
    printf("\x1b[K\x1b[1m[1]\x1b[0m - Regresar\n");
    printf("\x1b[K\x1b[1m[2]\x1b[0m - Visualizar el tiempo\n");
    printf("\x1b[K\x1b[1m[3]\x1b[0m - %s temporizador\n", tempo.time == 0 ? "Iniciar" : tempo.running ? "Parar" :  "Reanudar");
    printf("\x1b[K\x1b[1m[4]\x1b[0m - Reiniciar temporizador\n");
    int opt;
    printf("\x1b[K\x1b[92;1mSelecciona:\x1b[0m \x1b[J");
    scanf("%d", &opt);

    switch (opt) {
      case 1:
        return;
        break;
      case 2:
        clear();
        viewTime();
        break;
      case 3:
        clear();
        if (tempo.time == 0) menu_SelectTimer();
        if (!tempo.running && tempo.time != 0) {
          if (tempo.mode == 0) { // Pomodoro
            unique_ptr<future<void>*> a = make_unique<future<void>*>(new auto(async(launch::async, runPomodoro)));
            a.reset();
          } else
          if (tempo.mode == 1) { // Temporizador
            std::make_unique<std::future<void>*>(new auto(std::async(std::launch::async, runTimer))).reset();
          }
        }
        if (tempo.running) tempo.running = false;
        break;
      case 4:
        restartTimer();
        break;
      default:
        break;
    }
  }
}

string strreplace(string orgString, const string search, const string replace ) {
  for( size_t pos = 0; ; pos += replace.length() ) {
    pos = orgString.find( search, pos );
    if( pos == string::npos )
      break;
    orgString.erase( pos, search.length() );
    orgString.insert( pos, replace);
  }
  return orgString;
}

void menu_Help() {
  clear();
  printf("\x1b[?25l");

  FILE *help;
  help = fopen("src/help", "r");
  if (!help) {
    printf("El archivo de ayuda no fue encontrado\n");
    printf("Presione una tecla para continuar\n");
    getch();
    return;
  }
  char s;
  string text;
  while((s = fgetc(help)) != EOF) {
    text += s;
  }
  text = strreplace(text, "\\e", "\x1b");
  //int a = system(("less \"" + text + "\"").c_str());
  cout << text;
  fclose(help);

  getch();
  printf("\x1b[?25h");
}

