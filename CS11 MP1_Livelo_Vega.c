#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
// Source - https://stackoverflow.com/a/17271636
// Posted by David Ranieri
// Retrieved 2026-11-16, License - CC BY-SA 3.0
#define clear() printf("\033[H\033[J")
#define AREA 15
#define FOOD 5
bool playing = false;
bool inMenu = true;
int snakeLength = 1;

enum DIRECTION { UP, DOWN, LEFT, RIGHT };
enum DIRECTION snakeDirection = RIGHT;
enum DIFFICULTY { EASY, DIFFICULT };
enum DIFFICULTY difficulty;
struct Point {
  int x;
  int y;
};

// Source -
// https://stackoverflow.com/questions/7469139/what-is-the-equivalent-to-getch-getche-in-linux
static struct termios old, current;

/* Initialize new terminal i/o settings */
void initTermios(int echo) {
  tcgetattr(0, &old);         /* grab old terminal i/o settings */
  current = old;              /* make new settings same as old settings */
  current.c_lflag &= ~ICANON; /* disable buffered i/o */
  if (echo) {
    current.c_lflag |= ECHO; /* set echo mode */
  } else {
    current.c_lflag &= ~ECHO; /* set no echo mode */
  }
  tcsetattr(0, TCSANOW, &current); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void) { tcsetattr(0, TCSANOW, &old); }

/* Read 1 character - echo defines echo mode */
char getch_(int echo) {
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

/* Read 1 character without echo */
char getch(void) { return getch_(0); }

/* Read 1 character with echo */
char getche(void) { return getch_(1); }
void PrintAt(int x, int y, char c) {
  // Source - https://stackoverflow.com/a/72073868
  // Posted by AMZG studio's, modified by community. See post 'Timeline' for
  // change history Retrieved 2026-11-16, License - CC BY-SA 4.0
  // Noted in comments that the STDOUT starts at index 1
  printf("\033[%d;%dH%c", y + 1, (x * 2) + 1, c);
}

void PrintBoard() {
  for (int i = 0; i <= AREA; i++) {
    PrintAt(0, i, '#');
    PrintAt(AREA, i, '#');
    PrintAt(i, 0, '#');
    PrintAt(i, AREA, '#');
    switch (snakeDirection) {
    case LEFT:
      PrintAt(5, 5, 'L');
      break;
    case RIGHT:
      PrintAt(5, 5, 'R');
      break;
    default:
      break;
    }
  }
}

void PrintSnake() {
  // TODO
}

void PrintBlockAndFood() {
  // TODO
}

void Menu();

void StartGame() {
  while (playing) {
    clear();
    PrintBoard();
    char key;
    key = getch();
    switch (key) {
    case 'a':
      snakeDirection = LEFT;
      break;
    case 'd':
      snakeDirection = RIGHT;
      break;
    default:
      break;
    }
    PrintBlockAndFood();
    PrintSnake();
    PrintAt(0, AREA + 2, '\n');
    usleep(100000);
  }
  inMenu = true;
  do {
    printf("Game over! Do you want to play again?\n(1) Yes\n(2) Exit\n");
    int input;
    scanf("%d", &input);
    switch (input) {
    case 1:
      Menu();
      inMenu = false;
      break;
    case 2:
      inMenu = false;
      break;
    default:
      break;
    }
  } while (inMenu);
}

void Menu() {
  int input;
  do {
    printf("The Snake Game:\nChoose an option:\n(1) Start Game\n(2) "
           "Instructions\n(3) Exit\n");
    scanf("%d", &input);
    switch (input) {
    case 1:
      input = 0;
      printf("Choose Difficulty\n(1) Easy\n(2) Difficult\n");
      scanf("%d", &input);
      switch (input) {
      case 1:
        difficulty = DIFFICULT;
        break;
      case 2:
        difficulty = EASY;
        break;
      }
      inMenu = false;
      playing = true;
      StartGame();
      break;
    case 2:
      // Add instructions here
      printf("");
      break;
    case 3:
      inMenu = false;
      break;
    default:
      printf("Wrong Input");
      break;
    }
  } while (inMenu);
}

int main() {
  Menu();
  return 0;
}
