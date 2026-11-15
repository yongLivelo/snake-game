/* Compile using gcc "CS11 MP1_Livelo_Vega.c" -lncurses && ./a.out
 */
#include <curses.h>
#include <ncurses.h>
#include <unistd.h>

int playerCoordinates[2] = {5, 5}; // {x, y}
void renderBoard() {
  for (int i = 0; i < 15; i++) {
    mvprintw(0, 2 * i, "#");
    mvprintw(i, 0, "#");
    mvprintw(15, 2 * i, "#");
    mvprintw(i, 1 * 2, "#");
  }
}
void renderPlayer() {
  // ncurses uses (y, x) order
  mvprintw(playerCoordinates[1], playerCoordinates[0], "@");
}

int main(void) {
  int direction[2] = {1, 0};

  initscr();
  noecho();
  curs_set(0);
  nodelay(stdscr, TRUE);
  while (1) {
    int c = getch();

    if (c != ERR) {
      if (c == 'w' || c == 'W') {
        direction[0] = 0;
        direction[1] = -1; // Up decreases Y
      } else if (c == 's' || c == 'S') {
        direction[0] = 0;
        direction[1] = 1; // Down increases Y
      } else if (c == 'a' || c == 'A') {
        direction[0] = -1;
        direction[1] = 0;
      } else if (c == 'd' || c == 'D') {
        direction[0] = 1;
        direction[1] = 0;
      } else if (c == 'q' || c == 'Q') {
        break; // Quit cleanly
      }
    }

    // Update position
    playerCoordinates[0] += direction[0];
    playerCoordinates[1] += direction[1];

    // Keep within terminal bounds (LINES and COLS are built-in ncurses globals)
    if (playerCoordinates[0] < 0)
      playerCoordinates[0] = 0;
    if (playerCoordinates[0] >= COLS)
      playerCoordinates[0] = COLS - 1;
    if (playerCoordinates[1] < 1)
      playerCoordinates[1] = 1; // Row 0 is for instructions
    if (playerCoordinates[1] >= LINES)
      playerCoordinates[1] = LINES - 1;

    clear();
    renderBoard();
    renderPlayer();
    refresh();

    usleep(100000); // 100ms (10 frames per second)
  }

  endwin(); // Restore normal terminal behavior
  return 0;
}
