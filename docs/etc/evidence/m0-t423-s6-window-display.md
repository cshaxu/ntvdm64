# T423 S6 Window display integration

Planning cross-reference correction (2026-09-27): retained entries below that
name S8 as the final audit reflect the sequence at the time of recording.
The current [proposal](../../proposals/proposal-kvm-window-graphics-presentation-001.md)
inserts S8 GUI launch/wait and S9 ConPTY; final audit is S10. This does not
change recorded S6 results or claim those later stages have completed.

## Question and baseline

Implement the owner-approved independent frontend display policy without
reviving worker/root-launcher presentation, rebuilding native execution, or
modifying shared libraries. Baseline is 30308ab72, S5 closed and S4's seven
runtime files still published at O:/winnt. This is an in-progress ledger,
not a P delivery or S6 closure. Later publication/rollback entries below own
the current deployed identity; the opening S4 identity is the admission baseline.

## Source and finite dependency review

Read-only source: O:/repos.hobby/nxvm at
52e5da766f335a78f93070de136c421d349ec010. Git reports no modifications in the
four selected component trees. Their READMEs and recursive quoted includes
were inspected. The selected closure is 44 byte-exact files: fifteen C units,
their required headers and four READMEs, plus the original root MIT notice.
The complete per-path SHA-256 and compile selection is
[nxvm-import.json](../../../src/ntkvm-exe/nxvm-import.json).
Original license: MIT, Copyright (c) 2026 Neko; retained as
src/frontend-exe/lib/LICENSE.nxvm, SHA-256
BF37952D329B48CEE37B42D90B96D30DC049940902E42D69338A19CF72003543.

The owner explicitly selected these four libraries. No OpenNT UI replacement
is imported: original guest device/video semantics stay in MVDM, with the
existing copied endpoint. The new product Window boundary has no directly
composable original Console-server implementation in this standalone process;
the admitted nxvm leaf supplies only native presentation and copied input.
No kernel/CSRSS UI import, alternate executor, guest patch or new scheduler.

Dependencies: types is header-only; base sync uses types/Win32; kvm-base uses
types/base sync; kvm-window uses types/kvm-base and public USER/GDI. The link
closure has no nxvm app, machine, Console broker, test library or Linux code.
Clock/process source is not needed. Library files are not locally patched.
The initial graph had not selected these objects. Subsequent production wiring
and the owner-approved SoftPC refresh are recorded chronologically below;
the current manifest is the authoritative per-file import identity.

## Existing implementation recovery

Reference codex/t423-original-reference-20260925 retains the prior Window work.
Its presentation/window_controller.h describes worker-owned callbacks and is
not reusable as an ownership unit. In particular, no worker UI, guest-pointer
callback or old native-console ownership is revived.

Its presentation/native_console_frame.c already performs Unicode Console
viewport rasterization, separate from guest graphics. Reuse its reviewed
conversion rather than invent another renderer, adapting the input to the
current complete hidden-Console snapshot. Audit sizing/error/cursor handling
before reuse: the old fixed 8x14 viewport limit must not silently truncate the
current arbitrary-size Console buffer. Native raster output remains logical
text and cannot force the DOS-graphics policy branch.

Current console_video.c already validates and copies complete indexed graphics;
console_channel.c owns the worker transport; native_console_frontend.c owns
the existing I/O arbitration; native_console_view.c owns native capture and
visible output. These remain the integration points, not duplicate transports.
The shared library callbacks must enqueue copied events only. Destruction and
resource rebinding run on the frontend owner after callback quiescence.

## Checklist

| Contract | Evidence / remaining work | State |
| --- | --- | --- |
| Exact four-library Win32 closure and notices | Manifest, source inspection, x86 link fixture below | Verified bounded import |
| 80x50/43 text, secondary font bank, indexed graphics, CAF make/break | Real imported functions in frontend_window_library_test.c | Unit passed |
| DOS DIB and native viewport conversion | Original-size raster and DOS graphics return pass on 5A8F8EBC. Owner excludes unrequested wide-viewport capacity extension; rejection remains a non-pass boundary, not an S6 gate | Bounded integration passed |
| Native presentation destination | Same hidden backend, alternate visible buffer isolation, actual Window-to-CMD input/output/exit verified below | Bounded integration passed |
| Per-character-session policy and frame classification | Production controller tests, both twelve-target chains, Window four-level pair and independent-session fault matrix verify retained per-session policy | Bounded integration passed |
| Window controller policy/lifetime | Private X/CAF/AltEnter, static graphics, native raster, independent instances and retirement tests pass; actual session input is wired | Bounded integration passed |
| DOS text/graphics and native frame consumers | Copied publisher tests, DOS17, Window EDIT, native return and held graphics probe verify the admitted consumer paths; not an all-video-modes claim | Bounded integration passed |
| Console CAF and Window CAF/AE/X | Actual switching and held-key tests pass; imported INPUT_RESET passes native Window loss-message -> FIFO -> frontend -> key-release and continued-input test | Bounded integration passed |
| Display/input destination changes | Stable Window handoff and DIV-317 correspondence tests retained; 5A8F8EBC passes actual native/DOS return and Window four-level pair | Bounded integration passed |
| Two frontend sessions and nested DOS/native transitions | 5A8F8EBC passes both actual twelve-target Console chains and five Window mixed-fault cases with unrelated-session survival; independent Window controllers pass | Bounded integration passed |
| Close/fault/teardown | Current candidate passes all five S5 mixed-fault cases in actual Window and Console modes, with independent-session survival and worker retirement before cleanup | Bounded integration passed |
| Formal build, DOS17, S4/S5 gates, separate WOW frontiers | 5A8F8EBC x86 build, Console/Window DOS17, component17, twelve-target pair, Console/Window fault matrices, graphics return and retained WOW frontiers pass; published-path checks below | Bounded verification passed |
| Coherent seven-file publication and governance/commit/push | Exact seven-file candidate republished; published Console17, Window return/nested/EDIT and WOW frontiers pass; configuration preserved. P formation follows final checks | Publication verified; commit/push pending |

Window mouse belongs to S7. Physical owner-desktop focus/clipping observation
is owner-waived, not passed. Safe background integration remains required.

## 2026-09-27 import verification

Command: build/M0-T423/S6/build-library.cmd imports VsDevCmd x86/host-x64 then
runs the checked-in
[verification script](../../../tests/component-integration/verify-frontend-window-library.ps1).
All compiler/linker products remain under build/M0-T423/S6/library. Toolchain:
MSVC Win32/x86, /MT /std:c11 /W4, selected Win32 leaf linked with USER32/GDI32.
No window is created, no desktop switched, no guest started in this unit test.

The first runner used Windows PowerShell after VsDevCmd and failed before
compilation because Get-FileHash was unavailable in that environment. Switching
the build-only wrapper to the configured PowerShell runtime resolved the test
environment; no source/library behavior was changed to make a test pass.

All fifteen selected units and the fixture compiled and linked. Observed:
`PASS nxvm x86 closure: 80x50/43, font banks, indexed graphics, CAF make/break, invalid lifecycle`.
The test also rejects text row 51 and malformed graphics stride, checks the
last pixel of row 50 and verifies unchanged rendering produces no damage.
The real Window create entry rejects missing callbacks without allocating a
window. Executable SHA-256:
01E7003C57006D60F84979D6F86BF5D607D5C350360FF8D8A0FD5949FC56E287.
Imported CRT wrapper declarations produce C4996 deprecation warnings; their
original code is retained rather than locally replacing strtok/strcpy.

This proves build/selected pure contracts, not actual Window lifecycle,
foreground activation, runtime DOS/native routing or overall S6 completion.

Final license/hash-gated rerun also passed; captured output is
build/M0-T423/S6/library-verification.log. Its rebuilt test executable hash is
B1C4B721BCA8421ECB31D34F1C77C20A1028A3B4FFA235701F82DEE8273A380D.
Git attributes disable text conversion only in the pinned library tree so
subsequent checkout does not change upstream bytes. Governance and diff checks
pass. Product wiring, runtime verification, publication and P delivery remain
pending; no partially integrated candidate has been deployed.

## Frame conversion and native presentation seam

Recovered the Unicode rasterization body from the reviewed old
native_console_frame.c into frontend-exe/window_frame.c. The retired fixed
viewport record is replaced by current run16_native_frame_info and a bounded
full-buffer snapshot. Preserve GDI non-antialiased glyphs, surrogate/DBCS spans,
palette, reverse video, underscore and cursor inversion. Buffer origin/stride
now uses the actual srWindow over dwSize, never an assumed packed viewport.
No Win32 window is created by this conversion. Its current raster cell size is
the retained 8x14; oversized visible viewports fail explicitly, not crop or
truncate. Production geometry/Window resizing remains an integration obligation.

The DOS converter consumes only console_video.c's completed publication,
unpacks top-down 1-bit/8-bit DWORD-padded rows and preserves the indexed palette.
It retains the last complete publication during a pending transfer; explicit
TEXT yields no graphics frame. No guest/painter/transport semantic change.

frontend_window_library_test.c now exercises these conversions alongside the
real imported library. build/M0-T423/S6/frame-verification.log records successful
x86 compilation and both PASS markers. The frame source SHA-256 is
FBDD28A5DB8DF4D08DF58C15CF89147386AE122EF4A800F2E238491AFEA720AB.

native_console_view.c now offers a finite synchronous Window sink for its same
fully captured hidden-buffer snapshot. A NULL sink preserves the original
visible Console behavior. While selected, it does not write cells/geometry to
the visible Console; callback failure is returned, with FRAME_END still sent.
No backend is recreated, no target completion policy changes and no input
reader is added. The callback must be selected under the existing frontend I/O
lock and copy any retained data. The actual display controller is not yet wired
to this sink, so this is not a claim of a usable product Window.

Incremental formal-cache build: build/M0-T423/S6/build-view.cmd, with output
view-build.log then view-build-r2.log. Native helper integration uses the
existing private-desktop observer, never SwitchDesktop or owner input.

- First full host attempt: O:/winnt/logs/m0-t423-s6-native-window-sink.txt.
  Exit 1 in the subsequent 604-record input-reclaim fixture: record 604 was
  an extra resize notification rather than the injected newer key. Retained
  failure, not called a pass or a guest defect.
- Test-only change moves the new PASS text after restoring the Console buffer;
  it no longer writes diagnostic text into the active small resize viewport.
  Assertions and production implementation are unchanged. A dedicated
  --window-sink case bounds this new seam without depending on later tests.
- O:/winnt/logs/m0-t423-s6-native-window-sink-r2.txt exits 0 and proves snapshot
  delivery, visible H remaining while hidden W changes, returned sink error,
  latest W on return and identical backend process handle.
- O:/winnt/logs/m0-t423-s6-native-host-r2.txt exits 0 through the complete
  native host fixture: resize, exact 604-record reclaim, real CMD 37/23,
  streams/environment, target 41 survival, partial-reply cancellation, bounded
  STOP and raw/processed control-input cases. This successful run does not
  establish why the first attempt gained a resize event. Full inherited S6
  regression remains required; no blanket flaky-test exemption or retries
  have been added to the fixture.

Final host fixture SHA-256:
EFC8519FFE1E0E2C8D0321FEFA1C9A2A94732289AF95B2448ADA9C130419DD6D.
Native view source SHA-256:
61AEE640498B875AE4E1BB536E6E2023135C36EC22795774C04B0C4F41DDFF40.
O:/winnt runtime files remain untouched; formal-cache frontend.exe is now an
unpublished candidate, not the S4 publication identity.

## Frontend-owned Window controller

window_controller.c recovers the earlier controller's close/route/checked-join
contracts without its worker-owned graphics provider, separate input reader or
polling thread. The frontend I/O owner serializes present/select/poll/clear;
library callbacks only forward copied input or record an idempotent console
request and signal a wake. Closing requests carry the Window generation so
an old request cannot close a new instance. Checked destruction retains live
callback storage on failed join. Actual input conversion/queue binding to the
session is still pending, not supplied by a second controller reader.

One controller holds one persistent display value and last complete frame.
An explicit content-owner handoff clears stale pixels but keeps the policy.
Native raster versus actual DOS graphics is an explicit presentation fact,
not inferred from the library's raster carrier. No repaint timeout changes
mode. Polling does not re-copy unchanged cached frames into the library.
The Window's X/CAF/AltEnter assignment is console; it never requests process
termination, DOS completion or pause/resume. With actual graphics still active,
that console policy correctly retains the same Window.

The checked-in frontend_window_controller_test.c refuses to run unless the
current desktop has the observer's NTVDMConsoleTest- prefix. It creates real
library Windows there, sends WM_CLOSE to its own handles, checks disposal and
retirement, preserves a second independent instance, and tests native rasters
and static DOS graphics separately. For CAF/AltEnter it uses a test-only
WH_CALLWNDPROC hook on its own private Window thread to set modifier state
while dispatching synthetic key messages. No SendInput, foreground switch,
shared library patch or production test-injection option. This verifies the
real Window message/matcher/controller path, not physical keyboard focus.

Build scripts: build/M0-T423/S6/build-library.cmd and the repository verifier;
logs controller-build.log and controller-hotkey-build.log. Private-desktop
observation logs at O:/winnt/logs/m0-t423-s6-window-controller.txt and
m0-t423-s6-window-controller-hotkeys.txt both exit zero with exact PASS text.
The expanded test verifies six first-instance retirements and one independent
second-instance retirement, including content handoff/recreation.

Exact final controller source SHA-256:
1E059CB11F8D15AA2F7751ABB15FCDC7325DA2D4E6B042152BF18A56FACE5867.
Test source SHA-256:
779BF9B5A1B6D8720E079F72F61B1F9F089B20435C6EC5592A07030EB97E421E.
Test executable SHA-256:
84FDF4FED54D4C95E51D09F3EF3A37A5E6F20680179958AC6F9510AA36FF71EE.

Remaining integration must connect this controller to the existing session
I/O owner, select the proper Console buffer when hidden/visible presentation
changes, normalize/retire Window keyboard state, and keep exactly one active
input reader. A native helper opened during Window presentation must not seed
from a blank/inactive presentation surface. Nested channel creation must bind
the same canonical Console storage, not whichever CONOUT$ is currently active.
These are implementation obligations before product/runtime acceptance, not
new scheduling or protocol requirements. Seven-file deployment remains gated.

### Canonical-buffer seed binding

The production native view now seeds through its retained output HANDLE via
`run16_native_capture_begin_output`. Capture duplicates that handle for the
bounded lease; it does not acquire the currently active presentation surface.
The existing native-helper `capture_begin` still opens the active CONOUT$
buffer, preserving native applications' screen-buffer switching semantics.
This is not yet the session-wide canonical-handle wiring for new DOS channels.

The actual helper fixture creates a second active buffer containing A while
the retained frontend buffer contains K. Native active capture reads A;
frontend reseeding publishes K to the hidden helper without modifying A.
The isolated case passed in O:/winnt/logs/m0-t423-s6-canonical-seed.txt and its
console transcript. Build: build/M0-T423/S6/canonical-build.log.

Full-suite attempts canonical-host and canonical-host-r2 failed the existing
605-record exact-order check: the first 604 records matched but an additional
host resize record preceded the newer Z. Disabling ENABLE_WINDOW_INPUT did
not resolve it and was reverted. The fixture now runs the unchanged strict
ordering assertions before real resize/active-buffer operations, isolating
those test concerns without filtering production input or weakening equality.
The final canonical-host-r3 report exits zero and retains native CMD 37/23,
stream, target-survival 41, cancellation, STOP and control-input PASS witnesses.
Logs retain both failures; this test ordering change is not a claim to repair
a production input-loss bug. Build: canonical-build-r3.log. Full S6 product
integration/regression/publication remains pending; O:/winnt binaries unchanged.

### Session-wide Console ownership

The production frontend now opens and retains canonical input/output once at
session creation. DOS channel creation duplicates these handles through the
frontend owner instead of opening current CONOUT$. Lazy native backend startup
uses the same retained handles through `run16_native_view_begin_on`; its view
owns duplicates and retains the existing input-mode restore/seed lifecycle.
All duplicates are non-inheritable, and partial failure closes acquired handles.
No execution identity, broker wire protocol or guest code changed.

The existing real channel lifecycle fixture now creates the frontend with K in
its canonical buffer, makes a separate A buffer active, then runs all 85 channel
lifetimes. Every channel must read K, own non-inheritable handles, join on stop
and show no handle growth. Retained external copies remain usable after frontend
teardown. Report O:/winnt/logs/m0-t423-s6-session-console.txt and console transcript
pass with exit zero. Full native host regression also exits zero in
m0-t423-s6-session-native.txt. Build log:
build/M0-T423/S6/session-console-build.log. Both use an unswitched private desktop.

This completes canonical-buffer binding, not Window input/routing or real mixed
DOS/native Window acceptance. Controller composition, keyboard ownership and
all production-P publication gates remain open; the deployed package is unchanged.

### Recovered keyboard binding

Recovered the project's prior physical-key conversion/held-key release carrier
from commit 286d54a306bcb8e991891fae849b79db540e897a,
src/ntvdm-exe/presentation/window_keyboard.[ch], into frontend-exe under the
same filenames. Only owner prefix/header guard and owner comments changed;
no external OpenNT or shared-library source was edited. The old
tests/ntvdm-presentation/window-keyboard-tests.c is now a standalone x86 fixture
at tests/component-integration/frontend_window_keyboard_test.c. These are
project-owned presentation adapters, not original mirror code.

Build/M0-T423/S6/keyboard-recovery-build.log records successful x86 compilation
and assertions for physical scan preservation, modifier sides, repeats,
scanless BMP input, explicit unsupported supplementary text, source identity
rejection, focus-release and retirement without duplicate breaks. The same
verifier rechecks all 44 byte-exact nxvm imports and existing frame tests.
Implementation SHA-256 AE386F3CC01677E61B11E74FEBDC03A591182C8EB6B4F332F24BFE2E4E0844CF;
fixture SHA-256 21AAEF5C9BD4531B56630A4A824EB96A1EEA7D9ACD7BB3354D59F5683B511366.

This recovered carrier is intentionally not claimed to implement native cooked
input: physical KEY records have UnicodeChar zero, while nxvm normalizes
representable WM_CHAR text into physical chords. The native Console binding
must restore the character contract without duplicate DOS translation, and
supplementary native text must not inherit the DOS single-WCHAR limitation.
That binding and source-local event queue/retirement remain open before formal
production selection. Accept/release state must be committed only for records
actually delivered, not for a failed sink. No runtime Window keyboard closure
or publication is claimed by these unit tests.

### Native cooked character binding candidate

Added a frontend-local native conversion entry alongside the recovered DOS
physical conversion. Original OpenNT/guest keyboard translation remains in
the worker; native cooked input instead uses Windows ToUnicodeEx with the
captured layout and delivered modifier facts. This is a necessary new
presentation binding: the imported nxvm library deliberately emits physical
keys without a duplicate WM_CHAR stream, and its shared source is unchanged.
No local character/layout table or dead-key composition algorithm is introduced.

The converter is restricted to one input-owner thread, outside the Window
message pump. Windows owns its pending dead-key state; layout changes and
source/backend retirement must reset that translation state without sending
the clearing space to a target. Native TEXT supports UTF-16 surrogate pairs
instead of inheriting the DOS BMP-only conversion limit. Keyboard messages
are still not connected to production transport; queue ordering, native
Alt-numpad/IME behavior, delivery failures and backend switching need further
integration evidence before full input acceptance.

API reference: [Microsoft ToUnicodeEx](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-tounicodeex).
It documents stateful dead keys, multi-unit UTF-16 output and interference
with TranslateMessage; those are why this binding cannot execute casually on
the Window message thread or assume a single WCHAR for all native text.

The expanded fixture passes ordinary lowercase/shifted text, Enter, key-up,
supplementary Unicode, US-International acute+e composition and retirement
clearing before the next e. On an unswitched private desktop, actual Windows
LINE_INPUT ReadConsole reads exactly aB CR LF from converted records. Reports
O:/winnt/logs/m0-t423-s6-native-keyboard-r2.txt and its console transcript exit
zero with the cooked-input and dead-key witnesses. Build log:
build/M0-T423/S6/native-keyboard-build-r2.log. This is not yet a real CMD-through-
Window claim. Implementation SHA-256:
87F20E29819DFDB3106982F9C260D90B9BF1018777DD6F8C12C44879DFEE164B;
fixture SHA-256 DBDA7E0A7640B1A5EA0C020CE71065AD2B67A6FCE461EF72FCE31659E1BACC73.

### Window callback FIFO and consumer-thread delivery

Recovered window_input_queue.[ch] from the same reference commit's
src/ntvdm-exe/presentation directory into frontend-exe. Removed its obsolete
handoff-generation, route, focus and consumer-pending interfaces and duplicated
source-pointer clearing; retained copied FIFO, UI-time modifier/layout capture,
explicit overflow/closure errors and closed-queue drain. This is product-local
transport, not a library edit or new guest scheduler.

