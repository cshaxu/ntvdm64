# T423 S7 Window mouse

## Current acceptance summary (P1 delivery)

The later dated/run-labelled findings supersede the initial audit below;
in particular, original software cursor recovery now requires registered
DIV-318 hooks. Initial claims of a conversion-only/no-mirror scope are history.

| Capability | Current evidence / remaining requirement |
| --- | --- |
| Native Window mouse | r9 actual frontend/helper to ReadConsoleInput passed; capture gesture, buttons and focus release covered. |
| Original DOS show/hide count | Real MCOUNT r10 and current-worker r20 count passed. |
| Original graphics cursor | Real MCVIDEO r11 and current-worker r20 video passed. |
| Original text cursor | Current-worker MCTEXT r17 passed actual B800 mask, move and erase assertions. |
| DOS input and callbacks | r15 normal and r16 source-retirement passed real INT33 checks; r12 no-input negative fails as expected. |
| Console regression | All 17 text-gated cases r18 passed. |
| Console mouse retention | Real KMTST r32 passed reset/position/callback/teardown, key release and subsequent MEM/COMMAND completion. |
| Window regression | All 17 text-gated cases r19 passed, including EDIT exit and nested MEM. |
| Nested chains | r21 passed both twelve-target chains, actual I/O, results and two distinct frontend groups. |
| Window fault retention | r22 passed normal/frontend/launcher/worker/helper cases with unrelated-session survival. |
| WOW depth retention | r24 observed WINMINE main window and retained SOL/WRITE OOM frontiers; not gameplay or SOL/WRITE success. |
| Physical focus/clipping | Owner-waived with source/unit evidence; never represented as physically observed. |
| Delivery | Renamed seven-file publication and formal-path verification complete; reviewed P1 submission records commit/push in Git. See final verification below. |

Runtime reports are in owner-approved O:/winnt/Logs2. O:/winnt now contains
the verified S7 ntkvm/ntsrv package; S: names its retained build package.
Q: retains the earlier old-name mouse package. Both the previous S6 and the
old-name S7 package remain recoverable in their distinct build-local backups.

## Baseline and scope

S6 P1 f0f671e5e and admission 4db21eea4 are pushed. The published seven-file
S6 set and its regression evidence are in the [S6 ledger](m0-t423-s6-window-display.md).
S7 is implementation work, not a completed mouse capability. Preserve the
unchanged imported four libraries, original guest, scheduler and Console input.

## Source-first boundary findings

- Imported lib/kvm-window/win32/component.c emits relative mouse events with
  content-scaled deltas and left/right button state. motion.c retains scaling
  remainders. Do not apply the Window/client scale a second time.
- The first uncaptured click is the library's capture gesture, not a guest
  press. Legacy WM_MOUSEMOVE is not a second motion stream. Lost focus/capture
  flushes pending motion and releases content buttons. Preserve this behavior.
- Original mvdm/softpc.new/host/src/nt_event.c::nt_process_mouse consumes
  MOUSE_EVENT_RECORD position and left/right bits, clamps against the current
  original mode and queues unchanged original mouse events.
- nt_mouse.c::ScaleToWindowedVirtualCoordinates consumes text cell positions
  through ConsoleTextCellToVPCellLUT, or graphics coordinates through
  WindowedGraphicsScale. It retains the original callback/button policy.
  A relative pixel delta is not an admissible row/column without conversion.
- frontend-exe/native_console_frontend.c::window_input currently delegates
  only to frontend_keyboard_dispatch. Non-key mouse events return success
  without delivery. The existing copied input queue and native/DOS routing
  are reusable; there is no need for another transport or worker UI owner.
- SoftPC common/ui/ui.c provides comparison evidence for excluding concurrent
  Console mouse input when Window owns input. Its application/machine owner
  is not imported into this frontend or used as a runtime dependency.

Original guest mouse policy remains directly selected. The necessary new
boundary is a frontend-local conversion from the explicitly adopted library
event to the existing MOUSE_EVENT_RECORD contract. OpenNT does not contain this
library interface; importing Console Server or changing original mouse policy
would not eliminate that conversion. No mirror or library patch is proposed.

## Implementation and verification checklist

### Native text conversion candidate

`frontend-exe/window_mouse.{c,h}` is a frontend-owned native Console conversion
candidate (initially isolated; production wiring is recorded below). It consumes approved
content-scaled relative motion, keeps subcell pixel remainder, adds the hidden
Console viewport origin, separates motion from button transitions and commits
its delivered state only after a successful sink. Reset releases held buttons;
retirement also removes source identity. No DOS policy, hardware state or
shared-library change is included. Original Console event structure is reused;
OpenNT has no kvm-window input provider, so this finite conversion is new.

`frontend_window_mouse_test.c` passed x86 /MT /W4 /WX with
`/D_CRT_SECURE_NO_WARNINGS`, frontend include root, compiling window_mouse.c.
Artifacts: `build/M0-T423/S7/native-mouse.exe`, with objects in the same directory.
The first compile failed on the unchanged imported types header's CRT
deprecation warnings; the existing compatibility macro resolved them without
editing the library. Tests cover nonzero viewport origin, accumulated subcell
motion, zero-button movement, separate button transitions, failed release sink,
reset, source retirement, signed extreme deltas, resize clamp and unsupported
event shapes. This proves conversion only, not native program interaction.

Production wiring must deliver mouse input/releases to the old native consumer
before DOS binding changes; the shared keyboard typeahead queue cannot blindly
carry old native viewport mouse coordinates into DOS. Also synchronize geometry
with the actually presented native frame before accepting its mouse events.
These are pending integration requirements, not waived tests or completed work.

The subsequent working-tree integration adds window_mouse.c to the formal
frontend-window library and calls its dispatcher from native_console_frontend.
The rasterizer and converter now share the existing 8x14 cell constants;
successful native Window presentation supplies the viewport geometry. Mouse
modifier state is captured in the UI callback alongside key/text state.
Input polling waits for lazy native backend availability and obtains an initial
native frame before accepting its mouse input. On native-to-DOS handoff the
old native input and final release are delivered before owner change. Returned
native MOUSE_EVENT records are excluded from DOS typeahead, while other records
retain order. This deliberately leaves DOS mouse implementation pending.

Formal cached x86 generation and `ninja -j4 frontend.exe` succeeded in six
incremental steps. Candidate SHA-256:
`4EC93E12DA829B30ECE7F75A3CBBF4FE4B0CAD532E8AA092937225362AC251BC`.
Existing imported CRT-deprecation and WIN32_LEAN_AND_MEAN warnings remain;
no library source was changed. This candidate is build-only and uncommitted,
not published or accepted. Native consumer, handoff/failure, visible pointer,
DOS integration and complete production regression gates are still open.

### Production-composition native mouse verification

Extended the existing `tests/app/native_console_frontend_test.c`, linking the
actual frontend, Window library, hidden-Console backend/view/helper. On an
unswitched private desktop it sends Window button messages, not global input.
A separate native target uses ReadConsoleInputW on its real hidden Console:
exactly four mouse records are required, left press/release then right press
and release caused by WM_KILLFOCUS, all at the actual viewport center. The
initial capture click must produce no content press. No test transport replaces
production input delivery. Raw physical motion and actual desktop focus are
not claimed; conversion motion remains covered by the unit test.

`m0-t423-s7-native-mouse-r1` exited 0 with a PeekConsoleInput target assertion,
but its early mouse marker was overwritten by later Console presentation.
The strengthened r2 consumes the records with ReadConsoleInputW and repeats
the successful summary after the existing lifecycle suite. r2 exited 0,
observed PASS marker, no timeout; the existing 37/0/259 completion, lazy helper
L/M ordering, helper loss/cancellation and stalled cleanup checks also passed.
Evidence: `O:/winnt/logs/m0-t423-s7-native-mouse-r2.txt` and `.txt.console.txt`.
Test executable SHA-256:
`805B6B6E57D300334B3AFCA292B421AB7EAE33238987D02CD9A8377A7D4ECCE8`.
The standalone frontend.exe candidate hash remains the preceding value.
No live package publication or S7 closure is implied by this bounded result.

### Native pointer feedback and publication ordering

Source inspection found frontend_window_present synchronously drains input.
Moved native geometry publication before that call, so queued mouse input sees
the frame's viewport instead of uninitialized/previous geometry. This is a
source-proven ordering repair, not a claimed reproduction of a reported bug.

Native Window rasterization now optionally draws the OS arrow on its copied
GDI raster. Console cells, text caret and guest memory are untouched; capture
and host-pointer ownership stay in the unchanged library. No extra transport
or library capacity is required. An initial monochrome-only palette assumption
failed with ERROR_INVALID_DATA (13): the loaded system arrow produced colors
outside the 34 assumed entries. The repaired conversion preserves exact colors
using the carrier's existing 256-entry palette and reports unsupported rather
than silently quantizing on exhaustion. No claim of arbitrary themed-cursor
palette coverage is made.

