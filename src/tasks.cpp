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

  string actualDay = getActualDate();

  for (int i = 0; i < LISTSIZE ; i++) {
    if (strcmp(lista[i].name, "") == 0) break;

    text += "\e[K";
    char s[180];
    int difference = diffBetweenDates(lista[i].date, actualDay) / secs_per_day;
    char diffStr[10];
    sprintf(diffStr, "%d days", difference);
    sprintf(s, "[%d] \"%s\" - %s %s (in %s) - %s", i, lista[i].name, lista[i].date, lista[i].time, diffStr, lista[i].completed ? "Done" :  "Nope");

    string color;
    if (lista[i].state == 0) {
      color = "";
    } else
    if (lista[i].state == 1) {
      color = "\e[94m";
    } else
    if (lista[i].state == 2) {
      color = "\e[93m";
    } else
    if (lista[i].state == 3) {
      color = "\e[91m";
    };
    text += color + string(s) + "\e[0m\n";
  }
  string a = "\e[KHola a todo el mundo!";
  lessText(text, taskly);
}

/* Añade una tarea */
void addTask() {
  clear();
  struct task newTask;
  newTask.completed = false;
  newTask.state = 1;

  printf("\e[H");
  string mag = "\e[1;95m";
  string clLine = "\e[K";
  string naimu = clLine + mag + "Añadir tarea" + "\e[0m\n";
  string desci = clLine + "Añade el nombre, fecha, y hora de la tarea" + "\n";
  printf("\e[K%s", naimu.c_str());
  printf("\e[K%s", desci.c_str());

  printf("Nombre: ");
  cin.getline(newTask.name, sizeof newTask.name);

  if (strcmp(newTask.name, "") == 0) {
    strcpy(conf.statusMessage, "Error: Ingresa un nombre");
    return;
  }

  printf("Fecha (09-12-2021): ");
  cin.getline(newTask.date, sizeof newTask.date);

  printf("Hora (13:15): ");
  cin.getline(newTask.time, sizeof newTask.time);

  if (strcmp(newTask.date, "") == 0) {
    strcpy(newTask.date, getActualDate().c_str());
  }
  if (strcmp(newTask.time, "") == 0) {
    string f = "%H:%M";
    strcpy(newTask.time, getActualTime(f).c_str());
  }

  lista[size] = newTask;
  size++;
  conf.changed = 1;
  printf("\nTarea añadida\n");
  printf("\"%s\" %s %s\n", newTask.name, newTask.date, newTask.time);

  printf("\n\e[KPresiona una tecla para continuar\n");
  printf("\e[?25l");
  getch();
  printf("\e[?25h");

  strcpy(conf.statusMessage, "Tarea añadida");
}

/* Elimina una tarea */
void removeTask() {
  clear();
  struct task copyList[LISTSIZE];
  int index;

  printf("\e[H");
  string mag = "\e[1;95m";
  string clLine = "\e[K";
  string naimu = clLine + mag + "Eliminar tarea" + "\e[0m\n";
  string desci = clLine + "Ingresa el índice de la tarea a eliminar" + "\n";
  printf("\e[K%s", naimu.c_str());
  printf("\e[K%s", desci.c_str());

  printf("Índice de la tarea: ");
  scanf("%d", &index);
  if (index < 0) {
    strcpy(conf.statusMessage, "Error: Índice debajo de 0");
    return;
  }
  if (strcmp(lista[index].name, "") == 0) {
    char sa[50];
    sprintf(sa, "Error: No hay tarea en el índice [%d]\n", index);
    strcpy(conf.statusMessage, sa);
    return;
  }
  if (lista[index].state != 3) {
    lista[index].state = 3; // Borrado
  } else {
    lista[index].state = 1;
  }
  conf.changed = 1;
  printf("\nTarea eliminada\n\n");
  printf("\n\e[KPresiona una tecla para continuar\n");
  printf("\e[?25l");
  getch();
  printf("\e[?25h");

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

  printf("\e[H");
  string mag = "\e[1;95m";
  string clLine = "\e[K";
  string naimu = clLine + mag + "Editar tarea" + "\e[0m\n";
  string desci = clLine + "Ingresa el índice de la tarea y los datos de la tarea" + "\n";
  printf("\e[K%s", naimu.c_str());
  printf("\e[K%s", desci.c_str());

  printf("Índice de la tarea: ");
  scanf("%d", &index);
  while((c = getchar()) != '\n' && c != EOF) {}
  if (index < 0) {
    strcpy(conf.statusMessage, "Error: Índice debajo de 0");
    return;
  }
  if (strcmp(lista[index].name, "") == 0) {
    char sa[50];
    sprintf(sa, "Error: No hay tarea en el índice [%d]\n", index);
    strcpy(conf.statusMessage, sa);
    return;
  }
  printf("Nombre: ");
  cin.getline(editedTask.name, 50);
  printf("Fecha: ");
  cin.getline(editedTask.date, 11);
  printf("Completado (1 ó 0): ");
  cin.getline(completed, 2);
  if (strcmp(editedTask.name, "") == 0) {
    strcpy(editedTask.name, lista[index].name);
  }
  if (strcmp(editedTask.date, "") == 0) {
    strcpy(editedTask.date, lista[index].date);
  }
  if (strcmp(completed, "") == 0) {
    editedTask.completed = lista[index].completed;
  } else {
    editedTask.completed = stoi(completed);
  }

  lista[index] = editedTask;
  conf.changed = 1;
  printf("Tarea modificada\n\n");
  printf("\n\e[KPresiona una tecla para continuar\n");
  printf("\e[?25l");
  getch();
  printf("\e[?25h");

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