The actual Window controller now enqueues ordinary input in its library-thread
callback and invokes the application input sink only from the serialized owner
thread. Successful Window destruction joins producers, drains the final source
retirement, then changes presentation. Borrowed library addresses never enter
the consumer payload. Overflow is reported as a controller failure, not silent
event loss; terminal error handling remains required in session composition.

The private-desktop controller fixture verifies 256 FIFO entries in order,
overflow rejection, UI-time layout/thread identity, close rejecting new input
while retaining pending entries, and cleared borrowed source pointers. Actual
X/CAF/AltEnter, static graphics, two instances and joined-retirement tests still
pass, now asserting application callbacks run on the consumer thread. Logs:
build/M0-T423/S6/queued-controller-build-r2.log and
O:/winnt/logs/m0-t423-s6-queued-controller-r2.txt with console transcript.
Exit zero and both exact PASS witnesses are recorded. Session pump, actual
DOS/native delivery and formal product graph selection remain incomplete;
no production Window-input acceptance or package publication is inferred.

### Actual Window to cooked input path

`frontend_keyboard_dispatch` now composes copied Window events, the recovered
physical-key ledger and native Unicode translation through a bounded record
sink. Physical held state commits only after successful delivery; retirement
emits retained breaks and resets native translation state. A failed native
delivery is terminal and must not be replayed because Windows translation
state may already have advanced. Mouse remains outside S6 implementation.

The real private-desktop Window fixture sends ordinary key messages to the
actual nxvm Window (test-only thread-local keyboard snapshot hook, no desktop
input or shared-library edits). Its copied FIFO invokes the production
dispatcher on the owner thread and writes to an actual Windows cooked Console.
ReadConsole receives exactly ab CR LF, and Window retirement leaves no held
source or translation layout. O:/winnt/logs/m0-t423-s6-window-cooked.txt and its
console transcript show exit zero plus the exact end-to-end input marker and
all previous Window policy/lifecycle markers.

The separate keyboard fixture also verifies a failing DOS sink does not commit
an undelivered press or release, followed by exactly one successful matching
break. Build and unit results: build/M0-T423/S6/window-cooked-build-r2.log.
This closes the leaf Window/FIFO/conversion/cooked-Console path, not the
frontend session's hidden-helper IPC or actual CMD/guest integration. Those
connections, formal composition and production-P gates remain required.

### Actual Window to hidden-helper CMD verification

Extracted the existing native input send loop into
`run16_native_backend_input`; normal visible-Console forwarding now uses it,
and Window delivery uses the same operation/version/bounded batches. There
is no new IPC protocol or second helper implementation.

The private-desktop Window fixture optionally opens the real formal
frontend.exe helper, launches actual CMD with cooked SET /P, sends A/B/Enter
through the actual Window message handler, copied FIFO, production keyboard
dispatcher and production backend transport. It requires CMD exit 37 and
reads the hidden Console frame to require WINDOW-NATIVE-OK:ab. It then verifies
Window retirement clears keyboard state and closes the same helper normally.
Report O:/winnt/logs/m0-t423-s6-window-cmd.txt and console transcript pass with
that exact witness, plus prior Window lifecycle tests. This goes beyond an
in-process ReadConsole test; it is still a component-composed fixture rather
than the frontend session's final automatic CAF implementation.

Builds: build/M0-T423/S6/window-cmd-build.log and
window-cmd-formal-build.log. The complete original native host regression also
exits zero in O:/winnt/logs/m0-t423-s6-window-cmd-native-regression.txt, retaining
CMD input/output/results, streams, target survival and cancellation/control
markers. Remaining production integration is the frontend session presentation
pump, DOS input and CAF routing, then full regression/publication. O:/winnt's
seven runtime binaries remain unchanged.

### Session-lifetime presentation thread

The production native frontend now creates its sole I/O/presentation thread
at character-session creation rather than on the first native launch. Its
wait set adds the helper process/input only after lazy backend creation;
without a backend or during DOS ownership it waits without reading Console
input. Native creation wakes the same owner thread. This supplies a stable
consumer for subsequent Window state throughout DOS-only and nested periods,
without eager helpers, another input reader or changes to task scheduling.

Formal x86 build passes (build/M0-T423/S6/session-pump-build.log). Private
desktop reports m0-t423-s6-session-pump, session-pump-dos and
session-pump-controls under O:/winnt/logs all exit zero. They cover original
37/0/259 target results with stalled helper, live-target preservation on
helper loss/cancellation, bounded stalled teardown, 85 DOS-only channel
lifetimes without handle growth, and four actual Ctrl+C/Break/default-exit/
DOS-owned I/O control cases. Window callbacks are not yet connected to this
production thread; remaining cross-thread snapshot calls must be separated
from Window event consumption before that connection is accepted.

### Handoff snapshot versus Window event ownership

Production DOS activation and final native drain now call
`run16_native_view_sync_console`, sharing the existing validated snapshot
implementation but explicitly bypassing the Window callback. They synchronize
canonical Console cells/geometry without changing the installed callback or
consuming Window events on a channel/service thread. Normal presentation still
invokes the Window sink on the stable presentation owner. This prevents the
upcoming Window/native character-state binding from silently crossing threads.

The actual-helper fixture leaves a deliberately failing Window sink installed:
ordinary presentation returns that error, explicit handoff synchronization
updates canonical cells successfully with no additional sink invocation, and
normal Window delivery resumes through the same unchanged callback afterward.
Full host and frontend-fault suites pass with exit zero and exact marker in
O:/winnt/logs/m0-t423-s6-owner-thread-host.txt and owner-thread-frontend.txt,
including native input/output, result preservation and bounded stalled cleanup.
Build log: build/M0-T423/S6/owner-thread-build.log. Formal Window composition
and actual session display/input routing still remain; no publication occurred.

### Formal Window source composition

The formal x86 generator now validates the nxvm license and all imported file
hashes, then builds frontend-window.lib from fifteen original library units
and four frontend-owned adapters. Only frontend.exe's product link selects
this private archive; run16, ntvdm, basesrv, monitor and VDMREDIR have no Window
library edge. Source-manifest.json records all nineteen selected source hashes
and the pinned import manifest. No sibling checkout is a build dependency.

The formal graph also builds library, controller and keyboard fixtures against
that same archive. Their actual frame/keyboard assertions and real Window to
formal helper/CMD test pass: build/M0-T423/S6/formal-window-library-test.log,
formal-window-keyboard-test.log, and
O:/winnt/logs/m0-t423-s6-formal-window-cmd.txt with its text/exit-37 witness.
Graph/build logs: window-graph-r2.log, window-formal-tests-build.log.

A read-only link-ownership assertion initially failed to find ntvdm.exe because
it assumed no implicit outputs before Ninja's colon; correcting the parser to
accept the existing `| ntvdm.lib` form confirms ownership for all five excluded
products. No product fix was needed for that inspection error.

Archive selection is not runtime reachability: the session pump still needs
to create/use the controller and route actual CAF/DOS presentation. No claim
that linking an unreferenced archive already implements Window mode; no package
publication or S6 closure occurs at this composition checkpoint.

### Serialize I/O handoff on the presentation owner

The original worker-triggered DOS activation/deactivation request now crosses
a single frontend-local synchronous request slot into the existing presentation
thread. Caller serialization has a separate lock; callers never retain io_lock
while waiting. The same previous buffer synchronization, input reclaim/mode
restore and hidden reseed sequence executes under io_lock on that thread.
No broker/task scheduler, new wire operation, helper or execution relationship
is introduced. Completion wakes the caller; frontend stop or owner-thread
failure also wakes it with an error rather than an indefinite wait.

Formal x86 build log: build/M0-T423/S6/handoff-owner-build-r2.log. Private-desktop
85-channel lifetime/handle/canonical-buffer tests pass, with an added assertion
that a cancelled owner returns ERROR_OPERATION_ABORTED within one second.
All four native/DOS-owned I/O Console control cases also pass. Reports:
O:/winnt/logs/m0-t423-s6-handoff-owner-dos.txt and handoff-owner-controls.txt,
including exact text witnesses and zero exits. Window event retirement still
must be inserted into this owner-thread handoff before final runtime wiring;
the admission and publication gates remain unchanged.

### Production session controller creation and native presentation

The actual native frontend presentation thread now creates/polls/destroys the
Window controller and owns its frame conversion and keyboard-delivery state.
Its native snapshot callback publishes a Window raster when display=window,
otherwise it updates canonical Console cells. Window inputs use the existing
native backend operation or original DOS Console-input endpoint. I/O handoff
clears/joins the old Window and releases keys to the old backend before rebinding,
while retaining display policy. A frontend-local display request wakes this
owner; no new process/wire protocol or run16 presentation owner is introduced.
Failed Window join retains callback context rather than freeing live storage.

The formal graph now resolves these runtime references (not merely an unused
archive). Native frontend tests request display=window through that production
API, observe the actual NTVDM Window owned by the frontend process, send X,
verify the Window closes while the native target remains alive, and then run
the full stalled/dead/cancelled-helper result matrix. All pass in
O:/winnt/logs/m0-t423-s6-session-window.txt, including the exact production
session Window witness and 37/0/259 result preservation. The 85 DOS-channel
lifetimes and four actual control-signal cases also pass in session-window-dos
and session-window-controls reports.

Build history is retained: session-window-build and r2 failed because a graph
edit incorrectly added link dependencies to the compile output list; r3 exposed
the newly included nxvm headers' C11 requirement. Correcting the graph edge and
enabling /std:c11 only for that frontend unit yielded successful r4. No library
source was modified to work around compiler diagnostics.

Outstanding: actual Console CAF recognition, queued Console input suppression
and held-key release on switches, DOS text/graphics frame supply, explicit
presentation buffer routing, native special-input coverage and complete mixed
regression/publication. Current default display remains console, and the tested
Window request is an internal frontend API, not yet owner-facing CAF acceptance.

### Visible Console surface selection

Recovered the finite buffer-selection mechanic from reference
`codex/t423-original-reference-20260925` (286d54a306bcb8e991891fae849b79db540e897a),
`src/ntvdm-exe/presentation/console_route.c`, into the existing frontend
presentation owner. No worker-owned UI or duplicate renderer is restored.
The presentation thread lazily creates a blank inactive text surface, hides
its cursor and selects it while Window is active. The retained canonical
buffer remains writable and is restored on return and teardown. Input and
redirected streams are not reassigned by this display-only operation. Failed
activation returns the actual Windows error; failed final restoration retains
handles for terminal cleanup. The old separate lock/select API is unnecessary
because the frontend already serializes this ownership.

Incremental x86 `build/M0-T423/S6/build-view.cmd` passes. Production-composition
test `native-console-frontend-test.exe` now verifies that a canonical marker
write is invisible on the selected blank surface, then closes the actual Window
and verifies native target survival. The complete existing fault matrix also
passes, with actual text assertions and exit zero in
`O:/winnt/logs/m0-t423-s6-console-surface.txt` and its `.console.txt` capture.
This private desktop was never switched onto the owner's desktop. This proves
output-surface isolation, not physical input isolation or CAF routing; those
remain open together with DOS frames and full production-P gates. No deployment
or S6 closure occurred.

### Native Console CAF enters the production owner

The native view now offers a synchronous owner-local consumed-record callback.
The frontend uses the F make/repeat/break debounce recovered from the same
reference's `presentation/window_input_binding.c::NtvdmFilterConsoleInput`.
It requests Window policy through the existing owner wake, consumes only the
CAF F sequence and leaves ordinary Console Alt+Enter unchanged. No second
reader, worker hook or wire operation is added. The callback is optional, so
the original standalone view retains its input behavior.

The formal frontend fixture now opens Window by writing actual CAF Console
records, not by calling the display API. Its real native target checks that no
F record reached its hidden input queue. The make/repeat/break opening, X return,
buffer isolation and complete completion/fault matrix pass with exit zero in
`O:/winnt/logs/m0-t423-s6-native-caf.txt` and its console capture. Incremental
x86 build passes. This is native-path evidence only: DOS peek/read interception,
cross-switch held modifiers and Window-visible input isolation remain open.
These are mandatory remaining S6 work, not accepted exclusions or publication.

### Production DOS graphics channel and pending handle audit

The channel now binds its existing complete-video storage to the frontend I/O
owner under the same lock used by dispatch. The owner consumes only completed
published serials, preserving the last completed image during partial DATA.
Explicit TEXT clears graphics; a static image remains visible. No wire-format,
guest, worker renderer or nxvm library change is made. Normal handoff detaches
the source; channel stop performs owner-thread retirement and detaches the
borrowed storage before freeing it. The channel fixture drives real BEGIN,
partial/complete DATA, TEXT and active-graphics channel shutdown through the
production pipe/dispatcher and observes actual Window creation/destruction.
This substitutes broker attachment only; it is not guest workload acceptance.

Incremental x86 compilation succeeds. The first graphics fixture run reached
the required Window transitions but its diagnostic output overwrote canonical
K after TEXT; re-establishing that fixture marker yielded the passing r2.
Adding active-graphics shutdown and synchronous retirement then exposed a
process-handle-count failure. Retained reports are
`O:/winnt/logs/m0-t423-s6-channel-graphics.txt`, `-r2.txt` through `-r6.txt`.
The latest timeline has 326 handles after initial warm-up, grows in steps to
433 during rounds 3--9, then remains 433 through round 16. The strict equality
assertion remains failing and is not waived. Attribution to asynchronous
system initialization versus project leakage is not yet established; this
must be resolved before S6 closure. The four actual control-signal cases still
pass in `m0-t423-s6-channel-graphics-controls.txt` with zero exit.

DOS text frames/CAF, input isolation, graphics target workload validation and
the full publication gate remain open. No candidate was deployed or committed.

### Handle-growth attribution and repeated Window regression

The diagnostic-only `console-channel-lifetime-test.exe --handle-audit` queries
only its own process handle snapshot/type counts and loaded modules. With no
new channel calls during a three-second idle interval, handles grow from 323
to 433. The presentation owner is then idle in its event wait. New objects
include ETW registrations, registry keys, events and thread-pool resources;
Windows text-input modules including MSCTF, textinputframework and
Windows.UI.Core.TextInput are loaded. The following full channel loop remains
433 -> 433 and passes. This proves that the observed initial growth is not
per-channel retention; the exact internal Windows allocation stack is not
claimed. Report: `O:/winnt/logs/m0-t423-s6-channel-handle-audit.txt`.

The fixture now allows bounded first-Window resource warm-up (two seconds of
unchanged handle count, maximum ten seconds) before the unchanged exact
before/after assertion. Repeated graphics open/TEXT/active-channel-stop cycles
are included after that baseline, so a per-Window leak is not hidden in warm-up.
An attempted 34-Window matrix exceeded the observer's 60-second total limit.
The r8 progress log proves continuous completion at about 4.2 seconds per
graphics round, not a deadlock; r7/r8 timeout snapshots alone were insufficient
to distinguish those cases. A 180-second observer argument was rejected before
launch and supplied no test evidence.

The retained bounded matrix keeps all 85 channel lifetimes and exercises four
graphics channels/two Windows each. It passes with exact 430 -> 430 handles,
complete frame/partial/static/TEXT behavior, active-graphics source disposal,
original fault cases and exit zero in
`O:/winnt/logs/m0-t423-s6-channel-graphics-r9.txt`. The diagnostic mode and
per-round progress remain reproducible test inputs. No production behavior was
changed to suppress system resources or relax a lifecycle failure.

This resolves the specific handle-growth gate under the tested scope, not the
remaining DOS text/CAF/input-isolation or real guest graphics acceptance.

### DOS text source and transport closure audit

Current source proves that `CONSOLE_IO_VIDEO_TEXT` is only a mode-retirement
notification: `console_client.c::ntvdm_console_publish_video(NULL,NULL,0)` sends
no payload, and `console_video.c::run16_console_video_text` disposes graphics.
It cannot supply a kvm text frame. Character writes and original text painter
invalidations already reach the canonical Console through `console_compat.c`
and the copied Console-operation channel, but that transport does not carry
guest font banks, exact VGA palette or split cursor scan ranges.

The reviewed reference at 286d54a306bcb8e991891fae849b79db540e897a contains:

- `softpc/mvdm_softpc_text_video.c` and its include: copied original EGA plane-2
  fonts using the eight-bank ordering, selected primary/secondary banks,
  current height, original visible dimensions and both cursor ranges.
- `presentation/text_producer.c`: combines those fields with the original
  RegisterConsoleVDM painter buffer, not a new guest text decoder.
- `presentation/text_frame.c/.h`: prefers the unchanged library text carrier;
  only extents/tall fonts/split cursor outside that carrier use tiled rendering
  through the same library. This remains TEXT for display policy even when the
  final payload is rasterized.
- Original `nt_graph.c::set_the_vlt` resolves VGA colours. The old branch
  copied that result and published after completed `nt_graphics_tick` updates;
  those hooks are not present in the current mirror. The old branch's worker UI
  bindings, renderer ownership and Window callbacks must not be revived.

The currently compiled fixed-ROM `mvdm_softpc_presentation_font_snapshot`
reads only the default 8x16 source at 0xc3990. It is not an adequate substitute
for the source-selected downloaded fonts, 43/50-row modes or dual banks. It
must not be used to make a superficially passing DOS text Window.

Implementation follows the existing admitted split: worker copies original
painter/font/palette/cursor data on the original update owner; a finite copied
text payload extends the existing versioned authenticated channel; frontend
converts it to the unchanged kvm carrier. A transport revision must reject old
peers and retain bounded complete-frame publication. Partial frames must not
flip policy or expose uninitialized cells. Dynamic fonts and cursor/palette
updates require tests, not just ordinary ASCII COMMAND output.

`STREAM_IO` is a distinct original path (`nt_graph.c::nt_graphics_tick` and
`stream_io_update`): it emits Console text without a completed VGA text frame.
The old reference forced the original stream-to-video transition for Window.
That dependency must be carried explicitly by the new request/update boundary;
silently displaying a stale VGA buffer or suppressing stream output is invalid.
This audit changes the next implementation from mere frame wiring to recovery
of that complete copied-text/update contract. No missing data is fabricated,
no shared library is changed, and this research is not runtime acceptance.

### Copied text receiver and source-reader recovery

Console I/O protocol 12 adds a typed TEXT_FRAME payload to the existing
BEGIN/DATA complete-publication contract. Fixed-width style contains two
256-by-32 font banks and both cursor scan ranges; cells are original glyph/
attribute pairs, not native pointers or library structures. Bad dimensions,
lengths, font heights and cursor ranges fail without replacing the previous
complete frame. Earlier/later protocol versions are rejected. DIB callers
explicitly zero-initialize the added kind field.

Frontend `text_frame.c/.h` recovers the audited reference's renderer, changing
only its owner naming and the current library's `lib_bool` validity type.
Standard text uses the library text carrier; tall fonts/split cursor use its
existing tiled raster path but remain TEXT for policy. nxvm files are unchanged.
Formal x86 builds passed in `build/M0-T423/S6/text-wire-{graph,build,package-build}.log`.
The formal library, keyboard, video-dispatch and Console frontend tests passed:
`O:/winnt/logs/m0-t423-s6-text-wire-{unit,keyboard,video,console}.txt`.

The next source-recovery increment copies the two audited project files
`softpc/mvdm_softpc_text_video.c` and `softpc/include/mvdm_softpc_text_video.h`
from 286d54a306bcb8e991891fae849b79db540e897a without their former UI owner.
Current SHA-256 respectively:
6403E1FFD2EDB363E725956B38AB3D74B04519DEB92C2BC55F135D6AB58A017D and
E15DA53CA848A2CEAA025294AF3613B8F251AC6D098750702C19C96E179417FC.
These project-owned files contain no separate license notice. Original source
evidence is `ega_vide.c::load_ega_fonts`, `egaports.h::FONT_BASE_ADDR` and
`FONT_MAX_HEIGHT`, `egagraph.h` and `gvi.h`; their bodies/layouts remain unchanged.
Direct original Console-server presentation cannot compose across this process
boundary; this bounded reader reuses original state, rather than replacing font
selection/loading or importing the old worker Window controller. Its production
update caller is still pending, so archive selection is not runtime wiring.

`tests/component-integration/softpc_text_video_test.c` links the actual copied
reader with original-layout synthetic state. All eight primary/secondary bank
selections, 1..32 scanlines, downloaded glyph mutation, 43/50 extents, split
cursor and invalid/disabled/mode-changing refusal without mutation pass.
Run `softpc-text-video-test.exe` from the formal x86 graph; report:
`O:/winnt/logs/m0-t423-s6-text-source-unit.txt`. This is unit/mock evidence only.

`tests/app/console_client_test.c` now uses the real worker publisher and
frontend dispatcher over their pipe to verify a multi-chunk 80x50 dual-font
frame, malformed-font rejection retaining the old frame, 43-row replacement
and explicit retirement. Broker binding remains the fixture substitute.
Private-desktop report `O:/winnt/logs/m0-t423-s6-text-source-client.txt` and its
`.console.txt` witness pass. Exit 73 (0x49) is the fixture's intentional original-
shape close-handler result after frontend death, not a product crash or a
normal-zero guest result. The earlier `text-wire-client.txt` has the same
expected close marker/result; only the new run includes copied-text assertions.
The same fixture's `--broken-pipe` case also exits zero with its explicit
transport-error/no-local-fallback witness in `m0-t423-s6-text-source-broken-pipe.txt`
under O:/winnt/logs. Documentation governance and `git diff --check` pass.

