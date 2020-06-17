#include <stdio.h>
#include <stdlib.h>

// lp.setupi L, uimmS, uimmL // L = x0, x1 | uimmS = n_times | uimmL = 4*n_instructions

// Simple loop
#define HWLP_TEST0 asm volatile ("lp.setupi x1, 10, 16\n\t\
                                  addi t1, t1, 20\n\t\
                                  addi t1, t1, 20\n\t\
                                  addi t1, t1, 20\n\t\
                                  addi t2, t2, 20\n\t\
                                  add t1, t1, t2" \
                                 : : : "t1", "t2")

// Nested loops
#define HWLP_TEST1 asm volatile ("lp.setupi x1, 10, 44\n\t\
                                  addi t1, t1, 14\n\t\
                                  addi t2, t1, 10\n\t\
                                  addi t1, t2, 23\n\t\
                                  addi t2, t2, 60\n\t\
                                  lp.setupi x0, 21, 12\n\t\
                                  addi t1, t1, 988\n\t\
                                  addi t2, t1, 188\n\t\
                                  addi t1, t2, 948\n\t\
                                  addi t2, t2, 928\n\t\
                                  addi t2, t2, 8\n\t\
                                  addi t2, t2, 865\n\t\
                                  add t2, t1, t1" \
                                  : : : "t1", "t2")

// Big nested loops
#define HWLP_TEST2 asm volatile ("lp.setupi x1, 100, 44\n\t\
                                  addi t1, t1, 14\n\t\
                                  addi t2, t1, 10\n\t\
                                  addi t1, t2, 23\n\t\
                                  addi t2, t2, 60\n\t\
                                  lp.setupi x0, 200, 12\n\t\
                                  addi t1, t1, 988\n\t\
                                  addi t2, t1, 188\n\t\
                                  addi t1, t2, 948\n\t\
                                  addi t2, t2, 928\n\t\
                                  addi t2, t2, 8\n\t\
                                  addi t2, t2, 865\n\t\
                                  add t2, t1, t1" \
                                  : : : "t1", "t2")

// Random memory operations inside the loop
#define HWLP_TEST3 asm volatile ("addi t1,x0,0\n\t\
                                  addi t2,x0,0\n\t\
                                  lp.setupi x1, 102, 60\n\t\
                                  addi t1, x0, 4\n\t\
                                  addi t2, x0, 8\n\t\
                                  lw t1, 24(t2)\n\t\
                                  addi t2, x0, 12\n\t\
                                  addi t1, x0, 4\n\t\
                                  lp.setupi x0, 21, 20\n\t\
                                  addi t2, x0, 4\n\t\
                                  addi t1, x0, 4\n\t\
                                  lw t2, 8(t1)\n\t\
                                  addi t2, x0, 4\n\t\
                                  addi t2, x0, 4\n\t\
                                  addi t2, x0, 4\n\t\
                                  lw t1, 0(t2)\n\t\
                                  sw t2, 0(t2)\n\t\
                                  addi t2, x0, 4\n\t\
                                  addi t2, x0, 4\n\t\
                                  lw t2, 8(t2)\n\t\
                                  addi t2, x0, 4\n\t\
                                  addi t2, x0, 4\n\t\
                                  sw t2, 12(t2)\n\t\
                                  addi t2, x0, 8\n\t\
                                  addi t1, x0, 12\n\t\
                                  lw t2, 4(t1)" \
                                  : : : "t0", "t1", "t2")

// Minimum size nested HWLPs
#define HWLP_TEST4 asm volatile ("addi t0,x0,0\n\t\
                                  addi t1,x0,0\n\t\
                                  addi t2,x0,0\n\t\
                                  addi t0, x0, 28\n\t\
                                  lp.setup x1, t0, 24\n\t\
                                  lp.setupi x0, 21, 12\n\t\
                                  addi t2, x0, 8\n\t\
                                  addi t1, x0, 12\n\t\
                                  sw t2, 24(t0)\n\t\
                                  addi t1, x0, 12\n\t\
                                  sw t1, 28(t0)\n\t\
                                  mul t0, t0, t0" \
                                  : : : "t0", "t1", "t2")

