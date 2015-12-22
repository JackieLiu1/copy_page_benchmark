/* This header file includes some functions which are used for timing.
 * The following functions should be defined for each timing model:
 * void enable_count_register(void)
 * void start_time(void)
 * void stop_time(void)
 * void print_time(void)
 * unsigned long long time_spent(void)
 * The following types should be defined:
 * benchmark_time

 * The start and end time can get via the following two variables
 * benchmark_start_time
 * benchmark_end_time
 * These two variables are of type benchmark_time
 *
 * Currently the following timing models are defined:
 * AARCH64_TIMING
 *   This requires the kernel to enable the performance counters in EL0 if running in EL0
 * OCTEON_TIMING
 * LINUX_TIMING
 *   This is the only timing model which is not cycle based.
 * X86_TIMING
 */

#ifndef __BENCHMARK_TIMING_H__
#define __BENCHMARK_TIMING_H__


#ifdef AARCH64_TIMING
  typedef unsigned long long benchmark_time;
#endif

#ifdef OCTEON_TIMING
  typedef unsigned long long benchmark_time;
#endif

#ifdef LINUX_TIMING
# include <time.h>
  typedef struct timespec benchmark_time;
#endif

#ifdef X86_TIMING
  typedef struct {
    unsigned hi;
    unsigned lo;
  } benchmark_time;
#endif

extern benchmark_time benchmark_start_time;
extern benchmark_time benchmark_end_time;


#ifdef AARCH64_TIMING
static void enable_count_register(void)
{
#ifdef AARCH64_PMU_TIMING
  long long a;
  /* Set bit E[0] of PMCR_EL0 to 1 to enable performance counters. */
  asm("mrs %0, PMCR_EL0" : "=r"(a));
  a |= 1 << 0;
  asm("msr PMCR_EL0, %0" : : "r"(a));

  asm("mrs %0, PMCNTENSET_EL0" : "=r"(a));
  /* Set bit C[31] of PMCNTENSET_EL0 to 1 so PMCCNTR_EL0 increments. */
  a |= 1 << 31;
  asm("msr PMCNTENSET_EL0, %0": : "r"(a));
#endif
}

/* reads the current (64bit) Performance Monitors Cycle Count Register */
#ifdef AARCH64_PMU_TIMING
#define BENCHMARK_MF_CYCLE(dest)        asm volatile ("mrs %[rt],PMCCNTR_EL0" : [rt] "=r" (dest)) 
#else
#define BENCHMARK_MF_CYCLE(dest)        asm volatile ("mrs %[rt],cntvct_el0" : [rt] "=r" (dest)) 
#endif
static void start_time(void) {
BENCHMARK_MF_CYCLE (benchmark_start_time);
}
static void stop_time(void) {
BENCHMARK_MF_CYCLE (benchmark_end_time);
}

static unsigned long long time_spent(void)
{
  return benchmark_end_time-benchmark_start_time;
}

static void print_time(void) {
  printf("total time = %llu.\n", time_spent());
}

static unsigned long long get_frequncy(void)
{
  long long a;
#ifdef AARCH64_PMU_TIMING
  /* Assume 2GHz for now. */
  return 2*1000*1000*1000;
#else
  asm volatile ("mrs %[rt], cntfrq_el0" : [rt] "=r" (a)) ;
#endif
  return a;
}

#endif

#ifdef OCTEON_TIMING
static void enable_count_register(void){}

/* reads the current (64-bit) CvmCount value */
#define BENCHMARK_MF_CYCLE (dest)  asm volatile ("rdhwr %[rt],$31" : [rt] "=d" (dest))
static void start_time(void) {
BENCHMARK_MF_CYCLE (benchmark_start_time);
}
static void stop_time(void) {
BENCHMARK_MF_CYCLE (benchmark_end_time);
}

static unsigned long long time_spent(void)
{
  return benchmark_end_time-benchmark_start_time;
}

static void print_time(void) {
  printf("total time = %llu.\n", time_spent());
}
#endif

#ifdef LINUX_TIMING
static void enable_count_register(void){}
static void start_time(void) {
  clock_gettime(CLOCK_MONOTONIC, &benchmark_start_time);
}
static void stop_time(void) {
 clock_gettime(CLOCK_MONOTONIC, &benchmark_end_time);
}

static unsigned long long time_spent(void)
{
 unsigned long long diff;
#define BILLION 1000000000LL
 diff = BILLION * (benchmark_end_time.tv_sec - benchmark_start_time.tv_sec) + benchmark_end_time.tv_nsec - benchmark_start_time.tv_nsec;
#undef BILLION
 return diff;
}

static void print_time(void) {
 printf("total time = %llu nanoseconds\n",time_spent());
}

static unsigned long long get_frequncy(void)
{
  return 1000000000LL;
}
#endif

#ifdef X86_TIMING
static void enable_count_register(void){}

/* reads the current (64-bit) CvmCount value */

static inline benchmark_time rdtsc(){
    benchmark_time res;
    __asm__ __volatile__ ("rdtsc" : "=a" (res.lo), "=d" (res.hi));
    return res;
}

static void start_time(void) {
benchmark_start_time = rdtsc();
}
static void stop_time(void) {
benchmark_end_time = rdtsc();
}

static unsigned long long convert_to_cycles(benchmark_time a)
{
  return (((unsigned long long)a.hi) << 32) | a.lo;
}

static unsigned long long time_spent(void)
{
  return convert_to_cycles(benchmark_end_time)-convert_to_cycles(benchmark_start_time);
}

static void print_time(void) {
  printf("total time = %llu.\n", time_spent());
}
#endif

#endif /* __BENCHMARK_TIMING_H__ */
