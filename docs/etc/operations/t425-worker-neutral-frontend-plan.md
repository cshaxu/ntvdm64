# T425 worker-neutral frontend and worker I/O

## Owner direction and baseline

Owner closes T424 and admits this package after approving unified NTCON
frontend mechanisms and worker-base contracts, with local worker adaptation.
Project-added identical frontend logic belongs in NTCON; original MVDM logic
stays original-owned and NTVWM implements its native counterpart against the
same NTVDM-shaped contract. Similar names alone do not justify consolidation.

Baseline: [T424 closure](../../history/m0-t424-component-control-closure.md),
ff50ff588, S12 46abdb554 and its eight-file release. CURRENT alone owns active
admission. No new executable/helper, scheduler, observation graph, lifecycle
policy, polling cleanup, guest/imported-library edit or worker font/extent
repair is admitted. The later owner-approved S8 exception permits registered
project adaptation hooks inside MVDM (DIV-314/318/322), not replacement of
original guest/device/painter algorithms. T closure requires owner acceptance.

## Ordered stages

| Stage | Bounded closure |
| --- | --- |
| S1 | Provenance/contract audit and worker-neutral mouse/keyboard production paths. Remove frontend native-only pointer and key/text kind selection; NTVWM consumes the existing relative/absolute contract locally. |
| S2 | One channel-based frontend I/O owner with independently cancellable pending requests for multiple channels, replacing DOS/native slots and wrappers while retaining final-paint, input return, release/resume and cancellation barriers. Pending is not a single-slot cardinality limit or an execution scheduler. |
| S3 | Operation-based publication/snapshot/locking and common worker clients. Replace type-only checks with explicit operation contracts; retain original Console/VGA production and native hidden Console capture. |
| S4 | Whole-package source/contract/dependency audit, duplicate removal review and retained acceptance; stop for owner T verification. |
| S5 | Owner follow-up: clean misleading DOS-specific names in the shared format decoder and actual callers/tests; preserve TEXT_FRAME/TEXT_CONFIGURATION/DIB processing and all runtime gates. Stop for owner T verification after delivery. |
| S6 | Owner-approved broker-controlled handoff repair: NTSRV owns associations and connection/takeover/release authorization; NTCON has zero or one physical I/O pipe, no pending-owner list. Close and acknowledge the old pipe before granting the next. Preserve original DOS block/resume/reentry; use the same worker-facing contract for NTVWM. |
| S7 | Owner follow-up: suppress unchanged native publication to preserve host cursor blink, and share the finite local character-start gate while WOW/native GUI skip it. Preserve original classifiers and execution; retained full package gates and publication apply. |
| S8 | Owner-superseded dedup proposal: unfiltered explicit frames, correct producer notifications, software VGA FULLSCREEN and natural mouse draw/erase at route edges. Preserve original algorithms, hardware exclusions, final-paint barriers and lifecycle gates. |
| S9 | Owner-approved test audit, simplification and throughput improvement: parallelize isolated checks, remove proved redundant/useless tests, and replace real ten-second policy waits with deterministic clock/event tests while preserving behavior coverage. |

## S9 test audit, simplification and throughput

Owner approval dated 2026-10-03 schedules S9 after S8 delivery. S8 remains the
sole active implementation packet until sequential admission in CURRENT.
This bounded follow-up permits test/fixture and verification-gate improvements,
not a new runtime component, production lifecycle policy or polling cleanup.

- Inventory each test's asserted contract, production path, dependencies,
  elapsed time and wait budget. Identify duplicated scenarios, repeated package
  setup/builds, unconditional grace periods and tests without useful assertions.
- Remove obsolete, assertion-free or redundant tests only with an explicit
  retained-coverage mapping. Preserve meaningful negative, handoff, completion,
  disconnect, independent-session and prior guest-frontier assertions; a slow
  test is not automatically a useless test.
- Run independent units and isolated fixtures concurrently with bounded
  concurrency. Group tests sharing the global BaseSrv endpoint or runtime
  package serially inside that group; parallelize other groups, not competing
  owners of the same endpoint. Do not add runtime brokers/helpers for tests.
- Stop spending ten real seconds repeatedly proving a ten-second retirement
  deadline. Use a controlled clock/deadline input at the existing policy owner
  and explicit events to test before/at expiry, new-work cancellation, rearming
  and cleanup. Keep the production ten-second policy unchanged. Retain focused
  real-process acknowledgement/wiring tests without waiting out the policy
  duration; do not substitute shortened production deadlines or sleeps.
- Replace test-only fixed delays with ready/completion/stop acknowledgements
  and bounded failure timeouts. Clean up fixture-owned resources explicitly
  instead of waiting for normal idle retirement between scenarios. Timeout
  itself is never a passing marker; do not disturb unrelated owner sessions.
