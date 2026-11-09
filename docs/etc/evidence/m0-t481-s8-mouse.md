# T481 S8 — full-screen pointer investigation

The owner reports working mouse movement and clicks, but no visible pointer,
after switching Win3.1 DOS prompts containing either EDIT.COM or EDIT.EXE to
full screen. Windowed interaction and the separate two-window black screen
were owner-tested in S7. This observation is not an independently reproduced
full-screen test.

For a native child in a guest VM, the reached source chain is the authored
NTVDM64.386 `mouse_event`, original INT 33 callback, host notification 19,
`mvdm_vm_executor_mouse_bop`, `mvdm_vm_io_mouse`, and NTVWM presentation
input. The last producer emits an ordinary `MOUSE_EVENT`, and the NTVWM
ordinary-event branch records the coordinates but deliberately sets its own
composed-pointer visibility to false. Relative frontend frame-mouse events
instead enable NTVWM pointer composition. Thus ordinary guest mouse input
alone cannot cause the NTVWM overlay to appear. This source fact explains
why working native-child input does not imply a visible NTVWM overlay; it
does not prove the observed missing pointer, because the guest may own the
pointer instead.

For the original DOS mouse owner, the BOP BF dispatch swaps the NIDDB instance
before calling `mouse_EM_callback`. That callback controls the enhanced-mode
disable flag; `cursor_display` also requires the frontend mouse route to be
active. The authored INSIGNIA.386 exposes the original BOP BF to VMD and
declares the original NIDDB state instanced. Whether VMD actually requests
disable/enable and whether the right VM and route are current during the
full-screen transition remain unobserved. Unconditionally enabling either
renderer could produce a second pointer in windowed mode and is not justified
by the source trace.

The nested NTVDM's ordinary `MOUSE_EVENT` follows the original absolute
mouse processing branch. The project mouse bridge is activated by a separate
relative ENTER record and is reset on ownership release. The input normalizer
passes ordinary mouse records through unchanged, so this particular producer
does not itself activate that bridge. This is a source-derived property, not
an observation that the bridge was inactive at every point of the live run;
the current trace cannot establish whether some other producer had entered it.

An exhaustive source check of the VM frontend's owned input queue found only
key and ordinary mouse producers; returned input validation likewise accepts
only KEY_EVENT and MOUSE_EVENT. It does not admit a relative-frame ENTER.
The nested worker's bridge is initialized/reset inactive, and only an accepted
relative ENTER (or preservation of an already submitted ENTER during IRQ
cancellation) can make it active. Thus a worker using only this VM frontend
cannot activate the software-cursor route through its mouse input. This proves
the reached transport mismatch for that frontend, subject to the separate
question of whether any other frontend can attach or hand off the same worker.
It still does not identify a safe fullscreen/windowed cursor ownership policy.

The existing relative-frame mouse protocol carries pixel deltas and only two
buttons; the Win3.1 VM callback currently supplies absolute text cells and
can report a third button. Replacing the latter record with the former would
therefore change input semantics unless both coordinate origins, geometry and
button coverage are explicitly reconciled. Moreover, the owner observed a
mode change without requiring mouse movement: making visibility depend solely
on the next mouse event would leave stale ownership at a stationary switch.
These are reasons not to apply an unconditional route conversion as a fix.

The original `mouse_io.h` describes the enhanced-mode disable flag specifically
as suppressing the DOS cursor in a windowed enhanced-mode box; the flag is in
`MM_INSTANCE_DATA`, accessed through the NIDDB handle. The BOP wrapper calls
`virtual_swap_instance` before the callback, which selects an instance using
the guest's virtualising byte and rejects an invalid identity. This establishes
the intended per-VM state ownership and why full-screen re-enabling matters,
but not whether the guest sent the enabling callback or whether the physical
route was active in the observed failure.

The host-only VM text test now composes a visible NTVWM text mouse and checks
that its attribute survives a VM publication copy; it passes in both x86 and
x64 cached builds. It does not observe Win3.1 rendering or verify the
ordinary guest mouse path. A full-screen integration probe was prepared but
automatic approval rejected its launch, citing a PowerShell conflict; it was
not executed. Its result must not be inferred.

The existing diagnostic BOP BF interposer now records the guest virtualising
byte and host PID alongside each original enhanced mouse callback. Its isolated
host object compiles with warnings treated as errors. This provides a way to
correlate disable/enable notifications with VM identity; it does not generate
callbacks, establish that a notification occurred, or verify visible pixels.

