# M0 T460 S3 — PIC-reset EOI repair plan

## Question and admission

The owner admitted this high-priority independent repair into active S3 on
2026-10-25.  The question is whether an ICW1 PIC reinitialization that clears
the emulated IRR/ISR/count state must also retire device-side EOI ownership,
without changing a guest or importing any other Win95 experiment.

The owner-provided read-only `win95-ntvdm-research` evidence bundle supplies
`candidate.patch`, `mvdm_softpc_pic_reset_eoi.c`,
`pic-eoi-probe-matrix.json`, `eoipic-reproduction.json`, and `report.md`.
They demonstrate an IRQ0/IRR symptom and a candidate result only.  They are
not source or build inputs, and are insufficient to establish ISR semantics.
The original Win95 corpus image is explicitly out of scope and remains unread.

## Current source contracts

`softpc.new/base/system/ica.c::SWPIC_outb` handles ICW1 by clearing one
adapter's `ica_irr`, `ica_isr`, `ica_count[]`, and CPU-interrupt state before
recording the next ICW sequence.  The selected original `SWPIC_clear_int`
does something narrower: when it withdraws a bit that is still in IRR, it
calls `host_EOI_hook(irq, -1)`.  `nt_eoi.c` defines negative call count as
“a pending interrupt is being canceled”; the hook is the existing owner of
device-side cleanup, not a project replacement.

The first audit confirms four distinct callback behaviors:

| Device / IRQ | normal callback | negative cancellation currently means | open reset question |
| --- | --- | --- | --- |
| Timer / 0 | regulates `EoiPending` and delayed ticks | clears `EoiPending` and queued backlog | required for the witnessed stranded timer path |
| Keyboard / 1 | retires output and may queue next byte | no separate branch; follows the original EOI path | whether reset may requeue output during ICW sequencing |
| RTC / 8 | clears `RtcEoiPending` and may queue register-C state | no separate branch; can requeue | cascade and requeue ordering |
| Mouse / 9 | releases mouse EOI state or schedules more input | flushes mouse events and clears state | whether a reset must discard, rather than merely release, queued input |

`host_ica_lock` is the shared critical section used by hardware event paths;
the candidate's callback-under-lock shape is therefore not enough by itself:
the audit must prove that ICW1 may safely invoke every registered callback
while holding it and that callback-raised work cannot become a stale request.

## Bounded decision

An ICW1 retirement applies to both IRR and ISR bits of the selected
controller.  An IRR bit has an undelivered device request; an ISR bit has an
already-delivered request whose device-side EOI gate otherwise has no future
PIC EOI to release it.  In both cases the pre-reset ownership has ended.  The
existing `host_EOI_hook(irq, -1)` is the only selected host cancellation
contract; no device state is copied into the PIC or repaired directly.

The change snapshots the selected controller's `IRR | ISR` under the existing
recursive ICA lock, performs the unchanged ICW1 reset, and sends exactly one
negative callback for each snapshotted bit.  The callback may queue a new
hardware request through the original route.  That later request is not in
the snapshot and remains owned by the post-reset controller.  A slave reset
does not cancel the master cascade bit, because that is master-owned ISR
state; a later master reset retires it.  AEOI has no retained ISR bit and so
does not manufacture a cancellation.

This is an owner-approved, source-proven original-host correction under the
source policy's canonical-source-ownership clarification.  The pinned SoftPC
comparison has the same original ICW1 omission, so it is provenance evidence,
not a matching correction to import.  Rejected alternatives are a Win95
special case, direct timer/device-state clearing, synthetic IRQ injection,
calling cancellation after unlocking, cross-controller cascade cleanup, and
using `ica_reset_interrupt_state` (which has the wrong all-controller/
IRR-only shape).

## Evidence plan and current results

1. Compare the selected original ICW1/reset and EOI paths with the pinned
   SoftPC source.  Establish whether this is an inherited host defect and
   register the owner-approved same-shaped exception, including rejected
   candidate alternatives.
2. Trace every registered EOI callback and its queue/lock behavior.  Distinguish
   a request merely latched in IRR from one already moved to ISR; do not treat
   `IRR | ISR` as correct until each device and cascade effect is proved.
3. Add a focused, production-owner fixture with controllable callbacks and
   actual `SWPIC_outb`/`SWPIC_clear_int`/INTACK calls.  It must cover pending,
   in-service, empty, master/slave, repeated ICW1, automatic EOI, callbacks
   that requeue, and callback failure/duplicate prevention.
4. Only after that matrix selects one contract, make the smallest local
   original-shaped change.  It must not reset timer variables, inject IRQs,
   special-case Win95, or substitute a success result.
5. Exercise the provided independent DOS timer probe/control through the
   project runner if its immutable source/input contract is admissible; then
   run related noninteractive DOS timing/input, protected-mode transition,
   WOW and Win3.x start/run/exit/restart checks.  Interactive-only cases are
   recorded as non-passes, never simulated.

