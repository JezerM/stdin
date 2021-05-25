#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
#include "global.h"
#include "winConf.h"
#include <time.h>

using namespace std;

void lessText(string text, struct lessConf lessy);
string getActualDate(string formatStr = "%d-%m-%Y");
string getActualTime(string formatStr = "H:%M:%S");
double diffBetweenDates(string date1, string date2, string formatStr = "%d-%m-%Y");
int validDate(string date, string formatStr = "%d-%m-%Y");
string formatDate(string date, string formatStr = "%d-%m-%Y");

int size;

struct task {
  char id[8];
  char name[51];
  char date[12];
  char time[12];
  bool completed = false;
  int state; // 0 "nada", 1 "añadido", 2 "editado", 3 "borrado"
};

#define LISTSIZE 100

struct task lista[LISTSIZE];

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
  file = fopen("data.bin", "r");
  struct task readed;
  memset(lista, 0, LISTSIZE * sizeof *lista);
  int i = 0;
  while (fread(&readed, sizeof(struct task), 1, file)) {
    lista[i] = readed;
    i++;
  }
  size = i;
  fclose(file);
}

struct lessConf taskly;

/* Muestra las tareas con *lessy* */
void viewTasks() {
  string text = "";
  text += "\e[K\e[1;95mLista de tareas\e[0m\n";
  text += "\e[KPresiona \e[1mq\e[0m para salir\n";
  text += "\e[K\n";

  const int secs_per_day = 60 * 60 * 24;
  const int secs_per_hour = 60 * 60;
  const int secs_per_minutes = 60;

  string actualDay = getActualDate("%d-%m-%Y");
  string actualTime = getActualTime("%H:%M");

  for (int i = 0; i < LISTSIZE ; i++) {
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
    sprintf(s, "\e[1m[%d]%s \"%s\" - %s %s (%s) - %s", i, color, lista[i].name, lista[i].date, lista[i].time, diffStr, lista[i].completed ? "Done" :  "Nope");

    text += color + string(s) + "\e[0m\n";
  }
  lessText(text, taskly);
}

/* Añade una tarea */
void addTask() {
  clear();
  struct task newTask;
  newTask.completed = false;
  newTask.state = 1;

  string f = "%H:%M";

  string actualDate = getActualDate();
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
    strcpy(newTask.date, getActualDate().c_str());
  }
  if (!validDate(newTask.date)) {
    strcpy(conf.statusMessage, "\e[91;1mError:\e[0m Fecha incorrecta");
    return;
  }
  strcpy(newTask.date, formatDate(newTask.date).c_str());

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

  lista[size] = newTask;
  size++;
  conf.changed = 1;

  strcpy(conf.statusMessage, "Tarea añadida");
}

/* Elimina una tarea */
void removeTask() {
  clear();
  struct task copyList[LISTSIZE];
  int index;

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
  index = stoi(ind);

  if (index < 0) {
    strcpy(conf.statusMessage, "\e[91;1mError:\e[0m Índice debajo de 0");
    return;
  }
  if (strcmp(lista[index].name, "") == 0) {
    char sa[50];
    sprintf(sa, "\e[93;1mError:\e[0m No hay tarea en el índice [%d]", index);
    strcpy(conf.statusMessage, sa);
    return;
  }

  char s[150];
  sprintf(s, "\n\e[1m[%d]\e[0m \"%s\" - %s %s - %s", index, lista[index].name, lista[index].date, lista[index].time, lista[index].completed ? "Done" :  "Nope");
  printf("%s\n\n", s);
  int edit;
  printf("¿Esta es la tarea a editar?\n");
  printf("1 ó 0: ");
  scanf("%d", &edit);
  char c;
  while ((c = getchar() != '\n' && c != EOF)) {}
  if (!edit) {return;}

  if (lista[index].state != 3) {
    lista[index].state = 3; // Borrado
  } else {
    lista[index].state = 1;
  }
  conf.changed = 1;

  strcpy(conf.statusMessage, "Tarea eliminada");
}

/* Edita una tarea */
void editTask() {
  clear();
  struct task editedTask;
  editedTask.state = 2; // Edited
  char c;
  int index;
  char completed[2];

  string f = "%H:%M";

  string actualDate = getActualDate();
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
  index = stoi(ind);


  if (index < 0) {
    strcpy(conf.statusMessage, "\e[91;1mError:\e[0m Índice debajo de 0");
    return;
  }
  if (strcmp(lista[index].name, "") == 0) {
    char sa[50];
    sprintf(sa, "\e[93;1mError:\e[0m No hay tarea en el índice [%d]", index);
    strcpy(conf.statusMessage, sa);
    return;
  }

  char s[150];
  sprintf(s, "\n\e[1m[%d]\e[0m \"%s\" - %s %s - %s", index, lista[index].name, lista[index].date, lista[index].time, lista[index].completed ? "Done" :  "Nope");
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

  printf("Fecha (%s): ", actualDate.c_str());
  cin.getline(editedTask.date, sizeof editedTask.date);

  printf("Hora (%s): ", actualTime.c_str());
  cin.getline(editedTask.time, sizeof editedTask.time);

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
  if (strcmp(completed, "") == 0) {
    editedTask.completed = lista[index].completed;
    t++;
  } else {
    editedTask.completed = stoi(completed);
  }

  if (t >= 4) {
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
  for (int i = 0; i < LISTSIZE; i++) {
    if (strcmp(lista[i].name, "") == 0) break;
    if (lista[i].state == 3) continue; // Si está borrado, ignorar
    lista[i].state = 0;
    fwrite(&lista[i], sizeof(struct task), 1, temp);
  }
  fclose(temp);
  remove(fileName.c_str());
  rename("temp.bin", fileName.c_str());
  strcpy(conf.statusMessage, "Datos guardados");
  loadData(fileName); // Cargar nuevamente los datos
  conf.changed = 0;
}