After read-only inspection found no PowerShell calls in the directly invoked
CMD scripts, their configuration files or the observer source, automatic
approval allowed the same isolated probe to run. `fullscreen01` reached an
editor but the observer reported `result=timeout` at 90 seconds, despite the
wrapper process returning zero; B.TXT contained `bbb`, A.TXT was empty. The
probe has no verified full-screen transition or pointer-pixel assertion and its
diagnostic host predates the new 0x523 trace. Consequently this run is a failed
diagnostic, not evidence for a pointer fix or either complete switch sequence.

`fullscreen02` adds seven guest-controller receipts and shows the controller
returned from both key-injection sequences for each editor (STAGE0 through
STAGE6). Those receipts do not prove that Windows actually switched display
mode. The observer again timed out; B.TXT contained `bbb`, A.TXT was empty,
and a captured native editor frame showed a Save As dialog with extra input.
Therefore the test's post-switch interaction/activation is not reliable and
the run cannot qualify mouse visibility or normal exit.

`fullscreen04` adds message-pump intervals between each injected Alt/Enter
edge, and the diagnostic log now shows separated key submissions for all four
sequences. All seven controller receipts were written, but the observer again
timed out with A.TXT empty and B.TXT containing `bbb`. Thus back-to-back input
was a weakness in the earlier probe but is not sufficient to make this test
pass; actual full-screen state and pointer visibility remain unverified.
The task-local wrapper now checks the observer's actual `result` and exit
fields instead of trusting its process exit alone. Existing successful and
timed-out reports were checked against those predicates; this only prevents
false success reporting and does not repair the switching probe.

The BOP BF interposition was compiled and linked into an isolated diagnostic
host (`bios-mouse.obj` is present in its link map). `fullscreen03` did not
reach Win3.1: the copied host lacked the isolated package's matching identity
resource and the root exited with ERROR_REVISION_MISMATCH (1306). The test
package's NTVDM was restored from its retained package001 copy. No callback
observations or product-failure conclusion can be drawn from this attempt.
The restored file's SHA-256 matches package001
(`67f40d174254ebe55d39ff50c9fa57bb4fb2a920d13fa92e9d691350f3948c6a`).
The maintained package-identity writer is PowerShell-only; no checked-in CMD
or addon equivalent was found. A replacement mechanism must not silently
weaken package identity or touch the production installation.

The existing observer can dump the first megabyte of the emulated machine at
timeout using a build-specific CCPU base RVA. Existing RVAs were recorded for
the separate production host, not the package001 diagnostic image, and the
existing guest-screen trace discards VGA cell attributes. Neither source can
currently prove that the pointer was visibly drawn in the target VM; assuming
the production RVA applies to the diagnostic build would make the evidence
unreliable.

The retained editor003 `inputs.json` records NTVDM SHA-256 beginning
`1371baf2`, while the current package001 and restored nb7s NTVDM both hash
`67f40d17...`. The historical input record therefore cannot supply symbols or
prove the identity of the current executable. No package001 diagnostic PDB
or matching historical map was found in the current S7 build tree.

Subsequent read-only binary analysis matched a unique `mouse_EM_callback`
instruction sequence in the exact package001 NTVDM: it references the same
instance-handle address twice and the expected cursor flag and enhanced-mode
disable offsets. The task-local probe records its RVA and refuses to run if
the isolated executable differs byte-for-byte from package001. A 60-second
diagnostic hold after the first requested switch recorded `disabled=0,
flag=-1` in one NTVDM process and `disabled=0, flag=0` in a second. The hold
intentionally produces a timeout and is not an exit test. This snapshot does
not establish which NIDDB VM each process had selected, actual full-screen
state, cursor pixels, or why the owner sees no pointer; it does show that the
read-only observation is possible without modifying the isolated package.
Original `mouse_io.h` defines displayed as 0 and the initial hidden count as
-1; the two sampled instances therefore had different requested cursor states.
Without the selected VM identity these values must not be assigned to either
editor or interpreted as evidence that a show request was lost.

The same binary analysis independently identified the cached NIDDB instance
number in `virtual_swap_instance`; the observer can now sample it read-only.
In `mousehold02` after the requested switch, one process sampled instance 2,
disabled 1, flag -1; the other sampled instance 0, disabled 0, flag 0. The
`mousewindow01` pre-switch control sampled the same respective values. Both
holds deliberately timed out. Since the state and cached instance number are
separate reads from running processes and the test does not prove display mode
or map the VMM handle to that instance at the snapshot, these comparisons do
not establish the target VM's state or a missing enable callback.