Build evidence: `text-source-graph.log` and
`text-source-build-authorized.log` under build/M0-T423/S6. The initial sandbox
Ninja was inspected live (zero log, no compiler child, EventPairLow wait), then
explicitly cancelled before the authorized run. It is not a failed compiler
result. Authorized x86 build linked the reader fixture, client fixture and
ntvdm.exe successfully, including the existing VdmTib owner check.

Still open: original completed-update publisher, resolved VGA palette and
STREAM_IO transition request, actual guest text/graphics, DOS CAF/input
isolation, inherited regression and coherent publication. Nothing here proves
DOS Window availability or S6 completion. O:/winnt binaries are unchanged.

### Production text update and native-handoff regression

The next WIP connects `nt_graph.c` DIV-314 to worker-local
`win32/console_text.c`: original completed calc_update, resolved set_the_vlt
palette, copied original font/cursor/visible extent and full text backing.
The mirror currently adds 28 lines and removes none. Original stream mode
uses the existing disable_stream_io only when the authenticated frontend
reports Window demand. Frontend remains the sole renderer; no library or
guest changes. The audited reference packer is recovered with copied wire
output instead of its rejected worker-local UI ownership. Original NT4
Console-server direct shared-state presentation cannot cross this admitted
process boundary; this is a finite binding, not a second VGA implementation.

Formal x86 text-publisher build succeeds. The new production-packer fixture
checks backing stride versus visible crop, dual/tall fonts, split cursor,
resolved and startup palettes, invalid inputs, transport failure and frontend
demand. Actual channel lifetime/demand regression passes 85 lifetimes and
eight Window cycles with exact handle count 430 before/after. These remain
unit/component evidence, not a claim of complete DOS Window input.

Real integration initially passed DOS17's first 16 cases but EDIT-to-MEM
timed out in DisplayErrorTerm from nt_graphics_tick. Logs under O:/winnt/logs:
`m0-t423-s6-text-publisher-dos17-edit.txt` plus its `.timeout-live.txt`.
One frame-observed repeat passed; further r3/r4/r5 repeats failed, so that
single pass was not treated as repair. The test-only observer now captures
bounded static dialog text on its unswitched private desktop. r4 gives
"The device is not ready"; r5 frame report proves VIDEO_BEGIN (op 32),
sequence 283, transport status 0, error 21 (ERROR_NOT_READY).
Real text frames had reached 80x25/font16 and 80x28/font14 before failure;
there was no malformed-frame receiver error. Exact prefixes are
`m0-t423-s6-text-publisher-edit-{r3,r4,r5}` and their `-frames.txt` reports.

Source causality: original nt_block_event_thread's DIV-313 relinquishes DOS
I/O; frontend dos_enter rejects a non-owner with NOT_READY. A queued graphics
tick can finish after this handoff. The new publisher wrongly escalated that
stale paint into an original fatal dialog. The fix is confined to the copied
text publisher: drop only the publication's NOT_READY paint, never retry into
the native child's surface; the next active update sends a complete frame.
Invalid data and broken pipes still fail, and no scheduler/guest/error-policy
change is introduced. Unit tests assert the refused paint does not publish,
reclaim publishes anew, and both other failures remain failures.

The fixed formal worker SHA-256 is
03B220EDBF7B5E27CF9FE851CD5CD4E98BF09512728A72C716D2B085702FA953;
ordinary frontend is
DFD1065BCF2033FE7C445C97A05186EF7EAF3A2873D887813A966D2AFB741528.
`build/M0-T423/S6/p` now contains those ordinary binaries, not the frame
observer substitute. Full DOS17 passed all 17 cases with prefix
`m0-t423-s6-text-handoff-dos17`, using the checked-in
Verify-CommandExitStatus.ps1, observer-dialog.exe, R:/ candidate and G7.COM
fixture. The unit test has passed; its retained output is
O:/winnt/logs/m0-t423-s6-text-handoff-unit.txt.

The additional observed real EDIT-to-MEM run
`m0-t423-s6-text-handoff-observed` also passes (exit 1 and MEM output).
Its frame log records the same rejected VIDEO_BEGIN, sequence 280/error 21,
then complete frames 21 through 28 and restored MEM/prompt output. Thus the
new behavior is positively exercised, not inferred from a non-reproduction.
The test substitute was then removed from the build candidate, ordinary
frontend restored and the temporary R: mapping removed. O:/winnt runtime
files remain untouched. Pinned nxvm file hashes and documentation governance
pass, as does git diff --check. No commit, publication or S closure is claimed.

Format audit follow-up before delivery: current and HEAD nt_graph.c use LF
under -text, whereas both original reference copies use CRLF. This predates
the new hook; the 28/0 count is its current baseline-relative semantic delta.
Restore original line endings during the remaining S6 mirror sweep and
rebuild affected inputs; do not mistake a full-file format diff for new logic.
DOS CAF/input isolation, full Window switching and remaining production-P
gates are still open.

### DOS Console CAF and independent-frontend startup

The production DOS dispatcher now applies the same frontend-owned consumed
Console hotkey filter already used by native text input. No worker or mirror
change is needed. PEEK remains observational; READ filters CAF make/repeat/
break while preserving ordinary records. Policy mutation is queued to the
existing presentation thread under its I/O lock, never executed by the channel
thread. This is not yet Window-input isolation: DOS Window events still enter
the canonical Console queue, whose replacement is an explicit remaining gate.

The extended console-channel-lifetime-test's mode 2 writes CAF plus an ordinary
X record to the real Console. PEEK must leave policy false; READ must return
only X; the real policy query must become true. All 17 rounds of this case,
85 total channel lifetimes, graphics Window retirement and exact handle
equality 433/433 pass. Run the formal fixture through observer-dialog.exe on
an unswitched private desktop with timeout 60000; evidence prefix under
O:/winnt/logs is `m0-t423-s6-dos-caf-channel`. Result exited/0 and Console
assertions pass. Broker attachment is still this fixture's explicit substitute.

For actual product acceptance the checked-in startup observer adds optional
MVDM_OBSERVER_CAF_RETURN=1. After a real COMMAND prompt, it injects public
Console CAF, requires a visible LibKvmWindow owned by frontend.exe, posts X,
requires that Window to disappear while the same frontend remains alive,
then runs the ordinary selected command and text/result assertions. It refuses
non-test desktops. This tests logical display and input, not physical focus.

The first actual case (`m0-t423-s6-dos-caf-guest-mem.txt`) failed: timeout
evidence shows a created but invisible LibKvmWindow. The source audit found
frontend_bootstrap_start passing STARTF_USESHOWWINDOW/SW_HIDE to its detached
frontend process. That startup setting hid the first later KVM Window; the
directly constructed component fixture did not cover this launch condition.
Remove only that startup flag: DETACHED_PROCESS still prevents a transient
Console, while the separate hidden-backend helper retains its own SW_HIDE.
No shared library change or post-hoc HWND discovery workaround is introduced.

The otherwise identical real r2 case passes: its `.caf.txt` reports visible
Window, X return and living frontend; MEM text and exit 1 pass. Exact command:
Verify-CommandExitStatus.ps1 -Observer <S6>/observer-dialog.exe -PackageRoot R:/
-ProcessPackageRoot <S6>/p -LogPrefix m0-t423-s6-dos-caf-guest-r2 -Cases mem,
with both MVDM_OBSERVER_PRIVATE_DESKTOP and MVDM_OBSERVER_CAF_RETURN set to 1.
The fixed ordinary run16 SHA-256 is
0D51E6C3303EFF7D4D30AC88CC727DF56176EF6B3512223502E1EE96D6356DF9;
frontend is E97C4F55848767E19C10397ECFD2962A35B06874702C4C6074C00DA1FC1FFE89.
Worker remains the preceding text-handoff hash. Full ordinary DOS17 for these
latest binaries passes all 17 cases under `m0-t423-s6-dos-caf-dos17`, including
EDIT-to-MEM. The temporary candidate R: mapping is removed after completion.
Documentation governance and git diff --check pass. No candidate is deployed
and S6 remains open. The next integration must separate Window-origin DOS
records from canonical Console input and preserve readiness, peek/read,
held-key release and native-handoff/typeahead contracts. This source-origin
isolation is not proved by the CAF round trip and must not be marked complete.

### Frontend-owned DOS input and source isolation

The subsequent implementation supersedes the preceding read-time CAF filter:
the frontend presentation thread now consumes physical Console input before
guest READ/PEEK. Both that producer and copied Window keys feed one frontend
INPUT_RECORD queue under the existing I/O lock. Guest PEEK remains read-only;
it does not prevent independently collected CAF from changing display policy.
Window keys never enter physical CONIN$. Inactive physical Console records
are discarded for both DOS and native Window presentation. Guest prepend is
separate and never filtered as physical input. DOS relinquishment atomically
prepends unconsumed records back to the actual Console before native seeding.
The existing authenticated channel readiness attachment is retained; only its
local producer event changes. No broker wire, scheduler or guest is changed.

Recovery disposition: reference 286d54a306bcb8e991891fae849b79db540e897a's
presentation/window_input_reader and window_input_queue were inspected. Their
worker-local bindings cannot be restored under the approved frontend owner;
the existing copied KVM queue/key translator remains reused. Original
OpenNT windows/core/ntcon/server/input.c owns Console buffer read/write and
resize policy, but its Console-server object/locking and private server
dependencies are a stopping boundary, not a standalone translation unit.
Importing that server or intrusive shared-library changes is neither needed
nor admitted. The retained new mechanism is only the frontend-local copied
transport queue behind existing read/peek/prepend shapes, not a replacement
keyboard device or cooked Console parser. Windows retains native cooked
processing, and original MVDM retains guest keyboard processing. Growth is
checked before mutation; overflow/allocation failure refuses the batch rather
than truncating or partially prepending it. Future migration must preserve
that owner and return contract, not revive worker UI.

Evidence below uses formal MSVC x86 /MT, the S1 restart-formal-x86 incremental
graph and observer-dialog.exe on an unswitched private desktop. All logs are
under O:/winnt/logs; no physical user desktop was manipulated.

- `m0-t423-s6-dos-input-channel` failed line 172: the old fixture cleared
  Windows input but not records already collected by the new frontend.
  Its initialization now clears both under the actual I/O lock. The r2
  repeat passes, with exact handle equality 431/431. Failure is retained.
- `m0-t423-s6-dos-input-contract` passes the expanded checked-in
  console_channel_lifetime_test.c: 260-record growth, whole-batch prepend
  ordering, observational peek, partial drain/readiness, arithmetic-overflow
  preservation, zero prepend and actual Console destination on DOS handoff.
  It also proves Window A make/break without inactive physical Z, all 85
  channel lifetimes, eight graphics Window lifetimes and handles 431/431.
- `m0-t423-s6-dos-input-dos17` passes all 17 text-gated DOS cases, including
  nested COMMAND and EDIT-to-MEM. Verify-CommandExitStatus.ps1 uses candidate
  S6/p via temporary R:/ and the authored S6/G7.COM fixture.
- `m0-t423-s6-dos-input-caf` runs the same script with -Cases mem and
  MVDM_OBSERVER_CAF_RETURN=1. Actual frontend Window visibility, X return,
  continuing frontend, MEM output and result 1 all pass.
- `m0-t423-s6-native-input-contract` passes the expanded
  native_console_frontend_test.c. The real hidden native target peeks A
  make/break delivered from Window, rejects physical Z and consumed CAF,
  and survives X return. Existing 37/0/259 completion, helper-death,
  cancellation and bounded stalled-helper teardown assertions also pass.
- `m0-t423-s6-dos-input-native-host` passes the full existing native host
  fixture, including cooked input, native results, streams/EOF, input reclaim,
  geometry/scroll, helper loss and control events.

The DOS17/CAF candidate frontend SHA-256 is
2EA923C8DA09409BD23B33F41DA737E08EE6367D5F8F13C14840CB4B445A48CD;
run16 is A072163BEA051C20BE51AA319F562CDDD43CE6084B2604AFC4746843CB5EC5D8;
worker remains 03B220EDBF7B5E27CF9FE851CD5CD4E98BF09512728A72C716D2B085702FA953.
Later comment-only corrections were rebuilt, not silently treated as the same
artifact. Remaining gates include held-key/focus/source retirement, complete
nested switching and independent-session regression, original mirror format,
inherited production-P/WOW frontiers and coherent seven-file publication.
O:/winnt remains the verified baseline; no S6 delivery or closure is claimed.

The comment-corrected rebuild passes both final repetitions:
`m0-t423-s6-dos-input-contract-final` and
`m0-t423-s6-native-input-contract-final`, exited/0 with their assertions.
Final frontend is 91EBD2832F485D2796E7B4B871CAA6502C20640D78F161223A71F2589C23451D;
channel fixture is D602C0C7288DAFD1C230407D0B3B156F848E65B6D9D2DF986AB68220EAE2443E;
native frontend fixture is 44F5E2BDAA28C57E8A8DE55B02FD118B3B73A6A5B9B08DC7DBCE06BCBD4ACB1A.
The full native-host run also used this rebuild. The DOS candidate hashes above
remain a separate tested checkpoint, not an assertion of latest deployment.
Documentation governance, git diff --check and pinned nxvm source hashes pass.

### Held-key retirement verification

The checked-in channel and native frontend fixtures now deliberately deliver
B down without B up, then retire the Window source. Both assert exactly one
B down and one B up at the original destination; the native case additionally
retains A make/break and rejects inactive Console Z. This proves joined source
retirement reaches the production keyboard delivery/queue, not only its leaf
unit. No production or imported source changed in this test increment.

Formal x86 build-view.cmd and private observer runs
`m0-t423-s6-held-dos` and `m0-t423-s6-held-native` both exit 0 with assertions.
The first includes all 85 channel lifetimes and exact handle equality; the
second includes the existing native completion/helper-fault/teardown cases.
Fixture SHA-256 values are respectively
E7D8FCC58D426DD831F8AFAF4C535CA778DCD7B98191EE454B6AC9C3E913D0F9 and
29D80B8EC924D10C95D20F03D3BD1136F87F4C5B823D5C1DB1BA163C5483DC03.

Focus is a separate open integration contract. The pinned and current nxvm
src/lib/kvm-window/win32/component.c WM_KILLFOCUS handlers release mouse only;
the public window_interface.h has no focus notification/native HWND accessor.
Original MVDM nt_event.c::nt_process_focus consumes FOCUS_EVENT, calls
AltUpDownUp on acquire and MouseOutOfFocus on loss. Thus neither successful
Window retirement nor the existing leaf release test proves actual focus
delivery to original MVDM. Do not change guest, shared library or imported
keyboard behavior speculatively: resolve the finite frontend event binding and
its ordering before S6 acceptance. Full nested switching and production-P
gates remain open; no publication or closure is claimed.

### Original line endings and rebuilt checkpoint

Restored nt_graph.c from 2,342 bare LF endings to original CRLF mechanically,
without changing any non-newline byte. Original OpenNT has 2,314 CRLF lines;
both baseline-relative and original-relative normalized diffs remain +28/-0.
Thus the apparent full-file Git change is restoration of upstream format,
not a new implementation. Formal build-text-publisher.cmd recompiles that
host unit, rearchives and links the x86 worker successfully; VdmTib ownership
verification passes. No warning-driven mirror rewrite is introduced.

The refreshed S6/p build-only candidate passes all DOS17 text/result checks
with prefix `m0-t423-s6-crlf-dos17`, including EDIT-to-MEM. Exact hashes:
ntvdm 92BC4ECEF9ADAD98401B967EEBBEDB2551033D2534E7E39CB0883DA0A393B20A;
frontend 91EBD2832F485D2796E7B4B871CAA6502C20640D78F161223A71F2589C23451D;
run16 A26B5705032F02DF301B7A6881B0B481B25E73A9E944EB68AA3740799DB0F6E6.
The private controller fixture also passes `m0-t423-s6-window-checkpoint`
(cooked Console) and `m0-t423-s6-window-helper-checkpoint` (formal frontend.exe
passed as its sole argument). The latter asserts actual hidden-helper CMD
input ab, output WINDOW-NATIVE-OK:ab and exit 37, alongside independent Windows,
CAF/AltEnter/X, static graphics and explicit text return. These retain their
component-composed scope, not a claim of complete product nested switching.

The finite focus ABI extension was submitted for owner clarification because
the shared library remains immutable under current approval; no polling,
window-discovery hook or library edit has been added. Other S6 gates continue.
No runtime-package publication, P delivery or S closure is claimed.

### Product Window input and nested return audit

The checked-in startup observer now has optional
MVDM_OBSERVER_WINDOW_INPUT=1: after a real COMMAND prompt it performs CAF,
verifies the visible Window belongs to frontend.exe on its private test
desktop, retains that frontend PID, and delivers subsequent unshifted ASCII
commands as native Window key messages. Every character verifies the same
frontend owner, allowing HWND recreation during handoff. It refuses shifted
characters rather than inventing UI-thread modifier state. This is test-only,
not a product injection option, and does not change physical desktop focus.

Ordinary S6/p with `m0-t423-s6-window-product` passes native-zero and mem:
actual Window input runs DOS-to-native ver, observes its Windows-version
output, returns, and separately runs MEM with expected text/result. However,
nested-mem times out at the original 20-second overall observation limit.
Its preserved line snapshots show two completed MEM reports and the next MEM
command. Before cleanup, stack mapping places worker in console_activate /
nt_resume_event_thread and frontend presentation in kvm_window_worker_start,
with the channel awaiting the handoff. This is an unclosed failure, not proof
of a guest defect, deadlock, input loss or successful nested Window return.

The observer timeout includes startup and all paced script input. The runner
now exposes an explicit ObservationTimeoutMs (default unchanged at 20000,
maximum 60000), and Window mode records elapsed time before exit waiting in
its .window-input.txt sidecar. A supplemental
interactive-native-dos-return case enters CMD, run16 COMMAND, MEM, returns
to CMD for an exact executed echo line, then returns to DOS for MEM/exit.
It verifies two MEM reports and result 1, not merely echoed commands.
Longer-budget repeats are diagnostic evidence, never by themselves a repair.

The `m0-t423-s6-window-product-budget` repeat still reports nested-mem timeout
at 60000. Its input sidecar records 22,797 ms before exit waiting, so the old
20-second budget was indeed exhausted during scripting, but increasing it
does not establish completion or fix the product. This attempt also exposed
the runner's outer fixed 55-second wait; it now follows the selected observer
budget plus 15 seconds (minimum unchanged at 55 seconds). The observer did
finish its own timeout capture and subsequent exact-path inspection found no
remaining physical-path process. That check was incomplete: the subsequent
interactive run's preflight correctly refused still-running R:/ alias
processes. CIM confirmed the original launcher 11360's broker 2456, frontend
1056, worker 14652 and helper 33512; their mapping, paths and parent identities
were checked before explicitly stopping only those test processes. Both
physical paths and active launch aliases must be included in future isolation
checks. The interactive native/DOS supplemental case
was not reached in that aborted matrix; it requires a separate run.

The independent `m0-t423-s6-window-interactive-r2` run also times out at 60000.
Its sidecar proves Window input delivered under frontend PID 12344 and
31,844 ms consumed before exit waiting. Captured output proves CMD launch,
inner run16 COMMAND, one MEM execution and return to CMD's exact
window-native-return output. Final outer DOS MEM/exit does not complete;
the full case is failed, not a partial pass. These two failures make complete
Window nested return an active S6 defect/investigation, despite passing
Console-mode DOS17 and component-level handoff fixtures. Do not publish.

The runner now recovers this run's recorded launcher PID in finally even if
normal result parsing was bypassed, retaining existing exact package/alias
checks for descendant cleanup. This test-only cleanup adjustment is syntax-
checked; it is not claimed as the Window runtime fix. No guest or library was
modified by these attempts.

### Pending-input handoff correction

The interactive failure's stack differs from the earlier creation snapshot:
worker is in host_release_timeslice / idle_kybd_poll / keyboard_io; frontend
is waiting in present_loop and its channel in transfer. Therefore a universal
Window-creation deadlock was not established. Source review finds a concrete
handoff edge: apply_dos_binding returns unread DOS records to canonical
CONIN$ with original prepend semantics; native_frame can open Window before
normal native forwarding reads them, after which the physical-source filter
discards that queue. Returned input and inactive physical input must not be
conflated.

The narrow candidate calls the existing run16_native_view_forward_input in
its existing 64-record batches, bounded by the old route's initial pending
count, before transitioning native Console to Window. This reuses filtering,
native helper transport and error behavior; no new ABI, original mirror,
guest, scheduler or shared-library change is introduced. The unavailable
historical external Window switch remains the admitted product boundary;
the source-first choice here is existing owner-code reuse, not a new input
provider. Physical input arriving after Window selection remains suppressed.

