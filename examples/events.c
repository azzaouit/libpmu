#include <pmu.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/random.h>

#define N (1 << 10)
#define BUF (sizeof(int) * N * N)

int main() {
  struct pmu_ctx p;
  uint64_t cntrs[7]; /* 3 fixed + 4 variable */
  int *a, *b, *c, ret;
  const char *e_str[] = {
      "INST_RETIRED.ANY",
      "CPU_CLK_UNHALTED.THREAD",
      "CPU_CLK_UNHALTED.REF_TSC",
      "LONGEST_LAT_CACHE.MISS",
      "CYCLE_ACTIVITY.CYCLES_L1D_MISS",
      "MEM_INST_RETIRED.ALL_LOADS",
      "MEM_INST_RETIRED.ALL_STORES",
  };
  union counter_config events[] = {
      {.event = 0x2E, .umask = 0x41, .user = 1, .en = 1},
      {.event = 0xA3, .umask = 0x08, .user = 1, .en = 1, .cmask = 0x8},
      {.event = 0xD0, .umask = 0x81, .user = 1, .en = 1},
      {.event = 0xD0, .umask = 0x82, .user = 1, .en = 1},
  };

  a = malloc(BUF);
  b = malloc(BUF);
  c = malloc(BUF);

  getrandom(a, BUF, GRND_RANDOM);
  getrandom(b, BUF, GRND_RANDOM);

  if ((ret = pmu_init(&p)))
    return ret;

  pmu_clear(&p);
  pmu_trace(&p, events, 4);

  /* Do work */
  for (int i = 0; i < N; ++i)
    for (int j = 0; j < N; ++j)
      ((volatile int *)c)[i * N + j] = a[i * N + j] * b[i * N + j];

  pmu_read(&p, cntrs, 4);
  for (unsigned i = 0; i < sizeof cntrs / sizeof cntrs[0]; ++i)
    printf("%s: %ld\n", e_str[i], cntrs[i]);

  free(a);
  free(b);
  free(c);
  return pmu_exit(&p);
}
