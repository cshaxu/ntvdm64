# T425 S8 producer triggers and software VGA fullscreen

## Final S8 delivery

The containing reviewed S8 P delivers the bounded producer/software-VGA
repair. T425 remains owner-open; S9 is planned but not admitted. The owner
requested natural mouse drawing before removal of periodic compensation,
software FULLSCREEN without hardware takeover, unfiltered frontend/transport
events, minimal focused verification and completion of the existing release
gates. This section supersedes the initial dedup candidates below.

Final production candidate is build/M0-T425/S8/r046/runtime; publication and
coherent r040 recovery are build/M0-T425/S8/r045. The publication script
validates x86 PE identity, backs up all eight files, replaces only the exact
O:/winnt product paths and checks all eight hashes, with rollback on failure.
Original guest binaries/configuration and imported libraries are unchanged.

| File | Published SHA-256 |
| --- | --- |
| run16.exe | 9C19B7C6C5CD82748B66D9F450E6D88341F5E35572093E0DD8BCBA0E51864E6A |
| ntsrv.exe | 8BE09E891D0C326F4A58234AD0426550D516826366AD984D6DD7216ADC4465C1 |
| ntcon.exe | 5004BEE0CFC15B2245927ADD337D7624B9850979B824790B47C3946A0ADC4BAA |
| ntvdm.exe | 3A4F619FA5992753321093CBA80F3EAA3E94FF1DB426A4C5CA6148069F5B6DA1 |
| ntvwm.exe | 2010EF6F83E6D620F234A17E284EC80E9C2F0D69E2B155C430E46FB44C080AAF |
| ntmon.exe | 9FDC95C114DBE2473D46415043AD5193CCB66DFA4B97D56A09B6351D49757EF2 |
| WOW32.DLL | BEB688FE832921D788DB297787B2D1062D7FDD095AF1A304824F31DE7F19D0C1 |
| VDMREDIR.DLL | 110D00408CCEC6EE3F3F4B0FC7E8D58534DD3744BBC31FC27A248C7739A93E67 |

Build uses the validated MSVC14.43 / SDK22621 Win32/x86 /MT CCPU40 cache,
APP 0.0.425, RPC/protocol37 and I/O25. Dependency-driven EXE relinks retain
the separate unchanged DLL closures. r046/release-source-manifest.json seals
284 selected inputs; s8-changed-inputs.json supplements the changed mirror,
adapter and test inputs. All 47 imported-library inputs match their baseline.
The generated graph and toolchain remain in build/M0-T424/S2/r001.

Final verification:

- r046/product-matrix.log: all Console17 and Window17 routes pass, preserving
  COMMAND/MEM/EDIT actual output and exit/cleanup assertions.
- r044/package-gates.log: independent WINMINE, SOL and WRITE frontiers match
  S7 and historical S2/S3/S5 observations. Their seven relevant binaries and
  inputs are identical to r046; this is not gameplay or SOL/WRITE completion.
- r044/remaining-gates.log: RPC11, GUI5, five version mismatch negatives and
  real modern EDIT Ctrl+Q -> native echo -> DOS MEM pass. These provider paths
  are unchanged by the final native producer dirty-row repair.
- r046/finish-gates.log: independent-session isolation, four worker/frontend
  loss cases, strict repeated DIR (dirty-prompt=0), service16, two nested
  Window handoffs (actual exit23 and executed parent-return marker) pass.
- The same final gate runs verify-dos-idle-frame-publication.ps1 against the
  source-identical r041 diagnostic NTVDM in Console and Window, using original
  COMMAND/EDIT. All four actual guest idle publication assertions pass.
- r044/units/build.cmd and build.log: assertion-enabled queue/bridge tests
  pass; r046/focused.log: DOS repeated-send exit73 and native465/0 pass.
- r043 actual cursor-register and stationary route frames pass as detailed
  below; r041 count/video/graphics/text mouse tests retain button/order gates.
- r045/postpublication.log: 12 native/DOS relaunch pairs, 12 interactive CMD
  relaunches and cooked outer-CMD return pass on O:/winnt itself. The driver
  subsequently refuses a live service rather than taking over an owner session;
  its overall abort is retained, not reported as a whole-driver pass. Independent
  postpublication-gui.log completes GUI startup0/wait37 and all-eight identity
  checks without touching the live text session. Owner briefly reports CAF
  failure, then withdraws it as working; no speculative hotkey patch is made.

