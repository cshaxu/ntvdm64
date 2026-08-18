# M0 T226 — OpenNT BOP owner-package code-completion plan

## Question

How can the current 203-endpoint BOP population become locally code-complete without falling back to trace-led, per-service implementation?

## Inputs

- `t225-s57-bop-owner-package-profile-ledger-001.json` — the complete source-inventory-derived population and current route/disposition baseline.
- T225 S19 and S20 package closures — existing DEM and COMMAND package regressions, which remain evidence and are not reopened merely because a native trace later reaches a member.
- The current Direct/Readonly profile ABI and formal MSVC x64 `/MT` Ninja graph.
- Retained OpenNT source trees, subject to the mandatory source-recovery ladder.

## Code-complete definition

An endpoint is code-complete only when its owner package has selected and locally tested one of the following results:

1. independently composable original OpenNT provider;
2. original provider through the smallest contract-preserving compatibility seam;
3. source-derived rehost with its original owner, layout/order/failure rule and rejected earlier rungs recorded; or
4. source-proven no-op, unavailable, deferred or typed-stop behavior with a negative test proving that it does not misreport success.

Typed routing alone is not code completion. An unavailable historical host composition may be code-complete for the declared Direct/Readonly profile only when its failure behavior is source-proven and locally tested. Overlay remains an ABI reservation, not a T226 implementation target.

## Ordered S work

| S | Owner package | Endpoint count | Local acceptance |
| --- | --- | ---: | --- |
| S1 | Shared ledger-to-conformance foundation | 203 | Every ledger row maps to exactly one package-local fixture or explicitly versioned negative fixture; existing DEM/COMMAND closure evidence is imported as baseline. |
| S2 | DEM direct-host namespace/file/FCB/search/state | 73 | Reconcile S19 against the shared fixture contract; repair only actual route/test gaps, then run the full Direct/Readonly family regression. |
| S3 | COMMAND bootstrap/launch/console/environment/lifecycle | 17 | Reconcile S20, including registration, launch, CWD, keyboard-layout, environment and terminal dispositions; run the full Direct/Readonly family regression. |
| S4 | XMS/A20/allocator/UMB/INT15 | 12 | Reuse mantle-backed mechanics where source-proven; test every unavailable UMB/INT15 disposition locally without enabling machine features by convenience. |
| S5 | DPMI protected-mode/LDT/exception/memory | 25 | Establish the complete source/ABI/failure map and local protected-mode negative/availability fixtures; do not export Bochs internals into the adapter. |
| S6 | Redirector network and IPC | 50 | Implement or retain the one complete source-proven unavailable family disposition with full family regression; network/IPC enablement remains a separately admitted profile. |
| S7 | Debugger, top-level/machine, and WOW host-composition | 26 | Verify debugger deferral, top-level failure/no-op/idle dispositions, machine ownership handoff, and WOW absence without moving their semantics into bx-vdm. |
| S8 | All-package reconciliation | 203 | Run every package-local Direct/Readonly regression, regenerate the reconciliation ledger, and certify no stale shim or untested route remains. |

## Execution constraints

- Complete a whole listed package before beginning the next one; do not create S work for a selector observed in a trace.
- Local fixtures validate copied ABI, guest-memory effects, resume/stop result, profile enforcement, source-defined ordering and failure disposition.
- A native guest trace is prohibited during S1--S8 except as a separately admitted package-end integration check; it may report a new owner domain but cannot create a leaf-service backlog.
- All implementation decisions retain the source-recovery ledger: original source, smallest seam, registered external intrusion, then authored behavior.

## Follow-up

After S8, select the Queue candidate for native Guest DOS integration and execution continuity. That later task validates the code-complete packages against immutable guest images and classifies remaining issues by owner; it does not reopen package architecture from the order of trace records.
