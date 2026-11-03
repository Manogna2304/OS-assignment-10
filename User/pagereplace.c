#include "types.h"
#include "stat.h"
#include "user.h"

#define N 20
#define MAX 5

int pages[N] = {
  7, 0, 1, 2, 0, 3, 0, 4, 2, 3,
  0, 3, 2, 1, 2, 0, 1, 7, 0, 1
};


// Display current frames
void
display(int frames[], int f)
{
  int i;

  printf(1, "[");

  for(i = 0; i < f; i++) {
    if(frames[i] == -1)
      printf(1, " - ");
    else
      printf(1, " %d ", frames[i]);
  }

  printf(1, "]");
}


// Check whether a page is present
int
search(int frames[], int f, int page)
{
  int i;

  for(i = 0; i < f; i++)
    if(frames[i] == page)
      return i;

  return -1;
}


// FIFO Algorithm
void
FIFO(int f)
{
  int frames[MAX];
  int i,pointer = 0;
  int faults = 0, hits = 0;
  int pos;

  for(i = 0; i < f; i++)
    frames[i] = -1;

  printf(1, "\n========== FIFO (%d FRAMES) ==========\n", f);

  for(i = 0; i < N; i++) {
    pos = search(frames, f, pages[i]);

    printf(1, "Page %d: ", pages[i]);

    if(pos != -1) {
      hits++;
      printf(1, "HIT ");
    }
    else {
      faults++;
      printf(1, "FAULT ");

      frames[pointer] = pages[i];
      pointer = (pointer + 1) % f;
    }

    display(frames, f);
    printf(1, "\n");
  }

  printf(1, "Page Faults = %d\n", faults);
  printf(1, "Page Hits   = %d\n", hits);
  printf(1, "Hit Ratio   = %d/%d\n", hits, N);
  printf(1, "Fault Ratio = %d/%d\n", faults, N);
}


// LRU Algorithm
void
LRU(int f)
{
  int frames[MAX];
  int last[MAX];
  int i, j, pos, victim;
  int faults = 0, hits = 0;

  for(i = 0; i < f; i++) {
    frames[i] = -1;
    last[i] = -1;
  }

  printf(1, "\n========== LRU (%d FRAMES) ==========\n", f);

  for(i = 0; i < N; i++) {
    pos = search(frames, f, pages[i]);

    printf(1, "Page %d: ", pages[i]);

    if(pos != -1) {
      hits++;
      last[pos] = i;
      printf(1, "HIT ");
    }
    else {
      faults++;
      printf(1, "FAULT ");

      victim = -1;

      for(j = 0; j < f; j++) {
        if(frames[j] == -1) {
          victim = j;
          break;
        }
      }

      if(victim == -1) {
        victim = 0;

        for(j = 1; j < f; j++) {
          if(last[j] < last[victim])
            victim = j;
        }
      }

      frames[victim] = pages[i];
      last[victim] = i;
    }

    display(frames, f);
    printf(1, "\n");
  }

  printf(1, "Page Faults = %d\n", faults);
  printf(1, "Page Hits   = %d\n", hits);
  printf(1, "Hit Ratio   = %d/%d\n", hits, N);
  printf(1, "Fault Ratio = %d/%d\n", faults, N);
}


// Optimal Algorithm
void
Optimal(int f)
{
  int frames[MAX];
  int i, j, k, pos, victim;
  int faults = 0, hits = 0;
  int farthest, next;

  for(i = 0; i < f; i++)
    frames[i] = -1;

  printf(1, "\n========== OPTIMAL (%d FRAMES) ==========\n", f);

  for(i = 0; i < N; i++) {
    pos = search(frames, f, pages[i]);

    printf(1, "Page %d: ", pages[i]);

    if(pos != -1) {
      hits++;
      printf(1, "HIT ");
    }
    else {
      faults++;
      printf(1, "FAULT ");

      victim = -1;

      for(j = 0; j < f; j++) {
        if(frames[j] == -1) {
          victim = j;
          break;
        }
      }

      if(victim == -1) {
        farthest = -1;

        for(j = 0; j < f; j++) {
          next = N + 1;

          for(k = i + 1; k < N; k++) {
            if(pages[k] == frames[j]) {
              next = k;
              break;
            }
          }

          if(next > farthest) {
            farthest = next;
            victim = j;
          }
        }
      }

      frames[victim] = pages[i];
    }

    display(frames, f);
    printf(1, "\n");
  }

  printf(1, "Page Faults = %d\n", faults);
  printf(1, "Page Hits   = %d\n", hits);
  printf(1, "Hit Ratio   = %d/%d\n", hits, N);
  printf(1, "Fault Ratio = %d/%d\n", faults, N);
}


int
main(void)
{
  int f;

  printf(1, "\n");
  printf(1, "====================================\n");
  printf(1, "    PAGE REPLACEMENT SIMULATION\n");
  printf(1, "====================================\n");

  for(f = 3; f <= 5; f++) {

    printf(1, "\n\n******** FRAME COUNT: %d ********\n", f);

    FIFO(f);
    LRU(f);
    Optimal(f);

    printf(1, "\n");
  }

  printf(1, "\n--- ALL ALGORITHMS COMPLETED ---\n");

  exit();
}
