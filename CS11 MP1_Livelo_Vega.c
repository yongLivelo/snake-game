#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_RESET "\x1b[0m"
// Source - https://stackoverflow.com/a/17271636
// Posted by David Ranieri
// Retrieved 2026-11-16, License - CC BY-SA 3.0
#define clear() printf("\033[H\033[J")
#define AREA 15
#define SNAKE_BODY_MAX_LENGTH (AREA - 2) * (AREA - 2)
#define MAX_NUMBER_OF_FOODS 10
#define MAX_NUMBER_OF_BLOCKS 15
bool playing, validInput, win;
int numberOfFoods, numberOfBlocks, score;
enum DIRECTION { UP, DOWN, LEFT, RIGHT, STOP };
enum DIFFICULTY { EASY, DIFFICULT };
enum DIFFICULTY difficulty;
struct Point {
  int x;
  int y;
};
struct Point coordinatesOfFoods[MAX_NUMBER_OF_FOODS],
    coordinatesOfBlocks[MAX_NUMBER_OF_BLOCKS];
struct Snake {
  struct Point body[SNAKE_BODY_MAX_LENGTH];
  int length;
  enum DIRECTION direction;
};
struct Snake snake;
struct Point *head = &snake.body[0];

// Source - https://stackoverflow.com/a/15821739
// Posted by Ben, modified by community. See post 'Timeline' for change history
// Retrieved 2026-11-19, License - CC BY-SA 4.0
void remove_element(struct Point *array, int index, int *array_length) {
  int i;
  for (i = index; i < *array_length - 1; i++)
    array[i] = array[i + 1];

  (*array_length)--;
}

// Source - https://stackoverflow.com/a/7469410
// Posted by niko, modified by community. See post 'Timeline' for change history
// Retrieved 2026-11-19, License - CC BY-SA 4.0
static struct termios old, current;

char getch() {
  char ch;
  tcgetattr(0, &old);
  current = old;
  current.c_lflag &= ~ICANON;
  current.c_lflag &= ~ECHO;
  tcsetattr(0, TCSANOW, &current);
  ch = getchar();
  tcsetattr(0, TCSANOW, &old);
  return ch;
}

void PrintAt(int x, int y, char c, char *color) {
  // Source - https://stackoverflow.com/a/72073868
  // Posted by AMZG studio's, modified by community. See post 'Timeline' for
  // change history Retrieved 2026-11-16, License - CC BY-SA 4.0
  printf("%s\033[%d;%dH%c%s", color, y + 1, (x * 2) + 1, c, ANSI_COLOR_RESET);
}

void PrintBoard() {
  for (int i = 0; i < AREA; i++) {
    PrintAt(0, i, '#', ANSI_COLOR_RESET);
    PrintAt(AREA - 1, i, '#', ANSI_COLOR_RESET);
    PrintAt(i, 0, '#', ANSI_COLOR_RESET);
    PrintAt(i, AREA - 1, '#', ANSI_COLOR_RESET);
  }

  if (numberOfFoods == 0) {
    PrintAt(AREA - 1, 7, '$', ANSI_COLOR_GREEN);
  }
}

void PrintScore() { printf("You have a score of %d\n", score); }

void PrintBlockAndFood() {
  for (int i = 0; i < numberOfFoods; i++) {
    PrintAt(coordinatesOfFoods[i].x, coordinatesOfFoods[i].y, 'F',
            ANSI_COLOR_GREEN);
  }

  for (int i = 0; i < numberOfBlocks; i++) {
    PrintAt(coordinatesOfBlocks[i].x, coordinatesOfBlocks[i].y, 'B',
            ANSI_COLOR_RED);
  }
}

void PrintSnake() {
  if ((head)->x < AREA - 1) {
    PrintAt(head->x, head->y, '@', ANSI_COLOR_BLUE);
  }
  for (int i = 1; i < snake.length; i++) {
    if ((&snake.body[i])->x < AREA - 1) {
      PrintAt((&snake.body[i])->x, (&snake.body[i])->y, 'o', ANSI_COLOR_BLUE);
    }
  }
}

