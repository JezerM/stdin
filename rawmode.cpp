/*** includes ***/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

#define CTRL_KEY(k) ((k) & 0x1f)

/*** data ***/

struct winConfig {
  int cx, cy;
  int srows;
  int scols;
  struct termios orig_termios;
};

struct winConfig conf;


/*** terminal ***/

void clear() {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
}

void die(const char *s) {
  clear();
  perror(s);
  exit(1);
}

void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &conf.orig_termios) == -1) {
    die("tcsetattr");
  }
}

void enableRawMode() {
  if (tcgetattr(STDIN_FILENO, &conf.orig_termios) == -1) {
    die("tcgetattr");
  }
  atexit(disableRawMode);

  struct termios raw = conf.orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
    die("tcsetattr");
  } 
}

char readKey() {
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN) die("read");
  }
  return c;
}


int getCursorPosition(int *rows, int *cols) {
  char buf[32];
  unsigned int i = 0;
  if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;
  while (i < sizeof(buf) - 1) {
    if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
    if (buf[i] == 'R') break;
    i++;
  }
  buf[i] = '\0';
  if (buf[0] != '\x1b' || buf[1] != '[') return -1;
  if (sscanf(&buf[2], "%d;%d", rows, cols) != 2) return -1;
  return 0;
}


int getWindowSize(int *rows, int *cols) {
  struct winsize ws;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
    return getCursorPosition(rows, cols);
  } else {
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
  }
}

/*** buffer ***/

struct abuf {
  char *b;
  int len;
};

#define ABUF_INIT {NULL, 0}

void abAppend(struct abuf *ab, const char *s, int len) {
  char *ne;
  ne = (char*) realloc(ab->b, ab->len + len);
  if (ne == NULL) return;
  memcpy(&ne[ab->len], s, len);
  ab->b = ne;
  ab->len += len;
}

void abFree(struct abuf *ab) {
  free(ab->b);
}

/*** input ***/

void moveCursor(std::string dir) {
  if (dir == "left") {
    if (conf.cx != 0) {
      conf.cx--;
    }
  } else if (dir == "right") {
    if (conf.cx != conf.scols -1) {
      conf.cx++;
    }
  } else if (dir == "up") {
    if (conf.cy != 0) {
      conf.cy--;
    }
  } else if (dir == "down") {
    if (conf.cy != conf.srows -1) {
      conf.cy++;
    }
  }
}

void processKey() {
  char c = readKey();

  if (c == '\x1b') {
    char seq[3];
    if (read(STDIN_FILENO, &seq[0], 1) != 1) {}
    else if (read(STDIN_FILENO, &seq[1], 1) != 1) {}
    else if (seq[0] == '[') {
      switch (seq[1]) {
        case 'A': c = 'k'; break;
        case 'B': c = 'j'; break;
        case 'C': c = 'l'; break;
        case 'D': c = 'h'; break;
      }
    }
  }

  if (c == CTRL_KEY('q')) {
    clear();
    exit(0);
  }
  if (c == 'k') {
    moveCursor("up");
  }
  if (c == 'j') {
    moveCursor("down");
  }
  if (c == 'l') {
    moveCursor("right");
  }
  if (c == 'h') {
    moveCursor("left");
  }
}

/*** output ***/

void editorDrawRows(struct abuf *ab) {
  int y;
  for (y = 0; y < conf.srows; y++) {
    if (y == conf.srows / 3) {
      char welcome[80];
      int welcomelen = snprintf(welcome, sizeof(welcome),
        "Use arrows, or khjl to move. CTRL+Q to exit");
      if (welcomelen > conf.scols) welcomelen = conf.scols;
      int padding = (conf.scols - welcomelen) / 2;
      if (padding) {
        abAppend(ab, "~", 1);
        padding--;
      }
      while (padding--) abAppend(ab, " ", 1);
      abAppend(ab, welcome, welcomelen);
    } else {
      abAppend(ab, "~", 1);
    }
    abAppend(ab, "\x1b[K", 3);
    if (y < conf.srows - 1) {
      abAppend(ab, "\r\n", 2);
    }
  }
}

void refreshScreen() {
  struct abuf ab = ABUF_INIT;

  abAppend(&ab, "\x1b[?25l", 6);
  abAppend(&ab, "\x1b[H", 3);

  editorDrawRows(&ab);

  char buf[32];
  snprintf(buf, sizeof(buf), "\x1b[%d;%dH", conf.cy+1,conf.cx+1);
  abAppend(&ab, buf, strlen(buf));
  abAppend(&ab, "\x1b[?25h", 6);

  write(STDOUT_FILENO, ab.b, ab.len);
  abFree(&ab);
}

/*** init ***/

void init() {
  conf.cx = 0;
  conf.cy = 0;
  if (getWindowSize(&conf.srows, &conf.scols) == -1) die("getWindowSize");
}

