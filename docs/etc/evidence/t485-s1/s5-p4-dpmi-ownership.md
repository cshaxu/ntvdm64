# S5 P4 DPMI ownership and DOS window/fullscreen verification

## Scope and implementation

The owner reported hangs when a windowed MS-DOS program in enhanced Windows
3.1 or Windows 95 enters fullscreen with Alt+Enter. The prior fullscreen test
did not establish the initial guest window and cannot prove this transition.
P4 precedes migration batch N004; directory delivery remains 39/159.

In dpmiint.c one private DpmiOwnsContext predicate now protects hardware,
software and exception dispatch. It requires existing DOSX LDT/IDT, protected
CPU mode and the existing DOSX LDT shadow. GDT must be the DOSX shadow or
its active WOW page-domain view, verified against the existing owner, source
GDT and current CR3/paging state. Initialization alone is insufficient. Rejection occurs before DOSX
emulation or CPU-state changes and returns to the unchanged CCPU dispatcher.
Hardware and exception paths defer FLAGS changes until stack-frame preparation
succeeds. Nested exception stack rollback remains. The instruction emulator
retains prefix/length/fetch/segment checks and its existing STI semantics;
its duplicate mode check is removed. No new owner state, queue or interrupt
acknowledgement is introduced. nt_inthk.c and original guest binaries are unchanged.

## CAF Window continuity follow-up

The owner accepted the DPMI repair manually, then reported a Window disappearing,
Console receiving the first text frame, and a new Window appearing after
Alt+Enter. In ntcon-exe/window_controller.c, apply() previously required a
valid frame even when display remained FRONTEND_DISPLAY_WINDOW. VIDEO_TEXT
clears the old frame before the following text publication, so clear() destroyed
the Window and routed Console, then the next frame recreated it. The old unit
test explicitly expected that destruction.

The existing controller now keeps an existing explicitly selected Window across
a frame gap. Clear invalidates the frame and its dirty flag, not the user choice.
Explicit Console selection, parking, management return and final destruction
retain their close paths. No new policy state, callback or queue was added.
This autonomous-code change adds no mirror difference. The private-desktop
controller test verifies stable HWND, no source retirement, no Console route,
and subsequent explicit Console closure. It passed.

## Mirror accounting

Fixed XP SP1 blob: 0893ad911182976ad213fba7f9917236cbb741a3.
Prior delivered commit: 6e3e88b53bc57e7f047c9438cae741df5dd14694.
Non-error differences: 40 points / 309 added-plus-deleted lines before;
44 points / 317 lines after: **+4 points / +8 difference lines**.
This is not the source-file length delta. The eight excluded original branch
bodies are paired at XP lines 116, 470, 555, 682, 725, 791, 824 and 969;
conditional frameworks remain counted. Existing #error bodies are unchanged;
no #line anchors were added or removed. Reproduction and pair ranges:
build/t485-s5-p4/measure-final.py and mirror-diff-final.json.

## Test contract

The existing independently authored Win16 startup probe finds the real DOS
window by its PIF title, checks visibility/non-iconic/active state and saves
its HWND and rectangle. The DOS keyboard probe must receive W in that window
before writing FULL.RUN. The observer verifies a graphics frontend, sends
Alt+Enter, observes the fullscreen text marker, uses CAF to select host window
text presentation, returns with Alt+Enter and sends R. RETURN.KEY proves
interaction after returning. It then enters fullscreen again and sends K;
the DOS program terminates, the Win16 probe waits for window retirement and
requests Windows exit. Before return-window input, a guest handshake reactivates
the DOS HWND and confirms GetActiveWindow, rather than treating the first
graphics frame as proof of input focus. The second fullscreen also requires
the same visible KVM HWND. Original COMMAND and Windows binaries are unchanged.
The tests use existing owned private-desktop runners; no Computer Use.

Full now invokes this strict contract for its enhanced Win3.1 and Win95 gates;
standard Win3.1 retains its desktop/normal-exit contract. Failure preserves the
guest handshake files before removing the owned installation copy.