The final driver invokes the tracked verify-ntvwm-management.ps1 -TwoSessions,
verify-broker-retirement.ps1 and verify-broker-io-handoff.ps1 -Case nested-window
entrypoints. All use the S6/r005 observer on an unswitched private desktop;
SUBST is Z: only and removed in cleanup. Authored COM probes and observer
executables stay below build/, never replace original guest media or enter the
published eight-file set. Exact commands and paths are retained in r044/
finish-gates.ps1, r046 logs and r045/publish.ps1.

Source review covers each changed production file, actual caller/provider and
registered mirror intrusion. The five changed original mirror files have
verified pinned upstream identities and intentional DIV-314/318/322 differences,
not format-only rewrites (r044/mirror-sweep/selected-assertions.txt). The wider
comparison detects 26 pre-existing format-only files outside this S's changed
set; it is not a passing whole-tree byte-clean audit and they are left intact.
Original VGA/mouse execution algorithms remain local; no OpenNT-host mirror,
guest, imported library, new component/channel or broker lifecycle change.

Acceptance limits: physical RDP/foreground observation is not an automated
pass. Owner hand tests accept restored EDIT performance and blinking; mild
DOS mouse choppiness is not attributed to CCPU without measurement. The
downstream-only 1000-motion retirement injector remains unstable diagnostic
evidence, not a normal-path release gate. Original CRTC bit-5-only hide
limitation is explicit TODO, not masked by a cache. Host scrollback is still
not promised. Original calc_update remains needed to populate frame backing;
only hardware-specific bypasses and project notification triggers changed.

## Final native producer repair (r044-r046)

The original fixed-timing cooked-CMD relaunch gate fails in r044 twice and
also in the owner-published r040 control: second COMMAND never starts and
MEM reaches outer Windows CMD (exit 216). The preceding S7 control passes
exit 19. These failures are retained, not converted to passes by a retry.
Removing the native sampling dirty-row predicate had turned cursor-only
changes into writes of every row of the hidden Console, including large
buffer extents. This was a producer regression, not a missing receiver cache.

presentation.c now derives changed rows at the native sampling boundary,
just as the VGA painter derives changed cells. A cursor-only sample still
publishes its cursor/frame but does not invent character writes. Explicit
text/exchange publications remain unfiltered; NTCON and worker-base have no
display deduplication. The retained 30 ms native sampling interval is unchanged.

The assertion-enabled native fixture now passes 465 checks, including cursor
movement/restoration with no row writes, actual cell change with row writes,
and unchanged resampling without a new write. DOS repeated-publication fixture
retains expected exit 73. r046 relaunch-a and relaunch-b both pass exit 19
with the original input timing and strict MEM/native-VER/output-return markers.
No delay, injected refresh, forced output or weakened assertion is used.

r046/runtime is the final formal eight-file candidate; only NTVWM differs
from r044. Console17 and Window17 pass again on r046. WOW tests use r044's
identical seven relevant artifacts and unchanged inputs, not the r046 NTVWM
repair; all three retained application frontiers remain separate.

Raw build, fixture, control and relaunch reports remain below build/M0-T425/S8.
The final delivery section records the remaining lifecycle and publication
gates; historical failed candidates below are not the delivered baseline.

## Natural route closure checks (r041-r044)

The owner accepts r040's recovered real EDIT performance and normal blinking.
S8 continues without another performance policy, timer or receiver cache.
The original mouse drawing, geometry and saved-background algorithms remain.
Consumed ENTER/LEAVE now call a three-statement original-owner route hook;
the stopped CPU handoff erases saved background before final extraction and
acknowledged release. The tick no longer calls mouse_refresh_pointer. No
fabricated position bit, callback or periodic stationary erase/redraw remains.

Final source checks and tests:

- r041 formal count/video/graphics move/graphics retire/text burst/text retire
  pass. The latter two use 200 downstream motions, retain button and deadline
  assertions, and do not prove physical Raw Input/RDP behavior. Extreme 1000
  downstream-injected retire instability remains diagnostic: that injector
  bypasses normal kvm-window motion accumulation. It is not reported passed.
- r041 actual transmitted text frames show/move/hide pass, five frames,
  without another guest text write between pointer states.
