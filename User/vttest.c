#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  char *p;
  int i;

  printf(1, "\n--- VIRTUAL TO PHYSICAL ADDRESS TEST ---\n");

  p = sbrk(3 * 4096);

  if(p == (char*)-1) {
    printf(1, "Allocation failed\n");
    exit();
  }

  // Touch the pages so they become mapped.
  for(i = 0; i < 3; i++)
    p[i * 4096] = 'A' + i;

  printf(1, "\nTranslating valid addresses:\n");

  for(i = 0; i < 3; i++) {
    printf(1, "\nPage %d:\n", i);
    vtop((int)(p + i * 4096));
  }

  printf(1, "\nTesting invalid address:\n");

  vtop(0x70000000);

  sbrk(-3 * 4096);

  exit();
}
