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

## Implementation and focused evidence

The private console_video_publisher.c/h is removed. worker-base/publication.c/h
now owns the copied opaque state engine used by both production workers.
NTVDM packs its description/payload privately. NTVWM packs Unicode cells,
Console geometry/cursor, title and composed text data, then commits the whole
transaction in its private callback. Its replaced captured_description/payload
full-state comparison is removed. Changed-row writes remain commit detail.
Neither callback reads mutable VGA or Console state; shared code has no worker
type branch. End disables/drains outside the channel lock, then captures and
commits final state synchronously before unused input and barrier. Normal close
follows drain; abnormal membership close signals the borrowed transport stop
before join. Original mirrors and the frontend/wire remain unchanged.

- x86 affected build: build/M0-T429/S6-build.log through S6-build5.log,
  generated formal cache M0-T427/S2/r001, both workers plus VDMREDIR and fixtures.
- tests/observation/verify-software-video-publisher.ps1:
  S6/r007/results.txt passes latest-of200,20ms, unchanged idle, palette-only,
  final drain, resume, sticky failure including rejected reactivation,
  quiesced commit equality and50 handle-clean stop/join cycles.
- Production-linked ntvwm-presentation-test.exe: S6/r005/native.txt,
  checks=500 failures=0. Real begin/async capture, transaction-END event,
  distinct Unicode with identical fallback glyph, unchanged idle and pending
  cursor drain are added without removing prior geometry/input/abort assertions.
  S6/r003 and r004 failures are retained: the new fixture initially failed to
  update its peer cursor projection and expected the old one-publication count.
  Those test-model mistakes are corrected explicitly; production unchanged.
- S6/r008 via S6/focused.ps1 reuses Verify-CommandExitStatus.ps1: Console4,
  Window4 and actual EDIT200 pass, with real input-sink acknowledgement.
- S6/r011 VGA copy/route and production video dispatcher fixtures pass.
- S6-ownership.log passes actual graph link ownership for both workers.

## Delivery and final review

- S6/r009/product/timings.json: Product passes in198824ms; retained WOW
  frontiers66841ms, Console17=61663ms and Window17=64998ms. Entrypoint is
  tools/audit/Invoke-ProductVerification.ps1 -Suite Product, RuntimeRoot
  S6/r006/runtime, BuildCache M0-T427/S2/r001, original observers from
  T427/S4/r049 and original G7.COM from T425/S9/r008. Baseline frontiers are
  S5/r028/product and S3/r009; no assertions or cases removed.
- S6/r010/results.json: all8 integration cases pass (108079ms case total):
  native and nested Console/Window via verify-broker-io-handoff.ps1 (real23),
  verify-command-native-edit-return.ps1, verify-frontend-relaunch.ps1 (cooked19),
  and verify-ntvwm-management.ps1 frontend-close/worker-loss/two-sessions.
  Unexpected native worker death returns1067 while the actual target survives.
- S6/r012/input-return.txt: production native final capture, key prepend order
  and empty native input queue pass, checks=689 failures=0.
- Final S6/r016/native.txt passes501 assertions, explicitly confirming that
  the distinct Unicode changes share one PC fallback glyph while both still
  publish their Unicode-grid transaction. This strengthens the test only;
  the verified/published eight-file runtime is unchanged.
- S6/r013/published-manifest.json pins all8 staged/published files; recovery
  is r013/recovery/system32. Publication source is the exact Product and
  integration-verified S6/r006/runtime, installed at O:/winnt/system32.
- S6/r015/verified-published-manifest.json and S6-smoke2.log pass actual
  published Console4/Window4 and all8 hashes. Initial r014 smoke refused a
  reused S5 log prefix before any case ran; retained S6-smoke.log records it.
  A fresh r015 prefix fixes only evidence naming, without a product change.

Published NTVDM SHA256:
055F2F07CD571826F90A21ED7FD90C2FF5A1BF6695ECE030F69B816307AD12CC.
Published NTVWM SHA256:
EAEDD0FA2A8E5D18D0F54C5201CAFCA4E04E1C1A068FE48B132661DA1C7FB82D.
VDMREDIR is relinked; WOW32 remains the sealed original-provider input.
Graph regeneration/relink identity is captured for every artifact rather than
assuming untouched component sources imply unchanged PE hashes.

Final review: shared publisher admits immutable complete copies; full equality
and success ownership live only there. Native Unicode dirty chunks remain
private transaction optimization, not a second full-send filter. Transport
callbacks execute outside shared state locks; owner drain/join has no transport
lock dependency. Native acquisition/end are serialized by their existing
membership owner; callback never takes that membership lock. Error is sticky,
reactivation cannot revive a failed sender, and borrowed cancellation remains
live until joined teardown. Both entries link worker-base.lib with the actual
publication object. Original mirror code, protocol, frontend, guest clock and
broker lifecycle have no new diff. src/mvdm/README.md only records relocation.
The unrelated side-session proposal remains untouched at its original hash.

Governance, relative links, graph ownership and git diff --check pass. S6
reaches bounded delivery; T429 remains open for owner acceptance. Physical
desktop/RDP latency remains unobserved/waived, native30ms acquisition remains,
and no speedup or one-minute regression target is claimed. S5's retained raw
failure/causal limitations are not erased or reclassified by this delivery.