- r043 Registers: vga-cursor-registers.asm changes CRTC position, shape and
  visibility without changing cells. Five actual copied frames contain the
  ordered cursor states (20,10, start=2,height=3,visible), hidden, then
  (21,10,visible); their character grids match exactly. Original do_new_cursor
  computes end-start, not end-start+1. Test entrypoint is
  tests/observation/verify-dos-mouse-frame-publication.ps1 -Case Registers
  -GuestPath Z:/REG8.COM, with r041/diagnostic-runtime and r043/registers.
- r043 Route: mouse-cursor-route.asm shows the mouse in Console before CAF,
  constrains its guest position, then observes ENTER drawing and source LEAVE
  restoring the background. Five actual frames contain 7020 -> 0720 at
  unchanged cell (20,10). The same entrypoint uses -Case Route -GuestPath
  Z:/ROUTE8.COM and r043/route; the retained S7MOUSE test hook supplies ordered
  ordinary motion/button/source-retirement events, not an alternative painter.
- r042's first Registers attempt fails its new ordered-frame assertion. It
  assumed end-start+1 and CRTC bit 5 alone would hide the cursor. Original
  vga_prts.c updates its start register only when the five start bits change,
  and do_new_cursor sets visibility from start/height; bit-5-only writes are
  not honored there. This unchanged original host-code limitation is retained,
  not repaired or counted passed. r043 tests the original supported hide
  route (start scan line 31 >= character height), without changing the VGA TU.
- r044 assertion-enabled x86 /MT queue and relative bridge fixtures pass exact
  signed accumulation, overflow/rejection, button ordering, route transitions,
  isolation and cancellation. DOS pipe fixture exit=73 proves repeated frames
  forward; native presentation fixture has 461 checks and zero failures.
- r044 formal six-EXE incremental build succeeds; the unchanged two DLLs match
  retained identities. Console17 and Window17 pass; three independent WOW
  frontiers match S2/S3/S5 and immediately preceding S7. These retained WOW
  dialog/window observations are not gameplay acceptance.

All runs, authored guest probe binaries and reports remain in build/M0-T425/S8.
The two ASM probes assemble with nasm -f bin; neither replaces original media.
Diagnostic NTVDM observes the production transport, not a test frame provider.
Final lifecycle, publication and reviewed P disposition are recorded below
when completed; these checks alone do not claim S8 delivery.

## Current producer-trigger repair and publication

### Software fullscreen implementation and retained failures (r030-r034)

DIV-322 uses the existing ScreenState for the CCPU software presentation route:
Window text or graphics selects FULLSCREEN; Console text selects WINDOWED.
STREAM_IO retains its original transition. No hardware fullscreen API, physical
regen mapping, X86GFX or MONITOR branch is enabled. Original VGA selection,
font/scan-line/BIOS writes and software DIB allocation remain active in both
routes. The FULLSCREEN host-caret early returns remain, while actual VGA
position/shape/visibility notifications still reach copied text frames.
Original calc_update remains necessary because it populates the transmitted
cell backing. The original block path settles mode, extracts final cells and
publishes the guest cursor before ResetConsoleState and ownership release.

Mouse IRQ flush no longer waits on full-frame transport in software fullscreen;
the existing video owner extracts pending simulated changes. Mouse drawing,
saved background, show/hide and position algorithms are retained, including
mouse_refresh_pointer until all natural route/mode paths are proved.

The project-added relative input queue now follows the original nt_process_mouse
pressure policy: above sixteen pending entries, merge only adjacent MOVE events
with identical button state. Relative deltas accumulate exactly rather than
overwriting an absolute position. Overflow falls back to another entry; button
changes, ENTER and LEAVE remain separate, ordered records. Queue and bridge
fixtures retain full-edge-capacity, rejection, remainder and isolation checks.

- r030: 200-motion text Window probe passes in 3453 ms (r029 failed at 52844 ms).
  Actual transmitted show/move/hide and COMMAND/EDIT idle Window checks pass.
- r031: 1000 motions still fail at 10937 ms before input accumulation. Retained
  as failure, not a release gate.
- r032: accumulated 1000-motion probe passes in 3547 ms; actual transmitted
  pointer transitions pass. Both x86 assertion-enabled queue/bridge fixtures
  pass. Nested native/DOS Window first run loses part of the parent command and
  fails its strict marker; the unchanged repeat passes. This is not yet proof
  of stable nested input delivery.
