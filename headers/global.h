#include <vector>
#include <iostream>

#ifndef GLOBALA_H
#define GLOBALA_H 1

// Rawmode

void clear();
void exitAll();
void disableRawMode();
void enableRawMode(bool t);
void rawTimer(bool mode);
char readKey();
void getch();
int getWindowSize(int *rows, int *cols);
int getCursorPosition(int *rows, int *cols);

// Timer
void restartTimer();

// Sound

void playBeep(float freq, int milliseconds);

// Others
std::vector<std::string> split(std::string text, std::string del);
std::string strreplace(std::string orgString, const std::string search, const std::string replace );
std::string catFile(std::string fileName, bool ansi);


#endif
