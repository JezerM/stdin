#ifndef TASKS_H
#define TASKS_H 1

#define MAXLISTSIZE 100

struct Task {
  char id[8]; //
  char name[51]; // Es el nombre de la tarea, es designado por el usuario.
  char date[12]; // Es la fecha en la que la tarea ocurre, es designada por el usuario.
  char time[12]; // Es la hora a la que empieza la tarea, es designada por el usuario.
  char endTime[12]; // Es la hora a la que termina la tarea, es designada por el usuario.
  bool completed = false; // Indica si la tarea ya fue completada.
  int state; // 0 "Nada", 1 "Añadido", 2 "Editado", 3 "Borrado".
};


extern struct Task lista[];

extern int listSize;

#endif
