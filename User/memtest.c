#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  char *p;

  printf(1, "\n--- MEMORY ALLOCATION TEST ---\n");

  printf(1, "\nBefore allocation:\n");
  memstats();

  printf(1, "\nAllocating 3 pages...\n");

  p = sbrk(3 * 4096);

  if(p == (char*)-1) {
    printf(1, "Memory allocation failed\n");
    exit();
  }

  // Touch each page to ensure it is allocated.
  p[0] = 'A';
  p[4096] = 'B';
  p[8192] = 'C';

  printf(1, "\nAfter allocation:\n");
  memstats();

  printf(1, "\nReleasing 3 pages...\n");

  if(sbrk(-3 * 4096) == (char*)-1) {
    printf(1, "Memory release failed\n");
    exit();
  }

  printf(1, "\nAfter deallocation:\n");
  memstats();

  exit();
}
