# SoftPC standalone-machine boundary audit

## Question

Can the selected `mvdm-host/softpc.new` execution path be extracted as an
independent virtual-machine backend without importing the NTVDM product host?

## Scope and method

This is a read-only, source-first boundary audit performed 2026-08-29.  It
starts at the original startup/CPU roots and records direct observed seams.
It does not claim a complete call-graph closure, runnable standalone machine,
or a new active project packet. It is archived evidence only: the active
packet remains M0 T310 S15, and this audit changes no production source.

Inputs were the selected source manifests and bodies under:

- `src/mvdm-host/softpc.new/base/{dirs,support,system,ccpu386,bios,...}`;
- `src/mvdm-host/softpc.new/host/{src,inc}`;
- `src/adapter-mvdm-host-out/softpc` and `src/session`; and
- `docs/STATUS.md`, architecture/execution/source-policy authorities.

## Finding

**Extraction is feasible, but the current recovered path is not yet a
standalone VM.**  The evidence establishes a useful machine nucleus
(CCPU/FPU, SAS physical memory, PIC/PIT/DMA, ROM/BIOS, device models and CPU
re-entry), but its historical outer sequence entangles NT host setup, console
and threads, DOS-media loading, and NTVDM BOP service dispatch.

The cut therefore must be semantic rather than directory-based:

```
machine backend
  CPU/FPU + guest physical memory + ISA devices + ROM/firmware mechanics
  finite execute/resume + typed mechanical stop
          ^ machine/host ports
SoftPC compatibility adapter
  legacy call shapes, bounded mappings, profile wiring
          ^
MVDM product host
  MS BOP services, DOS/DEM/COMMAND, WOW, VDD, console/CSR-era behavior
```

The machine must retain a **generic firmware-to-host trap** mechanism.  It
must not retain the current NTVDM `MS_bop_0..F` service table or interpret a
BOP selector as a DOS/WOW operation.  Firmware-facing built-ins (for example
BIOS video, disk, reset, EMS and mouse bridge entries) need either a profile
registered dispatch table or explicit `unsupported` result; that dispatch is
not the MVDM BOP product dispatcher.

## Most important source observations

1. `base/support/main.c` contains a full historical application composition,
   not a machine constructor: it parses an application command line, invokes
   `host_applInit`, configuration/UI setup, conditionally calls
   `InitialiseDosEmulation`, then calls `host_start_cpu`.  It is outside the
   future machine core.
2. `host/src/nt_cpu.c` reduces the CPU-run root to `cpu_simulate()` but its
   nested `host_simulate()` asserts against the NT TEB/ICA lock.  The run
   mechanics belong in the core; the NT lock/TEB policy must become a host
   scheduler port or compatibility adapter responsibility.
3. `base/ccpu386/c_main.c` is the executor nucleus and directly consumes
   CPU, SAS, interrupt, timer, BIOS and CCPU contracts.  Its source-missing
   `EDL_fast_bop(ULONG)` fallback is currently a debug-break disposition, not
   a standalone stop contract.
4. `host/src/nt_reset.c` owns console, window, WOW command-line, heartbeat
   thread, process/thread handles and idling.  These are host/product setup,
   not a machine reset implementation.  The underlying `base/bios/reset.c`
   reset semantics remain a machine candidate once supplied with ports.
5. Existing adapter evidence already isolates three needed edge classes:
   session-bound CPU outer-run observation (`mvdm_softpc_execution.c`),
   immutable firmware/DOS media lookup (`mvdm_softpc_firmware.c`), and
   physical external-memory/EMS aliases (`mvdm_softpc_physical_mapping.c`).
   These are migration seams, not proof that the core has no direct host
   dependencies.

## Admission recommendation

Do not start an extraction implementation from this artifact. First admit a
separate, post-T310 T package to complete the outbound/inbound closure at
definition-and-caller granularity, choose a minimal machine profile, and
design a typed stop/trap ABI.  Its first executable acceptance fixture should
not link `dos`, `wow32`, BOP ingress, BaseSrv/CSR or `host/src/nt_reset.c`.

The proposed acceptance path is:

```
create(profile, ports) -> attach RAM/ROM -> reset -> run(budget)
-> {budget exhausted | firmware trap | halt | fault | unavailable}
-> destroy
```

No claim should be made that an unbounded `host_start_cpu()` return is a
finite run slice: source currently exposes `void` execution and uses nested
`c_cpu_unsimulate()` returns.  A new run-result boundary must distinguish
outer termination from nested BIOS/device re-entry.
