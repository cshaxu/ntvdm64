# T435 S1 — Windows 1.01 mouse source audit

## Admission and confidence

Owner admits the Windows 1.01 candidate and confirms EGA has passed. S1 is
source/design only: no production edit, runtime rerun or new mouse success is
claimed. Current installed baseline is T434 APP434/RPC45/I/O25. Earlier
RPC42 driver traces remain supporting diagnostics, not current-package proof.
Owner also authorizes reviewing/committing all pending planning documents.

## Sources and procedure

Read source policy and the existing Windows 1.01 proposal/diagnostic ledger.
Read current base/keymouse/mouse.c, its mouse.h/ica.h declarations, NTVDM
mouse bridge/guest adapter, and DoMouseInterrupt/MouseEoiHook. Search all
production C/C++ for mouse_send calls. Compare current mouse.c SHA256 with
both pinned OpenNT mirrors; all three are byte-identical. The finite burst is
therefore inherited original host behavior, not a project-introduced mirror
diff. Read the accepted SoftPC T85 S1 closure and correction ee62ad01 reviewed
by9d102563. Its mouse.c has no later committed changes through current HEAD.
The SoftPC tree is read-only comparison, never a build/runtime dependency.

Read-only hashes of O:/Windows/MOUSE.DRV and WIN100.BIN still match the
proposal's Mouse6.24 and EGA installation hashes. No installation is changed.

## Confirmed current-source gaps

| Boundary | Current behavior | Required repair |
| --- | --- | --- |
| InPort timer/interrupt mode | mouse.c mode1–4 decrements loadsainterrupts from5 and requests100 IRQs in a burst; no rate-specific periodic quick event. | Adopt matching SoftPC periodic30/50/100/200Hz mechanism, enable/HOLD/reset/cancel gates, not a Windows-version heuristic or extra burst. |
| Data IRQ | mouse_send accumulates/clamps signed deltas and updates buttons but currently raises IRQ unconditionally. | Adopt matching device mode/data-enable/HOLD gating with the same controller. |
| Frontend input into hardware | mvdm_softpc_mouse_submit stores its INT33-oriented queue and calls DoMouseInterrupt. No production caller invokes mouse_send. | Restore actual relative movement/button delivery into the device owner, on correct CPU/PIC ownership; retain INT33 and avoid duplicate movement/IRQ. |
| Coordinate domain | Existing bridge scales movement with INT33 VirtualX/VirtualY and can reject motion when either is unavailable. | Hardware motion cannot blindly reuse those scaled values or depend on an INT33 client; audit raw-to-hardware units separately. |
| Interrupt acknowledgement | DoMouseInterrupt/MouseEoiHook use existing pending/EOI/lazy/delay logic on AT slave adapter1/input1. | Verify PIC mask, IRQ9/IRQ2 vector compatibility, acknowledgement and quick-event CPU delivery before declaring initialization fixed. |

Prior real driver observation identifies the InPort chip successfully, then
fails its enabled/disabled timer IRQ detection and falls back to COM2/COM1,
where original synchronous open-error dialogs block startup. The initial
counter snapshot was0, not a complete count for every attempted IRQ. The
current source explains missing faithful timer and hardware input delivery;
it does not independently prove timer alone is the entire detection cause.
Input delivery is a separate post-initialization movement/button gap.

## Minimal repair design and proof

Keep NTCON's worker-neutral input protocol and NTSRV authority unchanged.
Use only the already accepted matching SoftPC device correction, with original
mouse.c owner/path and registered DIVERGENCE/README accounting; exclude its
snapshot subsystem and any absent fake8255 path. Do not copy its complete
mouse.c or create a second controller. Project-added routing/unit/thread/queue
adaptation remains NTVDM-owned; no guest-version or filename predicate.

Before implementation finalize quick-event lifecycle/reset ordering, PIC/vector
and movement-unit/INT33 coexistence design. Test selected production device
identity/index, signed movement/buttons, HOLD, both IRQ enables, four timer
rates, disable/reset/teardown and no duplicate input. Then prove original
Mouse6.24 IRQ detection without COM dialogs/Ignore, actual Win1.01 movement,
menu clicks/releases and retained EGA output. Regress EDIT/INT33, DOS graphics,
Console/Window and DOS/native handoff. Only Z: for owned short-path tests.

The reported Invalid handle on Windows Close is not established as the same
cause; retain an independent exit/resource investigation and normal return/
restart regression. No Sleep, forced guest detection or driver patch is a
repair. S1 audit is preliminary until the PIC/quick-event boundary review is
complete; no mouse implementation or runtime closure is claimed.
