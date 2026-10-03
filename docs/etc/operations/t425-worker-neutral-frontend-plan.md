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
policy, polling cleanup, guest/mirror/imported-library edit or worker font/
extent repair is admitted. T closure requires owner acceptance.

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