void HandleControls() {
  char key;
  key = getch();
  switch (key) {
  case 'A':
  case 'a':
    if (snake.direction == RIGHT) {
      validInput = false;
      break;
    }
    validInput = true;
    snake.direction = LEFT;
    break;
  case 'D':
  case 'd':
    if (snake.direction == LEFT) {
      validInput = false;
      break;
    }
    validInput = true;
    snake.direction = RIGHT;
    break;
  case 'W':
  case 'w':
    if (snake.direction == DOWN) {
      validInput = false;
      break;
    }
    validInput = true;
    snake.direction = UP;
    break;
  case 'S':
  case 's':
    if (snake.direction == UP) {
      validInput = false;
      break;
    }
    validInput = true;
    snake.direction = DOWN;
    break;
  default:
    validInput = false;
    break;
  }
}

void MoveSnake() {
  if (validInput) {
    struct Point temp;
    for (int i = snake.length; i > 0; i--) {
      snake.body[i] = snake.body[i - 1];
    }
    switch (snake.direction) {
    case LEFT:
      snake.body[0].x--;
      break;
    case RIGHT:
      snake.body[0].x++;
      break;
    case UP:
      snake.body[0].y--;
      break;
    case DOWN:
      snake.body[0].y++;
      break;
    default:
      break;
    }
  }
}

void CheckCollisions() {
  // Collision disabled at (7, 14) when all food is eaten
  if (head->y == 7 && head->x == AREA - 1 && numberOfFoods == 0) {
    return;
  }

  // Collision for tail hitting '$'
  if (snake.body[snake.length - 1].x == AREA - 1 &&
      snake.body[snake.length - 1].y == 7 && numberOfFoods == 0 &&
      head->x != AREA - 1) {
    playing = false;
    win = true;
  }

  // Collision for head hitting borders
  if (head->x == 0 || head->x == AREA - 1 || head->y == 0 ||
      head->y == AREA - 1) {
    playing = false;
  }

  // Collision for head hitting foods
  for (int i = 0; i < numberOfFoods; i++) {
    if (head->x == coordinatesOfFoods[i].x &&
        head->y == coordinatesOfFoods[i].y) {
      snake.length++;
      score++;
      remove_element(coordinatesOfFoods, i, &numberOfFoods);
      if (numberOfFoods == 0) {
        memset(coordinatesOfBlocks, 0, sizeof(coordinatesOfBlocks));
        numberOfBlocks = 0;
      }
      break;
    }
  }

  // Collision for head hitting blocks
  for (int i = 0; i < numberOfBlocks; i++) {
    if (head->x == coordinatesOfBlocks[i].x &&
        head->y == coordinatesOfBlocks[i].y) {
      playing = false;
      break;
    }
  }

  // Collision for head hitting body
  for (int i = 1; i < snake.length; i++) {
    if (head->x == snake.body[i].x && head->y == snake.body[i].y) {
      playing = false;
      break;
    }
  }
}

void GenerateBlockAndFoodCoordinates() {
  srand(time(NULL));
  {
    bool generating = true;
    int i = 0;
    while (generating) {
      bool duplicateFound = false;
      if (i >= numberOfFoods)
        break;
      coordinatesOfFoods[i].x = (rand() % (AREA - 2)) + 1;
      coordinatesOfFoods[i].y = (rand() % (AREA - 2)) + 1;
      if (coordinatesOfFoods[i].x == 1 && coordinatesOfFoods[i].y == 1) {
        duplicateFound = true;
      }
      for (int j = 0; j < i; j++) {
        if (coordinatesOfFoods[j].x == coordinatesOfFoods[i].x &&
            coordinatesOfFoods[j].y == coordinatesOfFoods[i].y) {
          duplicateFound = true;
        }
      }
      if (!duplicateFound) {
        i++;
      }
    }
  }

  {
    bool generating = true;
    int i = 0;
    while (generating) {
      bool duplicateFound = false;
      if (i >= numberOfBlocks)
        break;
      coordinatesOfBlocks[i].x = (rand() % (AREA - 2)) + 1;
      coordinatesOfBlocks[i].y = (rand() % (AREA - 2)) + 1;

      if (coordinatesOfBlocks[i].x == 1 && coordinatesOfBlocks[i].y == 1) {
        duplicateFound = true;
      }

      if (coordinatesOfBlocks[i].x == AREA - 1 &&
          coordinatesOfBlocks[i].y == 7) {
        duplicateFound = true;
      }
      for (int j = 0; j < i; j++) {
        if ((coordinatesOfBlocks[i].x == coordinatesOfBlocks[j].x &&
             coordinatesOfBlocks[i].y == coordinatesOfBlocks[j].y)) {
          duplicateFound = true;
        }
      }

      for (int j = 0; j < numberOfFoods; j++) {
        if ((coordinatesOfBlocks[i].x == coordinatesOfFoods[j].x &&
             coordinatesOfBlocks[i].y == coordinatesOfFoods[j].y)) {
          duplicateFound = true;
        }
      }
      if (!duplicateFound) {
        i++;
      }
    }
  }
}