`mousehold03` extends the read-only observer to enumerate the original NIDDB
VM records and their mouse-instance pointers. The isolated process contained
system VM `805e1000` (disabled 0, flag -2), DOS-editor VM `80798000`
(disabled 1, flag -1), and native-editor VM `80835000` (disabled 0, flag -1).
The two editor identities are correlated with that run's command-frame log.
The snapshot reads a running process without a cross-thread atomic lock, and
the probe still does not prove actual full-screen mode or physical pixels.
The native-child parent mouse state was therefore requested hidden in this
sample, but neither causation nor a correct show policy has been established.

The corresponding `mousewindow02` pre-switch per-VM control had the same
three VM identities and flags, but native-editor VM `80835000` sampled
`disabled=1` rather than the requested-switch sample's `disabled=0`;
its show count stayed -1. DOS-editor VM `80798000` was not the switch target
and remained disabled 1/flag -1 in both runs. These independent snapshots are
consistent with a VMD disable/enable transition for the native editor but do
not prove atomicity, actual display mode, visual pixels, or cause. Both runs
are intentional holds, not normal-exit tests.

`mousedos01` targets the DOS-editor VM rather than the native-editor VM. Its
requested-switch hold sampled DOS VM `80798000` as disabled 0 / flag -1,
versus disabled 1 / flag -1 for the window control; the native-editor parent
VM was disabled 1 / flag -1. The nested NTVDM process sampled disabled 0 /
flag 0 (requested visible). These values are consistent with VMD enabling the
parent VM's renderer while the nested DOS editor requests its own cursor.
As above, the run has no independent full-screen or rendered-pixel proof.

The actual nested mouse path sends guest callback data as ordinary
`MOUSE_EVENT` via `mvdm_vm_io_mouse`; the nested NTVDM dispatches it to
`nt_process_mouse`, not the relative-frame queue that activates
`mvdm_softpc_mouse_route_active`. The retained software `cursor_display`
returns when that relative route is inactive. This is a concrete difference
between input delivery and the software cursor's routing condition, and a
candidate explanation for a visible request that produces no pointer. It is
not by itself proof of the observed defect: fullscreen/window ownership,
route state at the relevant event and the effect on final pixels remain to be
observed. Enabling this unconditionally could also duplicate the guest Windows
pointer in windowed mode.

The test input API `0x7f82` sends each injected keyboard edge through the
diagnostic host's `nt_key_down_action`/`nt_key_up_action` and emulated keyboard
controller; successful injection is still not an acknowledgement by the
Win3.1 VDD that it entered or left full screen. The authored INSIGNIA VMD
query supplies BOP BF and advertises NIDDB instancing, matching the shape of
the earlier isolated `mouse-vmd-callout.asm` fixture. This narrows the next
observation to actual VMD calls and selected VM state, without establishing
that the missing full-screen pointer is caused by that interface.

An experimental addon temporarily requested the parent VM's original INT 33
software cursor and restored its display count on completion. Isolated
`s8dos01` and `s8native01` snapshots each showed the requested fullscreen VM
at disabled 0 / flag 0; these are state observations, not pixel verification.
The same candidate failed the dual-editor interaction/exit probe twice:
`s8candidate` timed out before both files were saved and `s8candidate2`
timed out after saving both. An identical baseline-driver run, `s8baseline`,
exited successfully and saved both files. The candidate is not qualified;
these timeouts do not by themselves identify their causal mechanism. No
candidate was published or installed into O:/w31.

There is an independent correctness issue with borrowing the parent software
cursor without coordinating screen writes: `software_text_cursor_display`
saves the covered guest cell and `software_text_cursor_undisplay` writes that
saved word back. Meanwhile `command_screen_apply` writes the child's snapshot
to those same guest text cells through INT 10. If a frame changes the covered
cell between those two cursor operations, the later restore can overwrite the
new cell. This source-level interference is sufficient to reject an
uncoordinated parent-cursor implementation, but does not establish it caused
either observed timeout. A replacement must preserve frame updates, the original
window/fullscreen visibility distinction, and both mouse input paths.

A second isolated experiment brackets child text-frame writes by hiding and
showing the borrowed parent cursor. `s8bracket01` completed the dual-editor
interaction and exit sequence with status zero and saved `aaa` and `bbb`.
The requested-switch holds `s8bracketdos` and `s8bracketnative` each sampled
their selected VM at disabled 0 / flag 0, with the other VM disabled 1 / flag
0. The holds intentionally end at observation timeout. Neither the sampled
state nor the dual-editor pass proves the fullscreen cursor was actually
drawn or survives frame changes; additionally the probe's private desktop had
no visible product window. This experiment remains confined to build output,
has not been released and is not yet a production implementation.

