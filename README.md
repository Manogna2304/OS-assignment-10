# Operating Systems Lab – Assignment 10
## Memory Management, Address Translation, Page Protection and Page Replacement in xv6

### 1. Introduction

This assignment focuses on understanding and implementing important memory management concepts in the xv6 operating system. The work involves modifying the xv6 kernel to monitor physical memory allocation, translate virtual addresses into physical addresses, inspect and modify page-table permissions, and simulate different page replacement algorithms.

The implementation was carried out using **xv6-public (x86)** running on Ubuntu through WSL2. Kernel modifications were made in C, and user-level test programs were developed to verify the functionality of each implementation.

### 2. Implementation and Kernel Modifications

#### Q1. Physical Memory Allocation Statistics

**Files modified:** `kalloc.c`, `defs.h`, `sysproc.c`, `syscall.c`, `syscall.h`, `user.h`, `usys.S`

The physical memory allocator in xv6 was modified to maintain memory allocation statistics.

Two global counters, `total_allocated` and `total_freed`, were introduced to track the number of successfully allocated and freed physical pages.

- In `kalloc()`, the allocation counter is incremented whenever a physical page is successfully removed from the freelist.
- In `kfree()`, the freed counter is incremented whenever a page is returned to the freelist.
- A new `memstats()` function traverses the freelist and displays the total allocated pages, total freed pages, currently allocated pages and currently available pages.

A system call was added to allow user programs to request these statistics.

**Testing:** The `memtest.c` program allocates three pages using `sbrk()`, accesses the allocated memory and subsequently releases the pages.

**Observed results:**
- Total allocated pages increased from 425 to 428.
- Total freed pages increased from 214 to 217 after deallocation.
- Currently allocated pages increased from 211 to 214 and returned to 211.
- Available pages decreased by three during allocation and returned to their original value after deallocation.

These results confirm that allocation and deallocation statistics are being tracked correctly.

#### Q2. Virtual-to-Physical Address Translation

**Files modified:** `vm.c`, `sysproc.c`, `defs.h`, `syscall.c`, `syscall.h`, `user.h`, `usys.S`

A system call was implemented to translate a given virtual address into its corresponding physical address.

The implementation uses the `walkpgdir()` function to locate the page-table entry corresponding to the supplied virtual address. The implementation checks whether the page is present before extracting its physical frame address and combining it with the page offset.

Invalid or unmapped virtual addresses are handled by checking the validity of the corresponding page-table entry.

**Testing:** The `vttest.c` program tests three valid virtual addresses and one invalid virtual address.

**Observed results:**

| Virtual Address | Physical Address |
|---|---|
| 0x3000 | 0xDEE3000 |
| 0x4000 | 0xDEE1000 |
| 0x5000 | 0xDF22000 |

The invalid virtual address `0x70000000` was correctly identified as unmapped.

This demonstrates successful virtual-to-physical address translation and invalid-address detection.

#### Q3. Page Table Information and Read-Only Protection

**Files modified:** `vm.c`, `sysproc.c`, `trap.c` (if modified), `defs.h`, `syscall.c`, `syscall.h`, `user.h`, `usys.S`

Two system calls were implemented to inspect page-table information and modify page permissions.

- `pageinfo()` retrieves the physical frame address and displays the Present (P), Writable (W) and User (U) permission bits.
- `setreadonly()` removes write permission from a specified virtual page by clearing the `PTE_W` bit. The TLB is flushed to ensure that the updated permission is applied.

**Testing:** The `pagetest.c` program initially checks the permissions of an allocated page. A child process then changes the page to read-only and attempts to write to it.

**Observed results:**

| Permission | Before Protection | After Protection |
|---|---:|---:|
| Present (P) | 1 | 1 |
| Writable (W) | 1 | 0 |
| User (U) | 1 | 1 |

When the child process attempts to write to the protected page, xv6 generates a page fault (trap 14) with error code 7. The child process is terminated, while the parent process continues execution.

This verifies that page-level write protection is functioning correctly.

#### Q4. Page Replacement Algorithms

**File implemented:** `pagereplace.c`

Three page replacement algorithms were implemented and compared:

1. **FIFO (First-In First-Out):** Replaces the page that has been in memory for the longest time. A queue-like replacement order is maintained.

2. **LRU (Least Recently Used):** Replaces the page that has not been accessed for the longest time. Recent usage information is maintained to determine which page should be removed.

3. **Optimal Page Replacement:** Replaces the page whose next use is farthest in the future. It provides the minimum possible number of page faults for a known reference string.

All three algorithms were tested with 3, 4 and 5 frames using the same reference string:

`7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1`

The program displays the frame contents after every page reference and calculates the total page hits, page faults, hit ratio and fault ratio.

**Observed results:**

| Frames | Algorithm | Page Faults | Page Hits |
|---:|---|---:|---:|
| 3 | FIFO | 15 | 5 |
| 3 | LRU | 12 | 8 |
| 3 | Optimal | 9 | 11 |
| 4 | FIFO | 10 | 10 |
| 4 | LRU | 8 | 12 |
| 4 | Optimal | 8 | 12 |
| 5 | FIFO | 9 | 11 |
| 5 | LRU | 7 | 13 |
| 5 | Optimal | 7 | 13 |

### 3. Overall Observations

The following observations were made from the test results:

- Physical memory allocation and deallocation counters correctly reflect changes in the number of allocated pages.
- Virtual-to-physical address translation works for valid mapped addresses, while invalid addresses are rejected.
- Clearing the writable permission bit successfully prevents user-mode write operations and generates a page protection fault.
- Optimal page replacement produces the minimum number of page faults for the given reference string.
- LRU performs better than FIFO for all three tested frame configurations.
- Increasing the number of frames reduces the number of page faults in the tested configurations.
- LRU and Optimal produce identical page fault counts when using four and five frames for the given reference string.

### 4. Compilation and Execution

The modified xv6 source code was compiled and executed using the following commands:

```bash
make clean
make
make qemu-nox
```

The following commands were executed at the xv6 shell prompt to test the four implementations:

```bash
memtest
vttest
pagetest
pagereplace
```

### 5. Conclusion

All four components of the assignment were implemented and tested successfully in the xv6 environment. The experiments provided practical understanding of physical memory management, page-table-based address translation, memory protection mechanisms and page replacement strategies.

The observed outputs were consistent with the expected behaviour of the implemented mechanisms and algorithms. The assignment demonstrates how operating system memory management concepts are implemented and verified through kernel modifications and user-level testing.