The expanded native_console_frontend_test reclaims the existing hidden
queue into DOS, prepends C make/break, relinquishes DOS while display remains
Window, then requires the actual native target to retain A/B/C pairs.
`m0-t423-s6-window-typeahead-before` and `...-after` failed, but both used
final drain incorrectly as a barrier: drain itself reclaims input and stops
the pump. Those failures cannot establish production causality. The corrected
fixture uses a second idempotent handoff request as a presentation-thread
barrier; `m0-t423-s6-window-typeahead-corrected` exits 0 and passes all retained
native result/fault/teardown assertions. The actual two product failures must
still be rerun before claiming their repair. No verified O:/winnt file changed.

The ordinary product repeat `m0-t423-s6-window-return-fix` uses frontend
BF59A97C327B759CDA2FB12EB273AC5441DE5C9A7AE089C92253DFC598E644E3 and the
same worker/run16 as the preceding checkpoint. nested-mem now exits 1 rather
than timing out, but strict output verification fails: its final MEM is
echoed/executed as EM with the original command-not-found response. Earlier
MEM output and final exit do not compensate for that lost first key. The
following interactive case was not executed because the matrix stopped.
Input delivery was reported complete at 22,250 ms. This remains a failed
product case; the retained forwarding change has focused positive evidence
but is not yet a proved complete repair of Window nested input. Next evidence
must locate the missing M across Window records, frontend owner handoff and
original guest keyboard consumption, rather than changing guest or merely
loosening text assertions/delays.

### Returned scan-only records and rolling-screen evidence

The next observed nested run is `m0-t423-s6-window-input-trace`, using the
existing test-only frontend-video-observer in the build candidate. It exits
1 normally but fails the final-screen MEM-count assertion. The line-04
snapshot contains the deepest MEM result (largest program size 4294901312),
line-06 also contains the middle result (4294913728), and the final snapshot
contains the middle and outer result (4294926144). All three executions have
output evidence; the earlier result has scrolled off the repeatedly copied
guest screen. This specific count failure is not a lost MEM execution.
The ordinary EM command-not-found run above remains a separate real failure;
this observed repeat does not repair or invalidate it. Do not sum repeated
screen snapshots as independent executions or waive command-error checks.

`m0-t423-s6-window-interactive-input-trace` then runs the real Window
CMD/run16/COMMAND/MEM/return sequence alone with the 60-second budget. It
times out (observer result 0x53504354), after the inner DOS exit. Its input
trace records the original PREPEND_KEYS returning the subsequent echo command
as physical make/break records with valid scans but UnicodeChar zero,
including Enter. This is not successful native command execution.

Source comparison identifies two distinct original consumers:
`mvdm/softpc.new/host/src/nt_keycd.c::KeyMsgToKeyCode` consumes scans, whereas
`nt_event.c::ReturnUnusedKeyEvents` returns the same original INPUT_RECORDs
to the Console. Original OpenNT `windows/core/ntcon/server/input.c`, around
2140, builds character-bearing Console records from WM_CHAR/WM_SYSCHAR.
The current Window dispatcher deliberately leaves DOS physical records'
characters zero. That differs from a complete, transferable Console record;
returning these records to a native reader does not recreate the characters.

The checked-in `frontend_window_keyboard_test --cooked` now includes a
bounded negative test: write scan-only aB/Enter followed by converted
aB/Enter as a sentinel. Actual Windows ReadConsole returns only the sentinel,
not both strings. Formal x86 incremental build and private-desktop execution
`m0-t423-s6-returned-scan-contract` pass with exit 0 and the explicit negative
contract marker. This proves the native boundary, not a product repair or
the sole cause of every prior timeout. Existing layout/dead-key, retirement,
failed-sink and cooked-input assertions remain passing.

Next repair must preserve one physical guest key while retaining sufficient
character information for native typeahead handoff, with layout/dead-key and
multi-character cases accounted for. Do not merely convert scans after losing
their original layout/modifier context, inject duplicate guest characters, or
modify guest/CCPU code. Reuse the existing frontend conversion owner where
the original Console-server translation unit cannot compose; its Win32k/CSR
Console internals remain a stopping boundary. No new provider or library edit
has been admitted by this finding. Correct rolling-screen test evidence is
also required before repeating the full Window nested matrix. These are open
S6 obligations; O:/winnt remains unchanged and no production P is delivered.

### Character-bearing Window input candidate

Further source audit: OpenNT ntuser/kernel/keyconv.c::_TranslateMessage marks
all but the last character of a multi-character translation FAKE_KEYSTROKE;
ntcon/server/input.c::HandleKeyEvent clears that scan and populates characters.
Read-only source SHA-256 values are respectively
31A080A3E464F88B6D9DB9B4CDB8A0975C8B803EA71AD0D3BBFB013A3ED1F465 and
84D7D4C5A422208D14900C8F3A86B0F9648A3755F03D6D49765B35266617252A.
Their private USER/Console message-loop dependencies cannot be directly linked;
no source file was imported. The existing frontend conversion is the finite
same-shaped modern binding: physical KEY records now receive layout text
regardless of DOS/native owner; character-only prefixes carry scan zero and
VK_PACKET, and only the final character retains the physical scan. No shared
library, guest or mirror code changed. Character-prefix handling through the
worker's existing scan-less normalization still needs explicit guest coverage;
host layout tests alone do not prove all international DOS behavior.

Formal x86 frontend candidate SHA-256 is
C9A8CFE17E610D20B2584E87E77A0066DD6469A3EDF206AA085201FB1954B651.
`m0-t423-s6-returned-character-expanded` exits 0: actual DOS-dispatch ab/Enter
records reach real cooked ReadConsole unchanged, physical make/break counts
remain one pair per key, noncomposing dead-key prefix/trailing-scan assertions
pass, and existing failure/retirement/native/dead-key tests remain passing.
`m0-t423-s6-returned-characters-channel` exits 0 with all 85 channel lifetimes,
held DOS B release, graphics/window retirement and exact handles 431/431.

The ordinary real Window interactive chain first exits 1 and executes the
return marker under `m0-t423-s6-window-returned-characters`, but final-screen
count fails because its first MEM header has scrolled off. The checked-in
Merge-ConsoleTextSnapshots test helper now reconstructs only contiguous
overlapping line captures, allows the last live prompt to grow, preserves
errors, and rejects disjoint repaints rather than inventing history. Its
synthetic tests cover duplicate frames, scrolling, prompt growth, retained
command errors and rejected missing continuity. It is used only for scripted
Window runs; existing Console assertions are unchanged.

With these strict temporal assertions, ordinary
`m0-t423-s6-window-character-interactive` passes the full
CMD -> run16 COMMAND -> MEM -> CMD echo -> outer DOS MEM -> exit chain,
including exactly two MEM reports, executed window-native-return and exit 1.
This proves that particular production chain, not general nested acceptance.
The separate `m0-t423-s6-window-returned-matrix` nested-mem run still produces
EM command-not-found. Its initial merger refusal was a growing prompt, now
covered by the helper test; reconstructing its retained frames preserves the
actual EM error. No successful exit or nearby passing chain cancels it.
Next investigation includes source retirement during apply_dos_binding: it
currently clears/destroys the Window at each I/O handoff and recreates it on
the next frame. That is a candidate input-loss boundary, not yet a proved
cause. All full production-P and S6 gates remain open; O:/winnt unchanged.

### Original 6805 unread-event count regression

The test-only frontend observer now records Window events, sink results and
original READ_INPUT/PREPEND_KEYS records. `m0-t423-s6-window-source-trace`
passed nested MEM; `m0-t423-s6-window-source-trace-r2` reproduced EM. In the
latter, M-down was successfully read at sequence 633, but the subsequent
PREPEND sequence 679 begins with M-up. This narrows the investigation beyond
Window receipt to worker keyboard/history return; it does not prove a CPU
instruction defect. These first reports used concurrent append handles, so
absence of a report line alone is not definitive. A test-only SRW lock now
serializes complete report blocks. Its x86 observer builds after explicitly
selecting C11 for the included production keyboard unit; the initial missing
C11 compilation failure is not a passing run.

Source review found `keys_in_6805_buff` has `else` inside its single-byte
comment, leaving `last_marker` set after a complete one-byte sequence.
Both OpenNT and OpenNT-4.5 original keyba.c files have SHA-256
D3326210BE154866924CA99F1E44E0A76E9A912E15975305804F6B59C4CF7953
and the same expression. `mark_key_codes_6805_buff` assigns distinct high-bit
markers to complete single-byte events. The original host consumer
`nt_event.c::CalcNumberOfUnusedKeyEvents` uses this count to select the suffix
of input history to return. The immutable guest is not the owner of this code.

Reproducer: `tests/observation/verify-keyboard-buffer-count.ps1 -BuildRoot
O:/repos.hobby/ntvdm64/build/M0-T423/S6/keyboard-count-original` extracts the
actual selected function, compiles MSVC x86 /MT and controls only ring state
and reset. No guest or instruction executor is involved. Original source
DBE61BA77FD716D349E0DA37FB6011EF3C6731BC53DA29C1B3AE85BF83CD1CD5
fails five of eight cases: four single-byte events count as one, including
wraparound; even one complete event is incorrectly marked partial. Empty,
complete multibyte and genuinely partial cases pass.

DIV-315 moves that `else` out of the comment, retaining the complete original
function, ring traversal, reset and downstream history owner. Recovery ladder:
the original translation unit is already formally composed; an external facade
cannot correct its private ring count without duplicating the algorithm.
The selected exceptional intrusion is one owner-local branch restoration;
no new provider/ABI is needed. All eight tests pass under build root
keyboard-count-fixed, extracted function SHA-256
EC02937F4F96AF04F61DE6759332BFB0F4D486BFA73341D9F9ACDF0BEFF064B0.
This proves the counter defect and bounded repair, not yet the sole cause of
every nested input failure. CPU/guest semantics and media are unchanged.

The keyba.c source had 3249 LF lines in HEAD, unlike original CRLF. Its
original CRLF format is restored; normalized task delta is +3/-1 (including
the divergence comment), not the all-lines raw formatting diff. Formal x86
incremental worker build succeeds. Ordinary frontend is restored in the
isolated candidate; the test observer is not published. Candidate hashes:
frontend C9A8CFE17E610D20B2584E87E77A0066DD6469A3EDF206AA085201FB1954B651,
worker 09170E33FAC8F41936B03EA239C02777B5575695A12F7F65F8E29709A48EB673,
run16 C9A60EB6EFB5BFFB1A6E375827B84286F39C0A548E8D415CC9C8275A4C49BA7E.
The run16 artifact was relinked by the formal graph; do not assume byte identity
with the previous tested launcher. Full affected regression remains required.

Private-desktop ordinary run `m0-t423-s6-keyboard-count-window` passes both
nested-mem (three MEM reports, exit 1) and interactive-native-dos-return (two
MEM reports, executed native return marker, exit 1). However the second run
`m0-t423-s6-keyboard-count-window-r2` still fails nested-mem: its retained
contiguous transcript shows MEM at the two inner levels and EM at the outer
level. The counter repair is therefore insufficient for the whole product
failure; do not claim a causal single-fix resolution or publish this candidate.
The attempted combined command stops before its Console suite on this failure;
Console regression is launched separately under keyboard-count-console.

`m0-t423-s6-keyboard-count-console` subsequently passes all 17 established
Console cases, including nested MEM, EDIT and explicit exit-code cases.
The serialized observer run `m0-t423-s6-keyboard-count-trace` also passes
nested-mem; instrumentation changes timing, so it does not cancel the ordinary
r2 failure. Its input report remains available for comparison. The candidate
frontend is restored to the ordinary formal executable after observation.
O:/winnt frontend/worker/run16 hashes remain the published S4 baseline; no
candidate publication, production P or S6 closure has occurred. Next evidence
must distinguish original keyboard/BIOS return from frontend receipt on an
actual failing run rather than infer success from another instrumented pass.

### Private keyboard-state observation

The checked-in `tests/observation/keyba_observed.c` includes the selected
original keyba translation unit and wraps only its externally called unread
counter. It snapshots markers, held count, output/pending state and scan data
before calling that original counter/reset. The formal graph exposes a
test-only `ntvdm-keyboard-observer.exe`; its ordinary worker edge is unchanged.
The map proves count/reset/key-down all resolve to this one included unit,
not a second keyboard provider. The expected export-name warning names the
test output versus the original ntvdm DEF; VdmTib ownership verification passes.
The test frontend's copied-input headers now carry ticks matching the worker
snapshot. No new production trace, guest change or behavior repair is made.

Build: regenerate the existing S1/restart-formal-x86 graph and build
ntvdm-keyboard-observer.exe plus frontend-video-observer.exe with MSVC x86 /MT.
SHA-256 identities are respectively
0C2AF81301BA05F288502583464F2A53EA6EF889E7B1DEA7687D5F8250B00C63 and
4B9846F70FA4350C9BF3DB799611BCB1C19E32107E4C193686FBA2253411622D.
They are isolated test substitutions below build, not publishable product files.

Use Verify-CommandExitStatus.ps1 with the existing private-desktop observer,
PackageRoot R:/, physical ProcessPackageRoot build/M0-T423/S6/p, Cases
nested-mem, ObservationTimeoutMs 60000 and the authored G7.COM fixture.
Set MVDM_OBSERVER_PRIVATE_DESKTOP=1, MVDM_OBSERVER_WINDOW_INPUT=1 and
MVDM_TEST_KEYBOARD_REPORT to the corresponding O:/winnt/logs worker log.
For both observed components, also select FrontendObserver and set
MVDM_TEST_FRAME_REPORT; for the ordinary frontend select OrdinaryFrontend.

Both fully observed runs keyboard-private-r1/r2 and both worker-only runs
keyboard-worker-only/keyboard-worker-only-r2 (all prefixed m0-t423-s6-)
pass the same strict three-MEM/exit-1 assertions. One worker-only handoff
records seven complete ring events plus one full output slot and correctly
returns eight, partial=0. These are useful state witnesses, not a reproduction
or explanation of ordinary keyboard-count-window-r2's EM failure. Logging
and test linking can alter timing. No further production edit is justified
solely by these passing repeats. Ordinary binaries are restored after the
diagnostic runs; the outstanding failure and full S6 gates remain open.

### Buffered key observation and partial-prefix counter audit

The test-only keyboard observer additionally wraps the original registered
host key-down/up functions. It buffers 256 transitions with disabled/scanning,
repeat/full and before/after key counts, dumping only at the original handoff
counter. Overflow is explicit. This avoids per-key file writes; it does not
remove all instrumentation timing effects. Source SHA-256 is
A2097A253938E7B7B34BBC0763AA1639734EC67E960D38B5379C94C1AE293838;
test worker is 79DA743739031A5737CAB9226CE92DB33A5AD0D59C43B8336ABC4871371DDEDC.
With ordinary frontend, keyboard-buffered-r1/r2/r4 (same m0-t423-s6- prefix)
pass nested-mem. The r2 trace contains accepted M down/up transitions and no
observation overflow. These passing runs do not explain the retained failure.

keyboard-buffered-r3 omitted MVDM_OBSERVER_PRIVATE_DESKTOP. The observer's
existing desktop-name guard refused Window input; the unchanged initial DOS
prompt timed out with 0x53504354. No CAF input was injected. This is a harness
invocation failure, not an EM reproduction. Verify-CommandExitStatus.ps1 now
rejects Window-input requests lacking PRIVATE_DESKTOP=1 before resolving paths
or starting processes. A negative invocation with a nonexistent Observer path
proves that this preflight, rather than path resolution, rejects the request.

The source-defined marker contract also permits a remaining multibyte end
marker after its start has already been consumed. Adding two complete
single-byte markers after that tail exposes another counter defect:
`[1, 0x82, 0x83]` returns zero complete events, not two. The same wrapped ring
fails. A remaining tail followed by a complete multibyte pair and a single
already counts correctly. The expanded extracted-function test at
build/M0-T423/S6/keyboard-count-partial-audit passes 9/11 and retains these two
failures against the previous DIV-315 body; no guest or CPU substitution occurs.

The minimal same-owner DIV-315 extension treats a high-bit single-byte marker
independently of an unmatched preceding marker, records the earlier partial
transfer, and clears that open marker. Original traversal/reset/history ownership
remains unchanged. An external facade cannot access this private marker ring
without copying the algorithm; no new ABI/provider is introduced. The complete
normalized task delta in keyba.c is now +7/-2, with original CRLF retained.
All 11 cases pass at build/M0-T423/S6/keyboard-count-partial-fixed. Selected
source SHA-256 is 5A0F42DF1D19E1527931587B8A1512AA0374D59C8580D3DA64AFAB11F57A5A22;
extracted function is 23586F8CEC57C6146D69C32C1EDBCB468D4C1BD31F5734B85AB0063FD553F252.
The incremental MSVC x86 /MT ordinary worker rebuild passes, including VdmTib
storage validation; worker hash is
12AF0293577FB2C7A7DF0160EC16D1C6F544DBFCBB34932A9F0A668D34118348.
The candidate worker has been restored to that ordinary build. This is proof
of a bounded counter repair, not proof that the intermittent Window loss is
resolved. O:/winnt remains the verified S4 package; no publication or P occurs.

Ordinary `m0-t423-s6-keyboard-partial-window` still fails nested-mem: the
retained reconstructed transcript has two MEM reports, then `em` at the prompt and
the native command-resolution error after returning to the outer COMMAND.
The test correctly fails despite exit 1. Its requested second interactive
case is not executed after this failure. Thus the expanded counter repair
does not close the Window failure; no unobserved partial prefix is asserted
to have occurred in that real run. Console comparison runs separately under
`m0-t423-s6-keyboard-partial-console` with the same ordinary worker.
Both Console cases pass: nested-mem has three reports/exit 1 and
interactive-native-dos-return has two reports, its executed native return
marker and exit 1. This is a two-case comparison, not the full 17-case gate
for the newly built worker. The next investigation must correlate the failing
Window input with original device/BIOS consumption and the subsequent history
return; further passing instrumented repeats alone cannot establish a repair.

### Mapped keyboard/port observation

The previous diagnostic still performed file I/O exactly at the handoff being
investigated. The test-only keyba observer now allocates one file-backed mapping
at keyboard initialization and records fixed 64-byte events by atomic slot
reservation/publication. Neither individual keys nor the unread counter opens,
writes or flushes a file. It preserves the original host-key entry functions,
counter/reset and keyboard port-read implementation. The existing input-port
registration is rebound only in this test executable to call the original
`kbd_inb` and record the returned port-0x60 byte. No production link or guest
change is made. Instrumentation still changes code layout/timing; it is not
equivalent to an ordinary non-observed reproduction.

Events: KEY contains key/down/disabled/scanning/last/full/count-before/count-after/
output; COUNT_BEFORE contains held/output-full/pending/KbdData/output/out-index/
in-index; MARKER contains index/value; COUNT_AFTER contains count/partial;
PORT60 contains returned-byte/output/KbdData/output-full/EOI-pending/delay-pending/
DelayIrqLine. Tick and thread identify each record. Concurrent key and port
records are reserved after their original calls, so their reservation order is
not proof of ordering inside those calls. The returned port byte is the actual
original result; other port state fields are observations after its lock release.

Use the previous private-desktop nested-mem recipe, but set
MVDM_TEST_KEYBOARD_REPORT to a fresh `.bin` path under O:/winnt/logs (CREATE_NEW
rejects accidental evidence overwrite). After worker exit and cleanup, run
tests/observation/Read-KeyboardObservation.ps1 -Path that-file. The decoder
requires magic/version, exact mapped size, completed record kinds and zero
overflow. It rejects the old text format in a negative check. The mapping stays
process-owned until exit, so an explicit cleanup also preserves observations.

MSVC x86 /MT test build and VdmTib verification pass. Source keyba_observed.c
SHA-256 E406AF23CBE2A0D9BE67040C2102C5F3E47FD63B392568C8357D2BFE4F6F4062;
decoder BF3B75E2D645325C6919B9FFDB288B28FA65A09DBD78C834427FA0B6125759EF;
test worker 10C28F99E237C2DFD81D1D2B6BD662856775855B9CBBF7E8CAF25086B50F4206.
Runs m0-t423-s6-keyboard-mapped-r1/r2 both pass nested-mem's three reports and
exit 1. Decoding proves complete, non-overflowed recordings: r1 has 239 events
(111 keys, 106 port reads, 11 counter pairs); r2 has 295 events (136 keys,
111 port reads, 26 markers, 11 counter pairs). The port trace includes original
M make scan 0x32. Neither run reproduces the ordinary EM failure; no causal
resolution is claimed. The ordinary candidate worker was restored and hash
checked as 12AF0293577FB2C7A7DF0160EC16D1C6F544DBFCBB34932A9F0A668D34118348.
The published S4 worker/frontend remain unchanged. S6 still needs the failing
run's receipt/port/BIOS-history correlation and its complete acceptance gates.

### Device/history cardinality boundary