A follow-up experiment brackets every screen write, including when the parent
cursor was already visible before command admission. `s8bracket02` again
completed the isolated dual-editor interaction and exit and saved `aaa` and
`bbb`. This also is not a fullscreen pixel or actual pointer-route witness;
the experiment was then confined to build output and was not published. After
the real Window frame evidence below, its tested implementation was copied to
the authored addon source for continued validation; it remains unqualified.

The isolated `s8moveonly` and `s8movebaseline` runs exercised the real NTCON
Window route, waited for the same DOS-editor switch request, and injected one
acknowledged mouse movement without a click. The test read the pinned NTCON
Window's stable 80x25 text frame, rather than the parked Console. After input,
both captures contain the same EDIT menu/status content; their 2,000 cells
match except for cell (2,0): the baseline has glyph 32, foreground 7,
background 0 and the bracket experiment has glyph 32, foreground 0,
background 7. This one-cell inverse is consistent with the software text
pointer reaching the frontend. The before/after candidate captures also
include a menu activation/status change, so that pair alone cannot identify
the pointer. The private-desktop runs intentionally hold and do not establish
actual window mode transition acknowledgement, sustained redraw, final exit,
or the native EDIT case; independent coverage remains necessary.

The real NTCON route was also exercised for native EDIT with the bracket
experiment (`s8movenative`). One acknowledged movement changed only two cells
in the captured 80x25 frame: cell (0,0) changed foreground/background 8/7 to
15/0 and cell (2,0) changed 15/0 to 8/7, while both glyphs were retained.
This directly observes the text-pointer pattern moving in the Window frame,
not merely an internal mouse flag. The test requests a switch and holds; it
does not independently establish the display-mode acknowledgement, later
frame persistence, switching back, or normal termination.

The `s8center` native-EDIT repetition moved the pointer into the blank editing
area instead of the menu. Its two captured Window frames differ in exactly two
cells: (0,0) returns from 8/7 to 15/0 and (20,5) changes from 7/0 to 0/7,
with both glyphs unchanged. This avoids attributing a menu highlight to the
pointer and confirms the visible frame's text cursor moved into the editor.

`s8redraw` repeated that real Window route and then sent one ordinary `a`
keystroke to native EDIT. The subsequent captured frame shows the new `a`,
the expected position/modified indicators, and the same inverted mouse cell
at (20,5). The mouse cell remained glyph 32 with foreground/background 0/7.
This observes pointer persistence across an actual child-frame update, not
just the initial movement. The intentional-hold probe is not an exit test.

`s8dosredraw` used the same real Window route with DOS EDIT. Moving into
the editor changed the cell at (20,5) from foreground/background 7/1 to
0/6 while retaining the glyph; that inverted cell remained present after a
subsequent key and frame update. The observed key changed a status character,
not editor text, so this is not evidence of DOS text insertion.

`assert-pointer-frames.py` now checks the captured Window cell geometry,
foreground/background inversion, preserved glyph, persistence across a changed
frame, and rejects missing updates. It passes the `s8redraw` and
`s8dosredraw` captures and rejects the baseline no-pointer capture. This is
an assertion over those observed frames, not an independent new execution.
The three capture SHA-256 values (before / after / update) are:

| Probe | Before | After | Update |
| --- | --- | --- | --- |
| `s8redraw` native | `F0653293AB5403DD1EF838FB318A0644C123D88348825260ABF10F688826CF2B` | `315347A3EDB19842AAE0825C8663788CA1BD9C9EB1CE6A71F9A0DE1BED7F7765` | `543A764E15340F8C185B491037AA3787A96AACAFC935F1F193BC537B538E9B4D` |
| `s8dosredraw` DOS | `5FF629D6CEE1357AA48DA91C49E0F670D1F0816067E11EA1F4D327BD0E93797E` | `D0E605B6B69973CDC55BA4C3267D14EACB720EEA42BBCBAA0CD28A8559ED4EA9` | `64D94704F79CE24AAE1465CBD447A6D460E48C97FDCA48F040E8C4A3DA13ACE7` |

`s8switch01` exercised the bracket candidate with both EDIT windows and
requested switch/back sequences. Stage files 0 through 6 were produced, but
the observer recorded `result=timeout`; only B.TXT contained its expected
content. The stage markers prove only that the controller issued its requests,
not that the switches completed or that either interaction/exit passed. This
is an outstanding failed candidate regression and cannot qualify a release.

The identical baseline-driver control `s8switchbaseline` also timed out,
before even producing stage 0; it therefore is not a valid completed-switch
comparison and does not establish that the candidate caused the failure.
The candidate trace contains `AAAXIT` at the returned COMMAND prompt: some
input intended for the editor reached the parent after that editor exited.
The current controller does not acknowledge completion of each display-mode
transition. Its timeout cannot by itself prove a product exit deadlock or a
valid full switch-back test. A corrected witness must synchronize the actual
transition and editor input/exit stages rather than claim success from the
existing marker files.

