#include <cstring>
#include <unistd.h>
#include <ctype.h>
#include <cstdio>
#include <stdlib.h>
#include <iostream>
#include <vector>

#include "global.h"
#include "winConf.h"

using namespace std;

struct lessConf g;

/* Marca la búsqueda */
string markSearch(string text, struct lessConf lessy) {
  string search = string(lessy.search);
  if (search.size() <= 0) {
    return text;
  }
  search = search.substr(1);
  if (search.size() <= 0) return text;
  string repl = "\e[103;30m" + search + "\e[0m";

  for (size_t pos = 0; ; pos += repl.length()) {
    pos = text.find(search, pos);
    if (pos == string::npos) {
      break;
    }
    text.erase(pos, search.length());
    text.insert(pos, repl);
  }
  return text;
}

/* Lee las flechas direccionales */
char readArrowKey(char c) {
  char s = '\0';
  char seq[3];
  if (read(STDIN_FILENO, &seq[0], 1) != 1) {}
  else if (read(STDIN_FILENO, &seq[1], 1) != 1) {}
  else if (seq[0] == '[') {
    switch (seq[1]) {
      case 'A': s = 'k'; break;
      case 'B': s = 'j'; break;
      case 'C': s = 'h'; break;
      case 'D': s = 'l'; break;
    }
  }
  return s;
}

bool searchKey = false;

/* Procesa las teclas leídas */
void processLessKey(struct lessConf *lessy) {
  char c = readKey();
  if (c == '/') {
    searchKey = true;
  }
  if (searchKey) {
    string chares = string(lessy->search);
    if (c == 127 || c == 8) { // Backspace
      chares.pop_back();
      if (chares.size() == 0) searchKey = false;
    } else
    if (c == '\x1b') { // For arrow keys
      rawTimer(true);
      if (readArrowKey(c) == '\0') {
        strcpy(lessy->search, "");
        searchKey = false;
        rawTimer(false);
        return;
      }
    } else
    if (c == 13 || c == '\n') { // Return key
      searchKey = false;
      rawTimer(false);
      return;
    }
    else {
      chares += c;
    };
    strcpy(lessy->search, chares.c_str());
    return;
  }

  /*
  if (iscntrl(c)) {
    printf("%d\r\n", c);
  } else {
    printf("%d ('%c')\r\n", c, c);
  }
  */
  if (c == '\x1b') {
    c = readArrowKey(c);
  }
  if (c == 'q') {
    lessy->running = false;
  } else
  if (c == 'k') {
    if (lessy->pos > 0) {
      lessy->pos--;
    }
  } else
  if (c == 'j') {
    if (lessy->pos + lessy->rows < lessy->lines + 1) {
      lessy->pos++;
    }
  }
}

/* Divide el texto para poder caber horizontalmente en la pantalla */
vector<string> stripText(string text, struct lessConf confi) {
  text = markSearch(text, confi);
  vector<string> origLines = split(text, "\n");
  vector<string> lines;

  //printf("Starting!\n");
  //printf("Original lines: %.2lu\n", origLines.size());
  for (int p = 0; p < origLines.size(); p++) {
    origLines[p] = strreplace(origLines[p], "\r", "");
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

/* Formatea el texto para caber verticalmente en la pantalla */
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
  char search[40];
  sprintf(search, "\r\n%s", confi.search);
  char tx[50];
  sprintf(tx, "\r\n\e[90mLines: %lu / %lu\e[m", confi.rows > lines.size() ? lines.size() : confi.rows + confi.pos - 1, lines.size());
  copy += strlen(confi.search) > 0 ? search : tx;
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

/* Crea un loop en el que se muestra el texto introducido, de forma interactiva */
void lessText(string text, struct lessConf lessy) {
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
    lines = stripText(text, lessy);
    lessy.lines = lines.size();
    clear();
    string formatted = formatText(lines, lessy);
    write(STDOUT_FILENO, formatted.c_str(), formatted.size());
    write(STDOUT_FILENO, "\e[?25l", 6);
    processLessKey(&lessy);
  }
  printf("\e[?25h");
  disableRawMode();
}

