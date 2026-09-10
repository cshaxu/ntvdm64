# Complete adapter ownership migration

## Purpose

Make `adapter-mvdm-host-out` describe current ownership rather than historical
accumulation.  In particular, leave `softpc` with only CCPU/SoftPC machine
mechanics and move every non-machine implementation, declaration, build input
and test reference to its one final owner.  This is a complete architecture
migration, not a partial cleanup before another recovery attempt.

## Why it precedes DPMI32/WOW32 runtime work

The original DPMI32 and WOW32 owners both use host-facing seams.  The current
`softpc` family contains machine mappings beside COMMAND, VDD, termination and
observation semantics.  Admitting or diagnosing the original
providers while those meanings are mixed would make a missing provider look
like a machine dependency and entangle a source recovery with a directory
migration.

## Required end state

1. A complete tracked owner ledger covers every file now below
   `src/adapter-mvdm-host-out/softpc`, including public headers, production
   implementations, tests, build references and generated-graph inputs.
2. Each row has exactly one final semantic owner, original caller(s), ABI
   surface, dependency direction, migration operation, and verification row.
   `unknown`, `temporary`, dual implementations and an unowned compatibility
   copy are invalid dispositions.
3. `softpc` retains original SoftPC/CCPU machine mechanics and its controlled
   host-resource bridges: synchronous
   guest-memory/physical mapping, A20, effective-address/descriptor handling,
   machine execution, machine-shaped host identity, and reads of already
   selected SoftPC firmware/V7 VGA ROM state where its original caller
   requires them.  Configuring a resource root or consuming a presentation
   result does not transfer that bridge's ownership to `app`.
4. COMMAND/BaseVDM, VDD/DOS-WOW shared state, and lifecycle/observation
   receive distinct architecture-approved owners.  S1 must decide whether
   each belongs in an existing specialist family, `session`, `app`, or a newly
   admitted narrow specialist family; it may not assume a new generic helper
   root.
5. Every relocated public header has one compatibility forwarding path only
   for the bounded migration interval.  The forwarding file contains no
   implementation, and every caller/build reference is then moved to the
   final path before closure.
6. The default formal product and focused fixtures have identical source
   behavior before and after the migration.  The work does not change a BOP,
   guest byte, source-owned MVDM control flow, machine backend, media
   selection result, or runtime acceptance claim.

## Work sequence

### S1 — Full ownership inventory and final placement decision

Freeze the current source/build/test consumer graph.  Produce the complete
ledger and decide every final owner with architecture review.  Record each
unavailable private contract as a named boundary, not as a reason to leave
the file in `softpc`.

### S2 — Move the VDD-owned cohort into its existing specialist family

Move the five ledger paths for VDD configuration, unavailable behavior and
SFT/JFT shadow state into the existing `vdd` family.  This is the first
migration because `vdd` is currently an empty but architecture-approved owner:
it gains its already-defined boundary without changing any competing command
or machine path.  Update its direct includes, source lists and focused VDD
fixtures only.  Preserve the existing VDD failure/status and copied-state
contracts.

### S3 — Move COMMAND standard-stream redirection into `redir`

Move `mvdm_command_redirection.{h,c}` to the existing redirector family and
update the direct COMMAND consumers and fixtures.  This small, low-fan-out
cohort makes pipes and standard-stream ownership explicit before the larger
COMMAND record migration, avoiding a second mixed-owner rewrite of the same
command execution surfaces.  Its acceptance is the unchanged focused
redirection fixture and formal build-input comparison.

### S4 — Move BaseVDM/COMMAND record state into `basesrv`

Move the four `mvdm_command_guest_state` and `mvdm_command_native_child`
paths to `basesrv`.  This is the highest-ROI remaining host-service boundary:
it removes COMMAND record production and native-child snapshot state from the
machine family.  It follows S3 because the two cohorts meet at COMMAND launch
consumers.  Preserve record layout, failure rules, ABI and child-launch
semantics; verify the existing command-native-child and BaseVDM focused
fixtures plus the formal product source graph.