Mapped run m0-t423-s6-keyboard-mapped-r3 also passes nested-mem. To exercise
typeahead across handoff rather than only paced lines, the checked-in matrix
adds supplemental nested-mem-typeahead: identical commands, paired 100 ms key
events, zero extra line delay, three distinct MEM reports and exit 1 required.
The mapped-typeahead run passes, including an eighteen-event unread backlog.
These remain diagnostic-artifact results, not ordinary Window acceptance.

A source audit identifies a separate correspondence problem: nt_event records
each raw key in history before processing, but original keyba::host_key_up
does nothing for a key whose down count is zero. CalcNumberOfUnusedKeyEvents
counts device events, while ReturnUnusedKeyEvents chooses that many raw history
records from the tail. Thus an ignored release after an unread valid make can
displace that valid make in the returned suffix. An ignored release before the
unread make does not do so. This is a host input/history boundary, not guest
code or a CPU-instruction conclusion.

Reproducer: tests/observation/verify-keyboard-buffer-count.ps1 -HistoryBoundary
-BuildRoot O:/repos.hobby/ntvdm64/build/M0-T423/S6/keyboard-history-boundary.
The fixture composes extracted actual host_key_up, keys_in_6805_buff and
ReturnUnusedKeyEvents; it controls an unread single-byte M marker, raw history
and the Console sink. No guest or full device execution is claimed. The
downstream key-up packet producer is mocked but is not reached for the ignored
release under test. The normal make and ignored-release-before-make controls
return M correctly. Ignored-release-after-make fails: history=2, device=1,
returned VK=69/down=0 instead of VK=77/down=1. It is an intentionally retained
failing preservation assertion, not a passing expected-error capability.
Extracted host_key_up SHA-256
310B5E55554F629569C589AA6769CB11D4409968F949A711B872F21315BB9F0B;
ReturnUnusedKeyEvents B212F925157448BB29794F0C20CDF6C707E139BE76C33DC36D389CA8849AEA43.

The mapped real runs do contain ignored releases, but have not shown this
failing controlled arrangement coinciding with ordinary EM. Do not claim that
the fixture proves the complete runtime cause. Before a production change,
review the original fake-make exceptions for VK_CANCEL/VK_SNAPSHOT, scanning
discontinuation/held events, repeat suppression, toggle synthesis and BIOS
return. Blindly dropping all unpaired key-up events would change those paths.
The ordinary candidate worker is restored after these observations; no runtime
publication, source repair for this new boundary, or S6 closure is claimed.

### Original key-dispatch exception controls

The HistoryBoundary fixture now also composes extracted original host_key_down
and nt_process_keys. It uses a controlled key-ID mapping and a mocked one-packet
producer, so these tests establish dispatch/acceptance decisions, not real
scan encoding, hardware timing or complete toggle synchronization. The mock
release producer resets key_down_count to zero, matching original
do_host_key_up rather than decrementing it.

Run root build/M0-T423/S6/keyboard-history-controls compiles MSVC x86 /MT.
The existing ignored-release-after-make preservation assertion still fails.
Six additional control assertions all pass:

- ordinary unpaired release produces zero device events;
- VK_CANCEL release invokes original synthetic make then release (two events);
- VK_SNAPSHOT release does likewise;
- scanning_discontinued queues an unpaired release as one held event;
- disabled keyboard accepts neither synthetic Ctrl+Break event;
- a contiguous repeat with KbdHdwFull=8 leaves the eight-event backlog unchanged.

Consequently, a frontend-wide unpaired-release filter or a history predicate
based solely on IsKeyDown would be wrong. Raw history count is not generally
identical to device-event count; zero-event suppression and synthetic multi-event
input both need a source-owned disposition. The active defect's repair belongs
at original nt_event history/device correspondence, not in a new frontend
keyboard policy. No such semantic change is made on the strength of these
controls alone. Actual failing-runtime correlation and a bounded implementation
that retains these original exceptions remain required. The native test does
not modify or execute guest code and does not select another CPU profile.

### Failing runtime history capture

Ordinary candidate 12AF0293577FB2C7A7DF0160EC16D1C6F544DBFCBB34932A9F0A668D34118348
reproduces EM again in m0-t423-s6-window-revalidate-20260927-nested-mem:
two MEM reports, then outer prompt EM/command-resolution failure.

The test-only keyboard observer now reads GetHistoryKeyEvent at the original
blocked-event-thread count boundary. HISTORY records contain newest-first
index, VK, scan, down, Unicode, control flags and repeat count, without
dequeueing or changing history. MVDM_TEST_KEYBOARD_HISTORY_ONLY=1 keeps the
original input/port function pointers and avoids per-key observation. Neither
option is linked into the ordinary worker. Build command remains
build/M0-T423/S6/build-keyboard-observer.cmd (MSVC x86 /MT, CCPU40).
Observer source hash 61927B6EC07BFD1084DE0A006664AE89298C3F3F5FAE791F04457F1CC6293D3B;
decoder 119E8641C9927C483C94B8C5B3B35524475E2937EA95ECBD1E2AA7C5D6593ED1;
test worker 9F1402BBA1BAF9AECFA1A6CCA7C25BEA23E013278F14D92018D76A382449FCF0.

Private-desktop history-runtime-r1 with full observations passes three MEM
reports (428 complete records). Crucially, history-only-r1 with the ordinary
frontend reproduces EM and records 256 complete, non-overflowed events:
ten count pairs, 163 history entries and 73 ring markers. Logs and binary
captures use prefix m0-t423-s6-keyboard- under O:/winnt/logs. Both use the
existing nested-mem recipe, Window input and immutable guest media.

At failing-run tick 66787656, records 151--207 show output_full=1, ring
indices 16/42, count=18/partial=0. The first M make is history number 19;
numbers 18 and 17 are M releases with characters 109 and zero respectively.
The eighteen-record history suffix excludes that M make. The same run's
Console transcript then reports EM. This localizes an actual failing handoff
arrangement, not only a hypothetical ignored-release example. It does not yet
identify both release producers or prove that changing the history algorithm
alone fixes every route. KeyQueue's separate count and BIOS return still need
end-to-end correlation before a final repair.

HistoryBoundary now includes this captured ring/output-full and relevant
raw-history sequence. Build root keyboard-history-captured: eight controls
pass; ignored-release-after-make and captured-handoff fail. The latter reports
history=29, device=18, returned=18, first-vk=77/down=0 instead of M make.
This retained failing regression composes the extracted original count and
history-return bodies; it is not guest modification or capability acceptance.
Candidate ntvdm.exe is restored to ordinary 12AF0293 after the diagnostic runs.
No S6 publication or closure is claimed.

### Actual prepend callers and duplicate-release provenance

The diagnostic worker additionally substitutes tests/observation/nt_event_observed.c,
which includes the entire selected original nt_event.c and wraps only its
WriteConsoleInputVDMW call. The wrapper executes the unchanged production
prepend, preserves result/last-error, then copies caller, count, written count,
KeyQueue.KeyCount and each actual record to the existing mapped report.
The first build lacked nt_event's per-file thread-start ABI forced include
and failed compilation; the generator now copies that original edge's exact
host_cflags, rather than inventing alternate compilation flags. x86 /MT
compilation/link and VdmTib owner verification then pass. No product edge uses
either observation object.

Worker SHA-256 DB24E4385019B832EE0E91441AFD309FE188AC64864C4FC0A61A6AB3837EEFEA;
key observer B3441FC8CB1CE2482B502F605186C22B63274A7EEA902B90FDD2F7C9E7F21B63;
event observer 9765D5FEAB2C759A9003C1E4C07DAA58A2E8B8061CC00006CBB2DF5714A7B7DC;
decoder AA767ABF62CA0DA78B2D3188D865B41B64D7A224F0FD6D9A51E16FAED5B7366B.
Private-desktop m0-t423-s6-keyboard-prepend-r1 passes, r2 reproduces outer EM.
Both use nested-mem, ordinary frontend, MVDM_TEST_KEYBOARD_HISTORY_ONLY=1 and
fresh matching .bin report paths. r2 has 344 complete/non-overflowed records:
ten count pairs, 163 history, 74 markers, ten prepend calls and 77 returned
keys. This is a failing run, not a passing duplicate-event tolerance test.

In r2 at tick 67290656:

- Record 187: ReturnUnusedKeyEvents successfully prepends five records,
  beginning with E-up/character-zero (188).
- Record 193: ReturnBiosBufferKeys successfully prepends fourteen records,
  ending with E-down/101 and E-up/101 (206--207). Prepend ordering therefore
  gives the receiver E-down, reconstructed E-up, then the retained physical
  E-up. The first source's caller is 0xCC4BDC (ReturnUnusedKeyEvents +0x10C);
  the second is 0xCC645B (ReturnBiosBufferKeys +0x71B), matching the formal
  map with the same 64-KiB-aligned image relocation.
- At tick 67294359, record 265 successfully prepends eighteen records with
  KeyQueue.KeyCount=0. Actual returned record 266 is M-up/109, not M-down;
  records 267--269 retain E-down, E-up/101 and E-up/zero. The corresponding
  Console transcript contains EM and command-resolution failure.

This closes the missing actual-return/extra-KeyQueue uncertainty from the
previous capture. The duplicate releases in this failed chain are supplied
by the original BIOS reconstruction and raw-history return, not inferred from
character content alone. Both pinned OpenNT nt_event.c copies have hash
7F555A87BA029627D6811C0F7B96964BB8A96AF90C60C89012975A4AC5442076 and contain
the same unconditional reconstructed pair. This is a host keyboard handoff
defect exposed on this route, not an immutable-guest exemption or CPU opcode
failure. Full historical Windows behavior is not claimed from this comparison.

Next repair review must reconcile that original overlap and device/history
accounting while retaining Ctrl+Break/PrintScreen fake makes, held events,
modifier/extended identities, repeats and failure cleanup. Do not add a
frontend-wide key-up filter or assume every raw record produces one device
event. No semantic repair is claimed by these observation additions. Ordinary
candidate 12AF0293 is restored, no test processes remain under R:/ or the
candidate root, and O:/winnt remains the previously verified package.

### Bounded BIOS/hardware join correction candidate

DIV-316 changes only original nt_event.c's existing return owner. It remembers
the first successfully prepended hardware record for that handoff. If the
newest translated BIOS key matches that record's VK, scan and extended identity,
and that record is a release, BIOS reconstruction reuses the physical release
instead of adding a second one. All other paths keep the original pair. The
single-record join state resets before every hardware return and after the
newest BIOS entry, including failed translation. No frontend queue filtering,
device acceptance change, guest mutation or new cross-process contract occurs.

Recovery review: both complete original return bodies are already composed
and contain the evidenced overlap; changing the public Console transport cannot
make those two independently generated records non-overlapping without moving
keyboard semantics into the wrong owner. No external implementation is needed.
The smallest retained correction is an owner-local join identity and condition,
not an autonomous replacement keyboard or general raw-history rewrite. Original
BIOS translation, traversal, prepend ordering, hardware reset and error branches
remain in place. The added state is process-local like its original history.

Removing one synthetic record also makes a batch odd-sized. The original
flush threshold assumes pairs; the candidate flushes when fewer than two slots
remain, avoiding a negative index for a subsequent pair. Normal even batches
are unchanged. Mirror CRLF is restored; relative to HEAD nt_event.c is +21/-2,
SHA-256 573EB3DBBC812206AA05E468963216AE2F4E88580164A550065A1D07EBCE42F3.

Checked-in test tests/app/keyboard_bios_join_test.c composes extracted actual
ReturnUnusedKeyEvents and ReturnBiosBufferKeys, controlling only BIOS memory,
translation and Console sink. Run verify-keyboard-buffer-count.ps1 -BiosJoin
-BuildRoot <repo>/build/M0-T423/S6/keyboard-bios-join-capacity. All fourteen
assertions pass: match, absent hardware, different key/scan/extended flag,
hardware make, failed prepend, successful zero write, physical modifier
preservation, BIOS wrap, newest-only matching, odd batch flush with twenty BIOS
characters, failed newest translation, and subsequent empty hardware state.
The test checks retained actual physical-record identity and BIOS drain.
Fixture hash 51BD2FBC74290572FDB4B5ED64B8D6BEABBCF6242F76F71840C1011ED124A451.
The first fixture link lacked User32 for an original unselected branch;
adding that original API library fixed the test build, not production code.
The previous seven unused-key count/order/cleanup tests also pass in
build/M0-T423/S6/unused-key-join.

The HistoryBoundary test remains deliberately red for already-contaminated
raw history (ignored-release-after-make and captured-handoff). Its other eight
controls pass in keyboard-history-after-bios-join. DIV-316 prevents the observed
overlap at its producer; it does not retrospectively repair arbitrary duplicate
or zero-device-event history. That distinct correspondence limitation remains
open and must not be reported as repaired or waived by the owner.

Ordinary worker before the capacity follow-up, hash
7E1536823E71FA9D33693D5905B62683F17F77B182FA28C65A16F35C31227AFB,
passes nested-mem and interactive-native-dos-return in private Window run
m0-t423-s6-bios-join-window-r1. After the capacity follow-up the formal x86
worker hash is BEE2D860B4845E417B9076C22DE6821F2E128194B9C913169709022F7155CF12;
its private Window r2 is the new acceptance candidate. Ordinary frontend and
immutable guest/configuration are retained. These are focused results, not
S6 closure, full inherited regression, coherent publication or a delivered P.

That exact BEE2D860 candidate subsequently passes all three ordinary Window
r2 cases: nested-mem (three reports), nested-mem-typeahead (three reports with
zero inter-line delay), and interactive-native-dos-return (two reports plus
executed window-native-return marker). Expected task exit 1 is verified in
each; text assertions and unexpected-command-error rejection remain enabled.
The same candidate then passes the full seventeen ordinary Console cases in
m0-t423-s6-bios-join-dos17-summary.json, including EDIT and nested MEM. The
complete command runner, not just its JSON exit-code summary, enforces those
text gates. Both summaries/log families remain under O:/winnt/logs.

O:/winnt/ntvdm.exe is still the verified S4 hash 68010363324704CDD187E62448CCEB5CA07E6602C2A5FC774A527EB867048F04.
This correction has not been published or committed as a P: general raw-history
correspondence, remaining Window/session acceptance, S4/S5 final regression,
separate WOW frontiers and coherent seven-file delivery still remain open.

### Window matrix and EDIT observer input-route correction

Revalidated the same ordinary candidate (worker BEE2D860 and frontend C9A8CFE1
with full hashes above), not the instrumented worker. The original observer
SHA-256 is F11AC944999EA3F982619CA9EE3AAB12F73822F67DAE60CA78E9BD9ACD452119.
Ran Verify-CommandExitStatus.ps1 with OrdinaryFrontend, the S6/p package via
R:/, ObservationTimeoutMs 60000, PRIVATE_DESKTOP=1 and WINDOW_INPUT=1.
Prefix m0-t423-s6-bios-join-window-full-r1 passed its first sixteen cases;
EDIT timed out with observer sentinel 1397769044 instead of task result 1.
The failed run is retained, not a seventeen-case pass.

Scope audit: only the eight scripted interactive cases request CAF and record
an actual Window owner. The other nine immediate/direct cases remain Console
routes even with WINDOW_INPUT set. Never label this invocation seventeen
Window workloads. The seven successful interactive cases are empty,
native-zero, missing (expected command error), mem, nested-empty, nested-mem
and mem-repeat. Their .caf.txt witnesses identify the frontend Window, and
the runner verifies guest/native text as well as results.

The EDIT observer had a concrete route error: its initial command and later
characters went to Window, but the Alt-F menu records still went directly to
the inactive Console. Reuse one test-only write_window_key_records helper for
checked same-frontend Window delivery; do not change product routing. A first
F10-menu attempt (m0-t423-s6-window-edit-menu-r1) also timed out: its captured
guest screen shows xmem inserted in the document, not a menu. It is retained
as a failed fixture choice, not a guest defect or a product pass.

The captured unchanged EDIT screen explicitly says to press ALT to activate
menus. The corrected Window test taps physical Alt make/break, chooses File
with f, then Exit with x. It retains Esc dismissal of the welcome dialog and
the original subsequent MEM text/result gate. Console testing retains its
existing Alt-F sequence. No UI-thread modifier fabrication, hook, SendInput,
desktop switch, guest modification or production binary change is involved.

MSVC Win32/x86 /MT /W4 compiled observer-window-menu.exe using the retained
build/M0-T423/S6/build-window-menu-observer.cmd. Existing fopen warnings remain;
no new runtime boundary is introduced. Observer SHA-256:
E93D7B32F2E38EC855D313F205853646485A52C10CD5E1F1A464CF7A076AE6E5;
source SHA-256 CAD2B1C582E987BB3F43531D23FDB8A3CAFD3755B39A8B16C5DEA91A559F233B.
Private Window prefix m0-t423-s6-window-edit-menu-r2 passes EDIT exit,
subsequent MEM output and final task result 1. Logs and snapshots are under
O:/winnt/logs. This resolves the observed fixture routing failure and proves
this product path; it does not close the separate focus-event, raw-history,
full-session and production-publication gates. Shared nxvm libraries and the
verified O:/winnt runtime package remain unchanged.

The same corrected observer also passes the original Console EDIT-to-MEM
route with WINDOW_INPUT absent, prefix m0-t423-s6-console-edit-menu-r2.
Both runs use the identical ordinary product artifacts; only the observer's
selected input surface differs. Each checks output text and final result 1.

### General history correspondence: expansion counterexample

Extended the existing extracted-source history fixture rather than modifying
production or weakening its two retained failures. A consumed M remains in
raw history but not in the mocked device ring; one subsequent Ctrl-Break or
PrintScreen release passes through the actual nt_process_keys body. Its
source-defined fake make plus release produces two controlled device packets.
The actual count and ReturnUnusedKeyEvents bodies then return both raw records,
incorrectly including consumed M. This complements the earlier zero-packet
release counterexample: raw events can under-count or over-count device events.

Command: tests/observation/verify-keyboard-buffer-count.ps1 -HistoryBoundary
-BuildRoot O:/repos.hobby/ntvdm64/build/M0-T423/S6/keyboard-history-expanded-return.
MSVC Win32/x86 /MT /W4 compilation succeeds. Eight controls pass; four
preservation assertions fail (ignored release after make, captured duplicate
handoff, expanded Ctrl-Break and expanded PrintScreen). Each new failure has
history=2, device=2, returned=2, first-vk=77, consumed-M-replayed=1. The fixture
returns failure intentionally until the product contract is repaired; this is
not an expected-failure acceptance exemption. The mock supplies packet
production and scan mapping, not a real guest/hardware-completion observation.
It proves the count-only selection ambiguity at that specified boundary, not
that a real interactive Ctrl-Break run has already reproduced it.

Test source SHA-256 3F41889E4992405B142C354D960369408A50DD4AF4032E977F6697750C9DB977;
fixture executable SHA-256 17EAC2E113E2F5E0C082928E63E1AF00396C930FB04E95785F4A1E00470F8467.
Exact regex-extracted text comparison against O:/repos.external/OpenNT/base/mvdm
confirms nt_process_keys, CalcNumberOfUnusedKeyEvents, host_key_down and
host_key_up are unchanged. Original nt_event.c hash is
7F555A87BA029627D6811C0F7B96964BB8A96AF90C60C89012975A4AC5442076;
original keyba.c hash is D3326210BE154866924CA99F1E44E0A76E9A912E15975305804F6B59C4CF7953.
The tested return body retains the separately registered initialized-slice and
BIOS-join repairs; neither supplies device-to-history correspondence.

Repair constraint established by this audit: the original source already owns
the transformations and device markers. Preserve those owners and their key
semantics; frontend cannot guess which raw events were accepted or consumed.
Filtering zero-effect releases alone cannot fix the expansion case. Nor can
ReturnUnusedKeyEvents infer consumption from its count alone. Audit the bounded
history/marker association at production/consumption, including held events,
repeat suppression, synthetic modifier events, partial packets, ring wrap and
reset, before selecting the smallest same-shaped bookkeeping seam. Do not
introduce a new scheduler, guest patch, or unconditional key-up suppression.
This is original host-source behavior, not an owner-waived immutable-guest
limitation. General preservation remains an active S6 obligation, not closure.

### Four-level Window versus Console handoff comparison

Further source audit rejects using the existing device marker value directly
as a durable raw-input identity: mark_key_codes_6805_buff cycles 1..127,
remove_from_6805_buff clears consumed slot markers, held events can be replayed
later, and the current 8042 output is outside that ring. KbdEOIHook, not every
port read, clears output_full in the NTVDM profile. Reset and command-response
paths also alter those resources. A complete association must account for
these lifetimes; a count/marker shortcut has not been implemented or accepted.

In parallel, the ordinary four-level matrix now exercises the actual Window
input route. The observer factors its same-frontend HWND lookup and supports
shifted ASCII through WM_CHAR; the unchanged library maps that public text
event into its own physical chord. Unshifted characters still use paired key
messages. This covers text-event normalization, not real host Shift state or
foreground activation. No shared library, guest or production binary changes.
Observer source SHA-256 5A64562F1A7C6C0CC7EF8AFF53EF690F5133F8CCCA703CA7D726CD9856A73498;
x86 /MT executable SHA-256 0DF74F10D24B91B4ECF94666846F8046EF0AB65ED7C46BF11F4189B394DE903A.

