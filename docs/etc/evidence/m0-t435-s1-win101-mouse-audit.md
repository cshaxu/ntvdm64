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

## Superseding owner direction: independent INT33 guest driver

Owner subsequently chooses an independently authored Windows 1.01 mouse
driver using the built-in NTVDM INT33 provider, and explicitly admits
src/ADDON/Mouse Driver 101 as its source destination. Original guest/core/media
remain immutable; a separately built driver in a separate test installation
is authorized. The preceding InPort repair plan remains diagnostic evidence,
not a prerequisite or an implementation admission for this selected route.
No mirror/device/host protocol change has been made.

Read-only retained ega-ports/ega-serial/ega-ignore snapshots all show
INT71=F000:EE00, the built-in MOUSE_INT1 entry, not the original F000:1C2F
BIOS redirect. Their capture point is serial fallback, not a complete history
of the earlier IRQ detection. This proves retained ownership at that failure
checkpoint, not that no disable/re-enable call ever occurred. INT33 points
to C502:23C4. These earlier RPC42 snapshots are not new RPC45 execution proof.

Reproducer: tests/observation/audit-win101-mouse-contract.ps1 reads the
original driver and retained snapshot only. It reports hashes, NE segments,
ordinal entry addresses and IVT; it does not execute or patch either input.
MOUSE.DRV has NE flags8005, automatic DS3, initialization CS1:0 and four fixed
entries in segment2: ordinal1=0000,2=0028,3=00B2,4=010C. From original
disassembly, 1 copies14 capability bytes, 2 accepts a far Windows event
callback and can replace it while enabled, 3 restores its interrupt state,
and 4 is a serial detection routine. In particular ordinal4 is NOT the later
Windows3 enhanced-mode MouseGetIntVect contract; do not inherit that ABI.

Original NTVDM mouse_io.c function11 returns/clears mouse_motion counters.
jump_to_user_subroutine passes that same accumulated counter in SI/DI;
the adapter must not treat repeated SI/DI as independent event deltas.
The candidate uses function11 to obtain/clear the interval movement, with
function20 exchanging the callback. Confirm callback-time nested INT33 use,
button conversion, disable/drain and prior-state restoration in execution.
The original provider remains the sole INT71/PIC/EOI owner. The independent
driver neither installs a hardware vector nor calls function31 to disable
its own provider. It must not request a DOS software pointer over Windows.

The archived Microsoft Windows3 DDK mouse chapter is secondary-version ABI
comparison only: [original Microsoft guide reproduction](https://www.pcjs.org/documents/books/mspl13/win/w3ddkadp/).
It describes flags/movement registers and14-byte MOUSEINFO, consistent with
the observed structure. Win1.01 binary/caller evidence overrides later guide
differences. No original implementation body is copied into the add-on.

Still open before implementation: Win1.01 loader/Setup linking and resident
callback placement, exact relative flag/event bits, accumulated-counter
consumption and units, interrupt stack/reentry, no-provider failure, Windows
disable/return lifecycle and independent Invalid handle investigation. NASM
is available; an existing independently authored NE fixture demonstrates
manual NE generation without historical linker dependency, not successful
Win1.01 driver loading. No build/runtime pass or installation claimed yet.

Further source review confirms a relative-motion boundary requirement:
nt_mouse.c host_os_mouse_pointer clears call-mask bit0 when
bPointerInSamePlace is true. The project's mvdm_softpc_mouse_apply computes
that flag from the confined absolute position after EmulateCoordinates.
Thus the existing callback is not guaranteed for motion beyond an absolute
edge, despite retained raw counts. The independent bridge must exercise a
normal guest-supported relative/recenter strategy and verify its idle/edge
behavior; do not change the provider merely to force callbacks. Function4
recenter is a candidate, not a tested solution. Original callback entry pushes
the provider continuation and restores through mouse_int2; guest bridge RETF
must preserve this contract and must not acknowledge the PIC independently.

The add-on README now carries the concrete candidate enable/disable/state,
NE build and callback contracts plus edge/failure acceptance. S1 remains
open until Win1.01 event semantics/loading and restoration design are settled.

### Actual Windows event consumer — design decision

Read-only cga-old-memory.bin has a mouse capability/state block at11DD0:
FF00020022000200020000000000, enabledFFFF at+12h, and event pointer
1B57:5BC2 at+14h/+16h. Inspecting that pointer's actual saved guest code
with ndisasm via an in-memory stdin stream (no guest write) shows:
5C1E TEST AL,1; 5C25 OR AX,AX; 5C27 JNS5C43. The negative branch multiplies
unsigned BX/CX by the display extents and takes high words. The positive
branch adds relative displacement. At5CFF TEST DL,2 and at5D07 TEST DL,4
select down/up; shifts at5C06/5C08 select the second button's next two bits.
Return at5CFE is RETF; the callback supplies its own first-level Windows stack
switch at5BD1..5BE5. This is actual retained Win1.01 runtime consumer evidence,
not later DDK inference or a new execution pass.

Select absolute events (bit15+movement bit0) from INT33 position, normalized
to unsigned16-bit fractions; retain button transition bits1..4, DX=2.
This removes the proposed recenter/relative-counter mechanism entirely.
No INT33 range, ratio or host clip changes are required. Movement at the
NTVDM absolute boundary correctly maps to the Windows screen boundary.
The native callback's absolute-position branch is substantial evidence for
the design, but new-driver loading and real clicks remain runtime gates.

S1's bounded audit/design now concludes with this selected contract and the
add-on README's state/rollback/load tests. S2 will implement/build/test the
independent guest prototype, including a mock INT33 provider in a disposable
guest harness followed by actual Windows installation/load/interaction.
InPort timer/input gaps remain diagnosed but are not silently counted fixed.