The reduced single-native-switch variant also times out on both the candidate
(`s8single-native`) and the identical baseline (`s8single-baseline`). Both
reach stage 3 and show the same `AAAXIT` text and command error after native
EDIT closes. This is a direct like-for-like reproduction before and after the
cursor change; it rules out treating that observation as a new candidate-only
regression, but does not establish whether the cause is the test controller or
the existing product input/switch path. Neither run is an acceptance pass.

The `s8ack02` and `s8ackbase` witnesses added Escape after the switch-back
request and waited for the native EDIT application frame to contain `aaa`
before sending save or exit. Both reached stage 3, but neither produced the
content receipt before the observation timeout. The watcher used the existing
diagnostic host's application-frame log; no guest image or runtime binary was
modified for it. Thus the previous `AAAXIT` sequence is not explained by
merely allowing more time for a confirmed editor update, but these tests still
do not distinguish rejected test key injection from an existing product input
or focus defect. No passing exit is claimed.

`s8realreturn` and `s8realreturnbase` additionally use the real NTCON Window
route and the same switch-back controller with candidate and baseline drivers.
Both produced a stable Window graphics frame showing the windowed EDIT and
reported an acknowledged mouse injection, followed by a test `a` key sent via
the Window input API. Neither diagnostic application-frame log showed that
character before the intentional hold expired. A single early graphics frame
in the baseline was partially painted, so it must not be called a black-screen
reproduction. The Window send succeeded, but that is not an acknowledgement by
EDIT; target focus, key semantics and production input propagation remain to be
distinguished. These captures do not qualify normal exit.

`s8returnfocus` repeated the baseline Window test after clicking the native
EDIT area; `a` still did not appear in the captured application frame. A
same-driver no-switch control, `s8noswitchfocus`, used the same Window mouse
click and keyboard injection and observed `a` in the native EDIT frame with
the modified indicator. Both are intentional holds. This positive control
supports the injection mechanism in the no-switch scenario and establishes a
repeatable switch-associated difference; it does not yet distinguish Win3.1
focus/routing state from the underlying keyboard forwarding path or prove the
physical-user path.

An independently authored diagnostic addon logs only the relevant INT 15
entries in the existing diagnostic host; it is never a release candidate.
After the marked Window key injection, `s8keycontrol2` observed the A make,
BIOS 9102 character commit and A break in the native editor's VM, followed by
the actual editor frame containing `a`. `s8keyafter` observed the preceding
Alt release but no A make/commit/break in that VM before the hold ended. In a
separate switch-back hold, `s8kbdstate` sampled the original keyboard slot
with EOI-pending 0 and output-full 0 (read 1, acknowledged 1), so a still-full
output slot at that later sample is not established as the cause. These are
separate non-atomic observations; they do not yet prove where the A input was
lost. The image-specific offsets were verified against the exact package's
compiled instruction patterns before the read.

`s8kbdqueue` extends the same delayed read to the original keyboard queue and
interface-enable state. At the final held sample, both NTVDM processes report
pending EOI 0, output-full 0, 496 free of 496 queue entries, and interface
disabled 0. This shows neither an outstanding output slot nor a queued or
disabled controller at that later point; it does not prove whether the A key
was never submitted, was consumed elsewhere, or was dropped earlier.

In `s8frozen2`, the observer sampled NTCON's real Window context immediately
before sending A and read `frozen=0`; its structure offset was compiled from
the exact current source, and the staged NTCON's `.text`, `.data`, `.rdata`,
and unwind sections match the built image (the staged resource differs).
The A was still not observed in the editor. This rules out a frozen Window at
that sampled instant, not rejection in another input-queue stage. The earlier
`s8frozen` attempt never reached its sample and is not evidence.

The `s8sinkcontrol` test-only Window-thread hook submitted B make/break through
the actual NTCON component input sink and observed the existing input queue
accept both; in the no-switch control the target VM received B and native EDIT
displayed `ba` after the ordinary A key. The otherwise equivalent `s8sink`
switch-back hold also acknowledged both sink calls, but its target VM did not
show B or A. This bypasses the Window keyboard normalizer only for diagnostic
B and cannot stand in for ordinary key delivery.

In `s8owner`, a test-only read of the live NTCON channel binding at those same
accepted B edges identified worker PID 14300. The process handle resolved to
the isolated root NTVDM, and the later snapshot lists both Win3.1 VM records
inside that process. The Window was not bound to the outer NTVWM at the sample.
The queue-count reads are non-atomic; they do not establish that the worker
actually consumed either key or rule out subsequent rebinding. The local
structure offsets were compiled from the current source after checking staged
NTCON executable sections against the selected build.