Window frontend-chain-a failed in m0-t423-s6-window-four-level-r1 at 45 seconds.
Its scripted input alone took 41,859 ms. The runner's case-specific 45-second
value incorrectly shortened the explicit 60-second caller budget; it now uses
the larger case minimum/caller budget. Runner hash after this test-only repair:
96BC9930C70172965A401015D73088FF8465C296C58294298A41548509D9FFE1.
The 60-second Window r2 also fails (input complete at 40,656 ms), so the budget
correction is not a product repair. Both timeout reports and live stacks remain.

Same ordinary BEE2D860 worker/C9A8CFE1 frontend and corrected observer pass both
Console frontend-chain-a and frontend-chain-b under prefix
m0-t423-s6-console-four-level-r1, with results 1 and 23, actual output markers,
native exit-code checks and frontend cleanup. The Window failure is therefore
not excused by those Console passes. The tests preserve S5's assertion scope.

Window r2's retained pre-cleanup root worker (PID 8496, image base 00e50000)
has this stack after relocation against the exact formal map:
cmdReturnExitCode -> nt_block_event_thread -> ntvdm_console_set_active ->
console_activate -> exchange -> client_transfer -> native wait. In the same
snapshot frontend's presentation thread is at present_loop's wait and its
channel thread is in transfer. This localizes the observed blocked operation
to the I/O handoff, not CCPU instruction execution. It does not yet identify
the pending pipe direction, packet sequence or root cause; do not attribute it
to the separate keyboard-history counterexamples without further evidence.
Next diagnostic priority is this request/response's completion and channel
identity at both endpoints, retaining normal execution and protocol semantics.
Formal map hashes: ntvdm AF9EBB0DB9324FF063663C2E146B3C45BEB6FFDB5EF53F62BF60F1C456C31912;
frontend 3BA211B156537755F1A3AB8EABE40D5A66A0819E6DE0B15592F283241953E0C1.

Separately invoked Window chain B also times out at 60 seconds, prefix
m0-t423-s6-window-four-level-b-r1. Input delivery finishes at 40,813 ms;
the last snapshot shows the inner MEM output and exit command, but no verified
return to the enclosing native prompt. It is a second failed Window route,
not an unexecuted matrix row or a passing partial milestone. All test processes
are gone after runner cleanup. O:/winnt remains unchanged; S6 stays open.

### Expanded handoff timeout sampling

The test-only console wire wrappers preserve the real ReadFile/WriteFile and
GetOverlappedResult results and last-error value, logging copied header words
only after successful transfer. Separate formal observer targets do not enter
ordinary product links. Diagnostic run m0-t423-s6-wire-a-r1 fails chain A:
worker FC930A694B54544AA49715345DB23D28C48F85181CC730FA5B749FD7C9AEDE89,
frontend EAD2017E7A101C9800BEF5F0F363DCA95FD9FE962C1F98F220889E1D91E3E2B7.
Its final received request is sequence 0x373, opcode 0x26 (DOS_ACTIVE), with
no corresponding reply in frontend PID 33376's log. Sequence 0x336 is not the
final request: subsequent traffic exists. Do not use the earlier partial-tail
interpretation as evidence of where communication permanently stopped.
The mapped presentation stack waits in kvm_window_worker_start; the channel
waits for DOS binding. The library signals startup readiness only after window
creation/show/activation. Source review also confirms that current content
handoff destroys and recreates the Window. These are observed dependencies,
not yet causal proof of a permanent deadlock or authority to modify the library.
The diagnostic package was restored to the ordinary BEE2D860/C9A8CFE1 pair.

The startup observer now records loaded-module identities and up to 128 threads
and 64 frames on timeout; the previous 16-thread cap omitted late Window/system
input threads. MSVC x86 /MT compilation passes (existing fopen warnings remain).
Final observer source hash
7F6B89246556160334587F106DE784B7F738D5B7501DC1102E590EFC8F1B3CFD;
executable 41AF8B90AC965E9F38327E7E9BB5ACE67073F610D450FC65FBC75EDB37A1C4E7.
No production code, guest, shared library or published package changed here.

Ordinary runs m0-t423-s6-window-thread-a-r1 and r2 use the same private-desktop
chain-A command and 60-second budget as the preceding comparison. Both fail.
The first (16-frame intermediate observer) captures a late thread waiting
inside MSCTF/IMM32/USER32 while presentation waits for Window readiness.
The second additionally uses the existing worker-thread-snapshot test on its
verified frontend PID 7264; its live wait/stack report is retained with suffix
-waits.txt. The system-input wait is subsequently gone: final Window thread
6164 is in the message loop. Thus this particular startup wait is not proved
permanent, and neither disabling IME nor changing the library is justified.

The second run's actual text proves return to native CMD, but its command has
lost the prefix and executes `-RETURN-%errorlevel%`, producing the corresponding
not-recognized error. Later exit /b 23 and outer MEM execute; full chain exit
still times out. Input submission completed at 40,297 ms, not after the budget.
This is a failed input-preservation/chain-completion result, not an accepted
handoff repair. Follow input ownership, queued events and original history
return across Window recreation; distinguish transient startup delay from the
remaining character-loss defect. All raw records remain in O:/winnt/logs.

### Preserve the Window input source across program handoff

The prior frontend-owned implementation conflated changing the DOS/native I/O
consumer with physical Window retirement. The original command handoff does
not require replacement of the user's keyboard/display. This is a correction
within the admitted standalone frontend, not a new OpenNT provider, scheduler,
guest change or shared-library modification. Existing copied input transfer
and original command completion remain the owners of their respective paths.

A new native-console-frontend-test assertion requires the same visible HWND
before/after DOS activation and native return under Window policy. It fails
on the previous implementation at line 153, retained as
m0-t423-s6-stable-window-before-r2.txt. The first invocation had an incorrectly
quoted executable argument and produced no workload report; it is not a test.
The candidate now drains accepted events before rebinding but retains the
Window and keyboard source. Console-policy graphics return still clears the
Window, while actual X/policy close still retires it and releases held keys.

Unread native input reuses the existing complete ordered batch collector, with
a finite optional destination sink that prepends to the frontend DOS queue.
The original default Console-prepend entry remains for other callers. It avoids
placing returned input in inactive physical CONIN$, where source suppression
would discard it. Window-origin records use the existing frontend pending
queue during the gap before lazy native helper creation; the presentation
owner forwards the batch when that backend exists. No extra input thread,
process, IPC operation or execution ownership is introduced.

An intermediate 1EF45C92 frontend passes the stable-HWND/unit lifecycle test,
but real chain A fails at the first native launch because the lazy-backend gap
was not covered. Retain m0-t423-s6-stable-window-four-level-r1 as failure.
The corrected ordinary frontend hash is
AC9AA04C29FE2D2E1B9C165D65B41D0374BC09F8C4824747F7218EE6B7246BA0;
worker remains BEE2D860B4845E417B9076C22DE6821F2E128194B9C913169709022F7155CF12.
MSVC x86 /MT incremental compilation passes. Real Window chains A and B both
pass under m0-t423-s6-stable-window-four-level-r2, with results 1/23, native
exit witnesses, complete parent echo marker, MEM output and frontend cleanup.
Chain A input submission takes 29,657 ms. These are full existing chain
assertions, not a timeout extension or a diagnostic-product acceptance.

Final frontend local lifecycle test passes as stable-window-frontend-r2:
stable HWND, returned A/B/C records, genuine close releases, completed 0/37/259,
live-target helper loss/cancellation and bounded stalled-peer teardown. Native
host suite r1 also passes. Channel r1 fails its CAF peek count assertion at
line 287; it is NOT a handle-growth failure. Added mismatch-value logging does
not reproduce that failure in r2, which passes all 85 lifetimes with exact
434/434 handles. That repeat does not establish the intermittent failure's
cause or excuse it. Final queue implementation repeat and full DOS regression
are pending at this checkpoint. No candidate was published to O:/winnt.

Follow-up on the exact AC9AA04C frontend: stable-window-channel-r3 passes all
85 lifetimes, 431/431 handles, real graphics close/text return, input growth,
prepend/peek/read, cancellation and stopped-owner refusal. The complete
ordinary Console seventeen pass under stable-window-dos17-r1 with actual guest
text and expected exits, including EDIT-to-MEM. The frontend/host/channel test
reports also explicitly say result=exited and exit=0x00000000; the observer
process's own successful exit alone is not used as acceptance. All 44 pinned
nxvm files and the license still match the import manifest. Window matrix
follow-up, remaining S6 contracts and complete delivery gates remain open.

stable-window-window17-r1 subsequently passes all seventeen matrix rows on
the same candidate. Eight interactive rows use the Window input route;
nine direct rows retain their defined Console route, not seventeen Window
tests. EDIT menu exit followed by MEM passes. Remaining S6 focus/keyboard,
broader session/fault and publication obligations are not waived by this result.

### Lazy native helper input acceptance

The production frontend fixture now covers the previously untested interval
before the first hidden-Console helper exists. Its new expected-39 case uses
the actual frontend and Window library, binds a copied DOS video frame, returns
an unread L make/break pair, relinquishes DOS ownership without retiring the
Window, then types M make/break before native launch. The helper PID mapping
is still zero at both pre-launch checkpoints and the same HWND stays alive.
The actual native target observes exactly L-down/up then M-down/up, including
the expected lowercase characters and zero characters on releases. No extra
keyboard record is accepted. This is a composed native boundary fixture, not
an additional real guest workload or proof of general raw-history correctness.

Build command: `cmd /d /c build\M0-T423\S6\build-stable-window.cmd`, retaining
the formal MSVC Win32/x86 /MT cache. Only the fixture object/link changed;
the existing shadowed-pid warning remains. Invocation uses
`MVDM_OBSERVER_PRIVATE_DESKTOP=1`, S6/observer-window-menu.exe, the formal
native-console-frontend-test.exe and its formal directory, a fresh report
under O:/winnt/logs, and `--observation-timeout-ms 30000`. No desktop switch.

Retained attempts, all prefixed m0-t423-s6-lazy-window-input:

- r1 fails the new total-record-count assumption. Diagnostic r2 shows three
  records: a normal WINDOW_BUFFER_SIZE_EVENT followed by correct L-down/up.
  The test now preserves all records but asserts the two keyboard records.
- r3 fails expected-39 completion. The test stalled the helper immediately
  after launch, before proving asynchronous pending input had arrived. A
  target-side ready event now witnesses four queued keyboard records before
  fault injection; launch completion alone is not that witness. r4 passes.
- r5 fails the older Window reopen visibility assertion before reaching the
  new case. Its loop stopped when the HWND existed, before the visibility
  condition being asserted. All three window-search loops now wait for both
  the correct process and visibility within the original five-second bound.
  This corrects a fixture race; it does not establish a product focus repair.
- Final r6 and r7 both report result=exited, exit=0x00000000, the ordered
  first-helper batch, original 0/37/259 results, live-target helper-loss and
  cancellation behavior, and bounded permanently stalled-helper teardown
  (5000/5016 ms). No helper or target is accepted merely because the observer
  itself exits successfully.

Final fixture source SHA-256:
48F8A0D232D3B995C03C9BD04223FC379357108007793E973734F6E192454A5B;
fixture executable C6BD04501D3A4D5A22D592272EF79C02A4E457B7E772BA10ED45AB34F11C826F.
Production frontend remains AC9AA04C29FE2D2E1B9C165D65B41D0374BC09F8C4824747F7218EE6B7246BA0.
No production code, library, guest or published package changed in this step.
General keyboard correspondence, focus contract and the remaining S6 gates
remain open; there is no P delivery or S closure.

### Keyboard provenance requirements beyond scalar compensation

Re-read the selected nt_event.c event/history loop, nt_process_keys,
AltUpDownUp, SyncToggleKeys and hardware mutex, plus keyba.c marker insertion,
removal, held-event replay, controller response and 8042/EOI paths. The
original OpenNT files contain the same history-versus-device abstraction;
searching the available NTVDMx64 tree found no corresponding replacement
body. The neighboring SoftPC copy retains the marker mechanism and is
read-only comparison, not an imported implementation or runtime dependency.

The current event loop stores normalized raw records before processing them.
Device admission can ignore an unpaired release or suppressed repeat, while
Ctrl-Break/PrintScreen releases and toggle synchronization can create multiple
hardware actions. AltUpDownUp also creates actions outside a raw-record
dispatch. Held actions are replayed later. Thus a per-record adjustment to a
single unread count is insufficient without preserving provenance across all
of these paths. A snapshot made by calling keys_in_6805_buff is particularly
invalid: that function resets both devices. Do not use it as a before/after
sampling helper, filter all releases, or reinterpret its result as raw-record
identity.

The focused HistoryBoundary fixture now adds two explicit controlled negative
states: an unread synthetic host action and an unattributed output-full slot,
both with an already-consumed M retained in history. Neither permits replaying
M. The latter represents possible keyboard response/output state, not a claim
that a specific guest command produced it in a real run. Original cmd_to_6805
0xF2/0xFF inserts unmarked responses; continue_output/translate_6805_8042 can
send those through do_q_int. Direct cmd_to_8042 responses are distinct: its
NTVDM branch clears output_full and must not be conflated with this case.

Ran tests/observation/verify-keyboard-buffer-count.ps1 -HistoryBoundary with
BuildRoot build/M0-T423/S6/keyboard-history-provenance-r1. MSVC x86 /MT builds;
the test deliberately fails with eight controls passing and six preservation
assertions failing (the four retained cases plus both new consumed-M replays).
Log: O:/winnt/logs/m0-t423-s6-keyboard-history-provenance-r1.txt. This is a
controlled source-boundary repro, not an observed new guest regression or a
passing contract. Source hash
46632D07BCF5395AC3CE8DE2EB5450FC9061ABBB962B457926BBE57F6F742E70;
fixture executable F46FF85A53FC02AE813D56B0ACD8287F0FC915317A72D9C32B934699D6109C20.

The next correction must associate original raw history with the actual
pending device data, distinguish internally generated data, and return each
still-pending raw record once in source order. Required coverage includes
one-to-zero/many conversion, held replay/reset, marker/ring/history wrap,
partial multibyte translation, 8042 output and EOI, synthetic toggle/focus
actions, original BIOS join, and teardown. Preserve original device algorithms
and locks; this provenance belongs at the original host keyboard boundary,
not frontend, BaseSrv, guest or CCPU instruction execution. No packet-tag or
weighted-history implementation is admitted as correct merely by this design
requirement. Production code is unchanged in this step; implementation and
all positive/negative proof remain necessary before S6 closure.

### Bounded keyboard-origin carrier selection

For implementation, retain the original 100-record host history, original
6805/8042 and held queues, translation, locking, BIOS return and Console
prepend. Add only provenance tags beside their existing slots and a bounded
selection bitmap that maps a returned ordinal to its original history age.
Tag zero means internally generated data, never the most recent user key.
An input producing several packets retains one origin; an ignored input has
no pending slot. Selecting origins deduplicates and preserves original order.
Tags and bitmap are process-local keyboard bookkeeping, never a wire ABI,
guest-memory value, scheduler or generic resource registry.

Recovery ladder: (1) the original translation units are already selected but
store only packet-boundary markers and scalar counts, not raw-input origins;
(2) a same-shaped Console API cannot recover information discarded inside
the device queues, and filtering at frontend changes the wrong owner's
semantics; (3) the smallest required mirror intrusion is metadata at original
raw-history admission/dispatch, slot insert/remove/translation, held replay,
output/reset and history selection; (4) the finite tag/selection arithmetic
is a new keyboard-only carrier under ntvdm-exe/softpc, with explicit state and
no allocation or I/O. It preserves the original record store rather than
introducing another keyboard implementation. Existing same-owner sources and
available NTVDMx64 comparison yielded no reusable origin carrier. Its removal
condition is recovery of an original equivalent correspondence mechanism.

The first implementation gate is the carrier's boundary tests. Production
composition is not complete until every above metadata transition is wired
and the original-function negative fixture plus real workloads pass; merely
linking the carrier or testing artificial tags cannot close the capability.

Implemented mvdm_keyboard_history.c/header with explicit caller-owned state,
zero-reserved 64-bit origins, bounded pending selection and newest-first
original-history age mapping. It allocates no memory, makes no host calls and
stores no second KEY_EVENT_RECORD ring. Device/held/translation/output tag
fields are metadata slots for the required subsequent original-owner hooks,
not an alternative device buffer. Serial overflow refuses admission without
wrapping; expired origins cannot alias newly overwritten history records.

tests/app/keyboard_origin_test.c passes four assertion groups: ignored/multiple
packet mapping and deduplication; modeled slot movement and zero provenance;
more than two history wraps with exact oldest retained/first expired bounds;
and invalid/nested dispatch, overflow and reset. The slot movement is explicitly
modeled, not claimed as verified original device wiring. Original-function
HistoryBoundary remains red until those hooks and selection are implemented.

The direct MSVC x86 /MT /W4 /WX build passes without warnings. Added an explicit
keyboard-origin-test.exe target to the existing Ninja generator, regenerated
the retained formal x86 graph and built its three fixture edges successfully.
Formal test log O:/winnt/logs/m0-t423-s6-keyboard-origin-r1.txt contains all four
passing groups; executable SHA-256
86821CD469D5FED602D25EC7ABD3101DAFA0139C451E95ACC620CA434194BDEA.
Carrier source 1CC95F298F828105D0F0FD0B8732D8C8B75F8E46297990BF7A3C7093C87C24BF;
header D74D6E641F381EAF0DC785237F6E12104308481DDF05578C008D0BE0D1734D99;
fixture source 8ADB797BD5C1E3DEDE158D5B3CEF52A7794C7930301AE99BF2A7633A367F9A20.
No production link selects the carrier yet. The next implementation must wire
all required metadata transitions before accepting a product candidate; no
production binary or O:/winnt package changed and no P is delivered.

### Origin carrier connected to the original worker candidate

DIV-317 now registers raw history admission and dispatch in nt_event.c and
keeps its existing KEY_EVENT_RECORD ring. GetHistoryKeyEvent maps a selected
ordinal to that original ring's age. CalcNumberOfUnusedKeyEvents snapshots
pending origins before invoking the original hardware reset boundary.
KeyQueue remains initialized empty in this selected dispatch path; it is not
replaced with another queue. keyba.c stamps normal insertion, clears internal
immediate-response origins, carries removal/translation and held replay tags,
publishes the 8042 output origin, and retires/reset-clears those tags with the
original state transitions. Original byte/translation/IRQ algorithms remain
unchanged. No guest patch or CCPU change is involved.

The explicit-state helper now selects wrapped ring, held, output, pending and
prefix origins and preserves that selection across device reset for history
return. An added unit group passes its actual selector, validity gates,
wrap, invalid bounds and reset-before-return. Earlier modeled movement tests
remain labelled modeled, not full original metadata-path proof. All five
groups pass in O:/winnt/logs/m0-t423-s6-keyboard-origin-r2.txt.

Formal MSVC x86 /MT worker and fixture build passes, with inherited original
source warnings; build log m0-t423-s6-keyboard-origin-worker-build-r1.txt.
Candidate worker SHA-256
064F1454E99E503B498D34D2D43CF19F86F6AEA799366D781BC625F76A2FA6DC.
Its prior ordinary candidate BEE2D860 is retained as
build/M0-T423/S6/pre-origin-ntvdm.exe before updating only build/M0-T423/S6/p.
The source line-ending check found keyba.c is LF in HEAD, whereas nt_event.c
is CRLF. A temporary whole-file CRLF normalization was corrected before
verification; current logical diff against HEAD is keyba +55/-2 and nt_event
+41/-3, including earlier S6 modifications. No whole-file format replacement
is accepted.

Real private-Window nested-mem, zero-delay nested-mem-typeahead and
interactive-native-dos-return all pass under m0-t423-s6-origin-window-r1 using
the existing observer, ordinary frontend and unchanged guest/configuration.
These enforce three/three/two MEM text reports, the native-return marker,
result 1 and cleanup, not just process exit. The full seventeen Console routes
are next. Complete original metadata-path negative tests and all remaining
S6 gates are still required; this build is not a delivered P or a general
keyboard-provenance acceptance.

Exact source identities for this candidate: nt_event.c
D152999C7977B5FC35F58B63CAB898442C6618A646D2333B275D7C1D5765EF3F;
keyba.c C10EAEAC5371C562A8B77B0C19A1C22FB5312BADED5A0793F55372D64E8FB0DC;
carrier 37887657CC1CBFE4A1E5E5D7E97E387E311788D62B3DC55076378BB425814E6C;
header A0BA3C33D246B02FAD55955B67ABC0B57CED299DAEFD08EE5AB0F4232FEA4979;
carrier fixture 5E954E8772D487D7909B1C11789AAEDB9FCF556FC92D450860B338F47A11D4DF.
The retained HistoryBoundary fixture explicitly supplies a raw-suffix mock
GetHistoryKeyEvent and passes the old scalar count directly; it now bypasses
the corrected production selection boundary. Preserve it as failure evidence,
but wire the new regression through actual history/selection functions and
metadata transitions before claiming those six cases pass in the product.

