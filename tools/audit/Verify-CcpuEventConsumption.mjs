// Focused extracted-source test, not a full CCPU or guest-runtime fixture.
// Compile the selected source's actual atomic helpers and consumer blocks.
import fs from 'node:fs';
import path from 'node:path';
import { spawnSync } from 'node:child_process';
const root = process.cwd();
const args = process.argv.slice(2);
let revision;
let requestedOut;
for (let i = 0; i < args.length; ++i) {
  if (args[i] === '--out') {
    requestedOut = args[++i];
    if (!requestedOut) throw new Error('--out requires a task build directory');
  } else if (!revision) {
    revision = args[i];
  } else {
    throw new Error(`Unexpected argument: ${args[i]}`);
  }
}
const sourcePath = 'src/mvdm/softpc.new/base/ccpu386/c_main.c';
const source = revision ? spawnSync('git', ['show', `${revision}:${sourcePath}`],
  { encoding: 'utf8' }) : { status: 0, stdout: fs.readFileSync(sourcePath, 'utf8') };
if (source.status !== 0) throw new Error('Cannot read selected source revision');
const text = source.stdout.replace(/\r\n/g, '\n');
function between(start, end) {
  const a = text.indexOf(start);
  const b = text.indexOf(end, a + start.length);
  if (a < 0 || b < 0) throw new Error(`Missing source boundary: ${start}`);
  return text.slice(a, b);
}
const helpers = between('LOCAL IUM32 c_cpu_event_snapshot', '/*\n   Prototype').trim();
const halt = between('       while ( TRUE )', '\tquick_mode = FALSE;');
const resetArea = between('   /* Action RESET first.', '   /* Action Insignia');
const resetStart = resetArea.indexOf('   if (c_cpu_take_event');
if (resetStart < 0) throw new Error('Missing normal RESET consumer');
const reset = resetArea.slice(resetStart);
const timerArea = between('   /* Action Insignia', '   /* INTEL inter instruction');
const timerStart = timerArea.indexOf('   if (c_cpu_take_event');
if (timerStart < 0) throw new Error('Missing normal timer consumer');
const timer = timerArea.slice(timerStart);
const irq = between('   if (GET_IF() && c_cpu_take_event(CPU_HW_INT_MASK))', '#else\t/* SFELLOW */');
const masks = [...text.matchAll(/^#define (CPU_(?:RESET|SIGALRM|SAD|HW_INT)[A-Z_]*MASK)\s+([^\r\n]+)/gm)]
  .map(m => `#ifndef ${m[1]}\n#define ${m[1]} ${m[2]}\n#endif`).join('\n');
const out = requestedOut ? path.resolve(root, requestedOut) :
  path.join(root, 'build/M0-T412/S1/event-profile', revision ? 'baseline' : 'fixed');
if (!path.relative(root, out).replaceAll('\\', '/').match(/^build\/M\d+-T\d+\/S\d+\//)) {
  throw new Error('Output must stay below a task S build directory');
}
fs.mkdirSync(out, { recursive: true });
const body = `#include <windows.h>
#include <stdio.h>
#include <assert.h>
typedef unsigned long IUM32;
typedef int IBOOL;
typedef unsigned long IU32;
typedef unsigned short IU16;
typedef long IS32;
#define LOCAL static
#define VOID void
#define IFN0() (void)
#define IFN1(t,n) (t n)
${masks}
static volatile LONG cpu_interrupt_map;
static unsigned resets, ticks, sad, steps;
static int interrupt_enabled;
${helpers}
static void c_cpu_reset(void) { ++resets; }
static void host_timer_event(void) { ++ticks; }
static void force_yoda(void) { ++sad; }
#define GET_IF() interrupt_enabled
#define SYNCH_TICK() do { if (++steps == 1) c_cpu_raise_event(CPU_RESET_EXCEPTION_MASK); assert(steps < 4); } while (0)
#define QUICK_EVENT_TICK() ((void)0)
static void halt_consumer(void) { ${halt} }
#define INTERNAL 0
#define SET_POP_DISP(x) ((void)(x))
static void reset_consumer(void) {
  int doing_contributory, doing_page_fault, doing_double_fault, doing_fault, EXT;
  ${reset}
NEXT_INST: ;
}
static void timer_consumer(void) { ${timer} }
static unsigned deliveries, acknowledges;
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
static void irq_consumer(void) { ${irq} }
int main(void) {
  cpu_interrupt_map = 0; steps = 0;
  halt_consumer(); /* first tick raises RESET while halted */
  reset_consumer();
  if (resets != 1 || (c_cpu_event_snapshot() & CPU_RESET_EXCEPTION_MASK)) {
    fputs("FAIL: HALT consumed RESET before normal reset handler\\n", stderr); return 1;
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
  puts("PASS: extracted HALT/RESET/timer/SAD/INTR and IRQ callback-boundary consumers; not full CPU/PIC execution");
  return 0;
}
`;
fs.writeFileSync(path.join(out, 'event-consumers.c'), body);
const vs = 'C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat';
fs.writeFileSync(path.join(out, 'compile.cmd'), `@echo off\r\ncall "${vs}" -arch=x86 -host_arch=x64 >nul\r\nif errorlevel 1 exit /b %errorlevel%\r\ncl /nologo /TC /MT /W3 event-consumers.c /Feevent-consumers.exe\r\n`);
const compile = spawnSync('cmd.exe', ['/d', '/c', 'compile.cmd'], { cwd: out, encoding: 'utf8' });
process.stdout.write(compile.stdout ?? ''); process.stderr.write(compile.stderr ?? '');
if (compile.status !== 0) process.exit(compile.status ?? 2);
const exe = fs.readFileSync(path.join(out, 'event-consumers.exe'));
if (exe.readUInt16LE(exe.readUInt32LE(0x3c) + 4) !== 0x14c) throw new Error('Not x86');
const run = spawnSync(path.join(out, 'event-consumers.exe'), [], { cwd: out, encoding: 'utf8', timeout: 5000 });
process.stdout.write(run.stdout ?? ''); process.stderr.write(run.stderr ?? '');
if (run.error) throw run.error;
process.exit(run.status ?? 2);
