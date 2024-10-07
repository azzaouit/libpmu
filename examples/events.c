#include <pmu.h>
#include <stdio.h>
#include <stdlib.h>

#define BUF (1 << 10)

int main() {
  int ret;
  uint64_t cntrs[7]; /* 3 fixed + 4 variable */
  struct pmu_ctx p;
  union counter_config events[4] = {
      /* BR_INST_RETIRED.ALL_BRANCHES*/
      {.event = 0xC4, .umask = 0, .user = 1, .cmask = 0},
      /* INST_RETIRED.ANY_P */
      {.event = 0xC0, .umask = 0, .user = 1, .cmask = 0},
      /* LONGEST_LAT_CACHE.MISS */
      {.event = 0x2E, .umask = 0x41, .user = 1, .cmask = 0},
      /* CYCLE_ACTIVITY.STALLS_TOTAL */
      {.event = 0xA3, .umask = 0x04, .user = 1, .cmask = 0x04},
  };

  if ((ret = pmu_init(&p)))
    return ret;

  pmu_clear(&p);
  pmu_trace(&p, events, 4);

  /* Do some work */
  char *big_array = malloc(BUF);
  for (int i = 0; i < BUF; ++i)
    big_array[i] = i * i;

  pmu_read(&p, cntrs, 4);
  for (unsigned i = 0; i < sizeof cntrs / sizeof cntrs[0]; ++i)
    printf("%ld\n", cntrs[i]);

  free(big_array);
  return pmu_exit(&p);
}
