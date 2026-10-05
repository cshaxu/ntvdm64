# T429 S6 shared worker publication

## Question and admission

Owner approves both NTVDM and NTVWM adopting the S5 publisher mechanism from
worker-base. CURRENT owns the active S6 packet. This record starts as a
source review and implementation checklist, not production/runtime proof.
S5 P903d25d8a and its sealed r019 runtime remain the source/product baseline;
S5 r028/r029/r018/r017/r021 retain full and published verification. The side
session's proposal edits remain untouched. No production source is changed
by this admission delivery.

## Reviewed sources and extraction judgments

| Mechanism | Current source and provenance | S6 disposition |
| --- | --- | --- |
| Copied latest-state thread, complete comparison,50Hz one-shot cap, drain/failure/join | ntvdm-exe/win32/console_video_publisher.[ch]; entirely project-authored S5 | Move/adapt to worker-base/video_publisher.[ch]; one implementation, explicit instance, immutable copied snapshot and owner-specific send callback. |
| VGA extraction, calc_update and software mouse/VGA algorithms | Original mvdm owners plus registered NTVDM adapters | Keep at original/NTVDM owners; no source relocation or additional mirror hook. |
| Hidden Console acquisition and Unicode-to-PC text frame | ntvwm-exe/console_state.c, text_frame.c and presentation.c; native worker boundary | Keep acquisition/conversion native; about30ms sampling remains. |
| Native full-publication unchanged comparison | presentation.c::ntvwm_presentation_capture; project-added producer cache | Replace its full snapshot comparison with shared complete-state comparison, including native Unicode cells and geometry/cursor metadata, not just glyph bytes. |
| Native publication transaction | capture: PUBLICATION_BEGIN, geometry/grid/cursor, text frame, PUBLICATION_END/ABORT | Preserve as one owner-specific commit callback consuming only an immutable snapshot; do not enqueue just its final video packet. |
| Native changed-row optimization | publication callback's Unicode grid writes | Remains native commit detail where required to preserve current logical Console semantics; not a second full-frame send filter. |
| Final release and input return | NTVDM pause/final paint; NTVWM presentation_end/broker release | Keep owner boundaries/order. Shared drain must complete before the existing final acknowledgement and unused-input return. |
| Common pipe protocol and frontend renderer | common/console/client and NTCON | Unchanged; not the home of worker publisher state or producer dedup. |

The applicable recovery ladder first reuses the existing project-authored S5
mechanism. Original OpenNT/VGA operations remain composed at their current
owners. No historical scheduler is extracted, no new mirror intrusion is
needed, and no new alternative engine is authored. The callback represents
worker-specific output operations, not a kind-aware policy in worker-base.

## Critical native ordering and ownership

Source inspection of presentation.c shows capture currently commits Unicode
cells, buffer/window geometry, cursor and text metadata in one transaction.
Its final frame alone is insufficient: two Unicode grids can map to the same
PC glyphs while remaining different logical Console states. The shared
publisher's immutable comparison unit therefore must encompass the complete
native publication, without changing the cross-process protocol. Owner-private
snapshot packing is allowed; native handles, live capture buffers, guest
memory and mutable font/mouse pointers must not reach the sender thread.

The shared implementation owns pending and last-successful copies, event,
one-shot timer and sender lifetime. It owns no Console, VGA state, task,
frontend association or broker authority. Only the latest unsent snapshot
remains; claimed sends complete or fail before drain. No shared is_dos/native
switch is planned. Native acquisition and NTVDM extraction retain their own
producer semantics.

NTVWM presentation_end currently holds its recursive channel lock around
capture, input return and barrier. After asynchronous publication, waiting for
drain while holding that lock would deadlock the sender. Quiesce/drain/join
must occur outside transport/capture locks, before acquiring the lock for
ordered final commit/input return. Cancellation must remain valid after channel
reopen; close must join before freeing callback context or channel resources.
Failure is sticky and cannot advance the last-successful snapshot. Resize
retry must preserve the existing explicit failure/final-state contract.

## Implementation and verification checklist

- Extract the existing S5 engine, update NTVDM adapter references and build
  ownership, and delete the private publisher implementation.
- Split NTVWM immutable acquisition from atomic commit; connect the same
  publisher instance mechanism and remove its replaced full-frame comparison.
- Preserve explicit control/title/geometry and final-state order; retain native
  acquisition timing, Unicode data and mouse-only updates.
- Extend production-linked tests for both actual callers, full-copy equality,
  replacement, failures, blocked sender plus release, resume/reopen and exact
  resource ownership. Native fixtures supplement, not replace real programs.
- Build the affected x86 closure, stage the complete eight-file package, then
  verify retained Console17/Window17/WOW frontiers, real EDIT200/native EDIT,
  nested DOS/native handoff, outer cooked return, close/fault/session isolation.
- Preserve recovery, publish the exact verified package to O:/winnt/system32,
  check all eight hashes and deployed smoke; run governance/link/diff review,
  commit and push. Owner physical/RDP and T429 acceptance remain separate.

There are no S6 build, test or publication results yet. No one-minute target,
native sampling removal, speedup or runtime closure is claimed by admission.