- Consolidate test entrypoints and shared setup, reuse build inputs only by
  proved source/toolchain identity, and select focused gates by affected
  contracts. Review the retained full-product gates for actual incremental
  coverage instead of rerunning overlapping suites mechanically. Any gate
  change must be documented with equivalent coverage, not silently waived.
- Record before/after wall-clock time, per-suite timings, parallel groups,
  removed-test dispositions and retained assertions. Repeat the optimized
  schedule to prove stable isolation and handle/process cleanup. S9 closes
  only with a measured reduction in test wait time, no real ten-second policy
  waits as routine tests, and no weakening of product/non-regression evidence.

S9 implementation must update the applicable execution guidance and its own
evidence to match the verified schedule. This planning approval does not claim
the optimization is implemented or change the currently admitted S8 gates.

## S6 connection ownership and implementation boundary

This approval supersedes S2's multi-pending frontend policy, not its retained
historical test evidence. Logical association with several workers lives only
in NTSRV. NTCON stores only its current channel and persistent presentation
state. Each worker stores its current authorized frontend/channel and its own
execution state, not a frontend association tree or acquisition schedule.

The control sequence is broker authorization -> old worker final publication
and input return -> confirmed release -> both endpoints close the old pipe ->
disconnect acknowledgement -> broker-authorized new connection -> incoming
state import acknowledgement -> resumed execution/input. Zero connected
workers is a valid intermediate state. An expected I/O disconnect neither
completes a task nor retires a component; broker loss and genuine faults remain
distinct failures. Pipes carry data and transport acknowledgements, not ownership
arbitration. A parent waiting for a child cannot autonomously reacquire.

Execution determines handoff: original NTVDM stops event input and flushes
final output/returns unused keys before its project release hook. NTSRV confirms
both physical pipe ends closed before granting another connection. Native
Windows children do not expose a shell-out hook to NTVWM; an admitted child or
resume phase therefore supplies the broker notification to finish native I/O.
An acquisition RPC alone must never trigger that notification or authorize
preemption. Existing association state carries the grant; no new lease ticket,
connection generation, worker queue or task scheduler is introduced.

Original NTVDM execution, task completion and blocking/resume code remains in
its mirror, with unchanged hooks into project adapters. Same-worker reentry
must distinguish a new admitted execution phase from the suspended parent,
not infer it from PID. NTVWM retains native process/Console semantics and
adapts the common control contract locally. Future full NTVWM reentry and
NTMON tree display are not silently added to S6. No new helper or component.

S6 is implemented and its complete r015 set is verified and published. The
[S6 ledger](../evidence/m0-t425-s6-broker-io-ownership.md) preserves early
partial/failing iterations, final bidirectional nested return, cancellation,
stale acknowledgements, input ordering, old-pipe closure, independent roots
and retained complete product gates. The containing reviewed P delivers S6;
owner T verification remains required. No next S is automatically invented.

Every production P builds affected x86 /MT CCPU40 closure, retains focused
negative/lifecycle tests and prior frontiers, Console17/Window17, native EDIT/
nested return/isolation/RPC/version/WOW gates, recoverable eight-file
publication and reviewed commit/push. Partial implementations or unused
wrappers do not close a stage. Reuse validated incremental cache by input
identity; new outputs only under declared build/M0-T425/S<n>/r<nnn> run roots.

## Initial source and operation disposition

| Mechanism and current location | Provenance and target | Independent boundary |
| --- | --- | --- |
| Native pointer conversion, NTCON native_console_frontend.c/window_mouse.c | Project-added frontend conversion: remove kind branch; normalize by input source/current frame. | NTVWM accumulates movement and generates native Console events; NTVDM preserves guest coordinates. |
| Key/text translation, NTCON window_keyboard.c | Host layout/held-key/dead-key mechanics: one frontend input path with complete event semantics. | Original DOS scan/character/returned-key behavior remains unchanged; native records consumed locally. |
| Relative bridge, NTVDM softpc/mvdm_softpc_mouse_bridge.c | Project-added backend device-coordinate seam, not frontend normalization. | VirtualX/VirtualY, IRQ and original INT33 reset/set-position/bounds/counters stay NTVDM-local. |
| Native mouse, NTVWM text_frame.c/presentation.c | Local Console interpretation adapts to the same relative samples and absolute cells. | Hidden Console coordinates and cursor composition remain native-owned. |
| DOS/native owner slots, NTCON native_console_frontend.c | Project-added frontend ownership becomes current channel/pending handoff and channel publication state. | Original worker block/resume remains authoritative; no frontend task scheduler/completion. |
| DOS import/native publication, NTCON console_channel.c/native_console_frontend.c | Storage/transaction orchestration becomes operation-based commit/locking, one logical grid. | Original DOS Console/VGA algorithms and native Console capture stay at their owners. |
| Common and worker-base clients | Existing protocol/transport/client mechanisms: audit both actual callers and retain one provider. | No EXE-private dependency or extraction of original execution/device semantics. |