void Menu();
void PromptEnd() {
  char input;
  bool running = true;
  do {
    clear();
    PrintBoard();
    PrintAt(0, AREA - 1, '\n', ANSI_COLOR_RESET);
    PrintScore();
    if (win) {
      printf("You Win! ");
    } else {
      printf("Game Over. ");
    }
    printf("Do you want to play again?\n(1) Yes\n(2) Exit\n");
    input = getch();
    switch (input) {
    case '1':
      Menu();
      running = false;
      break;
    case '2':
      running = false;
      break;
    }
  } while (running);
}

void StartGame() {
  // Source - https://stackoverflow.com/a/10898450
  // Posted by user405725
  // Retrieved 2026-11-19, License - CC BY-SA 3.0
  memset(coordinatesOfBlocks, 0, sizeof(coordinatesOfBlocks));
  memset(coordinatesOfFoods, 0, sizeof(coordinatesOfFoods));
  score = 0;
  snake.direction = STOP;
  playing = true;
  win = false;
  GenerateBlockAndFoodCoordinates();
  head->x = 1;
  head->y = 1;
  snake.length = 1;
  printf("\e[?25l");
  do {
    // Sourcce -
    // https://stackoverflow.com/questions/30126490/how-to-hide-console-cursor-in-c
    clear();
    PrintBoard();
    PrintBlockAndFood();
    PrintSnake();
    PrintAt(0, AREA - 1, '\n', ANSI_COLOR_RESET);
    PrintScore();
    HandleControls();
    MoveSnake();
    CheckCollisions();
  } while (playing);
  printf("\e[?25h");
  PromptEnd();
}

void PromptNumberOfBlocksAndFood() {
  clear();
  do {
    if (difficulty == EASY) {
      numberOfBlocks = 0;
      break;
    }
    int input;
    printf("How many blocks do you want (5-15)\n");
    if (scanf("%d", &input) != 1) {
      printf("Wrong Input\n");
      // Source - https://www.w3schools.com/c/c_input_validation.php
      while (getchar() != '\n')
        continue;
    }
    if (input >= 5 && input <= 15) {
      numberOfBlocks = input;
      break;
    } else {
      printf("Invalid input\n");
    }
  } while (true);

  do {
    int input;
    printf("How many food do you want (1-10)\n");
    if (scanf("%d", &input) != 1) {
      printf("Wrong Input\n");
      while (getchar() != '\n')
        continue;
    }
    if (input >= 1 && input <= 10) {
      numberOfFoods = input;
      break;
    } else {
      printf("Invalid input\n");
    }
  } while (true);
}

void PromptDifficulty() {
  char input;
  bool running = true;
  do {
    clear();
    printf("Choose Difficulty\n(1) Easy\n(2) Difficult\n");
    input = getch();
    switch (input) {
    case '1':
      difficulty = EASY;
      PromptNumberOfBlocksAndFood();
      running = false;
      StartGame();
      break;
    case '2':
      difficulty = DIFFICULT;
      PromptNumberOfBlocksAndFood();
      running = false;
      StartGame();
      break;
    }
  } while (running);
}

void Menu() {
  char input;
  bool running = true;
  do {
    clear();
    printf("The Snake Game:\nChoose an option:\n(1) Start Game\n(2) "
           "Instructions\n(3) Exit\n");
    input = getch();
    switch (input) {
    case '1':
      PromptDifficulty();
      running = false;
      break;
    case '2':
      clear();
      printf(
          "Use WASD to move\nAvoid the walls, the blocks, and your body\nEat "
          "all the food and a door will open, click any button to continue");
      input = getch();
      break;
    case '3':
      running = false;
      break;
    }
  } while (running);
}

int main() {
  Menu();
  return 0;
}
