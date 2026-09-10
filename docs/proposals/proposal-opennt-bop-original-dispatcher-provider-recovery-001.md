# OpenNT BOP Original Dispatcher and Provider Recovery

## Status

This proposal is admitted as **M0 T271**.  It replaces neither the original
source policy nor the selector-blind `adapter-bop` boundary.  It corrects the
T270/S4 finding that a large portion of the current `opennt-bop` root is
project-authored route, bridge, observation, or session composition rather
than an OpenNT mirror.

## Objective

Make `opennt-bop` an actual original-code mirror component again: retain
original OpenNT BOP dispatch/provider source and only the smallest registered
same-shaped divergence at a call boundary.  Rehome project-authored mechanics,
session lifecycle, diagnostics and historical platform facades to their
proper owners; then recover the DEM, COMMAND and XMS package paths from their
original dispatcher/provider structures.

The intended top-level structural reference is OpenNT
`base/mvdm/softpc.new/host/src/nt_bop.c`: its `MS_bop_0`, `MS_bop_2`,
`MS_bop_3`, `MS_bop_4`, `MS_bop_7` and related selector ownership establish
the source order and failure contracts.  It is not linked wholesale: its
historical CCPU/SAS, DLL, WOW, VDD and NT4 product-shell dependencies must
first be supplied through a same-shaped adapter or remain source-proven
unavailable.

## Boundaries

- `adapter-bop` keeps only copied-frame BOP-entry ingress and typed
  completion. It may recognize the mechanical entry envelope needed to enter
  `opennt-bop`, but may not interpret a selector/service or own an OpenNT
  provider.
- `adapter-softpc` owns same-shaped CCPU/SAS/SoftPC facades that turn an
  original machine-facing call into bounded Bochs mechanics through
  `adapter-bochs`; it may not interpret a selector/service.
- `adapter-win32` owns same-shaped replacements for unavailable historical
  Win32/NTDLL interfaces.
- `session` owns neutral per-session state, resources, mapping instances and
  completion/event state; it has no BOP vocabulary.
- `app` owns final wiring only.  `adapter-bochs` remains Bochs-only.
- `opennt-bop` retains only selected OpenNT BOP provider/dispatcher source and
  smallest registered mirror boundaries.  Observation and test-only material
  belongs in tests or diagnostic evidence, not this production mirror root.

## S Sequence

### S1 — non-mirror BOP composition extraction

Inventory every production file under `opennt-bop/ingress` and
`opennt-bop/observation`, plus all `*_composition`, `*_session`, `*_bridge`,
`*_compat` and source-derived route files. Rehome diagnostics and top-level
composition to their unique owner, or delete them when superseded by the
source-first recovery below. A current selector/service bridge cannot move to
`adapter-bop`: it remains temporarily in `opennt-bop`, frozen with an exact
original-dispatcher replacement packet, until the matching S replaces it.
Do not move selector recognition into `adapter-bop`; replace it only through
an original-shaped dispatcher in a subsequent S. Restore missing directly
needed original headers/data units where they are an actual input to an
admitted package.

### S2 — DEM original package recovery

Use `dos/dem/dem.c`, `demdata.c`, `demdisp.c` and the DEM provider files as
the sole service/order/layout source.  Replace project-authored DEM route and
composition logic with same-shaped `adapter-softpc`, `adapter-win32`,
`opennt-host` or `session` facades.  Preserve the 73-slot original dispatcher
and source-defined unavailable paths; do not add a DEM service merely because
its selector exists.

### S3 — COMMAND original package recovery

Use `dos/command/cmd.c`, `cmddata.c`, `cmddisp.c` and the reached provider
files as the source of initialization, global/session state, service table,
ordering and failures.  Replace `command_*session` and composition algorithms
only where an original call requires an adapter-owned platform seam.  Keep
unreached WOW, Redirector and DOS EXEC parent-return contracts explicitly
unavailable or transferred to their owner packages.

### S4 — XMS original package recovery

Use `xms.486/xms.c`, `xmsdisp.c`, `xmsmemr.c`, `xms.h` and reached provider
sources as the package boundary.  Rehome A20, memory, guest-address and
session seams to their named mechanical/neutral owners while retaining the
original XMS table, layout and failure behavior.  Do not enable DPMI or add a
new generic memory manager.

### S5 — mirror closure and source-recovery audit

Measure every remaining `opennt-bop` and `opennt-bop-overlay` production
input against a pinned OpenNT path.  The desired result is no overlay body;
an exception is allowed only for one demonstrably inseparable original state
body, with a minimal mirror boundary, exact line ledger and a named future
adapter owner.  No project-authored route, observation, generic bridge or
session implementation may remain in either mirror root.  Verify focused
DEM/COMMAND/XMS positive and source-failure paths, formal Ninja closure,
component README/DIVERGENCE register consistency and documentation governance.

## Non-goals

- No DPMI, WOW16, Redirector, VDD/debugger, ISV DLL, CSR/BaseSrv, multi-VDM
  broker, or full guest EXEC-parent-return recovery.
- No new Bochs CPU, device, BIOS, memory or machine behavior.
- No trace-driven leaf BOP implementation.  Trace may only verify a completed
  source package.
- No fabricated success where an original dependency is unavailable.

## Exit Criteria

1. `opennt-bop` contains only selected original source, true subsets, or
   same-shaped minimal modifications with local `DIVERGENCE:` and README rows.
2. Every remaining source operation maps to a cited original span and every
   unavailable external dependency maps to one same-shaped adapter/failure
   disposition.
3. The original DEM, COMMAND and XMS dispatch/provider ordering is active for
   their admitted paths; no project-defined replacement dispatcher remains.
4. `opennt-bop-overlay` is absent, or contains only the S5-proved minimum
   exceptional body and has no external consumer.
5. Formal Ninja builds and focused package tests pass; the tracker and
   component registers state the exact remaining cross-owner deferrals.