The raster fixture verifies clipped arrow pixels, unchanged input cells and
restored no-pointer output, alongside existing text/caret/graphics cases. Formal
x86 frontend and fixtures compile; frontend-window-library-test passes.
Private-desktop native production-composition r3 also exited 0 with the final
mouse ReadConsoleInput and inherited lifecycle PASS markers. Evidence:
`O:/winnt/logs/m0-t423-s7-native-mouse-r3.txt` and `.txt.console.txt`.
Current build-only frontend SHA-256:
`D028CB6A814E3DC7AD62585639D891AD077CDB710A319F1720D7FA05E4ADAA20`;
composition fixture:
`577F08990CD1898640CF4E860A82EF41D5C831B714DEB84820145DF5B677B4C9`.
Native raw-motion hardware observation, DOS pointer rendering/input and full
production regression/publication remain open. No S7 closure or deployment.

### Executed original-algorithm fixture

`tests/component-integration/verify-original-mouse-coordinates.cjs` extracts
the current mirror's unchanged TextScale, LimitCoordinates and
EmulateCoordinates bodies into build-only generated source and compiles
`original_mouse_coordinates_test.c` with MSVC x86 /MT /W4 /WX. It mocks only
the BIOS row-count read and surrounding state; it does not run guest code,
IRQs, Window input or the production worker. All three extracted function
bodies were also compared, after EOL normalization, with read-only
`O:/repos.external/OpenNT/base/mvdm/softpc.new/host/src/nt_mouse.c`: identical.
The test itself depends only on the current repository, not that reference.

Run from VsDevCmd `-arch=x86 -host_arch=x64`:
`node tests/component-integration/verify-original-mouse-coordinates.cjs`.
Evidence: `build/M0-T423/S7/original-coordinates/{build.log,test.log,result.json}`.
Initial sandbox compiler spawn returned EPERM; authorized execution outside
that sandbox succeeded, exit 0, 14 assertions. Source SHA-256:
`c4cd4a5965a9452f7e138efebe47cfc1e84b227599b5c906852d597c28f4f00d`.

Verified isolated contracts: reset center; signed motion; guest-set position
then motion; button-only unchanged position; negative bounds; 25/43/50-row
text bounds; mode 13h virtual width 640 (not its 320-pixel raster width);
mode 12h bounds; application text/graphics ranges; pending position consumes
the current sample without adding its delta. These constrain the binding;
they do not close any end-to-end mouse checklist row. In particular, original
algorithm reuse is now executable evidence, while delivery, capture ownership,
visible pointer feedback and guest integration remain open.

### Pointer ownership audit follow-up

Current-source inspection of nt_mouse.c establishes two distinct contracts:
`ScaleToWindowedVirtualCoordinates` uses the Console event's absolute cell/pixel
position when `bPointerOff` is false; its hidden-pointer branch instead uses
`WarpSystemPointer` and original `EmulateCoordinates`. `mouse_set_position`
records `newF4x/newF4y` and `bFunctionFour` for the hidden-pointer or fullscreen
branches. `EmulateCoordinates` already applies reset, guest-set position and
original bounds. Reimplementing those rules in frontend is not justified.

The current boundary is not yet suitable for treating captured Window motion
as this original hidden-pointer path: `console_client.c::MvdmGetCursorPos` and
`MvdmSetCursorPos` forward to frontend, whose CONSOLE_IO_GET/SET_POINTER handlers
still operate on the real desktop pointer. Original `MovePointerToWindowCentre`
and focus routines also request clipping/warping, while the imported Window
library owns actual capture and clipping. Connecting these unchanged to the
desktop during Window capture would create competing pointer owners.

Consequently a row/column accumulator alone cannot close S7. The next design
check must resolve the original relative-counter/guest-set-position contract
and its presentation binding together, without equating presentation Window
with original hardware FULLSCREEN, and without letting worker pointer APIs
fight the library's capture. This is source evidence of an incomplete binding,
not a reproduced runtime failure or a requirement to modify the library.
Native hidden-Console mouse coordinates remain a separate absolute-cell
contract. No 200-column DOS requirement or shared-library extension is admitted.

### Reviewed recovery candidates

- Read-only SoftPC `src/app-softpc/machine/driver.c` and `machine.c` dispatch
  Window deltas/buttons to `mouse_send`, the InPort device entry. This project
  retains `base/keymouse/mouse.c`, but the selected NTVDM branch of
  `mouse_io.c::mouse_int1` calls `host_os_mouse_pointer`. The SoftPC UI contract
  is useful evidence; its device route is not a drop-in replacement for the
  admitted NTVDM mouse path and unchanged guest.
- Reference branch `codex/t423-original-reference-20260925` contains
  `presentation/mouse_delivery.c` and `mouse_irq.c`: bounded relative samples,
  signed-short splitting, source/button tracking and original IRQ invocation.
  These are recovery candidates, not imported production files. Its worker UI
  ownership and extra queue must not be revived merely to reuse code.
- The same reference's `nt_mouse.c` DIV-313 bypasses host warp input and calls
  original `EmulateCoordinates`, then retains original mask/button publishing.
  An EOL-normalized diff against current HEAD shows no corresponding change
  to `mouse_set_position`. With WINDOWED and `bPointerOff == FALSE`, that
  original function does not set `bFunctionFour`. Thus recovering the old
  hook alone does not establish guest-set-position correctness. This is a
  conditional source-contract finding, not a claim of an observed old binary
  defect. Require reset/set-position followed by motion tests before adoption.
- Original `host_os_mouse_pointer` only performs its X86GFX guest pointer
  drawing branch for `sc.ScreenState == FULLSCREEN`. Switching input alone
  does not prove visible pointer feedback. Do not falsify hardware screen state
  just to obtain drawing; establish the presentation contract independently.

| Contract | Required evidence | State |
| --- | --- | --- |
| Relative motion to original DOS text/graphics coordinates | Frame-size/cell-height, remainder, clamp and overflow tests; real consumer | Open |
| Native text mouse coordinates | Hidden viewport origin and cell geometry, existing input pipe and actual native consumer | Open |
| Button changes without movement | Separate press/release, movement with zero buttons, failed sink accounting | Open |
| Capture/reset/retirement | Existing library release followed by frontend delivery; no invented press or duplicate release | Open |
| DOS/native and Console/Window handoff | Target-specific state, queued-event disposition and independent sessions | Open |
| Pointer position and visible feedback | Review guest-set cursor/host cursor ownership before choosing the position state; no unrelated text cursor mutation | Open |
| Full production delivery | x86 build, inherited DOS17/Window/nested/fault/WOW gates, seven-file publication, governance and push | Open |

The current library does not emit wheel or middle-button Window messages.
Their presence in a neutral event struct is not proof of a provider. Do not
expand the shared library or make new hardware-feature support a closure gate;
classify malformed/unproduced event values in the converter's negative tests.
Physical desktop capture/clipping observation remains owner-waived, not passed.
No production source or live package changed during this initial audit.

## DOS relative carrier recovery in progress

Original nt_event.c::nt_process_mouse deduplicates identical absolute positions,
overwrites the latest motion when sufficiently queued and drops the oldest
record on ring overflow. Applying these policies directly to relative vectors
would lose displacement. Original MoreMouseEvents/MouseEoiHook still own IRQ
continuation; they must observe any admitted relative pending input rather than
creating another interrupt scheduler. Original mouse show/hide/draw paths are
guarded by FULLSCREEN, independently of coordinate input; their integration
must preserve real screen state and guest cursor display counts.

Recovered the bounded relative sample carrier from reference
`codex/t423-original-reference-20260925:src/ntvdm-exe/presentation/mouse_delivery.c`
into `ntvdm-exe/softpc/mvdm_softpc_mouse_input.{c,h}`. It is not yet selected by
the worker build or transport. No UI, guest coordinate policy or IRQ scheduling
is in this candidate. Corrected a recovery detail: for a large sample split
into signed-short pieces, its button transition is delivered at the final
piece, not at the first partial position. Overflow rejects before mutation;
the future ingress must propagate that failure, not ignore it.

`softpc_relative_mouse_queue_test.c` compiles x86 /MT /W4 /WX and passes:
exact 70000/-70000 and INT_MIN/INT_MAX sums, split/button ordering, zero-motion
release, FIFO wrap, unsupported buttons, full-queue unchanged rejection and
explicit cancellation. Executable `build/M0-T423/S7/relative-mouse.exe`.
This is carrier-only evidence; original IS16 coordinate arithmetic, scaling,
route activation/retirement and actual guest/IRQ delivery are not yet verified.

The working tree now defines a pointer-free private relative-input payload in
`product-abi/console_mouse.h`, carried through the existing copied input reply.
It is a distinct private event tag, never an ordinary native MOUSE_EVENT. It
retains signed 32-bit deltas, frame dimensions, left/right state and explicit
enter/move/leave shape. Frontend encode and worker decode validate that shape;
the worker's ordinary SHORT coordinate check does not truncate this private
payload. Direct Console protocol increments from 12 to 13, so old peers reject
the new contract rather than silently misinterpret it. It is not yet emitted
by frontend or consumed by original mouse/IRQ code; native forwarding guards
and complete peer rebuild remain required before activation/publication.

