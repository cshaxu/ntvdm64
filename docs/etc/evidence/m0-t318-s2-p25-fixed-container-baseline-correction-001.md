# M0 T318 S2 P25 — Fixed-container baseline correction

## Purpose

Re-run the single admitted non-debug, console-owning observation without
changing its launcher, arguments, media, firmware, timeout, or stage. This
corrects the current-runtime baseline before any new startup-owner repair is
considered.

## Immutable execution

The observation used the P22 formal container exactly:

- launcher: `build/tools/console-startup-observer.exe`;
- linked x86 CPU40 product: `build/M0-T318/S2/cpu40-v7vga-r4/original-softpc-process.exe`;
- fixed short stage: `build/M0-T318/S2/r`;
- child contract: `-f -o --ordinary-child`;
- real launcher-owned `CONIN$` and `CONOUT$`;
- non-debug execution and an eight-second timeout.

The resulting report is `build/M0-T318/S2/r/startup-observation-p25.txt`:

```text
container=console-owning-nondebug
result=timeout
exit=0x53504354
timeout-ms=8000
```

The sentinel exit is produced by the observer only after it terminates a
still-active child. It is not a product crash or a guest result.

## Correction and disposition

P22's earlier `0xc0000005` and P24's statement that it was the current
baseline are superseded for the formally admitted product/stage combination.
They remain historical observations; they must not be used to attribute the
current product to a C-VID slot, console initialization, BOP, NTIO, NTDOS, or
EXEC.

The fixed container proves only that the present selected product survives the
complete eight-second startup window under a real console. It does **not**
prove that it has reached NTIO, NTDOS, or guest EXEC.

## Static owner-cluster consequence

The source scan still retains the startup order from P24, but no longer treats
console/display or C-VID as the active terminal condition:

1. `host_applInit -> init_host_uis -> InitScreenDesc -> SetupConsoleMode` uses
   the real console handles supplied by the fixed container.
2. `nt_start_event_thread` uses the same source-shaped thread bridge already
   selected for `nt_event.c`; its failure surface remains original
   `DisplayErrorTerm`.
3. `scs_init -> GetNextVDMCommand(NULL)` remains the first Base VDM boundary;
   `CMDInit`, `DemInit`, `XMSInit`, and `DBGInit` follow it in the original
   order.
4. `host_start_cpu -> cpu_simulate` remains a separate execution boundary.

The next evidence must locate a source-defined boundary between those stages
without enabling a debugger or changing the fixed container. It must not
resume trace-led BOP implementation.

