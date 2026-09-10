# M0 T318 S2 P31 — Original unhandled-exception boundary audit

## Purpose

Classify the current formal `runtime-r30` exit `0xC0000005` without changing
the fixed non-debug, console-owning observation container or introducing a
debugger/in-product probe.

## Inputs

- P30's immutable `runtime-r30` manifest and eight-second observation;
- `src/mvdm-host/softpc.new/obj.vdm/ntvdm.c`;
- `src/mvdm-host/softpc.new/host/src/nt_timer.c`;
- selected `CPU_40_STYLE` CCPU entry sources
  `softpc.new/base/{support/main.c,ccpu386/c_main.c,ccpu386/ntthread.c}`;
- read-only Windows Application event query for
  `original-softpc-process.exe`.

## Original control boundary

`ntvdm.c::main` and the original timer/event thread wrappers all use
`VdmUnhandledExceptionFilter`. Its original operation is narrow:

1. Suspend the timer heartbeat.
2. Call public `UnhandledExceptionFilter` with the actual exception.
3. If that filter selects `EXCEPTION_EXECUTE_HANDLER`, terminate the current
   process with the original exception code.
4. Otherwise resume the heartbeat and return the filter disposition.

Thus the observed `0xC0000005` proves only that an access violation escaped a
wrapped SoftPC thread. This boundary neither identifies a CCPU instruction nor
authorizes an error-to-success conversion, BOP change, or synthetic guest stop.

## CCPU startup check

The selected source order is intact:

`host_main -> c_cpu_init -> c_cpu_simulate`.

`c_cpu_init` calls the original `ccpu386InitThreadStuff`, which allocates the
TLS index and establishes the main-thread simulation buffer before
`host_start_cpu` calls `c_cpu_simulate`. Therefore a null
`ccpu386SimulatePtr()` result is not an evidenced current cause. It remains a
normal original failure path, not a candidate for a speculative shim.

The C-VID binder is likewise called at original `setup_vga_globals` before
`host_init_screen` and before `InitialiseDosEmulation`; P23's explicitly
unavailable slots remain a source-defined disposition, not proof that one was
called by the current product.

## Event-record limitation

The read-only Application log contains an `original-softpc-process.exe`
access-violation report for historical mutable `runtime-r28-r4`, not for the
P30 hash-identified `runtime-r30` product. It has an unknown fault module and
offset zero, so it cannot be used to attribute the current product. No r30
fault frame or symbolized source location is available through this
non-invasive channel.

## Disposition

The fixed r30 observation remains the only runtime row. The next recovery unit
is the complete original CPU40 execution-start cluster (CCPU state, generated
C-VID vector, GDP carrier, first BIOS-visible dispatch and their original
failure branches), reviewed statically as one owner contract before any source
change. The container, its media, firmware, console ownership, arguments and
timeout stay unchanged.
