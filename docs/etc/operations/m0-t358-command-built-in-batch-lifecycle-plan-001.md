# M0 T358 — COMMAND built-in and batch lifecycle

## Purpose

Prove a bounded original COMMAND `/C` workload completes and returns under the
same CPU40 product and immutable package contract that reached COMMAND-ready
in T357.  This package excludes external DOS EXEC, native child execution and
WOW.

## Sequence

1. **S1 — original COMMAND workload/return topology.** Map the selected
   built-in and batch route through original COMMAND.COM, `cmdGetNextCmd`,
   Base-VDM declaration, relevant COMMAND BOPs and source-owned completion
   markers.  Select one built-in and, only if its owner path is complete, one
   immutable batch workload.  Identify any immediate unselected original owner
   component before changing source or running the product.
2. **S2 — source-shaped owner recovery and formal link.** Recover only any
   exact immediate original owner S1 proves missing, through its original
   source and same-shaped bindings; prove local behavior and the formal CPU40
   link.  Do not substitute a BOP leaf or guest behavior.
3. **S3 — fixed built-in observation.** Use a fresh capacity-valid stage and
   one fixed console-owning observation.  Require a source-owned completion
   or return marker, not merely BOP ingress.
4. **S4 — optional immutable batch observation and closure.** Run a batch
   only if S1 proves it does not need external DOS EXEC.  Otherwise record the
   exact handoff to the next DOS EXEC package without claiming batch closure.

## Exclusions

No DOS `.COM`/MZ child, `cmdExec32`, CreateProcess, WOW, CPU/device semantic
change, guest-media mutation, CPU30, Bochs, x64 runtime, CSRSS/kernel VDM,
Redirector or graphics work.

## Completion

T358 closes only when the selected built-in completes through original
COMMAND and a source-owned return marker is observed.  A batch is included
only when its complete owner path stays inside this package; otherwise the
precise boundary is transferred to T359.

