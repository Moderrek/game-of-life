#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

#ifdef __unix__
#define LINUX
#include <unistd.h>
#define BLACK_BG "\x1b[40m"
#define WHITE_BG "\x1b[47m"
#define ROW_END "\x1b[0m"
#define ALIVE_CELL WHITE_BG" "
#define DEAD_CELL BLACK_BG" "
#elif defined(_WIN32) || defined(WIN32)
#define WINDOWS
#include <windows.h>
#define ALIVE_CELL " "
#define DEAD_CELL  " "
#endif

static volatile sig_atomic_t running = 1;

static void signal_handler(const int _) {
  (void)_;
  running = 0;
  fprintf(stdout, "Game ended.\n");
}

void wait(const int seconds) {
#ifdef LINUX
  sleep(seconds);
#else
  Sleep(seconds * 1000);
#endif
}

void clear() {
#ifdef WINDOWS
  system("cls");
#endif
}

void show(const void* arr, const int width, const int height) {
  const unsigned (*board)[width] = arr;
#ifdef WINDOWS
  const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      unsigned alive = board[y][x];
#ifdef WINDOWS
      SetConsoleTextAttribute(hConsole, alive ? 255 : 0);
      fprintf(stdout, "%s", board[y][x] ? ALIVE_CELL : DEAD_CELL);
      SetConsoleTextAttribute(hConsole, 15);
#else
      fprintf(stdout, "%s%s", alive ? ALIVE_CELL : DEAD_CELL, ROW_END);
#endif
    }
    fprintf(stdout, "\n");
  }
  fflush(stdout);
}

int count_neighbors(const void* arr, const int width, const int height, const int x, const int y) {
  const unsigned (*board)[width] = arr;

  int n = 0;
  for (int y1 = y - 1; y1 <= y + 1; y1++) {
    for (int x1 = x - 1; x1 <= x + 1; x1++) {
      if (y1 < 0 || y1 >= height) continue;
      if (x1 < 0 || x1 >= width) continue;
      if (board[y1][x1]) n++;
    }
  }
  if (board[y][x]) n--;
  return n;
}

void simulate(void* arr, const int width, const int height) {
  unsigned (*board)[width] = arr;
  unsigned next[height][width];

  // Evolve cells
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      const int n = count_neighbors(arr, width, height, x, y);
      next[y][x] = (n == 2 && board[y][x]) || n == 3;
    }
  }

  // Copy 'next' to board
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      board[y][x] = next[y][x];
    }
  }
}

void init_game(const int width, const int height) {
  running = 1;

  unsigned board[width][height];

  srand(time(NULL));
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      board[y][x] = rand() < RAND_MAX / 5 ? 1 : 0;
    }
  }

  while (running) {
    clear();
    show(board, width, height);
    simulate(board, width, height);
    wait(1);
  }
}

int main(void) {
  signal(SIGINT, signal_handler);
  init_game(50, 25);
  return EXIT_SUCCESS;
}
