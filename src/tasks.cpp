#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>
#include <iostream>
#include <time.h>
#include "global.h"
#include "winConf.h"
#include "tasks.h"

using namespace std;

void lessText(string text, struct lessConf lessy);
string getActualDate(string formatStr);
string getActualTime(string formatStr = "H:%M:%S");
double diffBetweenDates(string date1, string date2, string formatStr = "%d-%m-%Y");
int validDate(string date, string formatStr = "%d-%m-%Y");
string formatDate(string date, string formatStr = "%d-%m-%Y");

double diffBetweenDates(struct tm date1, struct tm date2);
struct tm getActualDate();
string formatDate(struct tm time_date, string formatStr = "%d-%m-%Y");
struct tm getDate(string date, string formatStr = "%d-%m-%Y");

int listSize = 0;

struct Task lista[MAXLISTSIZE];

/* Genera un número aleatorio entre 100 y 900 */
int randNumber() {
  const int maxNum = 900;
  const int minNum = 100;
  int ranNum = (rand() % (maxNum + 1 - minNum) + minNum);
  return ranNum;
}

/* Crea un ID aleatoriamente, comparando si existe o no el ID */
string createID() {
  string id = to_string(randNumber());
  for (int i = 0; i < listSize; i++) {
    if (strcmp(lista[i].name, "") == 0) break;
    if (strcmp(lista[i].id, id.c_str()) == 0) return createID();
  }
  return id;
}

/* Verifica si las ID cargadas son correctas
 * Si no, crea un ID */
void checkLoadedID() {
  for (int i = 0; i < listSize; i++) {
    if (strcmp(lista[i].name, "") == 0) break;
    if (strcmp(lista[i].id, "") == 0) {
      strcpy(lista[i].id, createID().c_str());
    }
  }
}

/* Intercambia la posición de dos tareas */
void swapTasks(int i, int j) {
  struct Task temp = lista[i];
  lista[i] = lista[j];
  lista[j] = temp;
}

/* Particiona la lista para el ordenamiento */
int partition (int low, int high) {
  int i = (low - 1);  // Index of smaller element
  struct tm pivot = getDate(lista[high].date);
  struct tm tmpJ;

  int pivTime = mktime(&pivot);
  int tmpTime;

  for (int j = low; j <= high- 1; j++) {
    tmpJ = getDate(lista[j].date);
    tmpTime = mktime(&tmpJ);

    //printf("[%d] [%d] %d - %d\n", i, j, pivTime, tmpTime);
    if (tmpTime <= pivTime) {
      i++;    // increment index of smaller element
      swapTasks(i, j);
      //printf("  Swapped [%d] [%d]\n", i, j);
    }
  }
  swapTasks(i + 1, high);
  //printf("  T  Swapped [%d] [%d]\n", i, high);
  return (i + 1);
}

/* Comienza el ordenamiento de la lista */
void quickSort(int low, int high) {
  if (low < high) {
    int pi = partition(low, high);

    quickSort(low, pi - 1);
    quickSort(pi + 1, high);
  }
}

/* Verifica si el archivo existe. Si no, lo crea */
void checkFile(string name) {
  FILE *buff;
  buff = fopen(name.c_str(), "r");
  if (!buff) {
    buff = fopen(name.c_str(), "w");
  }
  fclose(buff);
}

/* Carga los datos en *lista* */
void loadData(string fileName) {
  checkFile(fileName);
  FILE *file;
  file = fopen(fileName.c_str(), "r");
  struct Task readed;
  memset(lista, 0, MAXLISTSIZE * sizeof *lista);
  int i = 0;
  while (fread(&readed, sizeof(struct Task), 1, file)) {
    lista[i] = readed;
    i++;
  }
  listSize = i;
  fclose(file);
  checkLoadedID();
}

struct lessConf taskly;

/* Muestra las tareas con *lessy* */
void viewTasks() {
  quickSort(0, listSize-1);
  //getch();

  string text = "";
  text += "\e[K\e[1;95mLista de tareas\e[0m\n";
  text += "\e[KPresiona \e[1mq\e[0m para salir\n";
  text += "\e[K\n";

  const int secs_per_day = 60 * 60 * 24;
  const int secs_per_hour = 60 * 60;
  const int secs_per_minutes = 60;

  string actualDay = getActualDate("%d-%m-%Y");
  string actualTime = getActualTime("%H:%M");

  for (int i = 0; i < listSize ; i++) {
    if (strcmp(lista[i].name, "") == 0) break;

    text += "\e[K";
    char s[230];
    double diffDays = diffBetweenDates(lista[i].date, actualDay, "%d-%m-%Y");
    double diffTime = diffBetweenDates(lista[i].time, actualTime, "%H:%M");

    int days = diffDays / secs_per_day;
    int hours = diffTime / secs_per_hour + 0.5;
    char diffStr[30];
    if (days == 0 ) {
      if (hours < 0) {
        sprintf(diffStr, "hace %d horas", -hours);
      } else
      if (hours == 0) {
        sprintf(diffStr, "ahora");
      } else {
        sprintf(diffStr, "en %d horas", hours);
      }
    } else
    if (days < 0) {
      sprintf(diffStr, "hace %d días", - days);
    } else {
      sprintf(diffStr, "en %d días", days);
    };

    char color[10];
    if (lista[i].state == 0) {
      strcpy(color, "\e[0m");
    } else
    if (lista[i].state == 1) { // Added
      strcpy(color, "\e[0;94m");
    } else
    if (lista[i].state == 2) { // Edited
      strcpy(color, "\e[0;93m");
    } else
    if (lista[i].state == 3) { // Deleted
      strcpy(color, "\e[0;91m");
    };
    char format[] = "\e[K\e[1m[%s]%s %s %s\"%s\" - %s [%s - %s] (%s)";
    sprintf(s, format, lista[i].id, color, lista[i].completed ? "\e[92;1m[Done]\e[m" : "[Nope]", color, lista[i].name, lista[i].date, lista[i].time, lista[i].endTime, diffStr);

    text += color + string(s) + "\e[0m\n";
  }
  lessText(text, taskly);
}

