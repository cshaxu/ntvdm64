# M0 T329 S1 — Original CPU40 recursive-frame lifecycle contract

Date: 2026-08-31

Status: closed static/lifecycle map; no missing selected CCPU frame binding was
found.

## Selected original lifecycle

The formal product selects `NTVDM`, `CPU_40_STYLE`, `CCPU`, `NEW_CPU` and
`PROD`; it does not select CPU30, MONITOR or PIG. The original startup order
is intact:

```text
ntvdm.c::main
  -> base/support/main.c::host_main
  -> cpu_init (CPU40 macro: c_cpu_init)
  -> ccpu386InitThreadStuff
  -> InitialiseDosEmulation
  -> host_start_cpu
  -> c_cpu_simulate
```

`ccpu386InitThreadStuff` allocates the original TLS slot and establishes the
main-thread `ThreadSimBuf` at level zero before `host_start_cpu` can call
`c_cpu_simulate`. `ccpu386SimulatePtr` then returns `sims[level++]`; the
executor creates the corresponding exception frame through
`ccpu386ThrdExptnPtr`. The BIOS `D6 FE` and extended BOP-FE forms both reach
`c_cpu_unsimulate -> ccpu386Unsimulate`, which decrements the same TLS frame
depth and `longjmp`s to the original simulation frame.

The external original `host_simulate` wrapper and the in-header
`host_simulate_func` vector both resolve to the selected `c_cpu_simulate`
body. The existing mirror divergences only make this selection and C `jmp_buf`
array spelling explicit; they do not introduce another executor or frame
carrier.

## Complete direct re-entry caller classification

The selected product includes direct original callers in DEM DASD, DPMI32,
BIOS RTC/ROM, CCPU SAS, floppy/FDC, keyboard, mouse, video and NT mouse
paths. Each saves or establishes original guest state before calling the same
CPU40 route and restores its documented state after the BOP-FE return.

`wow32/wcall16.c` and `wow32/wkman.c` are direct original callers but are not
selected by the formal product. They remain the later WOW owner package; this
CPU40 S neither enables nor changes their callback/monitor semantics.

## Historical base-level behavior

`ccpu386Unsimulate`'s level-zero diagnostic followed by decrement/`longjmp`
is byte-for-byte the same in the three approved local baselines:
`opennt`, `opennt-4.5`, and `opennt-src-2`. It is an invalid-call diagnostic,
not a source-provided recovery path. No current fixed-container artifact
contains its diagnostic. Replacing it with a return would hide a possibly
wrong BOP-FE or guest control transfer and is not admissible.

## Focused behavior recheck

The existing selected-source bounded CCPU fixture was rerun without source or
fixture changes on both architectures:

```text
build/M0-T310/S8/execution-20260829/x86/ccpu-bounded-execution.exe
build/M0-T310/S8/execution-20260829/x64/ccpu-bounded-execution.exe
```

Both emitted the original lifecycle sequence through `returned-start`,
`reenter` and `returned-recursive`. Its leading pre-initialization
`ccpu386ThrdExptnPtr ... Bad Id` line is the existing intentional negative
check; normal initialization and the nested BOP-FE return succeeded after it.

## Disposition

The reached CPU40 frame contract is source-selected and operational for the
bounded original nested execution. No earliest missing original binding exists
for T329 S2 to recover. The fixed guest observation still needs exact runtime
exception context (CS:EIP, stack and record), rather than a speculative frame
guard, vector change or BOP/device patch. That diagnostic-only attribution is
the successor owner package.