Extended `console_frontend_test.c` uses the production dispatcher with a
copied-input callback to verify exact INT_MIN/INT_MAX motion, dimensions,
malformed buttons/leave and old-version rejection. Formal x86 compilation
succeeded; private-desktop `m0-t423-s7-relative-wire-r1` exited 0 with both
relative-input and existing real Console stream/scroll/cursor/error PASS rows.
Evidence: `O:/winnt/logs/m0-t423-s7-relative-wire-r1.txt` and `.txt.console.txt`.
This verifies framing/encoder, not full worker decode execution or guest input.

### Relative bridge boundary tests

The candidate `mvdm_softpc_mouse_bridge.{c,h}` now takes explicit zero-initialized
worker-owned state rather than introducing another process-global current
machine. It scales relative frame pixels to original VirtualX/VirtualY, retains
signed fractional remainders and commits producer state only after enqueue
succeeds. The only interrupt notification is the original DoMouseInterrupt
under original ICA locking; no new interrupt scheduler is introduced.

Review found and corrected a candidate lifecycle error before production
selection: retiring a consumed LEAVE must not clear already accepted later
ENTER/MOVE records. `mvdm_softpc_mouse_leave` retires only the consumer route;
`mvdm_softpc_mouse_cancel` remains the explicit whole-state cancellation.

Reproducible focused command, from `build/M0-T423/S7` after the VS2022
VsDevCmd x86 / host_arch=x64 environment (paths below are repository-relative;
resolve them against the repository root, not the build working directory):

```text
cl /nologo /MT /std:c11 /W4 /WX /DCPU_40_STYLE /I <repo>/src
  <repo>/tests/component-integration/softpc_relative_mouse_bridge_test.c
  <repo>/src/ntvdm-exe/softpc/mvdm_softpc_mouse_input.c
  <repo>/src/ntvdm-exe/softpc/mvdm_softpc_mouse_bridge.c
  /Fe:relative-bridge.exe
relative-bridge.exe
```

Compilation and execution returned 0. The test asserts text half-unit positive
and negative remainders, 320-to-640 raster/virtual scaling, held-button idle
polls, explicit release, rapid LEAVE/ENTER/MOVE survival, separate state,
invalid transitions/buttons, absent virtual extent, arithmetic overflow,
full-queue rejection without state mutation or IRQ notification, and explicit
cancellation. ICA and IRQ are test observers, not real CCPU execution.
Executable SHA256:
`D24290DF53FFC743BE8932728CF0DCC4F1C34CFFB3175250F1689C3328F4DAD1`.
The carrier test was rebuilt against the added action field and again passed;
SHA256 `73ED978E6E2F6045A07930C7595D0A602E2E2B522B181E3DEAB15B4643B1E7F0`.

No library diff or production publication was made. This candidate still needs
worker state binding, original IRQ/coordinate/cursor hooks, frontend emission,
native-input isolation, and real DOS text/graphics acceptance. In particular,
the carrier's exact large-vector sum is not proof that original IS16 coordinate
addition cannot wrap; do not infer that property from these boundary tests.

### Frontend DOS conversion and worker-state selection

`window_mouse.c` now contains the DOS-specific copied-record converter beside
the native converter. It preserves KVM's content-scaled relative dx/dy and
frame dimensions without a second client-size scale. First input queues ENTER
and MOVE atomically; focus reset releases buttons without retiring the source;
source retirement emits LEAVE; a failed sink does not change converter state.
It does not own guest position, ranges, cursor shape or drawing. Native Console
conversion continues using its separate native coordinate contract.

The expanded `frontend_window_mouse_test.c` compiled with MSVC x86 /MT /std:c11
/W4 /WX /D_CRT_SECURE_NO_WARNINGS, include roots src and src/frontend-exe, linked
with window_mouse.c, and ran successfully. Existing native assertions and DOS
copied-record/INT_MIN/MAX, malformed shape/source, failed sink, reset/release,
retirement/re-entry and dimensions assertions all passed. Artifact:
`build/M0-T423/S7/frontend-mouse.exe`, SHA256
`6BAC4BD9DB30AC271083099DAC807EBCA5FD1257D2127738D4C400317D9DA42E`.
These are converter tests, not guest runtime evidence.

The bridge state is embedded in the existing worker Console client and reached
through the already bound session; it needs neither a process-global current
mouse nor a new teardown registration. Existing session disposal rejects live
thread bindings before invoking client teardown. The formal graph now selects
the relative carrier and bridge, but original mouse consumers and frontend
emission are still pending. The imported shared library tree has no diff.

Formal graph generation succeeded. The subsequent incremental ntvdm.exe and
frontend.exe build remains unverified: shell session 84524 / Ninja PID 20424
was confirmed live with an empty `build/M0-T423/S7/mouse-build.log`. Repeated
polls did not report completion. Reading its parent/child details via CIM was
denied, and the escalated diagnostic request was rejected because this
environment forbids escalation. This is not a compile pass or source-error
diagnosis. Do not restart merely from the observation timeout or replace the
live package. No S7 candidate has been published.

### Original-coordinate ingress constraints

The retained extracted TextScale/LimitCoordinates/EmulateCoordinates bodies
were rechecked byte-for-byte against extraction from the current mirror;
combined SHA256 `7e8b2aa4bda6d83efad459ca5cf69e8be4c0074166584acbea166e238e0d68c8`.
The expanded original_mouse_coordinates_test.c was compiled directly using
that verified build-only header with x86 /MT /std:c11 /W4 /WX /DCPU_40_STYLE.
All 17 assertions passed; `build/M0-T423/S7/coordinates-boundary.exe` SHA256
`C0A48A829890E21ECADCC7148C195CC690FF6560037A54B25BDB204C1A4A7591`.

New observations constrain the pending production hook:

- A zero-motion call first consumes original pending reset/INT33 function 4;
  the following motion then survives. At guest-set 100,50, delta 7,9 yields
  107,59, unlike passing that delta with the pending-set operation itself.
- Signed-short queue pieces alone are insufficient: from x=100, adding
  INT16_MAX wraps before original LimitCoordinates and yields x=0 rather
  than the right edge. This is a characterization of unchanged arithmetic
  under an extreme authored input, not proof of a normal OpenNT runtime bug.
  The new relative ingress must account for this boundary before activation;
  do not modify the original routine merely to make the candidate fit.

Source review also confirms FlushMouseEvents is used both for cancelled IRQs
and original DOS/native handoff. Route retirement, queued future re-entry and
that explicit cancellation must not be conflated. The formal Ninja process
was re-polled through the same shell session and remained live without output;
no duplicate build or candidate publication was started.

### Original worker mouse hooks, candidate only

Ninja 1.13.2 `--version` and read-only `-n ntvdm.exe frontend.exe` both
succeeded. The latter enumerated 16 pending actions; it is not a compile pass.
The earlier actual build remained live. To edit its selected source inputs
without concurrent compilation, the agent explicitly cancelled only verified
Ninja PID 20424 (matching name and start time); session 84524 then ended with
exit 1. This is deliberate candidate cancellation, not a diagnosed timeout
cause or an automatic retry. No other build or user process was terminated.

MVDM-HOST-DIV-318 now registers the candidate original worker hooks:

- nt_event dispatches the private copied mouse tag and reports ingress failure
  through its original DisplayErrorTerm contract. Original pending/EOI and
  FlushMouseEvents also observe/cancel the worker-owned relative queue.
- nt_mouse selects relative conversion when active/pending and otherwise keeps
  original absolute Console input. Original EmulateCoordinates synchronizes
  pending reset/position before movement. The adapter bounds the intermediate
  signed-short coordinate sum, while original LimitCoordinates owns guest
  limits and the raw motion counters retain delivered displacement.
- Original show/hide, guest-set-position, movement and conditional-hide paths
  admit relative Window presentation without changing sc.ScreenState. Original
  ResetMouseOnBlock restores the pointer before native handoff. Guest cursor
  code and display counts remain original; no host mouse driver is substituted.

New adapter `mvdm_softpc_mouse_guest.c` plus changed original nt_mouse.c and
nt_event.c compiled to build/M0-T423/S7 objects with x86 /MT CCPU40 using the
formal graph's flags. Results are compile-only, not worker/guest acceptance.
Retained attempts:

- `mouse-guest-build.log`: initial environment import did not resolve cl;
  retry in the same VsDevCmd shell reached compilation and exposed missing
  original host_rrr/nt_uis declarations. Added those original includes.
- `mouse-guest-r2-build.log`: compile 0.
- `nt-mouse-r2-build.log`: missing adapter include search path; corrected the
  mirror include to its existing repository-root-qualified path.
- `nt_mouse-r3-build.log`: compile 0.
- `nt_event-r3-build.log`: manual use of common host flags omitted its existing
  per-file thread_start_compat/ntexapi forced includes; not a source regression.
