#include <windows.h>
#include <stdio.h>
#include <assert.h>
typedef unsigned long IUM32;
typedef int IBOOL;
typedef unsigned long IU32;
typedef unsigned short IU16;
typedef long IS32;
#define LOCAL static
#define GLOBAL
#define IPT1(t,n) (t n)
#define VOID void
#define IFN0() (void)
#define IFN1(t,n) (t n)

static volatile LONG cpu_interrupt_map;
static unsigned resets, ticks, sad, steps;
static int interrupt_enabled;
#include "notification_body.inc"
static void c_cpu_reset(void) { ++resets; }
static void host_timer_event(void) { ++ticks; }
static void force_yoda(void) { ++sad; }
#define GET_IF() interrupt_enabled
#define SYNCH_TICK() do { if (++steps == 1) c_cpu_raise_event(CPU_RESET_EXCEPTION_MASK); assert(steps < 4); } while (0)
#define QUICK_EVENT_TICK() ((void)0)
static unsigned c_cpu_q_ev_get_count(void) { return 0; }
static void mvdm_softpc_ccpu_wait_for_event(void) { assert(0); }
static void halt_consumer(void) {
#include "event-halt.inc"
 }
#define INTERNAL 0
#define SET_POP_DISP(x) ((void)(x))
static void reset_consumer(void) {
  int doing_contributory, doing_page_fault, doing_double_fault, doing_fault, EXT;

#include "event-reset.inc"

NEXT_INST: ;
}
static void timer_consumer(void) {
#include "event-timer.inc"
 }
static unsigned deliveries, acknowledges, hook_calls;
static int hook_handled;
BOOL host_hwint_hook(IS32 vector) { assert(vector == 9); ++hook_calls; return hook_handled; }
static int ack_result, reassert;
static IU16 cpu_hw_interrupt_number;
static int EXT, CCPU_save_EIP;
#define EXTERNAL 1
#define GET_EIP() 0
#undef SYNCH_TICK
#define SYNCH_TICK() ((void)0)
static IS32 ica_intack(IU32 *hook) {
  (void)hook; ++acknowledges;
  if (reassert) c_cpu_raise_event(CPU_HW_INT_MASK);
  return ack_result;
}
static void do_intrupt(IU16 vector, int a, int b, IU16 c) {
  (void)a; (void)b; (void)c; assert(vector == 9); ++deliveries;
}
static void irq_consumer(void) {
#include "event-irq.inc"
 }
int main(void) {
  cpu_interrupt_map = 0; steps = 0;
  halt_consumer(); /* first tick raises RESET while halted */
  reset_consumer();
  if (resets != 1 || (c_cpu_event_snapshot() & CPU_RESET_EXCEPTION_MASK)) {
    fputs("FAIL: HALT consumed RESET before normal reset handler\n", stderr); return 1;
  }
  reset_consumer(); assert(resets == 1);
  c_cpu_raise_event(CPU_RESET_EXCEPTION_MASK | CPU_SIGALRM_EXCEPTION_MASK);
  reset_consumer(); assert(resets == 2);
  assert(c_cpu_event_snapshot() & CPU_SIGALRM_EXCEPTION_MASK);
  timer_consumer(); assert(ticks == 1);
  timer_consumer(); assert(ticks == 1);
  c_cpu_raise_event(CPU_SAD_EXCEPTION_MASK); timer_consumer(); assert(sad == 1);
  c_cpu_raise_event(CPU_HW_INT_MASK); interrupt_enabled = 1;
  steps = 0; halt_consumer(); assert(steps == 0);
  assert(c_cpu_event_snapshot() & CPU_HW_INT_MASK);
  assert(c_cpu_take_event(CPU_HW_INT_MASK));
  assert(!c_cpu_take_event(CPU_HW_INT_MASK));
  c_cpu_raise_event(CPU_HW_INT_MASK); interrupt_enabled = 0;
  irq_consumer(); assert(acknowledges == 0 && c_cpu_event_snapshot() & CPU_HW_INT_MASK);
  interrupt_enabled = 1; ack_result = -1; irq_consumer();
  assert(acknowledges == 1 && deliveries == 0);
  c_cpu_raise_event(CPU_HW_INT_MASK); ack_result = 9; reassert = 1;
  irq_consumer(); assert(acknowledges == 2 && deliveries == 1);
  assert(c_cpu_event_snapshot() & CPU_HW_INT_MASK);
  reassert = 0; irq_consumer(); assert(deliveries == 2);
  assert(hook_calls == 2);
  hook_handled = 1; c_cpu_raise_event(CPU_HW_INT_MASK);
  irq_consumer(); assert(hook_calls == 3 && deliveries == 2);
  puts("PASS: extracted HALT/RESET/timer/SAD/INTR and IRQ callback-boundary consumers; not full CPU/PIC execution");
  return 0;
}
