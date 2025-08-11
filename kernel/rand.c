#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

static uint64 next_rng_state(uint64 state) {
  return state * 6364136223846793005ULL + 1;
}

void
rng_state_init(uint64* rng_state, uint64 seed) {
  if (!*rng_state) {
    *rng_state = 0x853c49e6748fea9bULL;
  }
  // 작은 값이 들어오는 경우 보정하기 위해 초기 값을 몇 개 버린다.
  // https://chatgpt.com/share/689643df-01a0-8011-9e4f-7c436051ba5c
  pcg32();
  pcg32();
}

static uint32
pcg32(void) {
  struct cpu *c = mycpu();

  uint64 oldstate = c->rng_state;
  // Advance internal state
  c->rng_state = next_rng_state(oldstate);
  // Calculate output function (XSH RR), uses old state for max ILP
  uint32 xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
  uint32 rot = oldstate >> 59u;
  return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

uint32
rand_range(uint32 min, uint32 max) {
  if (min >= max) {
    return min;
  }

  uint32 range = max - min + 1;
  return min + (uint32)(((uint64)pcg32() * (uint64)range) >> 32);
}