- `nt_event-r4-build.log`: exact `ninja -t commands obj/host/nt_event.obj`
  command, changing only object output to the S7 build directory, compiled 0.

The new file is selected by the generator; a fresh generated graph and final
link remain required. Frontend emission, old-consumer input disposition and
real DOS text/graphics mouse gates remain open. The imported libraries are unchanged;
there is still no S7 publication, commit or runtime-completion claim.

### Frontend emission candidate

DOS Window input now dispatches to the DOS converter. Before publishing a DOS
frame, frontend obtains the approved library's existing frame-size result and
sets relative-input geometry. New consumers defer input polling until geometry
exists. Window-to-Console route retirement queues LEAVE to the still-active DOS
owner. Original DOS block already flushes worker mouse IRQ input; its frontend
handoff now drops that old consumer's queued private/absolute mouse records,
preserves non-mouse typeahead and resets the relative producer. Native backend
input rejects a private DOS tag across the whole batch before native delivery.
These changes remain candidates requiring handoff and guest tests.

Path clarification: imported libraries live at `src/frontend-exe/lib`, not
`src/lib`. A targeted `git diff --name-only -- src/frontend-exe/lib` is empty.

Regenerated the formal graph. Developer-environment capture was found to emit
both PATH (with VS tools) and Path (original path); importing both overwrote the
toolchain path. The latest invocation preserves the existing path and explicitly
prepends the verified HostX64/x86 compiler directory. Get-Command resolves cl.
This explains the environment-import failure, not yet the earlier Ninja wait.
Current build shell session 31024 remains live with empty
`build/M0-T423/S7/mouse-wired-build.log`; Ninja PID 33708 was observed live.
An additional Ninja PID 11000 appeared during the preceding failed environment
attempt, but its parent/command association has not been verified. No speculative
process-tree cleanup was performed. Final build and runtime results are pending;
no live-package update or source commit has occurred.

### Observation evidence failure and current build disposition

The earlier session-31024 build and the separate minimal Ninja child-spawn
probe were explicitly cancelled; their shell handles returned exit 1. They
are no longer live waits. The minimal Ninja rule was only `cmd.exe /d /c exit
0`; its hang is not evidence of a product source defect. No unrelated Ninja
process was terminated. Direct MSVC compilation/linking continues to work.

Exact generated-graph commands, with object outputs redirected to the S7
build root, compiled the changed native frontend/backend, fixture, mouse,
frame and input-queue units. A separately linked integration fixture,
`build/M0-T423/S7/native-frontend-wired.exe`, has SHA-256
EAB30BE054A9C12E27E58E3051E9D1DF087E535B0C5D0DFA3377DAF41889EC0A.
It is not a formal product package. Its new assertions inspect private DOS
ENTER/press/release records and require their exclusion from native handoff
while preserving returned L/M keyboard typeahead.

Private-desktop observer attempts r1-r3 returned zero without producing the
requested `O:/winnt/logs/m0-t423-s7-dos-native-wired-r*.txt` reports. They are
not passes. Source review found that the observer ignored failed report
creation and final writes. It now checks report creation before any target
or private desktop is started, checks final stream/close errors, and returns
70 on report failure. This is a test-tool fix, not a product workaround.

The x86 /MT rebuilt observer `build/M0-T423/S7/observer-evidence.exe` has
SHA-256 4E4AA1117B2259C025A87D419DE0374AD9D51D02754F43A7991729FE1267DD19.
Attempt r5, using the same fixture with `MVDM_OBSERVER_PRIVATE_DESKTOP=1`
and `--observation-timeout-ms 60000`, reports:

```text
observer: cannot create report O:\winnt\logs\m0-t423-s7-dos-native-wired-r5.txt (errno=13)
observer_exit=70
```

The immediate evidence blocker is thus denied report-file creation, not a
passing integrated mouse run. No alternate runtime-log destination or desktop
interaction bypass is used. The earlier observer's missing-argument control
returned 64 and its nonexistent-target control returned 67; the bounded
argument-validation control returned 68. None proves the product fixture ran.

The existing frontend mouse, 17-case original-coordinate and relative-bridge
executables were rerun successfully. They remain converter/arithmetic and
mocked-ICA/IRQ evidence only. The actual `src/frontend-exe/lib` diff is empty.
Full worker/guest mouse, handoff, product build/regression and coherent
publication gates remain open. O:/winnt has not been updated by this work.

### Production mouse adapter composed boundary fixture

Added `tests/component-integration/softpc_relative_mouse_guest_test.c`.
It links the actual `mvdm_softpc_mouse_guest.c`, bridge and queue providers
with unchanged extracted TextScale/LimitCoordinates/EmulateCoordinates bodies.
Only memory reads, pointer show/hide, thread binding and ICA/IRQ notification
are mocked. This checks the adapter itself, not a handwritten substitute for
its implementation. It still does not execute a guest or prove cursor drawing.

Cases cover inactive fallback, copied ENTER and initial position, movement
without invented presses, held-button idle polling, guest-set-position followed
by movement, reset followed by movement, positive/negative signed-short
displacements, original bounds with raw counters retained, LEAVE followed by
queued re-entry, hidden-pointer preservation and explicit cancellation.
The initial test failed only its final notification-count assertion: nine
submitted records were incorrectly expected to yield ten notifications. All
preceding assertions had passed; the corrected expectation is nine. NDEBUG
is explicitly prohibited by the fixture.

Reproduction uses the generated graph's
`ninja -C build/M0-T423/S1/restart-formal-x86 -t commands obj/adapter-softpc/mvdm_softpc_mouse_guest.obj`
compile command. Replace only its source with the fixture, object output with
`build/M0-T423/S7/mouse-guest-test.obj`, and add the existing
`build/M0-T423/S7/original-coordinates` include directory. The extracted header
is generated by `verify-original-mouse-coordinates.cjs`; its full contents were
rechecked against the current mirror, SHA-256
7e8b2aa4bda6d83efad459ca5cf69e8be4c0074166584acbea166e238e0d68c8.
Compile the actual guest adapter with that same formal command to
`build/M0-T423/S7/mouse-guest.obj`. Compile bridge/input separately with
MSVC x86 `/MT /W4 /WX /c /I<repo>/src`, with explicit S7 object outputs.
Link these four objects and kernel32.lib using the x86 linker.

The rebuilt `build/M0-T423/S7/mouse-guest-test.exe`, SHA-256
0735C186B2CA6B7619C43A0786DF6182628B2877E877ACE8CC3DF862C5988B5E,
returns 0 and prints the production-adapter boundary PASS. The formal header
set retains its existing NTVDM macro-redefinition warning; no mirror or library
change was made to suppress it. No product publication or guest acceptance
is inferred. Runtime report access and complete product gates remain open.

### Final-link counterexample: cursor owner not yet recovered

Replayed the pending commands reported by the formal graph's
`ninja -n -v frontend.exe ntvdm.exe native-console-frontend-test.exe`, in
dependency order, using the same x86 VsDevCmd environment and original outputs.
This avoids the stalled Ninja child scheduler, not any source/permission gate.
The first 20 commands completed; step 21 failed at the ntvdm final link:

```text
softpc-bindings.lib(mvdm_softpc_mouse_guest.obj): LNK2019 unresolved _mouseCFsysaddr
ntvdm.exe: LNK1120
```

The build transcript is `build/M0-T423/S7/graph-replay-build.log`.
The replay shell (54520) is terminal with exit 1; do not describe it as waiting.
Direct replay does not update Ninja dependency/log metadata and must not be
represented as an up-to-date Ninja acceptance build.

Source attribution: mouse_io.c defines mouseCFsysaddr in its MONITOR-only
registration block. The selected CCPU40 graph does not compile that block.
nt_mouse.c host_show_pointer/host_hide_pointer bodies are X86GFX-conditional;
the general mouse_io.c cursor_display/cursor_undisplay bodies are excluded by
NTVDM. Thus the earlier proposal to use these calls as already-working guest
cursor drawing is unproved and currently not a composed provider. The new
fixture's mocked symbol/show-hide functions cannot establish that production
closure. Its arithmetic/input results remain valid only within that mock scope.

Do not add a dummy mouseCFsysaddr, enable MONITOR/CPU30, patch guest media or
modify shared libraries to satisfy this link. Next inspect the original CCPU
software text/graphics cursor owners and their state/layout dependencies before
selecting the smallest source-shaped Window binding. This is an S7 integration
gap, not an original CCPU execution defect. No candidate was published.

Separately rechecked 44 approved library files and their license bytewise,
and ran verify-frontend-link-ownership.ps1 including five deliberate leakage
controls: all pass. Those checks do not waive the failed worker final link.

### Remove the disproved MONITOR binding; software cursor recovery still open