Dedicated switch-back holds `s8returnnative` and `s8returndos` each produced
stages 0 through 3 after requesting full screen and then window mode. At the
intentional hold, the respective selected parent VM was sampled with VMD
disabled 1 / visibility count 0, consistent with original window-mode
suppression; the peer VM was also disabled. These held state snapshots do not
independently prove displayed geometry or normal exit.

The official addon build `build/t481-s8-official001/build.json` used the
current source and produced SHA-256
`A2D689BAD411B09AF02B8D8BB0DCEF7F0217BFA23BD1E4A3203758249D8665BB`,
byte-identical to the driver used in the bracket and Window-frame probes.
Both cached x86 and x64 product builds reported no work, and their non-desktop
CTest selections passed 46/46 each. These are not the Full product gate or a
replacement for the unresolved transition/exit witness; no publication follows
from them alone.

Full product verification subsequently exited zero and reported `PASS Full
selected gates; identical 11-file package` in `build/t481-s7/full-s8-child.out`.
The DOS4GW/Nesticle case was explicitly skipped because its external fixture
was not supplied. Full exercised the unchanged host package and is not itself
evidence for the new addon pointer or the unresolved switch/exit test.

The owner reported ordinary manual interaction works and directed attention to
the pointer fix rather than treating an unverified automated-input failure as
a confirmed product defect. The 11 Full-tested host images were compared with
both release and O:\winnt\system32: all match the recorded manifest. The
official S8 addon was installed to O:/w31\patch for owner testing after
closing the two existing project NTVDM processes, and its SHA-256 matches the
official build. This is a manual-test candidate, not a completed S8 acceptance:
the switch-back automation and normal-exit witness remain unresolved and the
owner has not yet verified the new driver.

Owner follow-up on the installed S8 candidate: most interactions work, but
starting windowed EDIT.COM, switching full screen, using CAF to select Window,
returning to the Win3.1 window and entering full screen again produced two
malformed pointer blocks on one attempt and a crash on another. This is an
actual user observation, not an automated reproduction or an established
cause; both render ownership and crash path require investigation. The
original report does not establish that the two outcomes share a cause.

The retained second failure was inspected without closing its window. The
system-error dialog belongs to the Win3.1 root NTVDM PID 5572; the EDIT.COM
child is a separate NTVDM PID 32684. `DisplayErrorTerm` formats Windows error
13 as the displayed message, but multiple callers can supply it. A read-only
live sample of the root's keyboard history, using offsets verified against
the installed image, found issued 52, dispatched 52, active 0. This is only
a later snapshot and cannot identify the dialog's caller. A full diagnostic
dump was successfully created under the disposable task build directory, but
its system/administrator-only ACL prevented the current executor from reading
it; no stack attribution is claimed. The user's running scene was not closed.

