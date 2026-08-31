# M0 T318 S2 P41 — Fixed-container startup prerequisite cohort closure

## Purpose

Make the selected CPU40 startup investigation reproducible and attributable.
This record closes the static scan of original host calls from process entry
through the first CPU dispatch. It does not claim that the product reaches
NTIO, NTDOS, or guest `EXEC`.

## Fixed observation contract

Every S2 runtime observation uses the tracked, non-debug, console-owning
launcher and its short executable-relative stage. The launcher supplies real
`CONIN$` and `CONOUT$`; the stage owns the same firmware, DOS media,
`-f -o --ordinary-child` argument profile, and an eight-second timeout. An
observation may change only the newly linked product EXE. It must not change
debugger attachment, console ownership, media, firmware, argument profile,
timeout, or launcher behavior.

## Original startup prerequisite scan

The directly selected original call chain is:

1. `obj.vdm/ntvdm.c::main` calls `TimerInit`, `CpuEnvInit`, `nls_init`, then
   `host_main` inside the original exception wrapper.
2. `base/support/main.c::host_main` establishes configuration and original
   SoftPC machine state, calls `cpu_init`, then `InitialiseDosEmulation`, and
   finally `host_start_cpu`.
3. `host/src/nt_msscs.c::InitialiseDosEmulation` initializes the original
   `scs_init` provider cohort, resolves the immutable staged `ntio.sys`, loads
   it through the original SAS sequence, and establishes the NTIO CS:IP entry.
4. `host/src/nt_cpu.c::host_start_cpu` has no host-policy branch: it calls
   `cpu_simulate`, which is the selected CCPU40 `c_cpu_simulate` path.

The early terminal or private-host prerequisites are grouped by original owner:

- **Timer/heartbeat:** `TimerInit` and timer/ICA/RTC may report
  `DisplayErrorTerm` and terminate on public handle/thread setup failure; P39
  establishes their original machine ordering.
- **NLS/error:** `nls_init` and the original error-resource path can terminate
  before host startup; P4 records the selected resource.
- **Console/display:** `host_reset -> ConsoleInit -> InitScreenDesc` uses real
  standard console handles or reports an original error. P38 proves the fixed
  `-o` profile selects the windowed original branch.
- **Base VDM/providers:** `scs_init` obtains initial command state, then
  initializes original DEM, XMS, and DBG. P6/P36 establish the selected Base
  VDM binding and provider cohort.
- **Media/SAS/CPU entry:** P32/P33 establish original contiguous SAS RAM,
  firmware initialization, staged NTIO load, and the first CCPU40 fetch. The
  session mapping manager is deliberately not part of ordinary initial RAM or
  NTIO loading.

There is no direct BaseSrv/CSR or console-policy call in `host_start_cpu`
before CCPU dispatch. A future failure must first be assigned to one of these
complete predecessors or to the CCPU first-dispatch contract; it must not
justify a new BOP service or guest workaround.

## Layered verification rule

1. Run formal Ninja only for changed static libraries and the final product
   link; do not turn a runtime question into a full rebuild.
2. When a new product EXE exists, run exactly one five-to-ten-second fixed
   container observation and record only product exit/timeout plus container
   identity.
3. Enable NTDOS/EXEC-specific evidence only after that observation proves
   NTIO reachability. A timeout, host termination, or unassigned exception is
   returned to the preceding original owner cohort.

## Disposition

The fixed container and complete pre-NTIO owner scan are now the S2 execution
discipline. No source edit, diagnostic instrumentation, observer variation,
BOP route, or guest-semantic change is admitted by this record.
