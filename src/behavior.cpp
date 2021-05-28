#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <errno.h>
#include <vector>
#include <thread>
#include <future>

#include "menu.h"
#include "global.h"
#include "winConf.h"

#define CTRL_KEY(k) ((k) & 0x1f)

using namespace std;

void askTime();
void runTimer();
void viewTime();
void askPomodoro();
void runPomodoro();

void addTask();
void removeTask();
void editTask();
void viewTasks();
void saveData(string fileName);
void loadData(string fileName);

void lessText(string text, struct lessConf lessy);

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

/* El menú de la Lista de Tareas */
void menu_Task() {
  while (1) {
    printf("\e[H");
    string naimu = "\e[K\e[1;95mLista de tareas\e[m\n";
    string desci = "\e[KEsta es la agenda.\n";
    printf("%s", naimu.c_str());
    printf("%s", desci.c_str());
    printf("\e[K\n");
    printf("\e[K\e[1m[1]\e[0m - Regresar\n");
    printf("\e[K\e[1m[2]\e[0m - Visualizar las tareas\n");
    printf("\e[K\e[1m[3]\e[0m - Añadir una tarea\n");
    printf("\e[K\e[1m[4]\e[0m - Editar una tarea\n");
    printf("\e[K\e[1m[5]\e[0m - Eliminar una tarea\n");
    printf("\e[K\e[1m[6]\e[0m - Guardar cambios\n");
    printf("\e[K\e[1m[7]\e[0m - Deshacer cambios\n");
    int opt;
    printf("\e[K\e[92;1mSelecciona:\e[0m \e[J");

    printf("\n%s\n", conf.statusMessage);

    printf("\e[2A\e[%dC", 12);
    int check = scanf("%d", &opt);
    char c;
    while((c = getchar()) != '\n' && c != EOF) {}
    if (check != 1) {
      strcpy(conf.statusMessage, "\e[91;1mError:\e[0m Ingresa un número");
      continue;
    }

    strcpy(conf.statusMessage, "");
    switch (opt) {
      case 1:
        return;
        break;
      case 2:
        viewTasks();
        break;
      case 3:
        addTask();
        break;
      case 4:
        editTask();
        break;
      case 5:
        removeTask();
        break;
      case 6:
        saveData("data.bin");
        break;
      case 7:
        loadData("data.bin");
        strcpy(conf.statusMessage, "Los cambios fueron deshechos");
        break;
      default:
        strcpy(conf.statusMessage, "\e[93;1mError:\e[0m Número no válido");
        break;
    }

  }
}

/* El menú para la selección del temporizador */
void menu_SelectTimer() {
  while (1) {
    printf("\e[H");
    string naimu = "\e[K\e[1;95mTemporizador\e[0m\n";
    string desci = "\e[KSelección del método de temporizador.\n";
    printf("%s", naimu.c_str());
    printf("%s", desci.c_str());
    printf("\e[K\n");
    printf("\e[K\e[1m[1]\e[0m - Regresar\n");
    printf("\e[K\e[1m[2]\e[0m - Iniciar Pomodoro\n");
    printf("\e[K\e[1m[3]\e[0m - Iniciar Temporizador\n");
    printf("\e[K\e[1m[4]\e[0m - \e[90mIniciar Cronómetro\e[0m\n");
    int opt;
    printf("\e[K\e[92;1mSelecciona:\e[0m \e[J");

    printf("\n%s\n", conf.statusMessage);

    printf("\e[2A\e[%dC", 12);
    int check = scanf("%d", &opt);
    char c;
    while((c = getchar()) != '\n' && c != EOF) {}
    if (check != 1) {
      strcpy(conf.statusMessage, "\e[91;1mError:\e[0m Ingresa un número");
      continue;
    }

    strcpy(conf.statusMessage, "");
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
        strcpy(conf.statusMessage, "\e[93;1mError:\e[0m Número no válido");
        break;
    }
  }
}

/* El menú del temporizador */
void menu_Timer() {
  while (1) {
    printf("\e[H");
    string naimu = "\e[K\e[1;95mTemporizador\e[m\n";
    string desci = "\e[KEste es el temporizador.\n";
    printf("%s", naimu.c_str());
    printf("%s", desci.c_str());
    printf("\e[K\n");
    printf("\e[K\e[1m[1]\e[0m - Regresar\n");
    printf("\e[K\e[1m[2]\e[0m - Visualizar el tiempo\n");
    printf("\e[K\e[1m[3]\e[0m - %s temporizador\n", tempo.time == 0 ? "Iniciar" : tempo.running ? "Parar" :  "Reanudar");
    printf("\e[K\e[1m[4]\e[0m - Reiniciar temporizador\n");
    int opt;
    printf("\e[K\e[92;1mSelecciona:\e[0m \e[J");

    printf("\n%s\n", conf.statusMessage);

    printf("\e[2A\e[%dC", 12);
    int check = scanf("%d", &opt);
    char c;
    while((c = getchar()) != '\n' && c != EOF) {}
    if (check != 1) {
      strcpy(conf.statusMessage, "\e[91;1mError:\e[0m Ingresa un número");
      continue;
    }

    strcpy(conf.statusMessage, "");
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
            strcpy(conf.statusMessage, "Pomodoro iniciado");
          } else
          if (tempo.mode == 1) { // Temporizador
            unique_ptr<future<void>*> a = make_unique<future<void>*>(new auto(async(launch::async, runTimer)));
            a.reset();
            strcpy(conf.statusMessage, "Temporizador iniciado");
          }
        }
        if (tempo.running) {
          tempo.running = false;
          strcpy(conf.statusMessage, "Tiempo detenido");
        }
        break;
      case 4:
        restartTimer();
        break;
      default:
        strcpy(conf.statusMessage, "\e[93;1mError:\e[0m Número no válido");
        break;
    }
  }
}

/* Reemplaza ciertos carácteres por otros en una cadena de texto */
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

/* Lee el contenido de un archivo y lo regresa como string */
string catFile(string fileName, bool ansi = true) {
  FILE *file;
  file = fopen(fileName.c_str(), "r");
  if (!file) {return "";}
  char s;
  string text;
  while ((s = fgetc(file)) != EOF) {
    text += s;
  }
  if (ansi) {
    text = strreplace(text, "\\e", "\x1b");
  }
  fclose(file);
  return text;
}

struct lessConf helpy;

/* Muestra el menú de ayuda */
void menu_Help() {
  clear();
  string text = catFile("src/help");
  //int a = system(("less \"" + text + "\"").c_str());
  lessText(text, helpy);
}