A subsequent user-readable full-memory snapshot of the same live PID was
created without dismissing the dialog. Installed and build NTVDM `.text`,
`.rdata`, `.data` and `.reloc` sections match (only `.rsrc` differs), making
the build link map applicable to those code sections. The snapshot contains a
return address from `mvdm_softpc_mouse_apply` at the `DisplayErrorTerm` call;
its error argument is 13. The called `mvdm_softpc_mouse_prepare` returns that
value when the selected virtual width or height is invalid. The selected
instance in the same snapshot has width 640 and signed height -4889. This
establishes the direct rejection of corrupt/invalid mouse geometry, not the
writer or timing that produced it and not a cause for the separately observed
colored vertical bars. Snapshot analysis is read-only; the window remains open.
The same snapshot's original NIDDB records contain two live VM instances:
slot 0 reports mouse area `(0,0)-(640,480)`, while selected slot 1 reports
`(0,0)-(640,-4889)`. The master mouse handle points to slot 1 and the cached
selected virtual byte is 1. Thus this is not merely a global geometry value
shared by both VM instances. These observations do not independently identify
the guest VM or the write that damaged slot 1.
The saved process-wide `jap_mouse` flag is 1. In the original
`EGA_graphics_cursor_display`, that mode temporarily scales the mouse Y and
virtual screen height by 19/8 but restores them only inside the drawing
intersection branch. An off-screen or excluded cursor skips restoration.
Applying the original signed 16-bit calculation seven times to the prior
200-pixel height gives 475, 1128, 2679, 6362, 15109, -29653, -4889, exactly
matching the retained failure. This proves the original function can produce
the observed value; the full seven-call sequence was not recorded live.
MVDM-HOST-DIV-893 moves restoration outside the conditional, preserving the
original calculation. `verify-mouse-ega-restore.cjs` extracts the actual body:
the original fails the first off-screen assertion, and the candidate passes
seven repeated off-screen calls, exclusion, actual drawing and non-Japanese
mode with bounded mock video/geometry. Real switching remains to validate.
The test additionally flips the mode during a drawn cursor and verifies that
the coordinates saved at entry are restored. Its negative control deliberately
fails against the unmodified source; the test runner exits successfully only
when that negative fails and the fixed body passes. The x86 worker was rebuilt
from its own nested Ninja tree, then a fresh coherent eleven-image candidate
was staged. Full selected product gates passed against that candidate;
DOS4GW/Nesticle was skipped for lack of its external fixture. The new host and
S8 addon also passed Win3.1 standard/enhanced mode crossed with DOSONLY 0/1,
including normal startup and exit. These runs do not constitute a verified
reproduction of the owner's complete repeated CAF/Alt+Enter sequence, nor a
Win95 regression. The documentation governance gate also passed.
The owner-test candidate was subsequently published as the complete 11-image
package `3d28a2e7-bb8a-42ce-a1de-b5572117d7d8`; every host image was
hash-verified in release and the configured system32 target. W31's addon pair
was verified against the published addon manifest, and SYSTEM.INI retained its
pre-publication hash. Publication records the repeated-switch, Win95 and
missing DOS4GW fixture limits; it does not claim user acceptance or S8 closure.
An x86 helper compiled using the selected worker's type/feature flags places
the original 16-element `graphics_cursor_background` at offset 798 and the
virtual-screen area at offset 872 in the same `MM_INSTANCE_DATA`. The retained
slot-1 snapshot reports mode 3, saved area `(640,100)-(656,116)`, save position
`(639,388)` and invalid virtual height. This layout makes an out-of-range
graphics-background write a concrete hypothesis to check; the snapshot alone
does not show such a write or explain the differing saved coordinates.
The same type-checked offsets show the selected instance's graphics cursor
size is 16 by 16 and its hotspot is (-1,-1), matching the system instance;
an oversized configured cursor is therefore not established in this snapshot.

The `s8double01` isolated diagnostic used the official S8 addon and a test
controller requesting three Alt+Enter transitions on the DOS editor. The
runner (PID 13220) exited 1 after its 90-second observation timeout; its
observer reported scripted console input failed and no transition-stage
receipt was obtained. COM1, COM2 and COM4 warnings were ignored in the test.
It therefore did not reproduce or clear the owner report, and does not include
the owner's intermediate CAF timing. The owner's separate live session was
not closed. The next witness needs reliable command delivery and actual mode
acknowledgements before it can localize the corruption.

The W31 `insignia.386` installation also matches its release hash. A separate
addon-manifest check found that the historical `compat95.asm` source hash is
the SHA-256 of its CRLF bytes (`FB198C...`), while the current LF checkout is
`3AE107...`; converting the current bytes to CRLF reproduces the manifest
hash exactly. This is a source line-ending identity difference, not evidence
of a changed driver or an S8 repair. The manifest remains unchanged for that
historical artifact; future package verification must account for its recorded
build input instead of silently substituting the current byte hash.

Source review of the candidate's visibility balancing: original INT 33 function
42 returns the signed cursor counter (`mouse_get_cursor_hot_spot`); show
increments it only until zero, and hide decrements it. The candidate records
the number of show calls it contributes per VM and issues that many hides on
command cleanup, including the existing failed-admission path. Frame updates
are bracketed by one hide/show pair. This explains restoration when no other
actor changes the counter during the borrow; concurrent independent changes
to that counter were not exercised and are not claimed covered.

While awaiting owner testing, a read-only S9 preparation pinned adjacent
SoftPC HEAD `2c0b0646` (2026-11-08). Candidate changes to classify include
`8c27a3b6` (CHAIN2 DWORD reads), `7a77c818` and `fd87c1bd` (BIOS access,
EOI and virtual text height), `a43c6d24` (GDP subfields and unchained DWORD
spacing), `19a68782` (CHAIN2 bulk fills), and `ecf263a8` (mirror/snapshot
cleanup). Matching filenames exist in this tree for `sevid000.c`,
`sevid019.c`, `slave_bs.c`, `ccpusas4.c`, `ega_vide.c`, `fdisk.c` and
`keyba.c`. Filename matches do not prove selected build paths, semantic
equivalence, missing fixes or import suitability. No S9 code has been imported;
S8 remains the sole active packet.

