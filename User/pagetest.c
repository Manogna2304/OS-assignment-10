#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  char *p;
  int pid;

  printf(1, "\n--- PAGE PROTECTION TEST ---\n");

  p = sbrk(4096);

  if(p == (char*)-1) {
    printf(1, "Allocation failed\n");
    exit();
  }

  // Touch the page to ensure it is mapped.
  p[0] = 'A';

  printf(1, "\nInitial page information:\n");
  pageinfo((int)p);

  printf(1, "\nCreating child process for protection test...\n");

  pid = fork();

  if(pid < 0) {
    printf(1, "Fork failed\n");
    exit();
  }

  if(pid == 0) {
    printf(1, "\nChild: Changing page to read-only\n");

    pageinfo((int)p);

    if(setreadonly((int)p) < 0) {
      printf(1, "Protection failed\n");
      exit();
    }

    printf(1, "\nPage information after protection:\n");
    pageinfo((int)p);

    printf(1, "\nChild: Attempting to write to read-only page...\n");

    // This should cause a protection page fault.
    *(volatile char*)p = 'B';

    printf(1, "ERROR: Write unexpectedly succeeded!\n");

    exit();
  }

  wait();

  printf(1, "\nParent: Child finished after protection test.\n");

  printf(1, "\n--- TEST COMPLETED ---\n");

  sbrk(-4096);

  exit();
}