### S5 — Function-level termination/observation and host-ABI ownership audit

`mvdm_softpc_termination.{h,c}` is not a component and must not be moved as a
unit. Before any split is admitted, map every exported function, backing state,
direct caller, formal source-list input and default-off report contract. The
audit separately establishes the owner of session termination/origin; app
startup report-environment capture; CCPU/SoftPC CPU, keyboard, PIC and
SAS/memory observations; COMMAND/DEM/BOP service-local observations; and
product-container main-return/unhandled-exception observations.

It also establishes the source-compatible home for `error_abi.h`. The existing
`opennt-abi` original-host ABI carrier is the candidate, not `win32`, but the
audit must verify original definition, include/export and formal-graph contract
before relocation. `monitor` is not a presumed destination: its historical VDM
monitor/TIB/DPMI-control boundary does not authorize a general diagnostics
directory. S5 changes no production source or call site.

### S6 — Session and app direct-boundary extraction

After S5 selects exact symbols, extract only the session termination/origin
bridge and app-only continuation-report environment capture into their existing
owners. Preserve session completion/escape result, inherited guest environment,
and report opt-in/default-off behavior. Do not move a machine or service-local
observation in this packet.

### S7 — SoftPC/CCPU machine-observation extraction

After S5's caller map, split only CPU, keyboard, PIC and SAS/memory observations
into an explicitly named `softpc` machine-observation interface. They remain
default-off and attach to CCPU/SoftPC mechanics; this packet may not alter
interrupt delivery, SAS access, timing, keyboard state or report semantics.

### S8 — Service-local observation and original-host ABI extraction

Move each audited COMMAND/DEM/BOP observation subset with its actual service
owner only where the S5 caller/dependency map proves that owner. Move
`error_abi.h` to the verified `opennt-abi` original-host ABI carrier, preserving
`ERROR_STRUCT`, enum spelling/layout, and force-include/formal build rules.
Product-container main-return/unhandled-exception observations move only to a
S5-proven app or separately admitted narrow owner.

### S9 — Remove transition paths and prove graph equivalence

After every consumer uses its final owner path, remove each forwarding-only
compatibility file.  Reconcile the ledger against the source, build and test
graph; prove that `softpc` contains its 23 approved current machine/resource-
bridge paths plus only S7-approved machine-observation paths (and its updated
namespace policy), with no unapproved non-machine implementation, dual body or
lingering forwarder. Re-run formal x86/x64 compile/link input comparison
and every focused non-runtime fixture from S2--S8.

## Dependency and ROI order

| Order | Packet | Cohort | Dependency rationale | ROI / risk |
| --- | --- | --- | --- | --- |
| 1 | S2 | `vdd` (5 paths) | No predecessor; establishes the already-approved but empty specialist owner. | High ownership clarity, low fan-out and behavior risk. |
| 2 | S3 | `redir` (2 paths) | No hard code prerequisite, but settles shared COMMAND stream consumers before record-state rewrites. | High clarity per changed path, low risk. |
| 3 | S4 | `basesrv` (4 paths) | Follows S3 at the shared COMMAND launch boundary. | Highest functional ownership ROI; moderate fan-out. |
| 4 | S5 | mixed termination/observation body + `error_abi.h` | Requires S2--S4; disproves the invalid whole-file `monitor`/`win32` placement before any move. | Highest risk-reduction ROI; audit only. |
| 5 | S6 | session termination/origin + app environment capture | Requires exact S5 symbol/caller map. | Small direct-owner extraction; low behavior risk. |
| 6 | S7 | CCPU/SoftPC CPU/keyboard/PIC/SAS observations | Requires S5 map and S6 closure. | Preserves machine locality while making diagnostic ownership explicit. |
| 7 | S8 | service-local observations + original-host ABI declaration | Requires S5 map and relevant S3/S4 final owner paths. | Removes source-owner/ABI drift without a logging family. |
| 8 | S9 | all migrated cohorts | Requires S2--S8 and no remaining caller on a forwarding path. | Closure ROI; validates the whole migration rather than adding behavior. |