/* Añade una tarea */
void addTask() {
  clear();
  struct Task newTask;
  newTask.completed = false;
  newTask.state = 1;

  string f = "%H:%M";

  string actualDate = getActualDate("%d-%m-%Y");
  struct tm actualDateTM = getActualDate();
  string actualTime = getActualTime(f);

  printf("\e[H");
  string naimu = "\e[K\e[1;95mAñadir tarea\e[0m\n";
  string desci = "\e[KAñade el nombre, fecha, y hora de la tarea\n";
  printf("\e[K%s", naimu.c_str());
  printf("\e[K%s", desci.c_str());

  printf("Nombre: ");
  cin.getline(newTask.name, sizeof newTask.name);

  if (strcmp(newTask.name, "") == 0) {
    strcpy(conf.statusMessage, "\e[91;1mError:\e[0m Ingresa un nombre");
    return;
  }

  printf("Fecha (%s): ", actualDate.c_str());
  cin.getline(newTask.date, sizeof newTask.date);

  if (strcmp(newTask.date, "") == 0) {
    strcpy(newTask.date, getActualDate("%d-%m-%Y").c_str());
  }
  if (!validDate(newTask.date)) {
    strcpy(conf.statusMessage, "\e[91;1mError:\e[0m Fecha incorrecta");
    return;
  }
  strcpy(newTask.date, formatDate(newTask.date).c_str());

  struct tm endDate = actualDateTM;
  endDate.tm_hour++;

  printf("Hora (%s): ", actualTime.c_str());
  cin.getline(newTask.time, sizeof newTask.time);

  if (strcmp(newTask.time, "") == 0) {
    strcpy(newTask.time, getActualTime(f).c_str());
  }
  if (!validDate(newTask.time, f)) {
    strcpy(conf.statusMessage, "Hora incorrecta");
    return;
  }
  strcpy(newTask.time, formatDate(newTask.time, f).c_str());

  printf("Hora de fin (%s): ", formatDate(endDate, f).c_str()); // Debe de sumarse una hora
  cin.getline(newTask.endTime, sizeof newTask.endTime);

  endDate = getActualDate();
  endDate.tm_hour++;

  if (strcmp(newTask.endTime, "") == 0) {
    strcpy(newTask.endTime, formatDate(endDate, f).c_str());
  }
  if (!validDate(newTask.endTime, f)) {
    strcpy(conf.statusMessage, "Hora incorrecta");
    return;
  }
  strcpy(newTask.endTime, formatDate(newTask.endTime, f).c_str());

  strcpy(newTask.id, createID().c_str());
  lista[listSize] = newTask;
  listSize++;
  conf.changed = 1;

  strcpy(conf.statusMessage, "Tarea añadida");
}

/* Elimina una tarea */
void removeTask() {
  clear();
  struct Task copyList[MAXLISTSIZE];
  int index = -1;

  printf("\e[H");
  string naimu = "\e[K\e[1;95mEliminar tarea\e[0m\n";
  string desci = "\e[KIngresa el índice de la tarea a eliminar\n";
  printf("\e[K%s", naimu.c_str());
  printf("\e[K%s", desci.c_str());

  char ind[20];
  printf("Índice de la tarea: ");
  cin.getline(ind, 10);
  if (strcmp(ind, "") == 0) {
    return;
  };
  for (int i = 0; i < listSize; i++) {
    if (strcmp(lista[i].id, ind) == 0) {
      index = i;
      break;
    }
  }

  if (index == -1) {
    char sa[70];
    sprintf(sa, "\e[93;1mError:\e[0m No hay tarea en el índice [%s]", ind);
    strcpy(conf.statusMessage, sa);
    return;
  }

  if (index < 0) {
    strcpy(conf.statusMessage, "\e[91;1mError:\e[0m Índice debajo de 0");
    return;
  }
  if (strcmp(lista[index].name, "") == 0) {
    char sa[70];
    sprintf(sa, "\e[93;1mError:\e[0m No hay tarea en el índice [%s]", ind);
    strcpy(conf.statusMessage, sa);
    return;
  }

  char s[150];
  char format[] = "\n\e[1m[%s]\e[m \"%s\" - %s [%s - %s] - %s";
  sprintf(s, format, ind, lista[index].name, lista[index].date, lista[index].time, lista[index].endTime, lista[index].completed ? "Done" :  "Nope");
  printf("%s\n\n", s);
  int edit;
  printf("¿Esta es la tarea a eliminar?\n");
  printf("1 ó 0: ");
  scanf("%d", &edit);
  char c;
  while ((c = getchar() != '\n' && c != EOF)) {}
  if (!edit) {return;}

  if (lista[index].state != 3) {
    lista[index].state = 3; // Borrado
  } else {
    lista[index].state = 1; // Añadido
  }
  conf.changed = 1;

  strcpy(conf.statusMessage, "Tarea eliminada");
}