The CCPU monitor fixture now tests the real fault entry rather than bypassing
ownership through direct emulator calls. Its 59 selected cases passed,
including CLI/STI/IRQ behavior, I/O widths and boundaries, rejection across
all three entries, failed stack preparation and preserved CPU state.
Its debug-register tests now use XP's BOOL DpmiSetDebugRegisters(PULONG)
contract. The obsolete no-argument BOP case 116 is retired, not called through
an incompatible signature. Positive and NULL-input API tests remain.

## Current delivery evidence

Cached full build passed. Final package DA93D9FA-CDD9-443F-B67C-0B13ECCC70CF contains
13 host images; assets/release and O:/WINNT/SYSTEM32 hashes match its manifest.
No archive membership or installed guest binary changed.

- fixed2-win31-enhanced: strict roundtrip, DOS interaction and normal exit passed.
- fixed2-win95: strict roundtrip and DOS interaction passed; exit accepted under
  the existing explicitly approved natural-retirement/return-code-one boundary.
- Initial fixed Win3.1 run completed the roundtrip but requested Windows exit
  before DOS window retirement. The test now waits for the real window to
  disappear; this was a test sequencing failure, not a passing shutdown.
- baseline2-win31 on unpatched N003 preserved WINDOW.BIN (14 bytes) and FULL.RUN
  (READY), sent the initial Alt+Enter, then timed out without RETURN.KEY. The
  same initial transition contract passes on P4. This is the retained negative control.
- Owner hand acceptance passed for the DPMI hang repair. The owner separately
  reported transient Console exposure despite CAF Window preference; source
  review identifies ntcon window-controller frame-clear/lifetime coupling.
  That presentation issue is included in the final package described above.
- Full2 reached the Win95 return-window input but did not receive R. It sent R
  as soon as a graphics frame appeared, without confirming DOS focus. The
  guest focus handshake now establishes that missing prerequisite; fixed3-win95
  passed with the strengthened input prerequisite. Full2 is not a pass.
- Final runtime3 Full4 passed all 23 gates in 595,340 ms, including three
  guest desktops, both strict round trips/same-window checks, real WOW windows,
  lifecycle/IPC gates and NESticle rendered menu/Escape interaction.
- Final CCPU/DPMI entry-chain suite passed 59 cases (unit3.log). NTCON
  controller fixture passed. Documentation governance and relative links passed.

All run artifacts are under build/t485-s5-p4. runtime3 is immutable for the
final tests. The build, publication and test launch scripts record owned logs;
Full completion corresponds to runtime3, not the historical N003 run.

## WOW regression caught during Full

Full3 passed all three desktop gates, including both strict round trips and
same-window continuity, but failed WOW startup. N003/current isolated WINMINE
comparison reproduced the regression. The debug child observer recorded worker
exit 120; Run16 subsequently returned 1067. The private diagnostic recorded
GDT changing from 0x124400 to 0x1110000 while the LDT remained 0x1343f8.
Source review traced that change to page_domain_create_wow_gdt and
mvdm_softpc_wow_page_domain_activate_wow_context, not guest VMM takeover.

The unified predicate now recognizes the existing active mapped GDT through
a read-only query at its page-domain owner. It does not recognize WOW by name,
selector or a new flag. An attempted extension of the initialized page-domain fixture was blocked by
its existing XP WOW header/registration ABI compilation errors. That extension
was withdrawn, and the failed run is retained in page-domain/build.log. It is
not counted as passing; final real-WOW gates verify this path. No algorithm
or state machine is copied.
Diagnostic binaries were isolated and were never published. Full3 is failed,
not partial delivery acceptance; runtime3 and Full4 supersede it.
## S5 closure and next packet

This delivery closes S5's N001-N003 and P4 scope. Migration remains 39/159.
The owner-inserted H-series S6-S13 repairs precede N004; old S5 N004-N012
transfers to S14. S6 next owns the selector0040 compatibility boundary in
c_seg.c, per the current repair plan. The optional old page-domain fixture
compilation limitation above is not a product pass and remains for the assigned
WOW repair scope; this delivery's actual WOW package gate passed.

Tracked final evidence: [Full timings](s5-p4-full-timings.json),
[non-error mirror accounting](s5-p4-mirror-diff.json),
[13-image package identity](s5-p4-package.json). Both publication directories
were hash-verified. Commit/push are the sequential P4 delivery carrying this
receipt; no original guest or winnt.zip content is changed.