# DOSX/DPMI32 runtime activation

## Purpose

Turn the already reached original DOSX path into a proved DPMI32 runtime
capability. This is deliberately separate from the pure-DOS first-shell
profile and COMMAND `cmdExec32`. Its product boundary is the complete
original DPMI32 host provider; the later `MS_bop_1` transition is owned by the
separate Win16/WOW32 package.

## Scope

Recover the complete original DPMI32 provider through DPMI initialization and
its first original `MS_bop_1` handoff boundary. Preserve original MVDM/DPMI
owners; use existing adapters only for same-shaped machine and mapping
bindings. Guest media is fixed project input and is not a task deliverable.

## Work sequence

### S1 — Reached DOSX dependency closure

Trace the reached DOSX startup through the actual MVDM and DPMI32 host
dependencies, and identify `MS_bop_1` as the successor owner boundary.
Classify each reached host symbol by original owner; do not patch a
trace-selected BOP leaf.

### S2 — Complete DPMI32 graph reconciliation

Reconcile the already-selected complete DPMI32 x86 provider with its current
CPU40/session mapping bindings: all 16 selected provider bodies and the full
`53:00`–`53:18` dispatch table remain one owner cohort. Correct the DPMI
mirror-audit allowlist from the divergence register. No WOW32 body, carrier or
provider image enters this T.

### S3 — DPMI formal link and focused lifecycle proof

Build the selected product and prove DPMI initialization through the complete
original provider, without claiming Win16 or WOW execution.

### S4 — Fixed-container DPMI workload observation

In the stable console-owning runtime container, run the frozen bootstrap
profile. Retain product/media identities and prove its observable transition
to the next original owner boundary. A source-owned failure is a valid result
only when it identifies that whole owner cohort.

## Exclusions

No guest `COMMAND.COM` rewrite, app-owned DOS parser, synthetic DPMI, CPU
workaround, WOW32 provider body, Win16/WOW runtime, or native `cmdExec32`
child recovery. Those remain distinct owner packages.
