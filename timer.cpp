#include <thread>
#include <chrono>

#include "menu.h"

using namespace std;

void runTimer(struct Timer *timer) {
  timer->running = true;
  timer->time = 90;
  while (timer->running) {
    timer->seconds = timer->time % 60;
    timer->minutes = (timer->time / 60) % 60;
    timer->hours = (timer->time / 3600) % 60;
    this_thread::sleep_for(chrono::seconds(1));
    if (timer->time == 0) {
      break;
    }
    timer->time--;
  }
  timer->running = false;
}