Initial source-level comparison narrows that list: the six CPL user-access
checks are already corrected locally as MVDM-HOST-DIV-891, the slave EOI uses
the command port under MVDM-HOST-DIV-883, and the virtual VGA text-height
case exists as MVDM-HOST-DIV-890 with a locally different CRTC calculation.
The CHAIN2 DWORD composition is documented under MVDM-HOST-DIV-889. These
are source comparisons, not proof that every upstream test or adjacent fix is
already covered. Conversely, all four protected/virtual BIOS word and DWORD
fallback returns in `ccpusas4.c` still cast `biosDoInst` to `IU8`; upstream
`fd87c1bd` returns `IU16`/`IU32`. This is a concrete code difference and a
potential truncation when the fallback returns a value wider than a byte;
whether the selected production path reaches it, its original calling
contract, and the required tests remain S9 work.

Further static tracing identifies the actual original call chain: `c_sas_w_at`
calls `bios_read_word`, and `c_sas_dw_at` calls `bios_read_double` for an
aligned-in-page read (or `bios_read_word` for a cross-page read). Their direct
physical and successful-translation paths retain full width; the four `IU8`
casts are confined to translation-failure virtualization. `biosDoInst`
returns `IU32` from the emulated EAX, so those casts discard upper bits before
returning to the wider SAS interfaces. This identifies the failure condition
from code, not a demonstrated runtime hit. Protected-mode selector availability,
virtual-8086 paging, split reads and real guest behavior still need verification
in S9 before import and publication.

The subsequent graphics comparison confirms that SoftPC `8c27a3b6` is already
registered and qualified locally under DIV-889, including its mode-1 offsets;
it must not be reimported blindly. In contrast, the local `sevid019.c`
contains the older 16-bit rotation sequence at the DWORD odd-head path, while
SoftPC `19a68782` uses a full-width rotation and additional empty/tail guards.
This is a source difference, not evidence of an observed product failure or
that the upstream GDP access adapter can be copied as-is. S9 must check the
actual selected function callers, layout and upstream positive/negative tests
before deciding the minimal compatible import.
The affected local entrypoints are `S_2700_Chain2WordFill_Copy` and
`S_2703_Chain2DwordFill_Copy`; `evidfunc.c` places both in the CHAIN2 Copy
dispatch table. This establishes a production dispatch-table reference, not
that a particular workload selected or executed either entry. S9's focused
regression should cover zero count, odd/even destinations and word/DWORD data
against the real local provider, then run the relevant DOS and Win3.1 paths.
SoftPC `19a68782` supplies a complete-plane oracle for six starting addresses,
four DWORD values and four counts, including zero; it also checks zero-count
byte and word fills. That test is an upstream reference and has not been run
against this project. A search of the checked-in test trees found no named
local CHAIN2 fill oracle; S9 must check existing indirect coverage and add a
maintained local test where needed instead of relying on disposable build logs.
The actual x86 worker Ninja graph includes both `ccpusas4.c` and
`SEVID019.c`. In source, `ev_glue.c` selects `chain2_evid` by write mode and
dispatches word/DWORD fills through the selected pointers. This establishes
that both translation units enter the product build and that the fill code has
a conditional production route; neither fact demonstrates a runtime hit.

SoftPC `ecf263a8` primarily changes that project's device-snapshot capture
and restoration, plus redundant mixer declarations. A symbol search of this
tree found none of its `snapshot_encode_active_command`,
`keyboard_anomalous_index`, `Q_SNAPSHOT_ENCODE_CALLBACK`, or
`MixerSetMasterVolume` identifiers. This makes a direct patch import
inapplicable by those names; it does not prove absence of equivalent behavior
under different names. S9 should classify the actual feature before carrying
any snapshot code across, rather than importing it as a generic keyboard fix.

S10 prerequisite review: the prior Win95 results in
`artifacts/t481-s5/unified-name.json` used addon SHA-256 `0E4DA9...`, not the
new `A2D689...` S8 candidate. The older addon passed two-VM production native
commands and diagnostic-host editor/mouse checks; its observer exit was 1
and normal shutdown was not proved. Those results cannot be attributed to the
new mouse candidate. Win95 has not been revalidated with the S8 driver and
owner acceptance remains outstanding.

For `a43c6d24`, local `S_2097_RdMode0UnchainedDwordRead` still advances its
second word by 8 where upstream changed it to 2. The upstream GDP subfield
half of that commit changes `compat/cvidc/gdp_state.c`, which has no same-named
local counterpart; local generated rules use direct GDP pointer offsets.
Therefore the arithmetic difference can be assessed independently, while the
subfield-alias fix requires a local layout/caller analysis rather than copying
the upstream adapter. No value-level local test was run for this comparison.