This is an admission order, not permission to run packets concurrently: only
one numbered S packet becomes active through `STATUS.md` at a time.  A later
packet may be resliced only if its prerequisite graph or the S1 ledger changes;
it may not reclassify a final owner without a new architecture decision.

## Per-packet audit and exit standard

| Packet | Admission evidence | Exit criteria and mandatory audit standard |
| --- | --- | --- |
| S2 | The corrected S1 ledger names exactly five VDD move rows, their direct consumers, ABI and focused fixtures. | Every one of the five bodies/declarations is owned only by `vdd`; every source/build/test include resolves to that owner or to one explicitly listed forwarding include; VDD configuration, unavailable and SFT/JFT shadow fixtures pass unchanged; x86 and x64 source lists contain one implementation each; a path/consumer sweep reports no stale owning definition under `softpc`. |
| S3 | S2 is closed; the ledger's two redirection rows and every COMMAND stream consumer are rechecked against current paths. | `mvdm_command_redirection.{h,c}` has one `redir` implementation/declaration owner; pipe/standard-stream fixture behavior and failure direction are unchanged; all COMMAND consumers and formal source lists resolve to `redir` or a listed temporary forwarder; the sweep finds no second body. |
| S4 | S3 is closed; the four BaseVDM record/native-child rows are re-audited for record layout, child launch, redirection adjacency and source-list consumers. | The two header/body pairs have only `basesrv` owners; binary record layout, ownership, error/failure result and native-child launch semantics are unchanged; command-native-child and BaseVDM fixtures pass; both formal product source graphs contain one body per symbol and no stale `softpc` definition. |
| S5 | S2--S4 are closed. Evidence inventories every function/static-state item, direct/transitive caller, environment/report variable, source-list edge, and `error_abi` include/import edge. | No production source changes. Each item has an exact candidate owner or explicit blocked boundary; session/app/machine/service/product subsets are not conflated; `monitor`/`win32` are rejected as blanket destinations; candidate `opennt-abi` is source/include/build reviewed. |
| S6 | S5 is closed with a complete symbol map; only session termination/origin and app environment-capture subsets are approved. | `session` owns the termination conversion/origin state; `app` owns only startup environment capture/removal; guest environment, completion result, signatures and default-off behavior are unchanged; no duplicate state/body. |
| S7 | S5/S6 are closed and the machine-observation subset has a complete CCPU/SoftPC caller map. | CPU/keyboard/PIC/SAS/memory observations are in named `softpc` machine-observation paths, retain default-off output/caller ABI, and prove no machine-state or execution change; one body per symbol. |
| S8 | S5 is closed; each service-local observation has an owner and S3/S4 established relevant destination paths; `error_abi.h` has a reviewed `opennt-abi` contract. | COMMAND/DEM/BOP observations move only with actual owner; `error_abi.h` has one `opennt-abi` owner with unchanged spelling/layout/force-include behavior; product-container probes have a proven owner or explicit deferred boundary; no generic logging family or duplicate body. |
| S9 | S2--S8 are closed and the ledger is refreshed against the actual graph. | Zero unclassified or dual-owner items after function-level decomposition; forwarding files removed; `softpc` contains only approved machine/resource-bridge and machine-observation paths; x86/x64 comparisons and all S2--S8 fixtures pass without a behavior, BOP, media or runtime-claim change. |

## Exclusions

This package does not admit DPMI32 or WOW32 source changes, new BOP behavior,
WOW provider runtime success, guest-media work, CPU changes, external source,
or an unconstrained `compat`/`common` family.  DPMI32 and WOW32 recovery begins
only after this migration closes.

## Acceptance

Closure requires a zero-unclassified full ledger, no unapproved non-machine
body under `adapter-mvdm-host-out/softpc`, no dual implementation or lingering
forwarding file, a reviewed build-input comparison, focused ABI/negative tests, and
architecture documentation that names every final family and its dependency
direction.
