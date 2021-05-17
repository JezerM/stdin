#include <unistd.h>
#include <ctype.h>
#include <cstdio>
#include <stdlib.h>
#include <iostream>
#include <vector>

#include "global.h"

using namespace std;

struct lessConf {
  int pos = 0;
  int cols;
  int rows;
  int lines = 0;
  bool running = false;
};

struct lessConf lessy;

void processLessKey() {
  char c = readKey();
  /*
  if (iscntrl(c)) {
    printf("%d\r\n", c);
  } else {
    printf("%d ('%c')\r\n", c, c);
  }
  */
  if (c == '\x1b') {
    char seq[3];
    if (read(STDIN_FILENO, &seq[0], 1) != 1) {}
    else if (read(STDIN_FILENO, &seq[1], 1) != 1) {}
    else if (seq[0] == '[') {
      switch (seq[1]) {
        case 'A': c = 'k'; break;
        case 'B': c = 'j'; break;
      }
    }
  }
  if (c == 'q') {
    lessy.running = false;
  } else
  if (c == 'k') {
    if (lessy.pos > 0) {
      lessy.pos--;
    }
  } else
  if (c == 'j') {
    if (lessy.pos + lessy.rows < lessy.lines + 1) {
      lessy.pos++;
    }
  }
}

/* Divide el texto para poder caber en el tamaño especificado de columns y filas */
vector<string> stripText(string text, struct lessConf confi) {
  vector<string> origLines = split(text, "\n");
  vector<string> lines;

  //printf("Starting!\n");
  //printf("Original lines: %.2lu\n", origLines.size());
  for (int p = 0; p < origLines.size(); p++) {
    string line = "";
    vector<string> wordArr = split(origLines[p], " ");

    //printf("Word count: %.2lu\n", wordArr.size());
    //printf("Starting slice\n\n");
    for (int i = 0; i < wordArr.size(); i++) {
      string word = wordArr[i];
      string nextWord = "";
      if (i + 1 >= wordArr.size()) {
        nextWord = "";
      } else {
        nextWord = wordArr[i + 1];
      }
      if (word == "\0") {
        line += " ";
        continue;
      }
      if (line.size() + word.size() >= confi.cols) {
        lines.push_back(line);
        line = "";
      }
      line += word + " ";
      if (i + 1 >= wordArr.size()) {
        line += nextWord;
        lines.push_back(line);
        line = "";
      }
    }
  }
  //printf("Sliced!\n\n");

  return lines;
}

string formatText(vector<string> lines, struct lessConf confi) {
  string copy = "";
  int p = 0;

  for (int i = confi.pos; i < lines.size(); i++) {
    if (i >= confi.pos + confi.rows - 1) {
      break;
    }
    copy += lines[i];
    if (p < confi.rows - 2) {
      copy += "\r\n";
    }
    p++;
  }
  char s[30];
  sprintf(s, "\r\n\e[90mLines: %lu / %lu\e[m", confi.rows > lines.size() ? lines.size() : confi.rows + confi.pos - 1, lines.size());
  copy += s;
  /*
   *for (int i = 0; i < confi.rows; i++) {
   *  copy += lines[confi.pos + i];
   *  if (i < confi.rows - 1) {
   *    copy += "\r\n";
   *  }
   *}
   */

  return copy;
}

void lessText(string text) {
  enableRawMode(false);
  clear();
  printf("\e[?25l");

  getWindowSize(&lessy.rows, &lessy.cols);
  vector<string> lines = stripText(text, lessy);
  lessy.lines = lines.size();

  struct lessConf confCopy = lessy;
  lessy.running = true;

  while (lessy.running == 1) {
    getWindowSize(&lessy.rows, &lessy.cols);
    clear();
    string formatted = formatText(lines, lessy);
    write(STDOUT_FILENO, formatted.c_str(), formatted.size());
    write(STDOUT_FILENO, "\e[?25l", 6);
    processLessKey();
  }
  printf("\e[?25h");
  disableRawMode();
}

