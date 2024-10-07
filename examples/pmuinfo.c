#include <pmu.h>
#include <stdio.h>

int main() {
  int ret;
  struct pmu_ctx p;

  if ((ret = pmu_init(&p)))
    return ret;

  printf("Version: %d\n"
         "Fixed counters: %d\n"
         "Fixed counter width: %d\n"
         "Programmable counters: %d\n"
         "Progammable counter width: %d\n",
         p.ver, p.nfcntrs, p.fcntrw, p.npcntrs, p.pcntrw);

  return 0;
}
