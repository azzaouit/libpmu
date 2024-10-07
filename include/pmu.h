#ifndef PMU_H
#define PMU_H

#if defined(__i386__) || defined(__x86_64__)

#include <inttypes.h>

#define IA32_FIXED_CTR_CTRL 0x38d
#define IA32_PERF_GLOBAL_STATUS 0x38e
#define IA32_PERF_GLOBAL_CTRL 0x38f

#define IA32_PERFEVTSEL0 0x186
#define IA32_PERFEVTSEL1 0x187
#define IA32_PERFEVTSEL2 0x188
#define IA32_PERFEVTSEL3 0x189
#define IA32_PERFEVTSEL4 0x18a
#define IA32_PERFEVTSEL5 0x18b
#define IA32_PERFEVTSEL6 0x18c
#define IA32_PERFEVTSEL7 0x18d

#define IA32_FIXED_CTR0 0x309
#define IA32_FIXED_CTR1 0x30a
#define IA32_FIXED_CTR2 0x30b

#define IA32_PMC0 0xc1
#define IA32_PMC1 0xc2
#define IA32_PMC2 0xc3
#define IA32_PMC3 0xc4
#define IA32_PMC4 0xc5
#define IA32_PMC5 0xc6
#define IA32_PMC6 0xc7
#define IA32_PMC7 0xc8

struct pmu_ctx {
  char nfcntrs;
  char fcntrw;
  char ver;
  char npcntrs;
  char pcntrw;
  int fd;
};

union counter_config {
  struct {
    uint32_t event : 8;
    uint32_t umask : 8;
    uint32_t user : 1;
    uint32_t os : 1;
    uint32_t e : 1;
    uint32_t pin : 1;
    uint32_t in : 1;
    uint32_t any : 1;
    uint32_t en : 1;
    uint32_t invert : 1;
    uint32_t cmask : 8;
    uint32_t reserved : 32;
  };
  uint64_t val;
};

int pmu_init(struct pmu_ctx *);
int pmu_clear(struct pmu_ctx *);
int pmu_trace(struct pmu_ctx *, union counter_config *const, int);
void pmu_read(struct pmu_ctx *, uint64_t *, int);
int pmu_exit(struct pmu_ctx *);

#endif
#endif /* PMU_H */