// Mixed operations and HWLP instructions.
// Extreme cases allowed by documentation:
// - (HWLoop[1].endaddress = HWLoop[0].endaddress + 8)
// - (Minimum size nested HWLP)
#define HWLP_TEST5 asm volatile ("addi t0,x0,0\n\t\
                                  addi t1,x0,0\n\t\
                                  addi t2,x0,0\n\t\
                                  lp.starti x1,16\n\t\
                                  lp.counti x1,12\n\t\
                                  lp.endi x1,100\n\t\
                                  lp.starti x0,52\n\t\
                                  addi t0,x0,100\n\t\
                                  lp.count x0,t0\n\t\
                                  lp.endi x0,76\n\t\
                                  addi t0, x0, 1024\n\t\
                                  addi t1, x0, 4\n\t\
                                  addi t2, x0, 8\n\t\
                                  add t1, t2, t2\n\t\
                                  mul t2, t0, t1\n\t\
                                  lw t2, 128(t0)\n\t\
                                  sw t2, 8(t0)\n\t\
                                  lw t2, 256(t0)\n\t\
                                  addi t2, x0, 4\n\t\
                                  lw t1, 0(t0)\n\t\
                                  lw t1, 4(t0)\n\t\
                                  lw t2, 8(t0)\n\t\
                                  lw t1, 12(t0)\n\t\
                                  lw t2, 16(t0)\n\t\
                                  sw t1, 20(t0)\n\t\
                                  sw t2, 24(t0)\n\t\
                                  sw t1, 28(t0)\n\t\
                                  lw t1, 32(t0)\n\t\
                                  sw t2, 36(t0)\n\t\
                                  lw t1, 0(t0)\n\t\
                                  sw t2, 0(t0)\n\t\
                                  lw t2, 8(t0)\n\t\
                                  addi t2, x0, 4\n\t\
                                  addi t2, x0, 4\n\t\
                                  sw t2, 12(t0)\n\t\
                                  addi t1, x0, 28\n\t\
                                  lp.setup x1, t0, 24\n\t\
                                  lp.setupi x0, 21, 12\n\t\
                                  addi t2, x0, 8\n\t\
                                  addi t1, x0, 12\n\t\
                                  sw t2, 24(t0)\n\t\
                                  addi t1, x0, 12\n\t\
                                  sw t1, 28(t0)\n\t\
                                  lw t1, 32(t0)\n\t\
                                  addi t2, x0, 8\n\t\
                                  addi t1, x0, 12\n\t\
                                  lw t2, 4(t0)" \
                                  : : : "t0", "t1", "t2")

void activate_random_stall(void)
{
  // Address vector for rnd_stall_reg, to control memory stalls/interrupt
  volatile unsigned int *rnd_stall_reg[16];

  // Setup the address vector
  rnd_stall_reg[0] = (volatile unsigned int *) 0x16000000;
  for (int i = 1; i < 16; i++) {
    rnd_stall_reg[i] = rnd_stall_reg[i-1] + 1; // It is a pointer to int ("+ 1" means "the next int")
  }

  /* The interposition of the stall generator between CPU and MEM should happen BEFORE the stall generetor is active */
  // Interpose the stall generator between CPU and D-MEM (rnd_stall_reg[1])
  *rnd_stall_reg[1] = 0x01;
  // Interpose the stall generator between CPU and I-MEM (rnd_stall_reg[0])
  *rnd_stall_reg[0] = 0x01;

  // DATA MEMORY
  // Set max n. stalls on both GNT and VALID for RANDOM mode (rnd_stall_reg[5])
  *rnd_stall_reg[5] = 0x10;
  // Set n. stalls on  GNT (rnd_stall_reg[7])
  *rnd_stall_reg[7] = 0x00;
  // Set n. stalls on VALID (rnd_stall_reg[9])
  *rnd_stall_reg[9] = 0x00;

  // INSTRUCTION MEMORY
  // Set max n. stalls on both GNT and VALID for RANDOM mode (rnd_stall_reg[4])
  *rnd_stall_reg[4] = 0x10;
  // Set n. stalls on  GNT (rnd_stall_reg[6])
  *rnd_stall_reg[6] = 0x00;
  // Set n. stalls on VALID (rnd_stall_reg[8])
  *rnd_stall_reg[8] = 0x00;

  /* Activating stalls on D and I Mem has to be done as last operation. Do not change the order. */
  // Set stall mode on D-MEM (off=0, standard=1, random=2) (rnd_stall_reg[3])
  *rnd_stall_reg[3] = 0x02;
  // Set stall mode on I-MEM (off=0, standard=1, random=2) (rnd_stall_reg[2])
  *rnd_stall_reg[2] = 0x02;
}

int main(int argc, char *argv[])
{
#ifdef RANDOM_MEM_STALL
    activate_random_stall();
#endif

    asm volatile("ecall" : : : "ra");
    HWLP_TEST0;
    asm volatile("ecall" : : : "ra");
    HWLP_TEST1;
    asm volatile("ecall" : : : "ra");
    HWLP_TEST2;
    asm volatile("ecall" : : : "ra");
    HWLP_TEST3;
    asm volatile("ecall" : : : "ra");
    HWLP_TEST4;
    asm volatile("ecall" : : : "ra");
    HWLP_TEST5;
    asm volatile("ecall" : : : "ra");

    return EXIT_SUCCESS;
}