Source review confirms the original CCPU software providers exist in
`base/keymouse/mouse_io.c`: software_text_cursor_display/undisplay,
graphics_cursor_display/undisplay and EGA/VGA variants. Its cursor_display,
cursor_undisplay and show/hide-count blocks are excluded by NTVDM; nt_mouse.c
host show/hide uses X86GFX-only MONITOR driver registration instead. The current
candidate therefore cannot assume either set is already production-connected.
The mouse_int1/2 callback-return drawing, cursor_flag, guest set-position,
mode changes, range/conditional-hide and saved-background lifetimes must be
reviewed together before restoring the software path for Window presentation.

Removed the adapter's invalid mouseCFsysaddr dependency and attempted host
show/hide calls. Removed the corresponding uncomposed X86GFX/fullscreen mirror
condition changes and native-handoff no-op hide. Original conditions are back;
no dummy symbols, fake hardware fullscreen, MONITOR, guest patch or library
change was added. Copied input, relative coordinates and button delivery remain
the candidate ingress, not cursor-visibility completion. The focused fixture
no longer supplies mock cursor symbols that could hide this missing provider.

The exact formal graph's changed-object/archive/final-link commands now succeed
for ntvdm, including its VdmTib storage check. The final post-cleanup worker
SHA-256 is A2076F017876B6B12BF18DD8834FDA8AD9186F2E0BCF77B446FD59D0ABE0A8B5;
transcript `build/M0-T423/S7/worker-link-r3.log`. This supersedes the intermediate
86C90D95 worker and the previous failed link, not the pending runtime gates.
The separately linked frontend is
617F683A195384B6E86493F0F131ACC950E899101C2D8D7AA9B30D4CCABE3BDD and its
formal native-frontend fixture is
345705D7940D43C317392DB52FA711C1D63A062C250DBAFEC902C7E65291D1DD.

Recompiled the updated guest-adapter fixture and linked the actual current
formal guest-adapter object; it passes with SHA-256
07843B5F3434E7A63B69A4866EDD5CF6451DBD10C441EB638CBDC70FB043E0E4.
Its result explicitly excludes cursor drawing and uses mocked memory/IRQ.
In particular, manually setting reset/position flags in that fixture does not
prove real guest calls reach those flags: their original X86GFX guards remain
part of the open integration audit. S7 and the full task remain incomplete;
no live-package replacement, commit or push is claimed.

### Reference cursor recovery candidate

Reviewed reference codex/t423-original-reference-20260925 at
286d54a306bcb8e991891fae849b79db540e897a: mouse_io.c, mouse_irq route snapshot,
nt_graph refresh hook and historical cursor-count/video/mouse positive and
negative results. Migrated its original non-MONITOR software count, shape,
geometry and saved-background painting, with the current ICA-protected worker
bridge route replacing the old presentation dependency. Current frontend
ownership and copied input are retained; no guest/shared-library change.
Hooks use DIV-318, not the reference's now-occupied DIV-315.

Formal graph x86 compile commands for mouse_io.obj, nt_graph.obj and
mvdm_softpc_mouse_guest.obj passed; build/M0-T423/S7/cursor-recovery-build.log.
An initial invocation from repository root failed on its relative output
path; corrected invocation used the graph directory and emitted objects only
under build. Archive/link and guest lifecycle checks remain pending.

The x86 /MT /W4 /WX original_mouse_text_cursor_test.c fixture passed masks,
saved-position background restore, video page offset, conditional exclusion
and off-screen exclusion. It uses extracted original painter bodies with
mocked memory/geometry, not a real guest or full drawing-route acceptance.
Outputs: build/M0-T423/S7/text-cursor.obj and text-cursor.exe.
The source changes invalidate the earlier worker artifact identity. No
publication, P delivery or S7 closure is claimed.

### Counter lifecycle and current final link

Source follow-up found that the recovered base mouse_set_position bypassed
host_mouse_set_position on Window, leaving EmulateCoordinates' private last
position stale. NT mouse_reset also selected counter-reset flags only under
X86GFX. The candidate now calls the original host set-position entry before
base geometry/painting, selects its existing newF4/bFunctionFour branch for
Window input, and resets original confinement/zero-reset flags on Window
reset without accessing MONITOR's guest fast-track variables.

Recompiled mouse_io and nt_mouse, rebuilt their original archives and linked
ntvdm using the formal graph commands. Final x86 link and VdmTib ownership
check pass. Worker SHA-256:
919DE2901B77F383AF1A5AF709CC37BB5EEDCCCCA611E4A6C20AE03F261FFE3F.
Transcript: build/M0-T423/S7/cursor-position-build.log. This supersedes the
intermediate cursor-recovery link 9DF02303 and previous A2076F01 candidate.

Recompiled softpc_relative_mouse_guest_test against the actual formal guest,
bridge and input objects. Existing movement/button/reset/position/overflow/
release checks pass, along with new locked-route snapshot assertions before
ENTER, after ENTER and after LEAVE. Reset/position flags in this fixture are
still supplied by the fixture; the real guest call wiring requires runtime
verification. No broad regression or live publication is inferred.

### Stationary Window route and recovered guest probes

