// Source - https://stackoverflow.com/a/17271636
// Posted by David Ranieri
// Retrieved 2026-11-16, License - CC BY-SA 3.0
#define clear() printf("\033[H\033[J")
#include <stdbool.h>
#include <stdio.h>
#define AREA 15
#define FOOD 5
int snakeLength = 1;
bool playing = false;
bool inMenu = true;
void Menu();
enum DIFFICULTY { EASY, DIFFICULT };
enum DIFFICULTY difficulty;
struct Point {
  int x;
  int y;
};

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
  }
}

void PrintSnake() {}

void PrintBlockAndFood() {}

void StartGame() {
  while (playing) {
    clear();
    PrintBoard();
    PrintBlockAndFood();
    PrintSnake();
    PrintAt(0, AREA + 2, '\n');
    char input;
    scanf("%c", &input);
    switch (input) {
    case 'w':
    case 'W':
      break;
    case 's':
    case 'S':
      break;
    case 'a':
    case 'A':
      break;
    case 'd':
    case 'D':
      break;
    default:
      break;
    }
  }

  printf("Game over! Do you want to play again?\n(1) Yes\n(2) Exit\n");
  Menu();
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