- r033: formal cursor count passes; graphics/text video probe fails with an
  invalid-handle dialog. Rebased captured NTVDM stack identifies
  publish_text_update -> NtvdmConsoleUpdateText, not graphics buffer creation.
  A mode-selection tick attempted text publication before the original palette
  and cell extraction completed, after the old graphics palette was released.
- r034 corrects the producer boundary: retain pending notifications across mode
  selection and publish only after the original software update phase. No
  palette retry, receiver/transport dedup or forced redraw is added. Formal
  count/video/move/retire checks pass. Its text-burst case fails; the cached
  test Window reports error 1400 before its FIFO acknowledgement. The authored
  text pressure guest redundantly reset mode after selecting Window; removing
  that reset leaves video-mode coverage in MCVIDEO and retains every input
  assertion. r035 formal 1000-motion text-burst then passes.
- r035 text-retire and r037 diagnostic text-retire at 1000 motions still fail
  stage 2; r036 formal 200-motion text-retire passes. r038 uses the same formal
  product and diagnostic-only callback mask output, and passes 1000 motions,
  mask=7. This is an unstable pressure/retire frontier, not proof of a sole
  probe cause or a completed repair. No deadline or button assertion is relaxed.
- r039 actual transmitted show/move/hide passes with five copied frames and
  no intervening text writes. Window COMMAND and EDIT both pass the retained
  real-guest idle publication checks: no repeated full frames in the measured
  five-second interval. These do not certify physical cursor blinking or RDP.

Build command remains the existing x86 cache's run-ninja-parallel.cmd targets
ntvdm.exe and ntvdm-console-wire-observer.exe. VdmTib is 4208 bytes, no overlap.
All reports and candidates are under build/M0-T425/S8/. The owner subsequently
explicitly requests publishing a side-test candidate before the unstable
pressure frontier is repaired. r040/publish.ps1 publishes the formal r035
eight-file package and verifies all destination hashes; r040/recovery retains
the complete replaced r028 package. No diagnostic executable, authored guest
probe or configuration change is published. NTVDM SHA256 is
AC4E6593990A8D35AB110DC742D590E728529C12CD81B6F223163B8387D5DB41;
the other seven hashes match r028. Six current EXE targets incrementally report
no work; unchanged DLLs reuse the retained verified identity. An initial attempt
to name DLL targets in this EXE-only graph fails with unknown target, before
any build/publication; it is not a successful full-package rebuild.
This is an expressly authorized incomplete candidate, not a non-regression
verdict or P delivery. Full gates, final-frame/route cleanup and physical RDP
behavior remain incomplete; S8 is open.

Postpublication background review leaves the side-test package untouched.
Assertion-enabled queue/bridge fixtures rerun successfully. One remaining
source-level pressure candidate is IRQ notification debt: the project relative
bridge calls original DoMouseInterrupt for every accepted sample, including
samples accumulated into an existing queue tail. Original DoMouseInterrupt
increments MseIntLazyCount while an IRQ is pending/suspended; MouseEoiHook
can schedule further 10000-us IRQs even after queued motion is consumed.
The original absolute queue also notifies after coalescing, so similarity alone
does not justify changing this policy. Actual callback/queue timing is needed
before identifying it as the cause or changing the adapter's notification rule.
No scheduler, IRQ-delay or frontend change is made from this hypothesis.

### Software VGA fullscreen audit; r028 stress regression remains open

Owner reports severe Window EDIT mouse backlog, blocked keyboard/route return
and a pointer surviving into Console. r029's text-mode original mouse guest
accepts the posted 200-motion burst and button transitions, but does not
complete: result=timeout, exit=0x53504354, 52844 ms to observer termination,
27 copied text frames. This is a failed pressure test, not a release gate.
The diagnostic executable is confined to build/; r028 remains the published
package and is not certified against this regression. No new package is
published by this audit.

The owner admits software VGA fullscreen recovery, excluding unavailable
hardware fullscreen. Audit disposition before production edits:

