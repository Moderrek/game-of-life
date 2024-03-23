#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef __unix__
#define LINUX
#include <unistd.h>
#elif defined(_WIN32) || defined(WIN32)
#define WINDOWS
#include <windows.h>
#endif

void wait(const int seconds) {
#ifdef LINUX
  sleep(seconds);
#else
  Sleep(seconds * 1000);
#endif
}

void show(const void* arr, const int width, const int height) {
  const unsigned (*board)[width] = arr;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      fprintf(stdout, board[y][x] ? "*" : " ");
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
  unsigned board[width][height];

  srand(time(NULL));
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      board[y][x] = rand() < RAND_MAX / 5 ? 1 : 0;
    }
  }

  while (1) {
    show(board, width, height);
    simulate(board, width, height);
    wait(1);
  }
}

int main(void) {
  init_game(50, 25);
  return 0;
}
