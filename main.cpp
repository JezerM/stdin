/*** includes ***/
#include <ctype.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <locale.h>

void enableRawMode();

void die(const char *c);

void processKey();
void refreshScreen();
void init();

/*** init ***/

int main() {
  char *locale;
  locale = setlocale(LC_ALL, "en_US.utf8");
  enableRawMode();
  init();
  while (1) {
    refreshScreen();
    processKey();
  }

  printf("\x1b[?25h");
  return 0;
}