The source-level fixture now links the selected production `ica.obj` and
`timer.obj`, not a copied PIC body.  Its 2026-10-25 result is
`PIC_RESET_EOI assertions=14 failures=0`: pending+in-service master state,
actual `TimerEoiHook` cancellation of `EoiPending/EoiIntsPending`, empty and
repeated reset, controller-local master/slave state, one callback-requeued
IRQ, and AEOI all pass.  The existing selected original keyboard/PIC fixture
also passes (`KEYBOARD_PIC_PROFILE assertions=46 failures=0`), including the
new contract that PIC-reset cancellation drops an unread keyboard output slot
rather than treating the negative cancellation callback as a normal early EOI.
The supplied
independent DOS probe remains the retained baseline/fix/control evidence:
its baseline has zero ticks with `eoi_pending=1`; its candidate and no-reset
control continue ticking.  It has not yet been re-run against this candidate
package, and neither RTC nor mouse has a dedicated reset-cancel fixture;
therefore these results do not yet qualify the change for default publication.

The current formal worker and native graphs were regenerated and fully built
after the final keyboard cancellation change in
`build/t460-s3/full-component-r6`: ten coherent images build and the second
Ninja pass rebuilds no intermediate.  The PIC fixture passes again from that
final selected-object graph (`PIC_RESET_EOI assertions=14 failures=0`), and
the keyboard/PIC fixture passes with 46 assertions.  The
separate formal WOW32 graph was built independently and its two static gates
pass: `WOW32_S1_FORMAL_GRAPH_OK` (77 original sources, resource and bounded
bindings) and `WOW_USER_INPUT_CALLBACK_ABI_21_OK` (all 21 original callback
slots).  These are compilation/composition results only.  They do not replace
the missing real DOS candidate run, RTC/mouse reset-cancel witnesses, or the
separately blocked public-Clipboard acceptance; no deployed runtime file has been
replaced and no publication claim is made.

An additional S3 audit separated a superficially similar keyboard case from
the PIC contract.  A normal early EOI clears the PIC ISR before a later ICW1;
the unread port-60 byte is then controller state, not an IRR/ISR-owned device
request.  ICW1 must not discard that byte merely because it resets the PIC.
Conversely, when ICW1 snapshots a still-pending IRQ1 IRR/ISR bit, its existing
negative callback remains the correct cancellation route and the focused
46-assertion keyboard fixture proves that it drops the old output slot rather
than treating cancellation as a normal EOI.  This preserves the bounded
`IRR | ISR` rule without inventing a cross-device reset policy.

## State authority and stop boundary

The PIC alone owns IRR/ISR/count/sequence; each device owns its EOI-pending
state; `host_EOI_hook` is a synchronous notification boundary.  No adapter
may duplicate either state.  A callback may only create a new request through
the original hardware-IRQ route; it cannot restore a canceled old request.

Stop and retain a candidate if the selected original source does not establish
the ISR contract, an existing matching correction cannot be attributed, a
callback requires a new lifecycle owner, or the real noninteractive regression
matrix cannot be run.  A passing Win95 boot is neither required nor accepted
as proof.

## 2026-10-25 owner-directed package publication

The owner directed that the completed PIC and keyboard shared corrections be
published before the separate Clipboard investigation continued.  The coherent
`package-r20-shared-clipboard` ten-image package was copied to both
`assets/release` and `O:\winnt\system32`; the candidate, release asset and
installed-file manifests were compared by SHA-256 after replacement and all
ten identities matched.  Recovery copies for both destinations are retained
under `build/t460-s3/publish-r21-pic-keyboard/recovery`.

This publication does not upgrade the unrun DOS-timer, RTC/mouse or broad
interactive acceptance cases to passes.  It publishes exactly the
source-reviewed PIC and keyboard corrections backed by the 14-assertion PIC
and 46-assertion keyboard/PIC production-object fixtures; the remaining
boundaries remain explicitly limited as recorded above.

## Subsequent actual DOS IRQ0 control and reset

`final-pic-combined-r1` subsequently passes both the fourteen production-PIC
assertions and actual DOS IRQ0 control/reinitialization using the coherent
`final-reviewed-package-r1`. The runner is
`tests/observation/verify-pic-reset-eoi.ps1 -OutputRoot
build/t460-s3/final-pic-combined-r1 -PackageRoot
build/t460-s3/final-reviewed-package-r1 -Observer
build/.ninja/msvc-x64-release/observation/console-startup-observer.exe`.

The independently authored `pic_reset_eoi_guest.asm` derives from the owner's
read-only `eoipic.asm` research diagnostic (SHA-256
`A490074764DCBE9A6EE466698FDF933968278931FE7435E36E566D2467C454F8`).
It waits for real IRQ0 delivery, then an actual pending IRR bit with interrupts
disabled, optionally initializes the master PIC through its original BIOS
vector/cascade configuration, and measures further real delivered ticks.
Unlike the research infinite hold it restores INT 08 and exits DOS normally.
It injects no IRQ, clears no host timer variable, and changes no original guest.
The retained original baseline/fix/no-reset reports are not overwritten.

Probe source SHA-256:
`8EE812ED568987CC09C79E70BDE174F319330EB960A41794FF6D3E0967E493B6`;
COM SHA-256:
`EC00E225DA3D2B6D99DD8F7135EC2BE155E5ED6C362791F92645F45ECCB27F87`.
Both runs report `PIC-REAL-IRQ0-PASS`, original task exit zero, and the fixture
still reports `PIC_RESET_EOI assertions=14 failures=0`. The runner retains
actual runtime/observer hashes and uses the established owned short stage.

This closes the unrun candidate DOS IRQ0 gap above, not dedicated RTC/mouse
reset-cancellation or interactive DOS/WOW/Win3.x acceptance. Those remain
unverified boundaries; read-only window creation is not input/gameplay or
normal application exit acceptance.
