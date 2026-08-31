# M0 T318 S2 P40 — CPU40 Entry Prerequisite and Termination Audit

Date: 2026-08-31  
Status: static prerequisite audit; no new runtime observation

## Complete startup prefix

The selected original entry and CPU40 order is:

```text
obj.vdm/ntvdm.c::main
  TimerInit()
  CpuEnvInit()
  nls_init()
  host_main()
    cpu_init()                         -> c_cpu_init()
    InitialiseDosEmulation()
      reset() -> BIOS/device reset
      scs_init()
      load NTIO.SYS and set CS:IP
    host_start_cpu()                   -> c_cpu_simulate()
```

This corrects the possible but false explanation that CCPU's TLS simulation
frame is initialized after `host_start_cpu`: original `cpu_init()` precedes
`InitialiseDosEmulation()` and `host_start_cpu()`.

## CPU40 simulation-frame contract

Original `c_cpu_init()` calls `ccpu386InitThreadStuff()`.  That allocates the
TLS index, registers the main thread, and establishes its `ThreadSimBuf`.
The selected CCPU manifest includes original `ntthread.c` and `c_main.c`.
`c_cpu_simulate()` then obtains its `jmp_buf` through the original
`ccpu386SimulatePtr()`, enters `ccpu(FALSE)`, and establishes the matching
per-thread exception frame through `ccpu386ThrdExptnPtr()`.

No CPU30, Bochs, generic `Cpu` vector, or adapter execution loop is selected
in this CPU40 call chain.  The only exceptional local failures are original
`TlsAlloc`/allocation/TLS-set failures; their original code reports a
diagnostic and returns a null frame.  There is no evidence that one occurred
in the fixed observation, and changing those failure semantics would not be a
source-shaped repair of the observed access violation.

## Direct termination and private-host scan

The top-level prefix has only these relevant explicit termination families:

- `TimerInit`: public `CreateEvent` failures follow original
  `DisplayErrorTerm`/`TerminateVDM`.
- `nls_init`: product resource strings or system error text failure raises the
  original insufficient-resources exception.  The selected product resource
  was restored and linked by P4.
- `CpuEnvInit`: an unavailable historical CpuEnv registry key is an original
  empty-configuration success path; it has no mandatory BaseSrv/CSR edge.
- `InitialiseDosEmulation`: missing/invalid `NTIO.SYS` follows the original
  `host_error` failure path; P10 already distinguishes its media decision.
- CCPU entry itself has no direct `DisplayErrorTerm`, BaseSrv, CSRSS or Console
  Server call before its first instruction decode.

The original top-level exception filter deliberately turns an unhandled
exception into process termination with the original exception status.  Thus
the fixed container's `0xC0000005` proves an unhandled in-process fault but
does not identify a source location.  It must not be reclassified as an
NTIO/DEM/COMMAND/EXEC failure.

## Conclusion

The entire pre-decode host/CCPU cohort is selected, linked, and ordered as the
original CPU40 product expects.  No concrete missing provider or modern-host
compatibility edge is found, so no product mutation, rebuild, or repeat fixed
observation is admissible.  The next work must examine the first-decode
machine/data cohort as a whole, or introduce an owner-approved diagnostic
whose operation does not alter the fixed-container contract.  No NTIO, NTDOS,
EXEC, or parent-return claim is made.