## Checklist and proof

- [x] Complete function/operation provenance, caller and owner inventory,
  including project additions inside mirrors without moving original logic.
  The [S4 audit](../evidence/m0-t425-s4-worker-neutral-audit.md) records the
  complete frontend file/operation families and actual worker callers.
- [x] S1: Console absolute cells, Window relative samples, buttons and
  ENTER/LEAVE/reset; no frontend kind conversion. Test accumulation, bounds,
  geometry changes, ordered release and stale-source rejection.
- [x] S1: physical keys, text/dead-key/layout, returned keys and held-key reset;
  no duplicate characters or missing scan/modifier state.
- [x] S2: both handoff directions, committed final state before acquisition,
  input return before parent output, stop/disconnect/stale owner; remove slots.
  The [S2 ledger](../evidence/m0-t425-s2-channel-owner-handoff.md) retains
  multi-pending FIFO/cancellation, exact publication and regression evidence.
- [x] S3: snapshot/publication validation, atomic tiled commit, rollback and
  resource release; no half-frame or late-owner overwrite; remove old paths.
  The [S3 ledger](../evidence/m0-t425-s3-operation-publication.md) records
  actual failure/pipe tests and the selected r004 published release.
- [x] Real worker clients wired through existing common/worker-base; original
  NTVDM algorithms never adapted to NTVWM convenience.
- [x] Retained production gates, exact release/recovery hashes, mirror/library
  review, governance/links and reviewed sequential P delivery.
- [x] S4 requirement audit and explicit remaining physical boundaries, with
  exact assertion mapping, fresh focused/lifetime/dependency tests and sealed
  release identity review. The containing reviewed P delivers this audit.
- [x] S5 shared format-decoder naming cleanup, affected build and retained
  runtime/publication gates. The [S5 ledger](../evidence/m0-t425-s5-format-decoder-names.md)
  records the containing reviewed P, published eight-file set and limits.
- [x] S6 replaces frontend-local pending arbitration with broker-authorized
  single I/O connection and both-end closure barrier. Full retained gates,
  source/build identity, recoverable eight-file publication and postpublication
  rapid/cooked-CMD/GUI smoke pass; the containing reviewed P delivers S6.
- [x] S7 owner follow-up: suppress unchanged native frame publication and skip
  character startup locally for WOW/native GUI through a shared worker-base
  gate. Classification/execution stay local; no mirror or renderer branch.
  The [S7 ledger](../evidence/m0-t425-s7-idle-cursor-gui-startup.md) records
  focused evidence, retained package gates, coherent publication and reviewed
  P delivery, including the attributed original guest environment limitation.
- [x] S8 owner supersedes suppression: remove shared frame-cache and frontend
  cursor no-op filters; explicit worker events remain unfiltered. Repair
  project-added producer notifications instead of hiding identical events.
  Admit registered DIV-314/318/322 software VGA FULLSCREEN integration without
  changing original device/painter algorithms or hardware exclusions. Connect
  natural mouse route draw/erase before retiring tick compensation; preserve
  native Unicode change detection and all handoff/failure assertions. The
  [S8 ledger](../evidence/m0-t425-s8-worker-frame-deduplication.md) records actual
  frames, source attribution, retained failures and final release gates.
- [x] S9 continued test audit and throughput improvement: coverage-mapped pruning,
  isolated parallel scheduling, deterministic deadline tests and measured
  before/after time reduction without weakening runtime assertions.
  The [S9 ledger](../evidence/m0-t425-s9-test-throughput.md) records measured
  service20 reduction, retained failures, full product regression and coherent
  eight-file publication. Initial P 51f09f7d1 is delivered. Owner continues S9
  for observed input/results, EDIT state waits and cleanup/timing optimization;
  owner closes S9 at 817fe636b with 31-32 percent measured reduction; the
  60-second target and supplemental snapshot-merge failures remain limitations.
- [ ] S10 owner-inserted investigation: second shared-WOW WINMINE launch while
  first task remains live. Locate submission/wakeup/load/startup outcome before
  proposing production repair; no guest patch or new scheduler.
- [ ] Owner verification and acceptance before T closure.

## RDP boundary

S1 implementation and validation are recorded in the
[input ledger](../evidence/m0-t425-s1-worker-neutral-input.md): both worker
clients consume one FRAME_MOUSE contract, frontend input no longer chooses
a worker-specific conversion. S2/S3 ownership/publication consolidation is
delivered; S4 audits its whole-objective coverage before owner acceptance.

Shared kvm-window checks capture ownership and clip bounds before delivering
motion. This precedes worker conversion, so branch removal alone is not proof
of an RDP release repair. Identify the failed predicate with low-perturbation
evidence; retain safety checks. Imported-library edits need separate approval.
Both workers use the common text renderer; worker frame/font/extent
differences are excluded for now. Do not claim physical acceptance from mocks.
