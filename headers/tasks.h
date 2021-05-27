#ifndef TASKS_H
#define TASKS_H 1

#define MAXLISTSIZE 100

struct Task {
  char id[8];
  char name[51];
  char date[12];
  char time[12];
  char endTime[12];
  bool completed = false;
  int state; // 0 "nada", 1 "añadido", 2 "editado", 3 "borrado"
};


extern struct Task lista[];

extern int listSize;

#endif
