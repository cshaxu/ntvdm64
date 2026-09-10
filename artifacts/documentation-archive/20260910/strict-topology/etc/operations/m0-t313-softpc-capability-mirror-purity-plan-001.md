# M0 T313 — SoftPC Capability And Mirror-Purity Acceptance

## Purpose

Recover and prove the selected Win32/x86 CCPU40 original SoftPC lifecycle as
one machine package.  This is a package-first continuation of T310 source
assembly and T312 route retirement: it is not a compiler-warning sweep, a
trace-selected repair series, an x64 recovery task, or a route back to Bochs.

The product path is the original selected SoftPC/CCPU40 source and its
same-shaped historical-interface bindings.  When a reached `mvdm-host` source
contains a project-authored shim, stale Bochs-era reference, duplicate adapter,
or an unneeded mirror divergence, this task removes it at the owning original
package instead of creating deferred cleanup debt.  A necessary binding or
overlay remains only with a registered `DIVERGENCE:` marker and an exact
source-contract rationale.

## Fixed Profile And Boundaries

- The sole acceptance profile is MSVC Win32/x86 `/MT`, CCPU40.
- CPU30/MONITOR is not a product profile.  x64 recovery belongs to later P02C.
- Bochs, `src.old`, kernel VDM, CSRSS and a second machine executor are not
  production inputs, fallbacks or acceptance routes.
- The session mapping manager remains mandatory wherever a historical MVDM
  numeric guest/host boundary requires stable identity; x86 compilation never
  authorizes raw host identity through a historical ABI.
- Original source bodies keep their algorithm, ordering and interface shape.
  `adapter-mvdm-host-out` may provide only named same-shaped external
  interfaces; it does not replace an original owner body.

## Ordered S Packages

### S1 — Selected-machine capability and purity ledger

Reconcile the selected formal graph with every reached original machine family:
CCPU/FPU/SAS/RAM, EMS/UMB, BIOS/ROM/CMOS, PIC/PIT/timer/DMA, input, video,
storage and communications.  For each family record its selected original
entry/reset/stop edge, formal ownership evidence, local behavior witness still
required, retained adapter/overlay, and an immediate remove/replace/retain
decision for every non-original form reached in `mvdm-host`.

This is the only whole-package inventory pass.  It reuses T310/T312 evidence;
later S packages must work original-owner cohorts, not rediscover the full
repository or select work from a trace.

### S2 — CPU, memory, firmware and reset lifecycle

Prove one source-shaped `create -> CPU/SAS initialization -> ROM/CMOS/BIOS
load -> reset` path with an explicit typed stop/failure result.  Recover only
the original lifecycle forms and their already named mapping/host bindings.

### S3 — Interrupt, timer, DMA and input lifecycle

Prove the original controller initialization and bounded delivery/teardown
contracts as one cohort.  Public Win32 waits, events and console input may
back same-shaped bindings; private NT4 product-shell behavior remains an
explicit unavailable result.

### S4 — Video, storage and communications lifecycle

Prove selected original controller startup and a bounded positive or explicit
unavailable endpoint for each device family.  This does not add a presentation
window, destructive-media test, Redirector protocol or printer product shell.

### S5 — Bounded execution, teardown and final purity acceptance

Run the selected source-shaped machine from initialized state through a finite
instruction budget or typed controlled stop, then teardown.  Re-read every
retained `mvdm-host` divergence and overlay reached by S2--S4.  The closure
records source/adapter/overlay provenance, positive and negative evidence, and
the exact later owner of any deliberately unavailable product shell.

## T313 Exit

T313 closes only when S1--S5 provide x86 CCPU40 source and behavior evidence
for every selected machine family, the formal product graph remains free of
Bochs/MONITOR/kernel-VDM/`src.old`, each reached mirror difference is either
removed or registered as necessary, and the final bounded lifecycle neither
fabricates guest success nor kills the host process.  It does not claim a
complete DOS/WOW16 workload, BaseSrv/CSRSS recovery, x64 compilation or a
release artifact.