The same 064F1454 worker and ordinary AC9AA04C frontend complete all seventeen
Console cases successfully under m0-t423-s6-origin-dos17-r1, including direct
and nested task results, redirected/native routes, and EDIT exit followed by
MEM. The summary contains all seventeen expected/actual matches and the
runner retains its guest-text/error/cleanup checks. This preserves the normal
DOS baseline for this candidate; it does not replace pending negative device
metadata tests, focus, broader Window/fault/WOW gates or coherent publication.

### Original history and device-path verification after origin wiring

Added keyboard_origin_return_test.c and -OriginReturn to the existing
verify-keyboard-buffer-count.ps1. The fixture composes extracted current
update_key_history, InitKeyHistory, GetHistoryKeyEvent,
CalcNumberOfUnusedKeyEvents, ReturnUnusedKeyEvents, PendingKeyboardHistory
and the original device-count function. Only pending device state/reset and
the unavailable Console sink are controlled. It no longer uses the rejected
raw-suffix GetHistoryKeyEvent mock.

MSVC x86 /MT build and all nine reported groups pass in
keyboard-origin-return-r1; raw log prefix m0-t423-s6-keyboard-origin-return-r1
under O:/winnt/logs. The original history ring now passes ignored releases on
either side, two special-key expansions returning one raw record, synthetic
and unattributed output with no consumed-key replay, the captured 29-record
history shape with 18 pending origins, ring wrap, duplicate held/output/prefix
identity, history overwrite/expiry and failed-prepend cleanup. The captured
case controls provenance and preserves the historical shape; it is not a
second capture of the old intermittent runtime failure.

Return fixture SHA-256 8E6DC26584B573DE282DB13D2BBC521A7EEFEC6606C368C11791DB63AC5155A3;
runner 1265449748BCCB1D0540554B189904AA547691E0109EE3C2FA8F0D14CE253521;
executable 1A4DB8FBEB6A87C969E36342FB37B0BC6B57A6B3FE7B2623FA0D9D6D19C2C1F4.
The older scalar counterexample remains explicitly labelled comparison rather
than the corrected product contract, with carrier declarations supplied for
the current extracted held-admission functions.

Added keyboard_origin_device_test.c and verify-keyboard-origin-device.ps1.
It extracts actual keyba.c insertion/removal, marker, buffer clear, original
host up/down decisions, immediate-response insertion, scan translation,
do_q_int, continue_output, EOI, pending selection and held replay bodies.
Its scan-byte generator, IRQ delivery and guest BIOS dependencies are
controlled: this is source-boundary evidence, not complete guest keyboard
emulation. r1 passes five groups. r2 adds actual Reset6805and8042 and passes
all six: ignored release, actual wrapped two-byte translation, delayed replay
without stale-held duplication, split prefix, unattributed controller response,
and full reset preserving the captured selection while retiring device tags.
No production code changes were needed in this verification step. Retain the
original unused-argument/partial-prefix warning and a fixture int-to-char
warning; successful build is not a claim of warning-free original source.

Final device fixture source
5505F3C9F9841D27A6542F514756628B228E5BFC9CFC1A1FE61FECA49864391D;
runner 2BD7D37F3A76981D8CF9C54AD0DCD32EFA34DC38129A937A6B42CF1258DFE0ED;
executable 4D2346BB1270205499791FC1B513A2A62FAA69B7F85EBA7A8B9A185265186A2C.
Run roots are build/M0-T423/S6/keyboard-origin-device-r1 and r2; raw logs are
O:/winnt/logs/m0-t423-s6-keyboard-origin-device-r1.txt and r2.txt.

The unchanged 064F1454 worker/AC9AA04C frontend also passes both real Window
four-level chains under m0-t423-s6-origin-four-level-r1, results 1/23 with
actual MEM output, complete parent/native-return markers and cleanup. These
add to the three focused Window routes and full Console17, not replace S6's
remaining focus, other display/session/fault/WOW and publication gates.

The retained scalar-reference fixture also rebuilds against the current
extracted functions in keyboard-history-scalar-reference-r1. It intentionally
reports the same eight controls and six counterexamples, exiting 1 as before;
log m0-t423-s6-keyboard-history-scalar-reference-r1.txt. This proves the rejected
mock contract remains a reproducible comparator, not that the origin-aware
production history return still has those six failures.

### Complete Window matrix and visible-viewport boundary recheck

The unchanged 064F1454 worker and AC9AA04C frontend complete the seventeen-row
Window-enabled matrix under m0-t423-s6-origin-window17-r1. Command:
Verify-CommandExitStatus.ps1 with observer-window-menu.exe, PackageRoot R:/,
ProcessPackageRoot build/M0-T423/S6/p, OrdinaryFrontend, the S6 G7.COM fixture
and ObservationTimeoutMs 60000; MVDM_OBSERVER_PRIVATE_DESKTOP and
MVDM_OBSERVER_WINDOW_INPUT are both 1. Eight interactive Window routes and
nine direct/Console routes pass their existing text/result checks, including
EDIT exit followed by MEM. The retained summary has seventeen matching
Expected/Actual rows; the runner completes its exact-process cleanup checks.
No owner desktop was switched and no candidate was published.

Separately, frontend_window_library_test.c now distinguishes a large backing
buffer from a large *visible* viewport. The existing 5001x300-buffer case uses
only one visible cell. New tests render 160x54 cells, checking the final
underlined pixel at 1280x756, then reproduce ERROR_NOT_SUPPORTED and an invalid
frame at 161 columns or 55 rows. The current native raster uses fixed 8x14
cells with the library's 1280x768 image capacity. These negative assertions
document a product limitation, not acceptable large-viewport functionality.
It remains an S6 display gap; no clipping, silent success or library-capacity
change is introduced by this test.

The formal x86 /MT frontend-window-library-test.exe incrementally rebuilds
and exits 0; log O:/winnt/logs/m0-t423-s6-native-viewport-boundary-r1.txt
explicitly labels the limitation. Source SHA-256:
162E1C6F1E2FAED45424981FA935349DA7A6F31A8D0811401D2DC9F4374856ED;
fixture executable:
66E881E3108610455CE8CDE662DE27E38FDD06800EBF40AB403DE5F8B5364889.
Existing imported-header CRT warnings are retained, not patched.

Read-only reinspection of the current nxvm window_interface.h gives the same
9FC21794A8C1E5DC875B4D3CD60D4D5A5287FBF22AEA18C181E7D10979FAD92F
hash as the pinned import. It still exposes no focus event or native HWND;
WM_KILLFOCUS/WM_ACTIVATEAPP remain mouse-release paths. Thus the earlier focus
binding gap has not been resolved by an upstream interface change. Do not
claim physical-observation waiver supplies that missing implementation.
Remaining display/focus/session/fault/WOW and publication gates stay open.

Native viewport repair remains within the already admitted frontend Unicode
raster boundary: retain its recovered GDI conversion, palette, cell order and
cursor rather than changing Console state or introducing a renderer. Original
OpenNT does not supply a hidden-native-Console-to-kvm-window raster; the
registered reference-branch converter remains the selected recovery source.
The smallest adaptation fits the complete nominal 8x14 viewport into the
existing image capacity with uniform scale, projects cell edges to that image,
and uses the same edges for glyph clipping, underline and cursor. Ordinary
viewports remain byte-compatible. No shared-library mutation, guest change,
protocol extension or hidden Console resize is needed. Very large viewports
necessarily lose pixel detail at fixed image resolution; they must not fail
solely because the unscaled raster exceeds capacity or crop off their edges.
Verification must cover both overflow axes, nonzero viewport origin, edge
attributes/cursor, subpixel cells and unchanged small-view behavior.

The viewport repair is now implemented in window_frame.c only. It bounds the
complete raster with wide intermediate arithmetic and projects all cell edges
against the resulting dimensions. Zero-area subpixel cells are skipped, while
the surviving cells cover the full image; cursor and underline use the same
edges, avoiding underflow at zero-height rows. Normal-size frames keep their
original dimensions and font. Large viewports are scaled, not cropped; this
cannot preserve individually readable characters when cells become subpixel.

Formal MSVC x86 /MT incremental frontend and fixture builds pass. The complete
frame test passes m0-t423-s6-native-viewport-fit-r1 and expanded r2, including
161x54, 160x55, offset 161x55, 5001x300, 32767x1 and 1x32767 visible areas,
last-cell underline/cursor and invalid cursor rejection. These are controlled
snapshot/pixel tests, not claims of physically usable 32767-column Consoles.
Source window_frame.c:
D6E6C1A8053F4F0B5B9EA8E214E20963447713046BDA7B49702963AB34094B84;
final frame-test source:
2A98B2948807D3621CD8EBB639783A346188B0BBE243677B59EF3DBBDC958170;
frame-test executable:
60A96718D8A5F6BA9C1819D9C91AFEF213E66DB3BFBE1544AAA933B940EDB798.

Actual private-desktop controller/helper integration also exits 0 under
m0-t423-s6-viewport-controller-r1, recording typed ab, WINDOW-NATIVE-OK:ab,
native exit 37, X/CAF/AltEnter, static graphics, independent instances and
joined retirement. Controller executable:
80169DD7ECF1003D5AA70808BFED00AF087A0E30BD625645D33EC461462E52E6.
The new frontend candidate C83C0FD9B7A2078E89DB36E072BD329BBB2E2A2BAAA00C781EC81BC6EDE9C83E
with unchanged 064F1454 worker passes ordinary private Window
interactive-native-dos-return and four-level chains A/B (results 1/1/23),
prefix m0-t423-s6-viewport-handoff-r1. Only build/M0-T423/S6/p was updated.
All 44 library files retain their pinned hashes. Full delivery regression,
focus and remaining S6 gates remain pending; O:/winnt is not updated.

### Real guest graphics Window-to-Console handshake

The old graphics-vram probe enters mode 13h, tests C-VID memory and immediately
restores mode 3. Its marker alone cannot prove a Window was displayed. Added
an optional WINDOW_HANDSHAKE to this authored test probe, never to original
guest media: after graphics initialization INT16 waits for g; after mode 3
it prints S6_GRAPHICS_TEXT_READY and waits for t before its existing success
marker and exit. Build-T420S23VideoGuestTest.ps1 -WindowHandshake selects it
and records selection in the fixture manifest. The default probe retains
its exact EB0F9D1F7C20AFEB8CABB98B321E0560E138B986632A5F7E7E67F8666AB9301F
binary, verified by rebuilding graphics-default-r1.

The observer's opt-in MVDM_OBSERVER_GRAPHICS_RETURN=1 refuses any desktop
outside NTVDMConsoleTest-*. It observes the actual visible LibKvmWindow owned
by frontend.exe without sending CAF, sends X, checks the Window and frontend
remain alive, sends paired G Window keys, waits for the text marker and Window
retirement with live frontend, then sends paired T through the real Console.
The runner additionally requires graphics-handshake=pass, the guest success
text, direct task result 0 and its existing exact-process cleanup. Manifest
validation limits this observer mode to the held direct-graphics-return probe.
This proves window lifecycle and both real guest input routes; it is not a
pixel screenshot or broad graphics-mode compatibility claim.

First probe build graphics-handshake-r1 failed because a scalar PowerShell
define was splatted rather than an array. Explicit array construction fixes
the builder; the failed root is retained. graphics-handshake-r2 builds and
the runtime prefix m0-t423-s6-graphics-handshake-r1 passes with ordinary
C83C0FD9 frontend / 064F1454 worker. Its graphics witness records automatic
Window, X preserving graphics, text closing Window with frontend alive, and
result=pass. Captured text contains both S6_GRAPHICS_TEXT_READY and
S23_GRAPHICS_VRAM_OK; task exit is 0.

Observer source: 96542AB5699A368D8359EEDA06C1C745216C1787F1C17B78ED938DB07E650BD0;
observer executable: 983A61245F96955562CD01575639A5C292C7D5B399D0FC000B344A39EE409D7C;
probe source: 3A62F0A4C977E0857DA8E1EAABA227C030B39E87182D85F4CAC66F763463ACBC;
held probe: 801760058264BB394A16CC58E4C2A61CBC85D49E84BC0F15AA2F9EDEB818EBC0;
builder: B5A82C9F5610E611CEFA8D383BC701D911272541B75881539159E12C9CDF5C4D;
runner: 83F4A0B380ABC90DF75D5D29A322117D0DB66AA805413B4C22859D866B222A55.
All new builds remain under build/M0-T423/S6; runtime logs remain O:/winnt/logs.
No production or shared-library source changed in this verification step.
The rebuilt unchanged default probe also passes both direct-graphics-return
(0) and graphics-return inside COMMAND followed by MEM (1), prefix
m0-t423-s6-graphics-default-r1, using the updated observer with handshake
disabled. These retain the original video-memory/return checks, independently
of the new held-probe Window assertions.

### Window-composed lifecycle and independent-session fault matrix

Extended the existing frontend_loss_noio_test.c and verify-frontend-lifetime.ps1
with -Window, retaining their original task/result/worker-retirement contracts.
After real guest readiness and frontend identity acquisition, the test sends
CAF to its own Console and waits for a visible LibKvmWindow belonging to that
exact frontend PID. It refuses non-NTVDMConsoleTest desktops. Only then does
it exercise normal native-parent completion, frontend death, launcher death,
worker death or helper death. ExpandedFaults keeps a distinct native character
session alive and asserts result 53 and natural retirement independently.
This is one Window session versus an independent Console session, not a claim
of two simultaneous product Window sessions.

Retained failures under O:/winnt/logs/m0-t423-s6-window-fault-r1 through r4:

- r1: fresh S6 candidate lacked logs/ for the authored guest's gate files;
  guest reported file-path failure. The runner now creates that prerequisite
  only inside its already-validated build candidate. Runtime reports still
  go to the declared O:/winnt/logs directory.
- r2: normal assertions passed, but the early Window witness was overwritten
  by later Console repaint. Retain the observed Boolean and emit it with the
  final verdict, rather than relaxing the requirement to see an actual Window.
- r3: frontend-loss assertions exited 0, but raw Console output mode left the
  diagnostic text truncated. After all lifecycle assertions, the fixture now
  restores only its own saved output mode and diagnostic cursor origin before
  printing. This does not prove product Console restoration after abrupt
  frontend death, nor change any product mode or lifetime policy.
- r4: normal/frontend/launcher/worker passed; helper reached the original
  ERROR_BROKEN_PIPE (109) dialog, rather than the old fixture's sole accepted
  ERROR_NO_DATA (232). Current native_console_backend.c::transfer and
  native_console_frontend.c helper/process wait paths explicitly return 109;
  native pipe writes can instead return 232. The fixture now checks exact
  FormatMessage text for only these two pipe-loss outcomes, requires the
  original visible Terminate button and live DOS waiter, then verifies the
  original action yields 1067 and worker retirement. It does not accept any
  arbitrary dialog, map failure to DOS success, or suppress original error UI.

Only tests changed. Guest NOIO.COM remains the authored original fixture hash
C5686BF3619BA5020DA62181AA74D4097669C43873C6D334C7F939E4DB1F721D.
Production candidate remains C83C0FD9 frontend / 064F1454 worker.

The final Window run m0-t423-s6-window-fault-r5 passes all five cases. Every
case requires actual Window observation before the transition, its original
task/result verdict, worker retirement before cleanup and unrelated-session
survival/result 53. Helper loss preserves native result 37 and guest file work,
then waits for the explicit original Terminate action and observes DOS result
1067. It is not successful DOS completion. Test executable SHA-256:
90A5ACAC3526FADE7E40F7C1D2F0602C4A277C6EC45952E937ECEDBC857CABB2;
fixture source: 3D854ED5118BACA13B59D5D8A338CB56538193B66EDF173EA197150D0E58DB30;
runner: C24C806BFEAD4867342C2C1D76B378710663FA14334DB043DF5F5AC968F3D174.
Reproducer: verify-frontend-lifetime.ps1, S6 observer-window-menu.exe,
PackageRoot R:/, physical build/M0-T423/S6/p, fresh EvidenceRoot and LogPrefix,
ExpandedFaults and Window. Window mode does not imply physical focus tests.
The same candidate and fixture also pass all five original Console-mode cases
under m0-t423-s6-console-fault-r1 (same command without Window). This confirms
both presentation modes against the inherited fault/result contracts. No
production binary was changed or published during these tests.

### Inherited twelve-target chains, WOW frontiers and link ownership

The current C83C0FD9 frontend / 064F1454 worker passes both required actual
twelve-target chains, GGGWDWGGGDWD and GGGDDWGGGWWD. Formal x86 chain fixture
targets were relinked through the current graph before running
verify-twelve-target-chain.ps1 with S6 observer-window-menu.exe, formal
BuildRoot, PackageRoot R:/, EvidenceRoot build/M0-T423/S6/twelve-chain-r1 and
LogPrefix m0-t423-s6-twelve-chain-r1. Each chain has 24 ordered ENTER/RETURN
records, expected direct-target results, per-stage actual Console input/output,
GUI non-membership, one authenticated frontend per character group, distinct
groups, first-group retention while the second executes, natural retirement
and no unfinished final DOS task. Identities are 30724:2 / 28992:13 and
13040:38 / 30984:52 respectively. These are Console-mode product chains;
they do not claim twelve-target Window-mode interaction.

The runner now puts observation/event/derived runtime reports in the existing
O:/winnt/logs via LogRoot/LogPrefix, while generated plans, batch fixtures and
the summary manifest stay below build. EventsPath in the summary points to
those logs; the record verifier retains a fallback for historical summaries.
No new directory outside build was created. Runner hash:
70DDBE3557CB02D6999F3968C35A675F2A20062B91F68E6D9000272A461895FA;
record verifier:
E937235FFE02E97CA580C6F9AECAF4D1F86BC93A8220A1FB7F54EDBDAB4C7031.

verify-wow-headless-frontiers.ps1 with PackageNetworkProfile also preserves
all three separate frontiers under m0-t423-s6-wow-frontiers-r1: localized
WINMINE main window, original SOL out-of-memory modal, and original Write
out-of-memory modal. No character frontend is created. Interactive playability
is owner-waived, not tested or inferred; SOL/WRITE remain incomplete apps.
Original SYSTEM.INI matches the live package at
F20A94235712C26B0499D61E7CC080B612BBFE230DE0C65A6832494FDBB1ACAA,
and existing WFWNET.DRV is retained. WOW32.DLL:
A8ABCF9D4637170075992A9318B0A194C19C8E41895FA2DF7A0AC71B8A8ACCA5.
The unchanged S4 window reader is
8BCDB247B99BF5236920D2B7A6B9B796BB2073131B533419A792A3DB07B700D8.
The WOW runner likewise gains LogRoot so isolated-package reports remain
under O:/winnt/logs; source hash:
3EC1E445EEC2C7B85D30D367C6E81DC9A3F1895E995D20BB26BC6A4B6BBBF181.

Strengthened verify-frontend-link-ownership.ps1 to include nested library
source paths, require the selected Window production owner, and reject both
full Window-archive and leaf-library-object leakage into ntvdm. Initial new
negative-control attempts failed because the mutation missed implicit link
outputs and used PowerShell's automatic input variable in a pipeline. Fixing
the mutation, not relaxing ownership checks, makes the current graph and all
five rejection controls pass. Final verifier hash:
44D51441D762E8D71BDCF268F4BD8AED754D612675283C9F54E2CA3E497DDE29.
The launcher still links only its finite client; worker links no frontend
implementation. No production bytes changed in this turn or were published.

### Current component matrix and Console keymouse regression

Rebuilt the selected formal x86 fixture closure, then ran
verify-frontend-transport-contracts.ps1 -ExpandedBackend with the S6 observer,
formal restart-formal-x86 BuildRoot and prefix
m0-t423-s6-final-contracts-r1. All seventeen cases pass their explicit exit
and capability-text assertions: Console facade/video, pointer-contract mock,
scope and request lifetimes, native capture/frontend, 85 real channel
lifetimes, normal/broken/hung close callbacks, native host/control/completion/
close-timeout, frontend controls and backend membership. Component mock claims
remain labelled and do not replace guest acceptance. Graphics channel
creation/retirement and exact post-warm-up handle equality are included.

The old keymouse observer hardcoded O:/WINNT> even when TEST_RUNTIME_ROOT
selected a different isolated package. It now derives the expected prompt
from that test root and still requires the current cursor line, rather than
accepting a historical scrollback prompt. No product discovery or prompt
policy changes. x86 r1 built with a signed/unsigned comparison warning; r2
adds the explicit nonnegative/cast check and rebuilds cleanly. Sources and
probe outputs remain under build/M0-T423/S6/keymouse-r2.

Real guest execution on R:/ now passes m0-t423-s6-keymouse-r1: original
COMMAND, authored KMTST.COM, keyboard/modifier release/PPI/mouse reset,
position/callback/teardown, subsequent MEM output and COMMAND result 1.
The outer observer exits 0; the guest report contains every required S25
marker and keymouse passed=yes stage=command-exit error=0 exit=1
focus-record-transition=injected. This uses ordinary KEY_EVENT/MOUSE_EVENT
and injected Console focus records on a private desktop; it does not verify
physical focus or Window mouse/focus delivery.