| Existing owner/branch | Software fullscreen requirement | Disposition |
| --- | --- | --- |
| nt_det.c hardware handshake, native BIOS and regen mapping; nt_fulsc.c X86GFX hardware registration | Emulated VGA remains locally owned | Keep hardware exclusions; do not enable X86GFX/MONITOR or use hardware Console fullscreen APIs. |
| nt_fulsc.c calcScreenParams WINDOWED guards | Select height and configure software scan lines/font/BIOS geometry | CCPU must execute the existing software body even if presentation is FULLSCREEN; otherwise consoleHeight is uninitialized. |
| nt_fulsc.c setVDMCursorPosition WINDOWED guards | Write emulated CRTC and BIOS cursor state | Retain original software register writes for software fullscreen; do not substitute frontend cursor state. |
| nt_graph.c make_cursor_change FULLSCREEN early return | Skip host Console caret, publish guest cursor metadata | Preserve guest position/shape/visibility publication, including changes without dirty character cells. |
| nt_graph.c graphicsResize FULLSCREEN early return | Allocate software graphics backing for emulated output | Do not discard this backing as if hardware scanned real VRAM. |
| nt_event.c block FULLSCREEN skips calc_update and may SetConsoleDisplayMode | Settle mode, extract final paint/cursor, preserve existing I/O acknowledgement | Software final paint must remain; no hardware desktop-mode operation. |
| nt_fulsc.c disable_stream_io enables updates only in WINDOWED | Software video must continue updating after stream transition | Retain enableUpdates for software fullscreen. |
| nt_reset.c FULLSCREEN disables idle detection | Software VGA still has observable changes | Do not inherit the hardware-invisibility assumption without a software-specific justification. |
| mouse_io.c recovered cursor algorithms and mouse_refresh_pointer compensation | Movement/show/hide/position/mode/route transitions draw or restore naturally | Keep recovered algorithms and compensation until each natural path is connected and tested; no stationary timer repaint. |

The selected CCPU host TUs build without X86GFX or MONITOR. Their normal tick
and flush calc_update are already not gated by ScreenState; they cannot be
classified as obsolete hardware work. Current text transport copies session
presentation storage populated by nt_cga.c's original nt_text painter, not
raw VRAM cells. Cursor/font metadata separately comes from simulated VGA.
Skipping calc_update without replacing that extraction would transmit stale
characters. Original software painter/device logic is therefore retained.
Hardware FULLSCREEN branches do not provide a composable CCPU replacement.

The project-added nt_flush_screen publication runs synchronous whole-frame
transport from a mouse-triggered original flush. Original mouse IRQs also
retain their deferred scheduling. Relative-input FIFO differs from the old
absolute mouse queue's tail coalescing. These are source-proven differences,
not yet proof of their individual contribution to the pressure failure.
No arbitrary delay, input discard, receiver dedup or worker-type special case
is admitted as a cure. Required recovery proof includes actual transmitted
pointer transitions, bounded burst/keyboard progress, Console route cleanup,
cursor-only updates, graphics/text modes and both shell-out directions.

### Owner-reported mouse publication regression

r025's mouse memory fixtures did not assert the transmitted intermediate
frames. The owner reports that Window EDIT pointer movement appears only on
the next unrelated redraw. r026 captures the original text-cursor guest:
CURSOR-TEXT-PASS, but only two transmitted frames, neither containing the
pointer. The new production-wire assertion fails stage=0 frames=2 against
that package, preserving the failure under r026/mouse-negative.

Original cursor_display calls host_flush_screen -> nt_flush_screen ->
calc_update, which consumes the changed video cells and invokes the original
update-start callback. The project flag was set outside graphics_tick and
then unconditionally cleared at the next tick; that tick found no dirty cells
and sent no frame. This is a project-added DIV-314 regression, not swallowed
input or an original guest mouse defect. The original mouse, dirty comparison
and painter algorithms remain unchanged. The notification now survives tick
entry; after an original flush completes, the existing copied text producer
publishes and consumes it immediately. The timer consumes other pending
updates through the same bounded helper. No transport or frontend dedup,
new timer, synthetic input or stationary redraw is introduced.

r027/mouse passes the same guest with actual acknowledged frames in order:
cell (20,10) XOR pointer -> cell (21,10) XOR pointer with old background
restored -> both backgrounds restored. No intervening guest text output
causes these transitions. verify-dos-mouse-frame-publication.ps1 checks these
copied protocol frames as well as guest exit/Window activation. It does not
claim physical Raw Input/capture or RDP verification. r027/idle Window
COMMAND and EDIT pass zero redundant frame publications in the retained
five-second interval. EDIT's distinct changed frames remain observable.