Migrated the reference's authored mouse-cursor-count.asm, mouse-cursor-video.asm
and window-mouse.asm byte content into the existing tests/observation directory.
These are test programs, not replacements or patches for immutable guest media.
NASM -f bin produced build/M0-T423/S7/*.com with SHA-256 respectively:
11314D379848BE456ED0D01BF9E54C2D44896E674297E7BAF9179D21519F7662,
642B672E5D8087E761B6E1616214BDFC3A5661916211F6494CC27C1A0CA45955,
D70588CE33713DA3D4421887F30DEC470763CE157484901766531AD2F56AA2E5.
Their historical positive results are not current run results; the input
injector must target frontend, not the old worker-owned Window.

Source audit found ENTER was previously emitted only on the first mouse event,
so stationary guest ShowCursor could not enable the software painter. The
frontend now emits an idempotent zero-motion/button-free ENTER on Window route
selection with known geometry, or after the first frame supplies geometry.
No synthetic physical mouse event or shared-library change is involved.
The converter fixture adds no-geometry, failed-sink, stationary ENTER,
duplicate ENTER and LEAVE assertions. All native/DOS converter cases pass
under MSVC x86 /MT /std:c11 /W4 /WX. The first compile omitted /std:c11 and
failed on the library's static assertion; corrected compile passed without
library changes. Real guest and full handoff acceptance remain pending.
Formal graph compilation of the two affected frontend objects and final
frontend link pass; transcript build/M0-T423/S7/mouse-route-build.log.
Frontend SHA-256: 1ADA784168026F3D6B1DB396B2DF2FAE01C7D968555DCC959AC01ED5A53A1919.
No seven-file publication or P delivery has occurred.

### Pre-Window guest position and integration preflight

ENTER previously seeded EmulateCoordinates from old_x/old_y (the last IRQ
cache). Original base INT33/4 can update cursor_status before the next IRQ,
so the adapter now seeds from that authoritative guest position instead.
The actual-adapter fixture adds differing base-position/IRQ-cache values,
then checks ENTER and subsequent relative movement preserve the guest's new
position. Recompiled production object and fixture pass; memory/IRQ remain
mocked. The previous worker final-link hash predates this change.
Rebuilt the affected archive and final worker with the formal graph commands;
link and VdmTib check pass. Current worker SHA-256 is
2D5BAC0453CD5B74ADFDB1E1FC226F4D187E7F80C9EE7BD6E7FF498ABE081DCD;
transcript build/M0-T423/S7/cursor-enter-link.log. This is still a candidate.

The fail-closed observer was re-run with private desktop requested against
the formal native-console-frontend-test target and report
O:/winnt/logs/m0-t423-s7-route-preflight-r6.txt. It again returned 70 with
errno 13 before starting its target. No runtime result or report was produced;
log access remains unavailable. Do not bypass this by writing runtime logs
elsewhere, and do not count the unexecuted fixture as passed.

### Owner-approved Logs2 fallback resolves observation access

Owner explicitly authorized retrying logs and creating O:/winnt/Logs2 as a
fallback. Normal sandbox retry r7 still failed with errno 13; normal creation
of Logs2 was denied. A scoped elevated execution then created Logs2 and ran
the private-desktop observer without changing ACLs or switching the desktop.
Report O:/winnt/Logs2/m0-t423-s7-log-retry-r8.txt records result=exited and
exit=0; its .console.txt contains actual native-frontend fixture PASS output
for CAF/X routing, returned typeahead, helper lifecycle and Window mouse
delivery to native ReadConsoleInput with button/focus release assertions.
The execution session completed. Log access is no longer an active blocker.

This run used the previously linked native-console-frontend-test.exe, not a
fixture relinked against the latest stationary ENTER change. It proves that
the fallback and that retained fixture execute, not current-source S7 closure.
Rebuild the fixture and continue current-source guest/integration verification.

### Current-source native integration and real DOS cursor runs

Relinked native-console-frontend-test.exe against the changed production
frontend/window objects: SHA-256
3CCAA247635F6C1C9F732FA3B04CF988FC91542C7E516A5F0FCD2F7C9B0A5DCD.
Private-desktop r9 (m0-t423-s7-current-frontend-r9.txt in Logs2) records
result=exited, exit zero and captured PASS markers including actual Window
mouse delivery to native ReadConsoleInput, button pairs/focus release,
DOS-returned typeahead and helper failure/teardown cases.

Replayed all 27 pending formal graph commands for the five executables,
then the VDMREDIR final link. WOW32 graph reports no work, and its retained
hash matches S6. Build transcript: build/M0-T423/S7/package-refresh-build.log.
Staged only below build/M0-T423/S7/p with unchanged S6 guest/configuration;
Q: maps to that candidate, leaving the existing R: S6 mapping untouched.
No live runtime package was overwritten. Candidate identities:

| File | SHA-256 |
| --- | --- |
| run16.exe | AE47B09DC5FF07500E424B19AAD1398E04B9E485FA122527B76047B3B21FDAA2 |
| basesrv.exe | 5B5510F1F6D71DD7EC950E119BA2ACC42DD1B601A68B50084ABC08A8B488E8A1 |
| frontend.exe | 0913221E115F4242635AEFE524D3030C5F6C51042BCB2A6F4193DBE63DEF1BED |
| ntvdm.exe | 44C082ACDC0693C03DE0936DB3FF8DD6D2028AE9C0FAD37CD2EAC994F77F49CF |
| monitor.exe | 83CB4E25A2397EC07155B250195BC037EA2133BBD6D2468531D4FF04FE848C7D |
| VDMREDIR.dll | E2E685C0BBB23CD1E8023CE357E591313422401A674EC9BD6B092B39BA37034D |
| wow32.dll | A8ABCF9D4637170075992A9318B0A194C19C8E41895FA2DF7A0AC71B8A8ACCA5 |

Under standing owner authorization, ended exact O:/winnt run16 PID 29636,
ntvdm 32500, frontend 28816/29684 and basesrv 24328 before candidate tests;
image paths were checked with process handles retained. No unrelated process
or desktop was controlled. Existing O:/winnt/tests MCOUNT.COM, MCVIDEO.COM
and WMOUSE.COM hashes match the recovered probes, so no copies were needed.

Real guest runs use observer-evidence.exe, PRIVATE_DESKTOP=1, Q:/run16.exe,
Q:/ working directory, the explicit O:/winnt/tests probe path and 60000ms
observation timeout. Reports and captured Console text under O:/winnt/Logs2:

- m0-t423-s7-cursor-count-r10.txt: exited zero, CURSOR-COUNT-PASS.
- m0-t423-s7-cursor-video-r11.txt: exited zero, CURSOR-VIDEO-PASS. This
  exercises original INT33/VGA memory: displayed cursor changes zero video
  pixels, hiding restores the zero background, and mode 3 returns normally.
  No physical mouse input was needed; stationary route activation is reached.
- m0-t423-s7-window-mouse-negative-r12.txt: exited with guest exit 1 and
  WINDOW-MOUSE-FAIL when no input was supplied, the expected negative outcome.
  The observer process itself returns zero after successful observation;
  acceptance therefore reads the report's target exit and captured marker.

These close the bounded count and graphics draw/erase checks, not full S7.
Real input/callback positive and retirement cases, text cursor integration,
full inherited regression and coherent publication remain required.

### Real guest callback failure, correction and positive/retirement results

Recovered the reference private-mouse-input.c test-only WH_GETMESSAGE hook.
The current observer verifies its desktop name starts NTVDMConsoleTest-,
finds the LibKvmWindow/NTVDM window and checks its process image file identity
against the candidate frontend.exe before installing the hook on that exact
thread. The pinned library context's first field is still kvm_window*, as
checked in frontend-exe/lib/kvm-window/win32/component.c. No library changed.
Injected copied KVM output events traverse actual frontend transport, worker
ICA/IRQ and guest INT33; this does not test physical Raw Input or capture.

r13 returned WINDOW-MOUSE-FAIL with guest exit 1 despite successful posting.
Added stage exits to the authored window-mouse.asm only, deployed separately
as O:/winnt/tests/WMS7.COM without replacing historical WMOUSE.COM. r14
returned stage 4: movement/press/release callbacks arrived and buttons were
released, but X did not advance. Source showed zero-reset and set-position
flags could both remain pending. The two EmulateCoordinates calls then
consumed reset followed by position and discarded the first real movement.
Window INT33/4 now clears its superseded pending reset before setting the
existing original position flag. This alters only the Window relative path.

Recompiled nt_mouse, rebuilt its archive and final worker; VdmTib check passes.
Candidate worker hash is now
DBF559CDDF231DE2E8607164A548DC2CD1100292E449D76F78B83263D65EECBC.
Build transcript: build/M0-T423/S7/cursor-reset-order-build.log. Other staged
files retain the candidate identities above; no O:/winnt product was replaced.

Use observer-mouse.exe with PRIVATE_DESKTOP=1 and
MVDM_OBSERVER_MOUSE_HOOK=O:/winnt/tests/S7MOUSE.dll, Q:/run16.exe, Q:/ cwd,
the explicit WMS7.COM path and 60000ms timeout. The hook posts relative (16,8),
left press then release; MVDM_OBSERVER_MOUSE_RETIRE=1 replaces release with
source retirement. Logs2 reports and captured text:

- m0-t423-s7-window-mouse-order-r15: exited zero, WINDOW-MOUSE-PASS.
- m0-t423-s7-window-mouse-retire-r16: exited zero, WINDOW-MOUSE-PASS.

Both probes require movement in X/Y, a released final button state, and exactly
one press and release. Earlier no-input r12 remains the negative control.
Observer SHA-256: 7A4D6161C1A4667CC21496CC14993E8EA1FD720B0454A8C85D5B0E4F1E99AB6D.
Hook SHA-256: E756BBDC2C7F3FE85ADEBD46871A4D4CC7548C50594FA88CD56F9D665B7C01A9.
WMS7 SHA-256: A60EDF938A866810CC3BD3D92800E72A5BEB73369C12580A14F1BEA8BA337E6A.
Remaining gates include current-worker cursor count/video repeat, actual text
cursor integration, full inherited regression, seven-file publication and P.

### Real text cursor acceptance

Added authored tests/observation/mouse-cursor-text.asm. It waits for a T key
after S7_TEXT_CURSOR_READY; the private-desktop observer sends public Console
CAF, verifies the frontend Window, then sends the key there. The probe uses
INT33 to select a software text mask, show it, move one cell and hide it,
checking B800 memory against both saved background words at each step.
It is test media, not an original guest replacement. No owner desktop touched.

O:/winnt/Logs2/m0-t423-s7-cursor-text-r17.txt reports exited zero; captured
text contains CURSOR-TEXT-PASS and .caf.txt records successful Window opening.
Current worker is DBF559CD as above. Observer-text SHA-256:
3C47A74C7FBB59B76257C9A258D571FBA2611ED4FC028D988438E244E807CFCD.
O:/winnt/tests/MCTEXT.COM SHA-256:
D918F3877A74A0137BDB41620A8A2AF7873041D0ED0351E4DAE73B76A5E87A2D.
Reproduce with PRIVATE_DESKTOP=1, TEXT_CURSOR=1 and WINDOW_INPUT=1 using the
MVDM_OBSERVER_ prefix, Q:/run16.exe, Q:/ cwd, explicit MCTEXT.COM and 60000ms.
The assertion proves guest text cursor draw/move/erase, not physical capture.

### Ordinary Console regression

Current DBF559CD worker/seven-file candidate passed all seventeen ordinary
Console cases using Verify-CommandExitStatus.ps1 with observer-text.exe,
PackageRoot Q:/, physical S7/p, OrdinaryFrontend, S7/G7.COM, 60000ms and
PRIVATE_DESKTOP=1. Reports and the 17-row summary are in O:/winnt/Logs2 with
prefix m0-t423-s7-console17-r18. All expected/actual exits agree; the runner
also checks actual guest text and EDIT return/MEM output. Exit 1 for the
interactive COMMAND cases is the accepted original contract, not failure.
No production changes occurred during this run and no package was published.

The corresponding full Window-input matrix also passed all seventeen cases:
same command plus MVDM_OBSERVER_WINDOW_INPUT=1, prefix
m0-t423-s7-window17-r19 in Logs2. The observer uses public CAF and actual
frontend Window keyboard delivery on an unswitched desktop. The final EDIT
case exited with accepted original COMMAND result 1 and captured editor and
post-editor MEM witnesses. Both r18/r19 runners ended normally; neither is
a still-running observation or an exit-code-only acceptance.

Final-worker repeats with prefix m0-t423-s7-final-cursor-r20 in Logs2 passed:
MCOUNT exit 0/CURSOR-COUNT-PASS; MCVIDEO exit 0/CURSOR-VIDEO-PASS; WMS7 with
no injected input exit 2/WINDOW-MOUSE-FAIL (the timeout stage, expected).
Each checks result=exited plus target exit and captured marker. The current
DBF559CD worker therefore retains both earlier cursor positives and the
no-input negative after the reset/set-position ordering correction.

### Nested, fault and independent WOW frontier retention

Current candidate, unchanged after r20, passed
tests/observation/verify-twelve-target-chain.ps1 with formal BuildRoot,
PackageRoot Q:/, EvidenceRoot build/M0-T423/S7/chains-r21, LogRoot
O:/winnt/Logs2 and prefix m0-t423-s7-chains-r21. Both GGGWDWGGGDWD and
GGGDDWGGGWWD have ordered ENTER/RETURN, direct results, actual character I/O,
two distinct frontend groups and natural retirement. The summary records
frontend identities 25904:2 / 6936:13 and 13908:38 / 2900:52 respectively.

tests/observation/verify-frontend-lifetime.ps1 with observer-text.exe,
PackageRoot Q:/, ProcessPackageRoot build/M0-T423/S7/p, EvidenceRoot
build/M0-T423/S7/faults-r22, LogRoot O:/winnt/Logs2, prefix
m0-t423-s7-faults-r22 and -ExpandedFaults -Window passed all five cases:
normal, frontend, launcher, worker and helper. Unrelated native/session
survival is asserted. Helper loss retains the original DOS pipe-error path;
explicit Terminate completes the DOS wait with 1067. This does not claim
error-free input after a dead helper or recursive native termination.

The separate WOW observer first failed at r23 without a report: its manually
quoted Q:/ root ended in a backslash which escaped the closing quote in the
Windows command line. The test runner now passes ProcessStartInfo.ArgumentList
entries and accepts a hash-checked physical package alias plus explicit LogRoot.
This is a harness failure/correction, not a guest or product result.

Corrected tests/observation/observe-wow-frontiers.ps1, observer-text.exe,
worker-window-snapshot.exe, PackageRoot Q:/, physical S7/p and LogRoot Logs2
completed prefix m0-t423-s7-wow-r24. Separate 4/8/12/16-second snapshots show
WINMINE's localized main window (class/title UTF16 00C900A800C000D7), SOL's
existing out-of-memory dialog, and WRITE's existing insufficient-memory
dialog/MSWRITE_MENU. SYSTEM.INI identity is checked before/after. The targets
remain interactive at the bounded timeout; timeout exit 0x53504354 is not an
application success. These retain the owner-approved headless frontiers only;
SOL/WRITE usability remains unproved and physical gameplay was not exercised.
No owner desktop was switched or controlled. Live publication remains pending.

### Reproducibility, graphics return and source-format review

The checked-in original-algorithm runner now accepts an optional text-cursor
mode and extracts both original software text cursor function bodies directly
from mouse_io.c. It generates only below build and compiles the retained
original_mouse_text_cursor_test.c; this removes reliance on an untracked
manually prepared header. Ordinary sandbox r25 could not spawn cl.exe (EPERM),
so it provides no test pass. Scoped execution r26 passed text cursor masks,
saved-position restoration, page offset and exclusion cases, plus all seventeen
coordinate cases. Commands: node tests/component-integration/verify-original-mouse-coordinates.cjs
build/M0-T423/S7/original-text-repro-r26 text-cursor and the same runner with
build/M0-T423/S7/original-coordinates-repro-r26, in the x86 MSVC environment.
These remain mocked algorithm tests, supplementary to the real guest results.

Verify-CommandExitStatus.ps1 with the earlier held graphics-handshake-r2
VIDTST.COM, Cases direct-graphics-return, PRIVATE_DESKTOP=1 and
GRAPHICS_RETURN=1 (MVDM_OBSERVER_ prefix), the same current candidate and
observer-text.exe passed m0-t423-s7-graphics-r27. Its graphics report verifies
automatic Window, X preserving graphics and frontend, mode-3 text closing
Window with frontend alive, then guest exit zero. The immutable probe hash is
801760058264BB394A16CC58E4C2A61CBC85D49E84BC0F15AA2F9EDEB818EBC0.
The r22 expanded fault runner repeated without -Window also passed all five
cases as m0-t423-s7-console-faults-r28 (evidence root faults-console-r28).

All 44 imported library files plus LICENSE.nxvm still match nxvm-import.json.
The frontend-only link-ownership verifier passes including all five negative
leakage controls. nt_event.c and nt_graph.c new lines were normalized to the
existing original CRLF; normalized text equality was asserted before/after.
Their generated compile commands, host archive and worker link pass again
(mirror-eol-build-r29.log). Fresh worker hash is
608899183B1AD18111D3F5BBD028961DB22447C43BF3491B7813CB053E3A5E03.
Byte comparison against the DBF559CD tested candidate finds only four changed
bytes in COFF/debug timestamp fields (296,297,2711788,2711789), with equal
3325952-byte lengths. PE directory parsing verifies those timestamp locations;
every other byte matches. Preserve the exact tested DBF559CD candidate for
publication, rather than silently substituting the timestamp-only fresh link.

Final converter rebuild r30 initially lacked the src include root and failed
C1083; corrected r31 compiles /MT /W4 /WX and passes native and DOS conversion
tests. A duplicate NDEBUG guard was removed from the test only. The command
adds both /I<repo>/src and /I<repo>/src/frontend-exe, compiling
frontend_window_mouse_test.c plus window_mouse.c inside the S7 build root.

Rebuilt the existing Console keymouse observer/probe using
Build-T420S25KeymouseGuestTest.ps1 in build/M0-T423/S7/keymouse-r32. Its KMTST
hash equals existing O:/winnt/tests/KMTST.COM:
92C642D5F92EC4EA8C255D956977B5075C068B26423287CA4BDB66F81A8F6629.
With observer-text.exe on its unswitched desktop, TEST_RUNTIME_ROOT=Q:/,
MVDM_TEST_KEYMOUSE_SHARED_CONSOLE=1, MVDM_TEST_KEYMOUSE_FOCUS_TRANSITION=1,
and MVDM_TEST_KEYMOUSE_COMMAND=O:/winnt/tests/KMTST.COM, r32 exits zero.
Logs2/m0-t423-s7-console-mouse-r32-guest.txt records real guest reset,
position, movement/down/up callback, disabled callback teardown, PPI, keyboard
and modifier-release markers. The observer then verifies MEM text, prompt and
COMMAND exit 1: keymouse passed=yes stage=command-exit error=0 exit=1
focus-record-transition=injected. This is public Console record injection,
not physical mouse/focus observation. Documentation governance passes.

### Seven-file publication and final format reconciliation

The original OpenNT copies show CRLF for all four changed mirror files.
The inherited LF mouse_io.c and nt_mouse.c were therefore also restored to
CRLF without changing normalized text. Normalized S7 code deltas are
mouse_io +50/-7, nt_mouse +21/-1, nt_event +12/-1 and nt_graph +5/-0;
OpenNT-host is unchanged. Raw full-file line-ending churn is not new logic.
Affected objects/archives and worker link pass (mirror-eol-build-r34.log).
The fresh 5B937815D98ECF7F71376C988A062C198B6B2DFBB5B25905377AE60BABBFD9E4
worker differs from the tested DBF559CD file only at the same four PE timestamp
bytes verified above. All executable bytes match; the exact tested file is kept.

The guarded build-local publish-verified.ps1 initially refused replacement
because O:/winnt/monitor.exe PID 10132 was open. Under standing authorization,
its retained process handle/image path were verified, then that monitor alone
was stopped. Publication subsequently succeeded: all seven tested candidate
hashes match O:/winnt, including the final DBF559CD worker. Recovery files and
before/after manifest are build/M0-T423/S7/pre-publication/publication.json.
SYSTEM.INI, CONFIG.NT and AUTOEXEC.NT match the tested package and retain their
prior live hashes; any existing NTVDM.REG is preserved. No guest/user data was
overwritten. Formal-path post-publication regression is still being completed;
publication alone does not claim S7/P delivery.

### Owner-added S7 executable/component naming

Published Console r33 passed all seventeen cases; Window r35 passed direct
MEM, nested MEM and EDIT-return/MEM. Separate published WOW r36 completed
with WINMINE main-window and SOL/WRITE original OOM frontiers, same headless
interpretation as r24. These old-name observations have finished.

Owner adds the naming change to S7, not a new S: frontend.exe/frontend-exe
becomes ntkvm.exe/ntkvm-exe; basesrv.exe/basesrv-exe becomes
ntsrv.exe/ntsrv-exe; run16 and ntvdm remain. The two directories were moved
with git mv, preserving all WIP and library contents. Production includes,
launcher siblings, process-image checks, active tests and build targets were
mechanically updated with byte-preserving encoding/line endings. Original
BaseSrv symbols/algorithms, service IDL and semantic frontend role stay intact.
The opennt-host vdm.c include changes only its existing project owner path.
Current authorities and live Markdown link targets follow the new directories;
historical evidence prose retains the names actually tested. All 44 imported
library file hashes remain pinned and unchanged.

The regenerated formal x86 graph completed 130 incremental actions for the
renamed product and component fixtures. WOW32 graph regeneration succeeded;
an initial uppercase WOW32.dll Ninja target was rejected (case-sensitive graph),
then actual wow32.dll completed seven incremental actions and linked. The
updated observer-renamed.exe compiles x86 /MT. No new-name candidate has been
published, and the seven-file old-name checkpoint remains usable at O:/winnt.

The 17-case component runner now accepts explicit LogRoot for the authorized
Logs2 fallback. r37 passed five cases then failed native-console-capture-test:
SetConsoleWindowInfo requested a fixed 25-row viewport on a host permitting
only a smaller viewport, returning 87. No product failure is inferred. The
fixture now queries GetLargestConsoleWindowSize and bounds only its test
viewport; its nonzero origin, 300-row backing buffer, exact-copy, 5001-column
scrolling and unchanged-font assertions remain. Rebuilt r38 passed this case
and all seventeen component contracts completed successfully. New-name DOS17
r39 also passed all seventeen cases, including native streams/EOF, MEM,
nested COMMAND, exact exit codes and EDIT return. Window and remaining
renamed-package integration/publication gates are not yet complete; these
results are not final rename acceptance.

### Renamed-package Window and nested-chain verification

Window DOS17 r40 completed all seventeen text-gated cases with the same
observer-renamed.exe, S:/ package and physical renamed-p identity used by r39;
MVDM_OBSERVER_PRIVATE_DESKTOP=1 and MVDM_OBSERVER_WINDOW_INPUT=1.
This includes EDIT return, MEM and nested COMMAND. No user desktop was touched.

The renamed formal graph rebuilt frontend-chain-cui.exe and
frontend-chain-gui.exe from the current caller source and renamed client
dependencies; the input fixture was already up to date. An initial CMD quoting
error did not run the compiler; the corrected x86 VsDevCmd/Ninja invocation
completed three actions successfully. verify-twelve-target-chain.ps1 r41 used
the rebuilt fixtures, S:/ and build/M0-T423/S7/rename-chains-r41. Both approved
GGGWDWGGGDWD and GGGDDWGGGWWD chains passed 24 ordered ENTER/RETURN events,
direct-target results, actual character I/O, authenticated within-group
identity, separate frontend groups, retained outer stack and natural retirement.
Logs use O:/winnt/Logs2/m0-t423-s7-rename-chains-r41 prefixes.

All 44 imported library files still match nxvm-import.json SHA256 entries.
An active src/tools/tests non-document search found no former executable or
component names outside excluded historical/legacy/library material. Original
BaseSrv API symbols remain unchanged. The no-I/O fault harness was rebuilt
as x86 /MT /O2 with the renamed ntkvm process identity; its outputs remain
under build/M0-T423/S7 and its copy is in renamed-p/tests, not the live package.

verify-frontend-lifetime.ps1 r42 (-ExpandedFaults -Window) and r43
(-ExpandedFaults, Console) each passed normal/frontend/launcher/worker/helper,
including their real fixture output assertions, original fatal-error handling
and unrelated-session survival. Evidence directories are rename-faults-r42 and
rename-console-faults-r43; Logs2 prefixes are m0-t423-s7-rename-faults-r42 and
m0-t423-s7-rename-console-faults-r43.

observe-wow-frontiers.ps1 r44, same candidate and worker-window-snapshot.exe,
retains each separate frontier: WINMINE main class/title 00C900A800C000D7,
SOL OOM text 00C400DA00B400E600B200BB00B900BB, WRITE Not enough memory.
The three reports are interactive timeouts, not successful application exits.
Original SYSTEM.INI is unchanged; this does not claim gameplay or SOL/WRITE
usability. Prefix: m0-t423-s7-rename-wow-r44.

Real mouse r45 count returned CURSOR-COUNT-PASS/guest exit zero but the
observer failed. The build-local orchestration used SetEnvironmentVariable
with null, leaving empty control entries that triggered optional observer
checks. Replacing that cleanup with Remove-Item Env: removes those entries.
No product code changed; r45 remains failed harness evidence. Fresh r46 passed
all five cases: MCOUNT count, MCVIDEO graphics draw/erase, MCTEXT B800 text
draw/move/erase, WMS7 movement/button release and WMS7 source retirement.
Each requires exited zero plus its actual captured guest PASS marker.
The same authored media and S7MOUSE.dll are used; optional observer controls
and commands are retained in build/M0-T423/S7/verify-renamed-mouse-r45.ps1.
Logs2 prefix: m0-t423-s7-rename-mouse-r46.

Verify-CommandExitStatus.ps1 r47, Cases direct-graphics-return, the retained
S6 graphics-handshake-r2/VIDTST.COM and GRAPHICS_RETURN=1 passed the full
automatic Window / X-preserves-graphics / mode3-returns-Console handshake,
then exited zero. Prefix: m0-t423-s7-rename-graphics-r47. Publication remains
pending; old-name O:/winnt is still the recoverable usable baseline.

### New-name publication checkpoint

No-input WMS7 r48 produced WINDOW-MOUSE-FAIL and exited 2: source stage 2
means the required movement/button callbacks never arrived, as expected.
The first orchestration assertion mistakenly expected legacy exit 1; source
inspection and the retained report establish the precise stage-2 negative.
It is not a product regression and no guest or source was changed.

Console keymouse r49 reused the unchanged authored KMTST and r32 observer,
with TEST_RUNTIME_ROOT=S:/, SHARED_CONSOLE=1 and FOCUS_TRANSITION=1 under
MVDM_TEST_KEYMOUSE_, and explicit COMMAND=O:/winnt/tests/KMTST.COM.
The renamed top-level observer exits zero. Captured guest text proves PPI,
reset, position, keyboard/modifier, mouse callback, modifier release and
disabled-callback teardown; subsequent MEM and COMMAND exit 1 complete.
The capture ends keymouse passed=yes exit=1. This is injected Console input,
not physical mouse/focus observation.

publish-renamed.ps1 verified all seven candidate hashes and unchanged
SYSTEM.INI/CONFIG.NT/AUTOEXEC.NT, preserved NTVDM.REG, and verified no live
package processes before copying. Full preceding old-name package/configuration
was saved under build/M0-T423/S7/pre-rename-publication. After verifying new
files, only the backed-up O:/winnt/frontend.exe and basesrv.exe were removed.
The coherent new-name package is now published; rollback copies and exact
before/after hashes are in pre-rename-publication/publication.json.
No guest, shared library or configuration was changed. Published Console17
r50 is running; this checkpoint is not commit/push or S7 closure.

| Published file | SHA256 |
| --- | --- |
| monitor.exe | C3F3300C7105110D038B6BD58067A9456E8F263457166DD1E39B91E0D1005967 |
| ntkvm.exe | 745C2F4F9908AFBFE50E7515EB3C90854E6FE39062B2D43203954CB627A4CAD3 |
| ntsrv.exe | 921C47BB53D681704A2C2FB868DCA32A130F7175E8A7AD9B483FBC08334FD02F |
| ntvdm.exe | 2C45F36D8A1F3DA470036ADB91540612B37E96FFBE541E612D3CEC9587B0EBD4 |
| run16.exe | EA5AA8F94613AC6E78D84591EEA0FADE582476EFA267EE75E48C2B336AA27140 |
| VDMREDIR.dll | 7B353B9E3EBDD17EF22EBF51BCA42DB2808F1FD5E7A316ACD999FE1CBB0546A6 |
| wow32.dll | AA084BBCB766246FA189AB9B06CCF37C2A88C9BDB7ECC05D9F6567E04DCC1AE8 |

### Final published-path verification and P1 review

Published r50 passed all seventeen Console cases. Published r51 passed Window
direct MEM, nested MEM and EDIT exit followed by MEM. Published r52 separately
retained WINMINE main-window, SOL OOM and WRITE insufficient-memory frontiers;
all three exact markers were checked again, without counting timeout as an
application success. All seven O:/winnt hashes still match the manifest above.

Documentation governance, staged whitespace check and frontend link-ownership
verification pass. The latter rejects all five deliberate ownership leaks.
Review covered copied mouse validation/version13, finite queue/chunking,
relative scaling, original IRQ/coordinate/cursor owners, handoff release and
consumer-specific record filtering, component rename/build/deployment references,
unchanged 44-file library manifest, test witnesses and preserved old-name
history. No original guest change, worker-owned Window or new scheduler is
introduced. New files belong only to adapters/ABI/tests, not mirror directories.
The four mirror files retain original CRLF; normalized code deltas are recorded
above. The OpenNT-host rename is one include-path substitution only.

Owner-approved concurrent GUI/ConPTY/root-search proposal and queue edits are
included without implementing those later stages. P1 is the production delivery
of S7 mouse plus naming; S7 remains the active packet until sequential closure
and next-stage admission governance is recorded. T423 and the full goal remain
open; physical focus/capture observation remains explicitly owner-waived.
