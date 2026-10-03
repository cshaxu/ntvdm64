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
| S2 | One channel-based frontend I/O owner and pending handoff, replacing DOS/native slots and wrappers while retaining final-paint, input return, release/resume and cancellation barriers. |
| S3 | Operation-based publication/snapshot/locking and common worker clients. Replace type-only checks with explicit operation contracts; retain original Console/VGA production and native hidden Console capture. |
| S4 | Whole-package source/contract/dependency audit, duplicate removal review and retained acceptance; stop for owner T verification. |

Every production P builds affected x86 /MT CCPU40 closure, retains focused
negative/lifecycle tests and prior frontiers, Console17/Window17, native EDIT/
nested return/isolation/RPC/version/WOW gates, recoverable eight-file
publication and reviewed commit/push. Partial implementations or unused
wrappers do not close a stage. Reuse validated incremental cache by input
identity; new outputs only build/M0-T425/S<n>/r001.

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

- [ ] Complete function/operation provenance, caller and owner inventory,
  including project additions inside mirrors without moving original logic.
- [ ] S1: Console absolute cells, Window relative samples, buttons and
  ENTER/LEAVE/reset; no frontend kind conversion. Test accumulation, bounds,
  geometry changes, ordered release and stale-source rejection.
- [ ] S1: physical keys, text/dead-key/layout, returned keys and held-key reset;
  no duplicate characters or missing scan/modifier state.
- [ ] S2: both handoff directions, committed final state before acquisition,
  input return before parent output, stop/disconnect/stale owner; remove slots.
- [ ] S3: snapshot/publication validation, atomic tiled commit, rollback and
  resource release; no half-frame or late-owner overwrite; remove old paths.
- [ ] Real worker clients wired through existing common/worker-base; original
  NTVDM algorithms never adapted to NTVWM convenience.
- [ ] Retained production gates, exact release/recovery hashes, mirror/library
  review, governance/links and reviewed sequential P delivery.
- [ ] S4 requirement audit and explicit remaining physical boundaries; owner
  verification before T closure.

## RDP boundary

Shared kvm-window checks capture ownership and clip bounds before delivering
motion. This precedes worker conversion, so branch removal alone is not proof
of an RDP release repair. Identify the failed predicate with low-perturbation
evidence; retain safety checks. Imported-library edits need separate approval.
Both workers already use the common text renderer; worker frame/font/extent
differences are excluded for now. Do not claim physical acceptance from mocks.
