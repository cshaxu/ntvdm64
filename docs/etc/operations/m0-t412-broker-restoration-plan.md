# T412 broker restoration working plan

## Authority and scope

Owner admission on 2026-09-13 covers the queue-head broker package and asks
for S decomposition first. [Status](../../states/CURRENT.md) alone owns the
active packet. S1 is active; S2--S6 are planned, not completed or simultaneous
active packets. The [single broker proposal](../../proposals/proposal-cross-process-broker-closure-001.md)
retains the detailed requirements; this plan supersedes its provisional stage
grouping without creating another proposal.

Restore D01--D07/D10 as one original BaseSrv/BaseClient lifecycle: command
selection, PIF, first-VDM state, current directories, exit, reentry, client
capture/wait/retry and image classification. Include CheckVDM, UpdateVDMEntry,
registration, dispatch, rollback and release, not isolated function bodies.
Original service policy remains in opennt-host; original MVDM behavior stays
in mvdm-host. Adapters/broker bind finite unavailable interfaces, not duplicate
those state machines. App owns composition.

## Sequential packets

| S | Work | Exit evidence |
| --- | --- | --- |
| S1 — Baseline audit and design | Freeze source/artifact/provider lineage; inventory scoped mirror diffs, overlays and autonomous policy against original owners. Design process state, wire/resource contracts, Console identity and launch reservation. | D01--D07/D10 disposition ledger; measured removable and retained lines by category; finite boundary and test matrix; prerequisite disposition; executable naming/deployment migration design. Report before implementation. |
| S2 — Original BaseSrv/BaseClient recovery | Restore the coherent selected original lifecycle including capacity, ownership, wait/retry, errors and cleanup; finite same-shaped bindings only. | Original selected bodies compile/link on x86 and pass focused positive/negative tests. A fixture duplicating provider policy is not proof. Track transitional harnesses for integration/removal. |
| S3 — Three programs and IPC | Compose run16.exe, basesrv.exe and ntvdm.exe; authenticate endpoint, reserve launch, let launcher create/register requested worker, dispatch and clean disconnects. Preserve original worker entry and consistent OS/SoftPC command-line facts. | Formal three-program build/provider maps; concurrent startup/registration and failure tests; copied versioned messages and explicit resource ownership. Update publication rules before deploying renamed products. |
| S4 — DOS execution and streams | Connect BOP 54:08 to run16, original DOS reentry/native-child waits, and asynchronous command/environment/stream capture. | Real COMMAND/MEM/EDIT and DOS/native-child runs; spaces, environment, stdin/stdout/stderr, EOF, exit codes, rollback and disconnect. Explicit direct CLI versus guest COMMAND redirection tests; no invented generic cmd parser. |
| S5 — WOW coordination and idle cleanup | Restore broker-side WOW acquisition/PIF/notification/waits/failures; singleton, eligible-idle retirement and empty-broker exit. | Source-owned coordination tests and reachable real-worker evidence; simultaneous startup, reconnect, new work versus retirement, restart/first-VDM and exact-once cleanup. Quiet active COMMAND/EDIT must not be reaped. |
| S6 — Integrated verification and removal | Build/test final composition, remove superseded autonomous policy and obsolete bridges, reconcile registers and publish verified products. | Final per-row disposition; measured mirror diff/overlay/autonomous line deltas; real-program and process-race matrix; no mandatory unproved contract; clean committed/pushed worktree and indexed closure evidence. |

## Required S1 decisions

- run16 validates/classifies, requests service selection, creates a worker only
  when requested and registers it. basesrv owns the original service policy;
  ntvdm owns one original CCPU40 machine. Do not substitute a new broker policy
  first and promise original-source recovery later.
- Reconcile the current ntvdm32.exe-only publication rules with three-program
  naming before changing build/deployment. Admission itself replaces no EXE.
- Specify per-user/session singleton identity, endpoint authorization, message
  version/length validation, stable IDs, process-local resource duplication,
  reservation expiry and exact-once completion. No native/guest pointer or
  local HANDLE value crosses IPC.
- Default idle eligibility is approximately 60 seconds after completed work,
  not 60 seconds without mouse/keyboard input. Assigned/queued tasks, launch
  reservations, active guests, parent waits/reentry, Console leases and pending
  cleanup prevent retirement. Define cooperative exit and timeout policy.
  Empty-broker exit must account for in-flight clients and startup/arrival races.
- Preserve service-instance first-VDM semantics and existing protection against
  mutation of host IO.SYS/MSDOS.SYS. Original-source recovery does not authorize
  host installation changes.
- Capture command tail, environment and stream identities before asynchronous
  guest leases expire. Keep stream lifetime and failure cleanup source-shaped.
- Revalidate U07-B/U08-B dependencies on worker-local U07-T/U08-W evidence.
  Historical closure and isolated binder/stub fixtures do not prove complete
  worker reset/wait/termination or rendering. Explicitly promote a source-proven
  missing direct dependency; do not add a second machine lifecycle to broker.

## Acceptance discipline

Runtime verification uses the formal Win32/x86 CCPU40 product and records
artifact identity, selected providers, expected/observed outcomes and trace
markers. Build/intermediate outputs stay in build/; real-package observations
stay in O:\ntvdm64\logs. Newly required autonomous semantics need divergence
reasons and exercised traces, not merely compilation success.

Full WRITE application recovery belongs to the later WOW16 package. This does
not waive mandatory WOW broker contracts: missing selected coordination blocks
closure. A tested unsupported result proves only its failure contract, never
successful guest execution. Distinguish source inspection, focused original
provider tests, real-process integration and full application success.

Measure changes against the frozen baseline separately for mirror changed
lines, private overlays and autonomous policy. A relocation is not a semantic
reduction. Zero autonomous policy is an objective, not a promise that modern
IPC requires no new glue. Restore composable original source before finite
same-shaped bindings/private overlays; newly authored behavior is last resort.
Mandatory unresolved work needs an explicit approved disposition, not a silent
transfer to a later package. Preserve unrelated work.
