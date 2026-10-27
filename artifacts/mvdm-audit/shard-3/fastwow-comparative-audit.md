# `fastwow.asm`: original and CCPU40 selected path

## Question and input

Can the original x86 fast WOW BOP/callback code be reconnected to the current
CCPU40 product without adding a competing monitor or changing guest media?
The frozen original/current source row is `wow32/i386/fastwow.asm`.

## Source and selected build

* Original `src/mvdm/wow32/i386/sources` explicitly lists `fastwow.asm`
  and `callpr32.asm` as `I386_SOURCES`. This is distinct from the disabled
  generated C dispatch tables in the `!IF 0` block of `wow32/sources`. The
  S1 ledger's common justification for both kinds of input is insufficient.
* Original `fastwow.h` chooses `FASTBOPPING=1` for `i386` without
  `DEBUG_OR_WOWPROFILE`; the x86 fast implementation is a real performance
  and calling path, not a disabled table.
* Current generated WOW32 Ninja `cflags` have CPU_40_STYLE, CCPU and modern
  x86 MSVC, with no explicit `/Di386`; `fastwow.asm` is absent from that
  graph and from the DLL map. With those preprocessor flags and `fastwow.h`
  in isolation, `fastwow-macro-probe.c` prints `I386_NOT_DEFINED` and
  `FASTBOPPING_OFF`. Independent compiled-object evidence is stronger: the
  production `wcall16.obj` SHA-256
  `3A4739BF1088341227727A1162114F1C9F60ED58C9F1B9086A31DC0E08D36B13`
  imports `_host_simulate` and does not import `FastWOWCallbackCall` or
  `CurrentMonitorTeb`. The isolated macro probe alone does not establish
  every macro potentially introduced by `precomp.h`.

## Original fast path and current path

`WOWBopEntry` uses NT4 private `ExpVdmTib` monitor/VDM contexts, switches
from 16-bit execution to the monitor stack, restores USER/TEB segments,
looks up the original thunk frame, dispatches and handles virtual interrupts.
`FastWOWCallbackCall/Ret` likewise exchange monitor/guest contexts and use
`DispatchInterrupts` and `iretd`. It assumes a real kernel VDM monitor,
private TIB, native segmented context and x86 privilege environment.

Current callback remains original `wcall16.c::wow_original_CallBack16`: it
prepares the original CBVDMFRAME, selects SETVDMSTACK, saves the emulated IP,
re-enters through `host_simulate`, restores IP, reads the original return
frame and unwinds. This is the original non-fast source branch with the
registered DIV-178/304/343/348 lifetime/admission fixes, not the assembly.

The current CCPU path is more specific than simply refusing every fast BOP:
`dpmimscr.c::DpmiGetFastBopEntry` calls the project CPU40 binding to allocate
a guest LDT endpoint; `W32GetFastAddress` and `W32GetFastCbRetAddress` expose
bounded worker offsets. Original CCPU `call.c::CALLF` intercepts only the
selected selector and three offsets: inline BOP, WOW dispatch (0x51), and
callback return (0xfe). It routes to the existing BOP dispatcher or stops
simulation rather than entering NT4 monitor assembly. The fixed NTVDM state
mode bit is maintained in guest SAS. These are selected CPU40 adaptations,
not proof that the old direct x86 fast-path timing, interrupt order or every
register/exception effect is identical.

## Evidence and limitations

`tests/mvdm-host/dpmi/fast_bop_mode_fixture.c` checks the reserved-selector
contract, three endpoint classifications, invalid address refusal and RM/PM
mode flag against a **mock CPU/SAS boundary**, not real guest execution.
T447 S2 scope-freeze records real guest generic/timer/hook/common-dialog/Shell
callback observations and separately compares slow C dispatcher restoration
with assembly. T473 S9 r10 verifies Win16 application startup and selected
callbacks. None is a complete controlled old-fast-versus-CCPU equivalence
matrix or performance measurement.

## Options

1. **Keep the original non-fast C branch and narrow CPU40 endpoint bridge;**
   trace the complete guest request/dispatch/return chain and finish tests
   for registers, nested callbacks, virtual interrupts, task cancellation,
   exceptions and reset. This retains existing CPU40 and guest constraints,
   minimizes new machinery and preserves previously witnessed behavior. Its
   exact x86 fast-path ordering and performance remain unproven.
2. **Recreate a monitor-compatible execution carrier for fastwow.asm.** Its
   private TIB, monitor stack, descriptor/segment transitions, direct `iretd`,
   exception and interrupt protocol require a broad kernel-VDM compatibility
   closure and violate the currently selected CPU40/no-private-monitor
   architecture unless separately redesigned and approved. Merely changing a
   macro or assembling the file is unsafe and not a complete implementation.
3. **Add a CPU40-oriented optimized gateway** only if measurement proves a
   need and source/contract audit defines exact preserved ordering. This is an
   independent new host implementation that adds code and must meet the same
   callback/interrupt/exception evidence; it cannot be labelled original
   fastwow.asm or treated as a no-diff restoration.

Recommendation: option 1 is the only currently justified path within the
existing architecture; keep `fastwow.asm` pending explicit source/role
disposition and characterize the lost acceleration. If old fast-path event
ordering proves observably different, obtain approval for that specific
boundary instead of silently declaring equivalence.