The corrected x86 formal NTVDM and diagnostic target build; VdmTib remains
4208 bytes with no overlapping symbol. The coherent r028 eight-file package
supersedes r025 at O:/winnt; r028/recovery retains the previous eight files.
published-manifest.json confirms all source/destination hashes. Corrected
NTVDM SHA256 is BC2C4914A436889FAA8803E7570777D29CF61EEA532E4F3260968282254DC41B.
Other seven product hashes equal r025. Published native exit19 and DOS
COMMAND /c VER output/exit0 pass under private unswitched desktops. No test
observer is published. Full S8 gates and owner physical validation remain
open, and other-session Queue/proposal edits are untouched.

Owner approves fixing publication triggers and deleting worker-base dedup.
frame_cache.h and every caller are removed. NTVDM's explicit video client
directly sends every request, including repeated configurations/frames.
NTCON's cursor no-op suppression stays removed. NTVWM owns its native sampling
comparison (Unicode grid, viewport, cursor, font/palette/composed mouse), not
a shared transport filter: an unchanged sample does not generate an update;
every explicit presentation_text call still sends its frame.

The existing DIV-314 hook now observes original nt_start_update instead of
setting presentation_updated unconditionally after calc_update. Original
tick, dirty algorithm, painter, timer and guest are unchanged. The second
root cause, captured in r023/Window cursor stacks, is the project-added
mouse_refresh_pointer -> cursor_display -> nt_flush_screen -> jazz_text_update
chain. DIV-318 erased and redrew a stationary saved pointer on every tick.
The added route repair now erases on inactive route and draws only when the
route is active with a displayed but unsaved cursor. Original guest movement,
show/hide, shapes and saved-background algorithms remain unchanged.

Affected x86 production and diagnostic targets build. r023 DOS fixture returns
the expected 73, proving 100 explicit identical frames still transmit. r025
native-forwarding2 returns zero, checks=461 failures=0, including another
identical explicit frame with the exact increased publication count. The first
new native test run retained an obsolete final serial of 6 after adding the
extra publication; it failed, then the exact expected total was corrected to
7. No production assertion was removed.

Real idle observations: r023/Console COMMAND and EDIT pass; r024/Window COMMAND
and EDIT pass, with zero redundant frames in each five-second interval. r023
Window EDIT had failed with 33 repeated frames and its recorded cursor stacks
proved the mouse-refresh cause; that failure is retained. These diagnostics
observe production worker calls, not generated replacement frames.

Real mouse probes in r025: count/video pass. The combined driver then failed
to find Window on its third independent private desktop, before guest text
cursor assertions. r025/mouse-isolated runs each case with a fresh test service:
text passes CURSOR-TEXT-PASS; move/retire pass WINDOW-MOUSE-PASS. The failed
combined report remains failed; fresh service isolation is a test setup change,
not a product lifecycle workaround.

The earlier coherent eight-file r025 product was published to O:/winnt, with exact recovery
under build/M0-T425/S8/r025/recovery and source/destination hash checks in
r025/published-manifest.json. Changed NTVDM SHA256:
6C7B8A019418F03BA6B2F190DDBD5D21DEB1DD0BBC86FF1B70C0148BE6B111D6;
NTVWM: 234C2E889040C4178F22EB5346689E80FC3672F30A3708ADF044EE30489408FC.
No diagnostic observer is in that package. Published native exit19 and DOS
COMMAND /c VER (exit0, MS-DOS Version 5.00.500 output) pass. The prior DOS
EXIT23 smoke incorrectly expected CMD's numeric-exit behavior and failed its
assertion; the recorded actual DOS exit was zero, not a hang or crash.

S8 is not closed: full final matrix, lifecycle/WOW gates, physical blink and
owner EDIT-return sequence remain open. This is the requested early product
publication after focused tests, not a claim of full task delivery/commit.

## Superseding owner-directed diagnostic baseline

The owner rejects transport/receiver deduplication as a root-cause repair.
The earlier suppression implementation and gates below are retained historical
attempts, not current release acceptance. r021 publishes eight hash-checked
files with frame matching disabled, native row skips removed and NTCON cursor
no-op skips removed. Recovery is r021/recovery. The production-linked DOS
fixture forwarding-test2.txt returns its expected 73 and proves 100 repeated
frames reach the receiver. Full current regression is not complete.