Keymouse observer source:
83D44BC4FA40C972F03BFDB4303CB8ECF13AACCB448E35D9929D5CD55693FF7C;
observer executable:
7F2795093DEC60600B4682CB28F88F7DAAD073F25BE53FF69F4E7D7B953C1FE5;
authored probe source:
60D29D3296CA6E6D5DDFC500A0B006E8138C813751318B9DE6A33A19AAE9632F;
probe executable:
92C642D5F92EC4EA8C255D956977B5075C068B26423287CA4BDB66F81A8F6629.
Environment: MVDM_OBSERVER_PRIVATE_DESKTOP=1, TEST_RUNTIME_ROOT=R:/,
MVDM_TEST_KEYMOUSE_SHARED_CONSOLE=1,
MVDM_TEST_KEYMOUSE_FOCUS_TRANSITION=1,
MVDM_TEST_KEYMOUSE_COMMAND=R:/tests/KMTST.COM. The S6 startup observer runs
the keymouse observer with its guest-report path and a 60000 ms budget.
Both reports remain under O:/winnt/logs. No original guest was modified.

The current C83C0FD9 frontend / 064F1454 worker subsequently passes all
seventeen ordinary Console/DOS routes under m0-t423-s6-current-dos17-r1,
including the final EDIT-to-MEM interaction and cleanup. This is fresh
evidence after viewport fitting, not reuse of the preceding frontend hash.

The current formal Window keyboard fixture also exits 0 on a private desktop,
m0-t423-s6-current-window-keyboard-r1. All six printed groups pass: physical
keys/modifier sides/source retirement; noncomposing dead-key prefix; dead-key
composition and retirement discard; native layout characters/key-up/
supplementary Unicode; failed-sink state preservation; and DOS physical
make/break with native cooked typeahead. Fixture executable:
05FB69C85C6A04B588B3B89B1E795320F2FF8674391D7A78C3C242CBAB0D4676.
This run predates the source-local reset import below. No publication,
production P or S6 closure is claimed.

## Owner-approved shared input reset refresh, 2026-09-27

Owner approves the audited four-component refresh and rejects automatic text
viewport/font shrinking. The earlier viewport-fit candidate is not accepted
product policy and must not be published. The selected 44-file dependency
closure now matches SoftPC 3186afcd7b743e9797364c6a762eccd3b5576ca1;
only four imported files differ from the prior nxvm pin. Exact hashes are in
src/frontend-exe/nxvm-import.json; no upstream files were modified.

frontend_keyboard_dispatch now consumes INPUT_RESET using delivered-key
release without source retirement. The added DOS/native cases verify foreign
source isolation, failed delivery preserving held state, repeated reset,
continued input from the same live source, and final retirement. MSVC x86
/MT /std:c11 fixture build passes; private-desktop observer
m0-t423-s6-input-reset-keyboard-r2 exits zero. The first compile omitted the
C11 switch and failed; the first observer attempt could not write the external
log. Neither is passing evidence. This is a focused test, not full publication
acceptance. Full component/product regression remains pending.

Read-only nxvm source inspection distinguishes two presentation mechanisms:
console-broker/win32/console.c grows native backing storage without shrinking
the font or moving the host viewport, retaining native scroll navigation.
The imported kvm-window has no scrollbar/scroll-offset interface. Therefore
that Console policy cannot be claimed to implement scrolling in the native
text-to-Window raster route.

### Owner clarification and implemented separation

Owner clarified that scrollbars apply to the native Console only. Window
keeps kvm-window's existing frame-size client layout and monitor-fit behavior;
no Window scrollbar extension is requested or implemented. The previous
request for permission to add Window scrollbars is withdrawn.

The frontend-native raster no longer shrinks its font to fit the fixed frame
transport. Normal cells remain 8x14; the library alone performs Window client
scaling. Oversized native Window rasters beyond 1280x768 remain an explicit
unsupported transport boundary, not a passing scroll/scaling feature. Console
backing storage is not subject to that Window frame limit.

Formal x86 frontend rebuilt successfully with the refreshed library and reset
consumer; SHA256 5A8F8EBC117A28BE1CA498A94A382EA0E3F43CD60D14FE5A26A0377131963DF3.
Private-desktop m0-t423-s6-reset-scroll-*-r1 runs passed native-console-capture,
frontend-window-library, frontend-window-keyboard and frontend-window-controller.
The capture fixture now explicitly verifies unchanged font size/family/weight/
face after full buffer operations and scroll to columns 4981--5000, with exact
far-edge cell content. Existing assertions retain 300-row history and scrolled
viewport copying. It does not claim physical scrollbar dragging observation.

m0-t423-s6-reset-controller-r2 adds actual private Window WM_KILLFOCUS and
WM_ACTIVATEAPP delivery through the library, copied FIFO, frontend ledger and
native Console sink. One delivered A-down yields one matching A-up, repeated
reset is harmless, source identity and Window remain live, and subsequent
ab/Enter cooked input succeeds. Ordinary policy/X/CAF/AltEnter/static-graphics/
independent-controller/retirement cases continue passing. No owner-desktop
interaction was performed. Full product regression and publication remain open.

The same 5A8F8EBC candidate subsequently passes all seventeen Window DOS cases
(m0-t423-s6-reset-window17-r1), all seventeen Console DOS cases
(m0-t423-s6-reset-console17-r1), and all seventeen ExpandedBackend component
contracts (m0-t423-s6-reset-contracts-r1). These are fresh runs after import and
font-fit removal, not reuse of the earlier C83C0FD9 artifact results.
m0-t423-s6-window-geometry-r1 verifies the unchanged library's client-sizing
function: 640x400 remains 640x400 in an 800x600 work area with decorations;
1280x768 becomes 784x471. Raster contents are not resized by this function.
The frontend-link ownership check and all five deliberate leakage controls
pass. Documentation governance and diff whitespace checks pass. Remaining
product gates/publication are still open; no S/P/T closure is inferred.

m0-t423-s6-reset-twelve-r1 passes both GGGWDWGGGDWD and GGGDDWGGGWWD
chains with actual stage I/O, independent character-session identities,
nested results and final DOS-record cleanup. The immediately following WOW
attempt reset-wow-r1 stopped at its prerequisite because the prior broker was
still retiring; no WOW application was tested by that attempt. Read-only
process inspection then found no broker/worker/frontend. Fresh reset-wow-r2
preserves all three separate frontiers under the unchanged package network
profile: WINMINE main Window, SOL original OOM, WRITE original OOM, with no
character frontend. These are inherited frontiers, not full WOW acceptance.
m0-t423-s6-reset-graphics-r1 passes the held actual-guest graphics handshake:
automatic Window, X retaining graphics, Window keyboard, mode-3 return to
Console input, and guest result zero. The published O:/winnt frontend remains
8923113D6221FB4925EAE708F1E4C9FC75AC7B61A88CA0B353B02B87126DAECA.
The refreshed candidate remains build-only pending the remaining S6 gates.

## Refreshed-library nested/fault verification and remaining frame boundary

m0-t423-s6-reset-window-chain-r1 passes interactive native-to-DOS return (1)
and Window four-level A/B (1/23). m0-t423-s6-reset-window-fault-r1 passes all
five normal/frontend/launcher/worker/helper cases with real Window activation,
original guest error handling, unrelated-session survival and test cleanup.
The source inputs and product frontend are the same 5A8F8EBC candidate.

The remaining native-frame capacity gap is structurally before monitor fit:
frontend_window_native_frame emits 8x14 pixels per cell, whereas imported
kvm_window_frame_validate rejects width above 1280 or height above 768 and
the frame embeds a fixed pixel array. A 200-column native viewport therefore
requires 1600 pixels and cannot reach kvm_win32_resize_client's monitor-fit
policy. Raising only a frontend check would overrun the imported representation.
No local library alteration, font shrinking, dropped cells or second renderer
is authorized as an implicit workaround. This requires an explicitly admitted
shared-frame capacity/interface change (or an owner-defined narrower product
contract); it is not a request for Window scrollbars. The full goal remains
unachieved and no publication/P/S closure is claimed.

## Owner scope correction: no speculative capacity expansion

Owner explicitly rejects changing shared libraries for requirements not
established by original OpenNT. The preceding capacity-extension request and
its S6-blocking classification are withdrawn. The 200-column test was an
implementer-selected Win32 viewport stress boundary, not an admitted OpenNT
requirement. This does not classify unsupported output as a functional pass.
Keep original library limits and errors; do not shrink fonts, crop frames or
change the library to make this test pass. Console scrolling and Window
monitor-fit remain separate owner-approved contracts.

Source review of frontend_window_native_frame confirms that dimensions come
from srWindow, not dwSize. Private-desktop x86 run
O:/winnt/logs/m0-t423-s6-viewport-boundary-r1.txt exits zero and explicitly
reports its 1600x350 rejection as a confirmed limitation, not acceptance.
The same test verifies a 5001x300 backing buffer with a small scrolled viewport,
80x50 DOS text, nominal raster and existing monitor-fit behavior. Thus there
is no demonstrated whole-buffer-versus-viewport conversion bug to repair.
No product binary or imported library changed during this scope correction.
The formal graph comment now accurately says keyboard origin tracking is
selected in production, rather than still fixture-only; it changes no edge.

Continue remaining admitted final review, coherent publication and P gates;
do not claim this documentation update delivers S6 or the full task.

### Current candidate Console fault repeat

verify-frontend-lifetime.ps1 with -ExpandedFaults (without -Window), observer
build/M0-T423/S6/observer-window-menu.exe, PackageRoot R:/ and physical
build/M0-T423/S6/p passes all five normal/frontend/launcher/worker/helper
cases on the unswitched private desktop. Log prefix is
m0-t423-s6-reset-console-fault-r1 under O:/winnt/logs; retained evidence root
is build/M0-T423/S6/reset-console-fault-r1. Fixture assertions check output
and lifecycle, not merely observer exit. Candidate frontend SHA-256 remains
5A8F8EBC117A28BE1CA498A94A382EA0E3F43CD60D14FE5A26A0377131963DF3;
worker remains
064F1454E99E503B498D34D2D43CF19F86F6AEA799366D781BC625F76A2FA6DC.
Both exactly match the formal x86 build. All 44 imported files still match
the pinned manifest and the frontend-only link ownership check passes.
No production change or live-package publication was made for this repeat.

### Final build identity review

The formal x86 Ninja targets frontend.exe, run16.exe, basesrv.exe, monitor.exe,
ntvdm.exe and VDMREDIR.dll build successfully. Ninja relinked run16 and
VDMREDIR without compiling any source. Exact comparison against the tested
build/M0-T423/S6/p package finds six changed bytes per relinked file, exclusively
inside the COFF and IMAGE_DEBUG_DIRECTORY timestamps: run16 offsets 248 and
196468 (four-byte fields), VDMREDIR offsets 296 and 171236. All other bytes
and file lengths are identical. An initial check against the export-directory
timestamp correctly rejected that mistaken field identification; the debug
directory RVA resolved through the PE section table identifies the actual field.
Do not describe the fresh links as hash-identical to the tested candidate.
Retain the exact tested files for coherent publication rather than silently
substituting those timestamp-only relinks. The other five formal artifacts,
including the separate formal WOW32 build, match the tested package by SHA-256.

Source review confirms the controller keeps display policy separate from
graphics classification, sends X/CAF/AE to console policy without task exit,
retains Window for static graphics, and drains copied input only on its owner
thread. Callback storage survives a failed Window join; source retirement is
drained before successful destruction. Native snapshots validate copied tile
bounds and geometry before painting; only srWindow is rasterized. Keyboard
reset releases delivered keys without retiring the surviving Window identity.
Existing controller, keyboard, reset, nested-return and fault tests provide
the corresponding bounded evidence above. This is final-review progress,
not publication or an S/P closure claim.

## Verified candidate publication

The exact previously tested seven-file candidate is now copied to O:/winnt.
Before replacement, no process was using that package. Recoverable original
files and the generated before/after SHA-256 manifest are retained under
build/M0-T423/S6/pre-publication/publication.json; the guarded build-local
publish-verified.ps1 verifies the candidate, backups, copied files and unchanged
configuration, with rollback on replacement failure. SYSTEM.INI, CONFIG.NT
and AUTOEXEC.NT retain their pre-publication hashes; no guest was replaced.

Published SHA-256 values:

| File | SHA-256 |
| --- | --- |
| run16.exe | C9A60EB6EFB5BFFB1A6E375827B84286F39C0A548E8D415CC9C8275A4C49BA7E |
| basesrv.exe | 5B5510F1F6D71DD7EC950E119BA2ACC42DD1B601A68B50084ABC08A8B488E8A1 |
| frontend.exe | 5A8F8EBC117A28BE1CA498A94A382EA0E3F43CD60D14FE5A26A0377131963DF3 |
| ntvdm.exe | 064F1454E99E503B498D34D2D43CF19F86F6AEA799366D781BC625F76A2FA6DC |
| monitor.exe | 83CB4E25A2397EC07155B250195BC037EA2133BBD6D2468531D4FF04FE848C7D |
| WOW32.DLL | A8ABCF9D4637170075992A9318B0A194C19C8E41895FA2DF7A0AC71B8A8ACCA5 |
| VDMREDIR.DLL | C5D9C9BC8073C5285A6BD1A5FA979F65BD77D65702958C41386FA70EC777BD7F |

Published-path Verify-CommandExitStatus.ps1 with -OrdinaryFrontend and 60000ms
observation budget passes all seventeen Console cases. Logs use prefix
m0-t423-s6-published-console17-r1 in O:/winnt/logs. Window and WOW published-path
checks are continuing; this publication entry alone is not an S6/P closure.

### Published-path verification failed; baseline restored

Window run m0-t423-s6-published-window-r1 passes nested-mem, then fails the
interactive-native-dos-return transcript's strict contiguous-overlap assertion.
The process exits 1 and output contains both MEM reports and the native-return
marker, but those facts do not override failed text validation. No S/P closure.
All seven original files were restored from pre-publication and matched the
manifest's previous hashes. O:/winnt is again the prior coherent usable package;
the candidate remains under build. No package process was live during rollback.

The failed line-07 snapshot describes buffer 120x9001 with an 80-column viewport
and shows diagonal/wrapped text; line-08 has a 120-column viewport and different
line layout. Observer write_console_snapshot reads geometry once, then performs
a separate linear ReadConsoleOutputCharacterA and indexes the result by the
earlier width without checking geometry afterward. A resize between those calls
can invalidate row boundaries. This is an evidenced observation-integrity gap,
not yet proof that it alone caused the failure or that the product is correct.
Next verify stable geometry around capture and rerun the strict comparison;
do not relax transcript assertions or discard failing snapshots as a pass.

### Snapshot consistency check does not resolve the failed route

Added test-only console_snapshot.h: bounded before/after geometry validation,
exact character-count validation, and explicit capture-error records rather
than silent evidence omission. console_snapshot_test.c passes deterministic
stable, one-resize, repeated-resize, short-read and API-failure cases under
MSVC Win32/x86 /MT /std:c11. The checked-in transcript test also rejects any
capture-error record instead of skipping it. No production code changed.

Observer observer-stable-snapshot.exe SHA-256
00175BA4AC2387871FAE8A02DC938EF2C3C26D0CF022F67EF53A9124B10A7EFC
reproduces the strict overlap failure in the build-only candidate, prefix
m0-t423-s6-stable-snapshot-window-r1, interactive-native-dos-return. It does
not reach EDIT because the first selected case fails. Geometry validation
did not report a race. Line 06 is 80x28, line 07 is 120x9001 with an 80-column
viewport and diagonal text, and line 08 returns to a 120-column viewport with
different row layout. This disproves treating a simple before/after geometry
check as the complete repair. Next distinguish linear character reads from
coordinate-addressed Console cell reads and inspect the corresponding handoff;
do not infer a worker execution regression or waive the failed output check.
O:/winnt remains the restored baseline and S6 remains open.

### Coordinate-addressed observation comparison

The observer-only OBSERVER_RECTANGLE_CAPTURE variant reads bounded 32-row
CHAR_INFO rectangles with ReadConsoleOutputA, validates the returned rectangle
and before/after geometry, then renders the same row-tagged evidence. It keeps
all strict transcript and guest-output assertions. The unmodified linear
variant remains available for comparison. The same five deterministic mock
cases pass for this variant, including cropped rectangles as rejected short
reads. No library, product or guest changes.

observer-rectangle.exe SHA-256
CBCA1C408643189FF790405381C8ED50C0DAA67688E108A0C918BC128C1D7EA8
passes interactive-native-dos-return and EDIT in
m0-t423-s6-rectangle-window-r1. A fresh r2 passes nested-mem, that native/DOS
return and EDIT, all on the same build-only candidate. Line 07 in r1 now
contains correctly aligned MEM rows and the real native-return marker; no
output filtering or relaxed overlap threshold was added. These two passing
runs support an observation-path dependency, but timing differs too; they
do not yet prove the exact cause of the previous linear-read corruption.
Retain the failed linear runs and compare the two capture methods directly
before calling the observed failure repaired. Live O:/winnt is unchanged.

### Same-run four-read comparison

OBSERVER_COMPARE_CAPTURE records linear/rectangular/linear/rectangular reads
from one handle, requiring exact counts, unchanged geometry and repeated
content for a stable comparison. Build-only private run
m0-t423-s6-capture-compare-r1 passes the original native/DOS-return assertions.
Of ten recorded captures, nine are stable and all nine have byte-identical
linear and rectangular contents; line 03 is unstable and is not an equality
claim. This does not reproduce a stable API-method disagreement. Therefore
the prior two rectangular passes cannot establish that ReadConsoleOutputCharacterA
itself is broken. The remaining evidence points to capture during changing
geometry/content, and source review shows resizing and repainting are separate
Console operations. A stable evidence capture must account for content changes
too, rather than only compare dimensions. No production fix is inferred from
this diagnostic pass; failed evidence and the old live package are retained.

### Bounded repeated-content capture

The ordinary linear observer now requires two complete byte-identical reads
as well as stable geometry, with four bounded attempts. Exhaustion is explicit
ERROR_RETRY evidence; the transcript parser rejects it. Tests add one content
transition and continuously changing content to the previous five cases; all
seven pass. This improves evidence integrity without changing production code
or treating an arbitrary delay as successful execution.

observer-stable-content.exe SHA-256
BD46B7B06919EEBC8489A2318408C49418A74E48480B797A18F4C530BEED5365
passes interactive-native-dos-return and EDIT on the build-only candidate,
prefix m0-t423-s6-stable-content-r1. The strict transcript contains both MEM
results and the native-return marker. Prior corruption is not relabelled as
passing; stable linear/rectangular equality and changing-content evidence are
retained as the scope of the diagnosis.

The same seven candidate hashes were subsequently republished after checking
the entire old live set and its untouched recovery copy. Configuration and
guest remain unchanged. Published-path Window validation is running with
prefix m0-t423-s6-published-stable-r2; no S/P closure is claimed yet.

The published stable-r2 run has now passed all three selected cases:
nested-mem, interactive-native-dos-return and EDIT. It uses the same strict
guest text, count, continuity and result assertions; the code change remains
observer-only. The current live seven files are the tested candidate, not the
earlier recovery state. Published WOW frontier verification remains in progress.

Published WOW prefix m0-t423-s6-published-wow-r2 preserves all three inherited
headless frontiers with the original WFWNET.DRV configuration: WINMINE main
window, SOL original out-of-memory modal, WRITE original out-of-memory modal.
No character frontend is created for these WOW tasks. This is non-regression,
not SOL/WRITE completion or physical WINMINE interaction acceptance.

## S6 P1 delivery review

This production delivery retains frontend-only Window/display ownership,
protocol-12 copied DOS text, original completed-video publication and the
source-owned keyboard correspondence repairs DIV-314 through DIV-317.
Console font/scrolling remains distinct from Window monitor fitting. All
44 selected shared-library files match the approved import manifest; no local
capacity extension or library patch was made. Window mouse remains S7 and
final whole-task cleanup/owner acceptance remains S8, not claimed by this P.

Retained limits: physical foreground/clipping observation is owner-waived;
SOL/WRITE remain their recorded original OOM frontiers; unrequested oversized
native Window rasters retain the imported rejection boundary. Failed release
attempts and recovery are recorded above, not deleted or turned into passes.
The strengthened observer rejects unstable captures and keeps strict guest
output assertions. No product change was made to evade the failed observation.

Documentation governance, whitespace review, imported-byte verification,
frontend-only link ownership and deployed seven-file/configuration identity
have passed. Commit and push are the remaining delivery actions; S6 task
closure and admission of S7 are recorded separately after delivery succeeds.

Delivery succeeded as f0f671e5e, ordinary push to origin/main, with clean
worktree and HEAD...origin/main divergence 0/0 verified. S6 display/keyboard
scope is closed in CURRENT; approved S7 owns Window mouse. The T and overall
frontend work remain open, with S8 cleanup and owner acceptance still required.