/* Edita una tarea */
void editTask() {
  clear();
  struct Task editedTask;
  editedTask.state = 2; // Edited
  char c;
  int befInd;
  int index = -1;
  char completed[2];

  string f = "%H:%M";

  string actualDate = getActualDate("%d-%m-%Y");
  string actualTime = getActualTime(f);

  printf("\e[H");
  string naimu = "\e[K\e[1;95mEditar tarea\e[0m\n";
  string desci = "\e[KIngresa el índice de la tarea y los datos de la tarea\n";
  printf("\e[K%s", naimu.c_str());
  printf("\e[K%s", desci.c_str());

  char ind[10];
  printf("Índice de la tarea: ");
  cin.getline(ind, 10);

  if (strcmp(ind, "") == 0) {
    return;
  };
  for (int i = 0; i < listSize; i++) {
    if (strcmp(lista[i].id, ind) == 0) {
      index = i;
      break;
    }
  }

  if (index == -1) {
    char sa[70];
    sprintf(sa, "\e[93;1mError:\e[0m No hay tarea en el índice [%s]", ind);
    strcpy(conf.statusMessage, sa);
    return;
  }
  if (index < 0) {
    strcpy(conf.statusMessage, "\e[91;1mError:\e[0m Índice debajo de 0");
    return;
  }
  if (strcmp(lista[index].name, "") == 0) {
    char sa[70];
    sprintf(sa, "\e[93;1mError:\e[0m No hay tarea en el índice [%s]", ind);
    strcpy(conf.statusMessage, sa);
    return;
  }

  char s[150];
  char format[] = "\n\e[1m[%s]\e[m \"%s\" - %s [%s - %s] - %s";
  sprintf(s, format, ind, lista[index].name, lista[index].date, lista[index].time, lista[index].endTime, lista[index].completed ? "Done" :  "Nope");
  printf("%s\n\n", s);
  int edit;
  printf("¿Esta es la tarea a editar?\n");
  printf("1 ó 0: ");
  scanf("%d", &edit);
  while ((c = getchar() != '\n' && c != EOF)) {}
  if (!edit) {return;}
  
  int t = 0;

  printf("Nombre: ");
  cin.getline(editedTask.name, 50);

  printf("Fecha (%s): ", lista[index].date);
  cin.getline(editedTask.date, sizeof editedTask.date);

  printf("Hora (%s): ", lista[index].time);
  cin.getline(editedTask.time, sizeof editedTask.time);

  printf("Hora de fin (%s): ", lista[index].endTime);
  cin.getline(editedTask.endTime, sizeof editedTask.endTime);

  printf("Completado (1 ó 0): ");
  cin.getline(completed, 2);

  if (strcmp(editedTask.name, "") == 0) {
    strcpy(editedTask.name, lista[index].name);
    t++;
  }
  if (strcmp(editedTask.date, "") == 0) {
    strcpy(editedTask.date, lista[index].date);
    t++;
  }
  if (strcmp(editedTask.time, "") == 0) {
    strcpy(editedTask.time, lista[index].time);
    t++;
  }
  if (strcmp(editedTask.endTime, "") == 0) {
    strcpy(editedTask.endTime, lista[index].endTime);
    t++;
  }
  if (strcmp(completed, "") == 0) {
    editedTask.completed = lista[index].completed;
    t++;
  } else {
    editedTask.completed = stoi(completed);
  }

  strcpy(editedTask.id, lista[index].id);

  if (t >= 5) {
    strcpy(conf.statusMessage, "La tarea no fue modificada");
    return;
  }

  lista[index] = editedTask;
  conf.changed = 1;

  strcpy(conf.statusMessage, "Tarea modificada");
}

/* Guarda los datos y vuelve a cargarlos */
void saveData(string fileName) {
  FILE *temp;
  temp = fopen("temp.bin", "w");
  for (int i = 0; i < MAXLISTSIZE; i++) {
    if (strcmp(lista[i].name, "") == 0) break;
    if (lista[i].state == 3) continue; // Si está borrado, ignorar
    lista[i].state = 0;
    fwrite(&lista[i], sizeof(struct Task), 1, temp);
  }
  fclose(temp);
  remove(fileName.c_str());
  rename("temp.bin", fileName.c_str());
  strcpy(conf.statusMessage, "Datos guardados");
  loadData(fileName); // Cargar nuevamente los datos
  conf.changed = 0;
}