Owner's fresh Windows Terminal tests distinguish native immediate blinking
failure from DOS failure after EDIT, persisting after EDIT returns. The r022
private-desktop diagnostic package observes the production NTVDM client;
it is never published. COMMAND emits zero full frames during the five-second
idle interval. EDIT emits 27 byte-identical full frames without intervening
modifying requests. The retained suppression assertion correctly fails; this
is diagnosis, not a passing repair. Snapshot headers/payloads are saved and
two retained identical snapshots have SHA256
0F0D5F95044CD9326E8344D5025CFF6CFA7C3E3C0DB8F77993E3944C63F31554.

Only one cursor-position stack is recorded during that EDIT observation:
original jazz_text_update -> nt_paint_cursor -> project Console binding.
Therefore this run does not show repeated guest cursor-position requests.
The repeated idle traffic is the project-added nt_graph.c DIV-314 hook:
calc_update can return without a dirty update, but the hook unconditionally
sets presentation_updated and publishes a copied frame. NTCON then projects
the received frame including its cursor. Original painters remain unchanged.
Native presentation_loop samples every 30ms; with frame matching disabled,
presentation_capture publishes each sample including cursor state. This is
project-owned sampling/publication logic, not CMD changing its cursor.

Still unproved: the exact EDIT-to-COMMAND stream/video restoration path in
the owner's reproducer, native actual wire counts and physical blink timing.
The next repair must correct publication triggers, not restore suppression at
worker-base/NTCON or blame unchanged guest binaries.

## Question and bounded source disposition

Owner confirms S7 repaired idle native Console blinking but COMMAND/EDIT still
look continuously refreshed, and authorizes deduplication only in workers.
S7 f30ee19b2 and build/M0-T425/S7/r002 are the immediate source/runtime baseline.
Other-session Queue/proposal changes are preserved and excluded.

| Logic | Source/current owner | Disposition |
| --- | --- | --- |
| Original graphics tick and calc_update dirty detection | mvdm/softpc.new/host/src/nt_graph.c | Read-only; preserve original timer, guest painters and existing hooks. |
| Full-frame assembly after the tick | Project-added ntvdm-exe/win32/console_text.c | Preserve payload/font/cursor/palette assembly; suppress identical acknowledged frames at its client boundary. |
| Acknowledged description/payload comparison and ownership | S7 project-added ntvwm-exe/presentation.c | Extract matching mechanism into worker-base/frame_cache.h and use in both production workers. |
| Native Unicode grid, viewport, attributes and cursor state | Project-added NTVWM presentation | Keep native capture comparison independently; equal PC glyphs do not prove equal Unicode cells. |
| Frame reception and host projection | ntcon-exe | Unchanged; no receiver deduplication/cache or worker-kind branch. |

Recovery ladder: (1) the composable original tick/painter remains selected;
(2) original display checks cannot acknowledge the project's remote copied
publication and therefore do not supply this cache contract; (3) no mirror
intrusion is needed; (4) reuse the owner-approved S7 project mechanism in a
worker-only, explicit-instance header. No new execution/lifecycle policy,
helper, timer, guest modification or wire extension.

## Mechanism and failure contract

The worker owns a copied acknowledged description/payload under its existing
I/O critical section. Compare the entire initialized description and payload:
geometry, glyph/attribute cells, cursor, font banks, palette and composed mouse
state. Native additionally compares its full Unicode grid and Console state.
NTVDM allocates its snapshot before sending, and installs it only on successful
video acknowledgement. Native transfers its already packed storage only after
PUBLICATION_END succeeds. Failed sends cannot install the candidate cache.

Successful delta output operations conservatively invalidate cached frames;
reads, input and title operations do not. Transport failure, video retirement
or physical-channel release invalidates the cache. New endpoints start empty.
This preserves delta/frame ordering, failure visibility and first-frame delivery.
Original NTVDM execution, refresh and device semantics remain independent.

## Reproducible focused verification

Build with the validated MSVC14.43/SDK22621 x86 /MT CCPU40 cache:

