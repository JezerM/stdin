#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
#include "global.h"
#include "winConf.h"
#include <time.h>

using namespace std;

void lessText(string text);

int size;

struct task {
  char id[8];
  char name[51];
  char date[12];
  char time[12];
  bool completed = false;
  int state; // 0 "nada", 1 "añadido", 2 "editado", 3 "borrado"
};

struct task lista[100];

void checkFile(string name) {
  FILE *buff;
  buff = fopen(name.c_str(), "r");
  if (!buff) {
    buff = fopen(name.c_str(), "w");
  }
  fclose(buff);
}

void loadData(string fileName) {
  checkFile(fileName);
  FILE *file;
  file = fopen("data.bin", "r");
  struct task readed;
  int i = 0;
  while (fread(&readed, sizeof(struct task), 1, file)) {
    lista[i] = readed;
    i++;
  }
  size = i;
  fclose(file);
}

void viewTasks() {
  string text = "";
  for (int i = 0; i < 100 ; i++) {
    if (strcmp(lista[i].name, "") == 0) break;
    char s[150];
    sprintf(s, "[%d] \"%s\" - %s - %s", i, lista[i].name, lista[i].date, lista[i].completed ? "Done" :  "Nope");
    if (lista[i].state == 0) {
      text += s;
    } else
    if (lista[i].state == 1) {
      text += "\e[93m" + string(s) + "\e[0m";
    } else
    if (lista[i].state == 2) {
      text += "\e[94m" + string(s) + "\e[0m";
    } else
    if (lista[i].state == 3) {
      text += "\e[91m" + string(s) + "\e[0m";
    }
    text += "\r\n";
  }
  lessText(text);
}

void addTask() {
  struct task newTask;
  newTask.completed = false;
  newTask.state = 1;
  char c;
  while ((c = getchar() != '\n' && c != EOF)) {}
  printf("--- Adding task ---\n");
  printf("Name: ");
  cin.getline(newTask.name, 50);

  printf("Date (09-12-2021): ");
  cin.getline(newTask.date, 11);
  if (strcmp(newTask.name, "") == 0 || strcmp(newTask.date, "") == 0) {
    printf("No task added\n");
    return;
  }

  printf("\"%s\" - %s\n", newTask.name, newTask.date);
  lista[size] = newTask;
  size++;
  conf.changed = 1;
}

void removeTask() {
  struct task copyList[100];
  char c;
  int index;
  while((c = getchar()) != '\n' && c != EOF) {}
  printf("--- Removing task ---\n");
  printf("File index: ");
  scanf("%d", &index);
  if (index < 0) {
    printf("Error: Index below 0\n");
    return;
  }
  if (strcmp(lista[index].name, "") == 0) {
    printf("No task in index [%d]\n", index);
    return;
  }
  if (lista[index].state != 3) {
    lista[index].state = 3; // Borrado
  } else {
    lista[index].state = 1;
  }
  conf.changed = 1;
}

void editTask() {
  struct task editedTask;
  editedTask.state = 2; // Edited
  char c;
  int index;
  char completed[2];
  while((c = getchar()) != '\n' && c != EOF) {}
  printf("--- Editing task ---\n");
  printf("File index: ");
  scanf("%d", &index);
  while((c = getchar()) != '\n' && c != EOF) {}
  if (index < 0) {
    printf("Error: Index below 0\n");
    return;
  }
  if (strcmp(lista[index].name, "") == 0) {
    printf("No task in index [%d]\n", index);
    return;
  }
  printf("Name: ");
  cin.getline(editedTask.name, 50);
  printf("Date: ");
  cin.getline(editedTask.date, 11);
  printf("Completed (1 or 0): ");
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
  printf("Task modified\n");
  conf.changed = 1;
}

void saveData(string fileName) {
  FILE *temp;
  temp = fopen("temp.bin", "w");
  for (int i = 0; i < 100; i++) {
    if (strcmp(lista[i].name, "") == 0) break;
    if (lista[i].state == 3) continue; // Si está borrado, ignorar
    lista[i].state = 0;
    fwrite(&lista[i], sizeof(struct task), 1, temp);
  }
  fclose(temp);
  remove(fileName.c_str());
  rename("temp.bin", fileName.c_str());
  printf("Archivos guardados\n");
  loadData(fileName); // Cargar nuevamente los datos
  conf.changed = 0;
}
