#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sched.h>
#include <stdio.h>
#include <unistd.h>

#include <pmu.h>

inline int rdmsr(struct pmu_ctx *p, uint64_t *val, off_t offset) {
#ifdef CR4_PCE_SET
  uint64_t a, d;
  __asm __volatile("lfence" /* rdpmc is not serializing */
                   "\n"
                   "rdpmc"
                   : "=a"(a), "=d"(d)
                   : "c"(offset));
  *val = (d << 32) | a;
  return 0;
#else
  return pread(p->fd, val, sizeof(*val), offset) != sizeof(*val);
#endif
}

inline int wrmsr(struct pmu_ctx *p, uint64_t val, off_t offset) {
  return pwrite(p->fd, &val, sizeof(val), offset) != sizeof(val);
}

inline int pmu_init(struct pmu_ctx *p) {
  int cpu;
  char buf[18];
  uint64_t eax = 10, edx;

  __asm __volatile("cpuid" : "+a"(eax), "=d"(edx)::"ebx", "ecx");

  p->nfcntrs = edx & 0x0f;
  edx >>= 5;
  p->fcntrw = edx;
  p->ver = eax;
  eax >>= 8;
  p->npcntrs = eax;
  eax >>= 8;
  p->pcntrw = eax;

  cpu_set_t mask;
  cpu = sched_getcpu();
  CPU_ZERO(&mask);
  CPU_SET(cpu, &mask);

  /* pin thread to core */
  if (sched_setaffinity(0, sizeof(cpu_set_t), &mask) < 0)
    return errno;

  sprintf(buf, "/dev/cpu/%d/msr", cpu);
  if ((p->fd = open(buf, O_RDWR)) < 0)
    return errno;

  return 0;
}

int pmu_clear(struct pmu_ctx *p) {
  int ret;
  uint64_t mask = 0;

  /* Disable PMU */
  if ((ret = wrmsr(p, 0, IA32_PERF_GLOBAL_CTRL)))
    return ret;

  /* Disable fixed counters */
  if ((ret = wrmsr(p, 0, IA32_FIXED_CTR_CTRL)))
    return ret;

  /* Clear variable counters */
  for (char i = 0; i < p->npcntrs; ++i) {
    if ((ret = wrmsr(p, 0, IA32_PERFEVTSEL0 + i)))
      return ret;
    mask |= (1 << i);
  }

  /* Clear fixed counters */
  for (char i = 0; i < p->nfcntrs; ++i) {
    if ((ret = wrmsr(p, 0, IA32_FIXED_CTR0 + i)))
      return ret;
    mask |= (1ULL << (34 - i));
  }

  /* Enable counters */
  return wrmsr(p, mask, IA32_PERF_GLOBAL_CTRL);
}

int pmu_trace(struct pmu_ctx *p, union counter_config *const events, int n) {
  int ret = 0;
  n = n > p->npcntrs ? p->npcntrs : n;

  /* Enable user mode for fixed counters */
  if ((ret = wrmsr(p, 0x222, IA32_FIXED_CTR_CTRL)))
    return ret;

  /* Event select */
  for (int i = 0; i < n; ++i)
    if ((ret = wrmsr(p, events[i].val, IA32_PERFEVTSEL0 + i)))
      return ret;

  return 0;
}

int pmu_read(struct pmu_ctx *p, uint64_t *vals, int n) {
  int ret;
  n = n > p->npcntrs ? p->npcntrs : n;

  for (int i = 0; i < p->nfcntrs; ++i)
    if ((ret = rdmsr(p, vals + i, IA32_FIXED_CTR0 + i)))
      return ret;

  vals += p->nfcntrs;

  for (int i = 0; i < n; ++i)
    if ((ret = rdmsr(p, vals + i, IA32_PERFEVTSEL0 + i)))
      return ret;

  return 0;
}

inline int pmu_exit(struct pmu_ctx *p) { return close(p->fd); }