```powershell
cmd.exe /d /c build\M0-T424\S2\r001\run-ninja-parallel.cmd ntvdm.exe ntvwm.exe console-client-test.exe ntvwm-presentation-test.exe
```

Execute both fixtures with the retained private, unswitched desktop observer
build/M0-T425/S6/r005/observer.exe. The DOS fixture intentionally dispatches
the original-shaped close callback and exits 73; native fixture exits zero.
Raw reports are build/M0-T425/S8/r001/focused-*/. Actual assertions live in
tests/app/console_client_test.c and tests/observation/ntvwm_presentation_test.c.

- DOS: 100 identical publications produce no new protocol request/serial;
  cursor/font/palette/cell changes publish; a delta forces republication;
  new-channel first frame publishes; retirement and failed-frame recovery do
  not suppress the following valid frame.
- Native: retained 459 checks, zero failures, including real Console capture,
  unchanged capture and changed cells/cursor.
- Initial reconnect fixture failed because its test decoder survived physical
  pipe closure. Production uses channel-local decoders. The test now disposes
  that decoder at the real close boundary, without relaxing the first-frame
  assertion. Initial failed reports remain non-pass evidence.

## Historical initial dedup candidate verification (superseded)

Affected production targets build. Candidate build/M0-T425/S8/r001/runtime
passes Console17/Window17, RPC11, GUI5, service16, five version negatives,
modern EDIT return, cooked outer-CMD return, independent session isolation,
four worker/frontend-loss cases and strict repeated DIR. WINMINE/SOL/WRITE
retain the exact immediately preceding S7 and S2/S3/S5 observation frontiers;
this is not a claim that the existing SOL/WRITE limitations are repaired.
Both ordinary native/DOS nested Window runs return actual direct exit 23,
real DOS/MEM output and the executed parent-return marker.

Driver-only failures remain recorded: the second nested driver initially
refused the first broker's normal ten-second retirement; its subsequent
read-only identity check could not resolve Z: after SUBST removal. Both
actual nested runs pass independently. Waiting on the prior service process
handle, without stopping or taking it over, fixes the driver ordering.

The test-only NTVDM console-client observer delegates the unchanged common
transport and records actual publication calls at the production worker-client
boundary. It is not a substitute frame producer or a literal byte-count trace.
Its package differs only by the diagnostic NTVDM executable; the normal eight
products are the independently sealed regression/publication candidate.
New tests/observation/verify-dos-idle-frame-publication.ps1 uses an unswitched
private desktop, original COMMAND/EDIT, a twelve-second live observation and
the actual I/O25 VIDEO_BEGIN operation for a five-second idle interval.

Earlier r002/r003/r004/r005 probes incorrectly required a first full frame or
continuing query traffic. Fresh COMMAND can remain STREAM_IO, and idle EDIT
can be entirely silent after its initial publication. These failed harness
assumptions are preserved, not claimed as passes. Console r007 proves actual
DOS banner / EDIT welcome screen, continued bounded execution and zero idle
full frames: COMMAND starts with zero publications and 51 other idle requests;
EDIT starts with two publications and no idle requests. Window r008 did not
actually trigger CAF because the retained observer requires scripted input;
it is not Window evidence. The final Window invocation adds actual VER input
and requires the CAF visible-window success report and initial full frames.

Reproduce each actual idle case with the diagnostic runtime containing the
ntvdm-console-wire-observer.exe build at its normal ntvdm.exe name:

```powershell
tests/observation/verify-dos-idle-frame-publication.ps1 -RuntimeRoot build/M0-T425/S8/r003/diagnostic-runtime -Observer build/M0-T425/S6/r005/observer.exe -ReportRoot build/M0-T425/S8/r007 -Display Console
tests/observation/verify-dos-idle-frame-publication.ps1 -RuntimeRoot build/M0-T425/S8/r003/diagnostic-runtime -Observer build/M0-T425/S6/r005/observer.exe -ReportRoot build/M0-T425/S8/r009 -Display Window
```

Choose fresh existing build report directories for reruns; old reports are
never overwritten. Physical blinking observation is owner-waived, not an
automated visual pass. In particular, fresh STREAM_IO COMMAND has no idle
full-frame transmission to blame: this repair proves the duplicated full-frame
mechanism is fixed, not that it is the sole cause of every observed cursor
symptom. Publication and reviewed commit/push remain pending. T425 stays open.
