# T481 S17 Win95 dynamic launcher and four-profile acceptance

Status: verified and published; S17 closure authorized after the exit/restart checks below.

## Tool implementation

One installed WIN.CMD selects DOSONLY and NTVDM64.386 independently, with
INSIGNIA.386 fixed enabled as owner-approved base support. The
accepted Win31 CMD configuration mechanism is reused. APPLY always copies
PIF.EXE, INSIGNIA.386 and NTVDM64.386 from assets; selection generates the PIF
and NT files and comments/uncomments only the two owned SYSTEM.INI entries.
Existing Win95 core preparation remains unchanged. Package runtime uses CMD
and existing addon tools, not PowerShell.

Task-local profile tests (build/t481-s17/job/profile.exit) and actual package
apply/select/unapply controls (job/package.exit) passed. These are not guest
interaction results.

## Current acceptance summary

Earlier `drivers0` records below sometimes mean both addons disabled. Those
historical controls are not the final profile: INSIGNIA is now always loaded,
and the selectable driver is NTVDM64 only.

| DOSONLY | NTVDM64 | Current evidence | Remaining boundary |
| --- | --- | --- | --- |
| 0 | On | editors10: two VM native/DOS editors, menus, read/modify/save/close and repeated window/fullscreen transitions; native and DOS scheduling progress witnesses | Normal Windows exit returns 1, unclassified |
| 0 | Off | editors29 native-first and editors33 DOS-first: both editors' menus/read/modify/save/close; native heartbeat blocks and recovers, DOS heartbeat progresses | Fullscreen shell-out/return verified; normal Windows exit returns 1, unclassified |
| 1 | On | editors12: two DOS editors with menus/read/modify/save/close and transitions; native-refusal1: pinned AMD64 image executes its DOS refusal stub | Normal Windows exit returns 1, unclassified |
| 1 | Off | editors13: two DOS editors with menus/read/modify/save/close and transitions; native-refusal0: same refusal contract | Normal Windows exit returns 1, unclassified |

All current rows use the package mouse driver in isolated installation copies.
These are private frontend/guest probes, not physical RDP Raw Input acceptance.
Full verification of candidate package 7b97a73d-08ce-4f0a-8a31-ed3519218703
passed its selected gates; DOS4GW/NESticle is skipped for an unavailable external fixture. Publication and closure remain pending.

## Runtime observations

All four startup controls reached Windows and reported the requested DOSONLY.
Their outer observer exit is 1 after ExitWindows; normal shutdown is not yet
proved. See runtime1/*/startup/matrix-results.json; the initial root matrix's
startup=false on an exception is superseded by the nested actual observations,
not by rewriting historical evidence.

Private NTCON input-sink probes supply mouse movement/buttons; original guest
and host input handling downstream is exercised. Physical Raw Input/RDP is not
proved. Windowed tests use the Win95 ttyGrab geometry. Fullscreen tests require
actual published text/graphics frames, menu contents, and return to graphics.
Guest USER input is used for ordinary typing except the explicit keyboard
control runs. All copies are owned and removed; original installation hashes
are checked.

| Case | Observed interaction | Limitation |
| --- | --- | --- |
| DOSONLY0, drivers1 | editors3: EDIT.EXE + EDIT.COM open/read/modify/save, mouse menus, two fullscreen/window rounds each, close | Old installed MOUSE.DRV inherited; repeat with actual package asset required; Windows exit1 unresolved |
| DOSONLY1, drivers1 | editors5: two guest EDIT.COM instances, same interaction and file checks | Native executable handling separately pending; same asset/exit limitations |
| DOSONLY1, drivers0 | editors4: blue screen before first mouse request; editors6 timeout; editors7: frontend keyboard Enter on first EDIT.COM followed by blue screen, fault06 at1767:00003685 | No extra host worker observed; not a proven host shell-out; old mouse asset discovered |
| DOSONLY0, drivers0 | Startup only | Blocking shell-out requires host-driven interaction; guest controller cannot drive while suspended |

## Material mismatch discovered before product changes

O:/WINDOWS/SYSTEM/MOUSE.DRV is the historical T479 S6 image:
F4A4C4F742161AAF7E83441C5440ACEA7C20B73BA6B4EEC0EFF1D5A8F214A36E.
The package APPLY installs assets/release/MOUSE31.DRV, the accepted S14 image:
5FA167B71FB12B69AEE78E9FF8AB46BB82D4A9FED00AAFDE94FAD87592CE9D07.
The old copy-based editor fixture inherited the former without checking it.
The S14 driver includes the no-instance callback-lifetime repair. Thus the
old no-addon failures are not valid acceptance failures of the new package.
They remain recorded controls; this difference alone is not proof of cause.

editors8 is the ongoing paired control with the package mouse driver staged
only in the disposable guest copy and all three hashes recorded. Original
O:/WINDOWS is preserved. No product mouse workaround is added on speculation.

## Outstanding acceptance

Complete corrected four-profile interactions and routing/scheduling checks;
verify DOSONLY1 native executable behavior; attribute shutdown exit1; review
all changes and run required cached build/gates/publication/commit/push.

### Corrected-material controls

editors8 (frontend keyboard) and editors9 (guest USER keyboard) used the
current asset hash and no addon pair with DOSONLY1. Both passed the former
first-editor blue-screen point and reached both fullscreen/window round trips.
They did not complete: original A.TXT/B.TXT seeds remained unchanged and the
observer timed out. editors9 retained six mouse-position receipts; the captured
live.bmp shows both DOS editors alive in windowed guest prompts, with text
entered into A but no completed menu save/exit. Thus the stale mouse asset
explains a material baseline discrepancy and its replacement removes the
observed early failure in these controls; it does NOT establish full no-driver
mouse/menu correctness. That remaining real path must be investigated before
four-profile acceptance. No product workaround or forced driver enable added.

The reusable copy helper now has an explicit MouseDriver input, used by the
new acceptance tests. It records source/tested/asset identities and modifies
only the owned copy. Other historical callers keep their explicit existing
material choices. runtime2 repeats startup/exit with the current mouse asset.

### Matched-material editor results and Win31 comparison

`editors10/editor-result.json` records Win95 DOSONLY0/drivers1 passing
EDIT.EXE plus EDIT.COM read/modify/save, mouse menus, fullscreen transitions
and editor closure. `editors12/editor-result.json` records the corresponding
Win95 DOSONLY1/drivers1 result for two guest EDIT.COM instances. The latter
follows an explicit target-window focus restoration after fullscreen return;
the preceding editors11 run did not save the second file and is not a pass.
Neither result proves physical Raw Input or normal Windows shutdown.

The same fixture now accepts Windows31 and selects the Win31 launcher and
window geometry, rather than assuming Win95 ttyGrab geometry. It also enables
the existing exact COM-port Ignore observer for its owned runtime. The initial
Win31 attempts did not reach editor input and are not mouse-failure evidence.
`w31-dos1-drivers1b/editor-result.json` records a passing two-EDIT.COM
read/modify/save/menu/fullscreen/close comparison, with observer exit zero.
The fixture still conservatively reports normalShutdownProved=false.

Win31 DOSONLY1/drivers0 comparison `w31-dos1-drivers0b` reached both editors
and fullscreen menu operations. Its read-only `live.bmp` captures windowed A
still open with `aaaexit` typed into the editor: the intended mouse save/exit
did not complete. Six MOUSE receipts have exact target/actual coordinates
(47,29), (47,77), (69,51), (69,99), (47,29), (47,137); both disk files still
contain their original seeds at that observation. This is the same observable
windowed-menu failure as the Win95 no-addon comparison, not a Win95-only
finding. Do not infer success from the older basic DOS-prompt matrix or claim
that DOSONLY1 bypasses the mouse path. The terminal job result remains to be
collected separately from this live observation.
The no-addon job ended with observer timeout; its final source-integrity check
also detected a changed source SYSTEM.INI (mtime 18:22:17 during the run).
The test therefore does not claim unchanged original installation bytes. Its
live input receipts, unchanged seed files and snapshot remain separate evidence
of the owned-copy interaction failure.

### Owner-approved contract adjustment

The owner approved fixed INSIGNIA.386 base support in enhanced mode, with the
option controlling only NTVDM64.386 multitask payloads. This applies to both
Win31 and Win95; Win31 Standard mode keeps both entries disabled. DOSONLY
remains independent. This explicitly supersedes the earlier two-driver-off
profile requirement; it is not a passing result for that historical profile.

Both systems' real INT2F AX1607/BX000C/CX1 receipts return unchanged AX1607,
DS:SI=0000:0000 without the pair, and AX0 with a valid callback with it.
The original VMD event entry rejects events with no registered callback.
The existing INSIGNIA implementation supplies that callback and original NIDDB
VM state selection; NTVDM64 owns multitask admission. No new guest patch,
duplicated mouse provider or host fallback has been introduced.

Updated Win31 six-profile and Win95 four-profile configuration fixtures pass
(`build/q17a`, `build/q17b`), including anchored active/commented INI assertions,
repeated selections, exit status and failure-without-launch. Earlier long-path
fixture roots exceeded PIF path capacity and failed; the existing short-root
convention fixes the test environment, not the product.
Real `editors13` (Win95) and `w31-base-multitask0` (Win31) both pass DOSONLY1
with base support enabled and multitask disabled: two guest EDIT.COM instances,
private frontend mouse menu interaction, alternating fullscreen/window,
distinct seeded-file read/modify/save and editor closure. Both job exits are
zero and both original-installation byte checks pass. Win31's observer exit is
zero; the fixture still does not assert normal Windows shutdown. Physical
Raw Input and DOSONLY1 native rejection remain separate unproved boundaries.

### Blocking-profile fixture work (not acceptance)

The host input helper now drives sequential EDIT.EXE and EDIT.COM while the
guest USER controller is suspended by cmdExec. The controller only selects
each prompt and resumes after host-driven editor return. Existing published
frame and mouse-menu checks are reused; no production protocol is added.
editors14 stopped on a missing fixture constant before runtime. editors15
exposed a probe key-up sent to a hidden window during Console handoff; the
helper now routes that event through its existing Console input facility.
editors16 returned from the first editor but saved only `aaseedA`, and failed
to obtain the second editor's text frame. It is a failure, not acceptance.
The next run requires actual seed contents before typing rather than merely
a text-mode frame. Scheduling independence and native rejection are pending.

Updated package round trip `build/s17c` passed all four selections and restore.
Its initial negative-test wrapper reused an old output directory and was
correctly refused (64). Fresh `build/q17-unapply`, `q17-failed_prepare`, and
`q17-command_safety` each pass the existing CMD negative tests.

S17 cached x86/x64 production builds completed with exit zero
(`build/t481-s17/job/cached.exit`). Publication and full integrated gates are
still pending. The blocking editor17 attempt did not reach its seed-bearing
text frame. The fixture now reuses guest USER command typing and sends only
the final Enter from the host before driving the blocking editor, removing
the separate command keyboard translation. editor18 is the pending control.

editor18 again failed on the second editor frame; editor19 spaces test key
pairs by 100 ms (test-only human typing cadence). It saved the first native
editor's exact `aaaseedA`, opened its File menu and returned to graphics.
The second editor still failed to produce the expected frame. Its owned
process receipt shows inner Run16 executing `o:\winnt\edit.com b.txt`, one
NTVDM and the now-resident NTVWM. This is not enough to prove a deadlock:
original cmdExec32 deliberately calls GetNextVDMCommand to allow DOS reentry
in the same worker. The missing second NTVDM is not itself an error. Further
inspection must distinguish original reentry progress from frontend handoff.

DOS-first controls editors20/21 fail identically, so the missing DOS frame
does not require a prior native editor. editor21's x86 worker snapshot shows
the main thread inside `ccpu -> vir_read_dword -> phy_r32 -> phyR`, rather
than a host wait. This is one sample, not proof of guest forward progress.
The cached build's .text SHA256 exactly matches the exercised S16 NTVDM
(.text 6c66afd6c7d03a45e97b135ad4ce26a6467db57c11a7e7bf1f6cd278ba5eab57),
supporting the mapped code addresses despite differing whole-image hashes.
The existing x86 thread snapshot helper was built in the task directory; an
initial x64 invocation was rejected at compile time for its x86 CONTEXT
fields. No product code was altered. editor22 adds the existing read-only
CCPU register/IRQ snapshot to the failure capture; result pending.

editor22 failed at the same frame boundary. The non-atomic CCPU samples show
the same IP; the raw code signature also exists at COMMAND.COM file offset
0x3747 and was not found in EDIT.COM or QBASIC.EXE. Paging and unsynchronized
CCPU globals limit interpretation: this does not prove a guest infinite loop
or identify its cause. Raw guest stack bytes were removed from the local
diagnostic receipt and are filtered from later snapshots; they are unnecessary
to this execution-path check. The editor runner already filters inherited
environment variables before launch, so the historical large-environment
defect must not be assumed. A matching Win31 blocking control is pending.

Win31 blocking31/blocking31b also fail the frame assertion, but are not the
same demonstrated host-reentry case: blocking31b has no inner Run16 and its
named original host fields are fBlock=0, IsRepeatCall=0, IsFirstCall=0,
Exe32ActiveCount=0, event_thread_blocked_reason=-1. The guest sample resembles
BIOS keyboard polling. The host probe's accepted Enter alone therefore does
not prove the command was submitted to the intended DOS prompt. Do not use
this control to claim a shared product defect. blocking31c additionally saves
the actual graphics surface on failure to identify focus/prompt state.

blocking31d corrects that earlier interpretation: its captured surface shows
EDIT.COM with B.TXT and seedB inside the guest DOS window, and the owned
process receipt includes inner Run16 executing EDIT.COM. Waiting exclusively
for a native text frame was therefore a fixture error, not evidence of failed
DOS launch. Original cmdExec32 explicitly supports DOS reentry through
GetNextVDMCommand; routing through Run16 does not imply a new DOS worker or
native Console presentation. Independent scheduling proof remains required.

blocking31e (owned runner PID 26440) adds the ordinary Alt+Enter transition
for this Win31 windowed DOS case. It reaches the seed-bearing text frame,
opens File through frontend mouse input, and inserts bbbseedB. The test then
fails at return because its native-editor pixel coordinates opened DOS EDIT's
Save As dialog. This is not an acceptance pass. The next fixture uses DOS
menu accelerators after the mouse File assertion and explicitly returns the
full-screen DOS prompt to its window. blocking31f (PID 10564) is pending;
no product source or original guest binary changed in this investigation.

blocking31f completed DOS File mouse opening, exact bbbseedB save, DOS editor
exit and return to the guest window. Its following native editor failed at
the fixture's pixel-based Save/Exit: the preceding DOS application left an
80x28 text mode, and the native Save As dialog was visibly open. A.TXT remained
seedA. The original installation byte check passed. This is another fixture
geometry failure, not a pass or a claimed product defect. The helper now uses
the shortcuts advertised by that actual native File menu (Ctrl+S/Ctrl+Q)
after its mouse-opening assertion; no fixed text-row assumption remains in
its save/exit sequence. blocking31g (PID 33864) is the pending full rerun.

blocking31g completed with runner exit 0 and observer exit 0: both editors
read their seed files, saved exact aaaseedA/bbbseedB, opened File through the
private frontend mouse sink, and closed. Original installation hashes remain
unchanged. Its result explicitly leaves normal Windows shutdown and physical
Raw Input unproved. Sequential completion is not independent scheduling proof.

The owner subsequently clarified the enhanced DOSONLY0/NTVDM64-off contract:
only native CUI shell-out blocks guest Windows. DOS programs reenter the same
worker and can run in windowed guest VMs with guest multitasking. The Win31
captured DOS window and inner Run16 agree with this distinction; no change to
production scheduling follows. The remaining scheduling acceptance must use
an independent guest progress witness for both child types rather than apply
one blocking expectation to the whole configuration. INSIGNIA stays loaded.

`verify-win31-setup-modes.ps1 -DosReentry` now reuses the existing DOS child
release handshake and independent Win16 heartbeat, selecting enhanced
DOSONLY0 with NTVDM64 on/off. `build/t481-s17/dos-scheduling31` (PID 36728,
runner exit 0) passes both. Multitask off: heartbeat bytes 170 -> 836 during
the live child interval, then 879 after release; on: 171 -> 739, then 741.
Both children return S14_OK, both mode checks and normal Windows exits pass,
and owned copies are cleaned. This proves nonblocking guest progress for
these DOS-child cases; the earlier editor receipt separately identifies the
inner Run16 route. It does not prove the Win95 equivalent.

Win95 editors24 (PID 22484) retries the outstanding DOS-first case with the
existing console_live_snapshot helper available on failure. Its read-only
invocation supplies no mutation option. This captures actual Console output
when the KVM window is absent; it is diagnostic evidence, not an alternate
success assertion or a production presentation change.

### Scheduling-route audit correction

The stronger `dos-scheduling31b` run (PID 2772, exit 1) rejects both rows:
the guest heartbeat progresses and shutdown succeeds, but no owned inner
Run16 command containing SCHED.COM is observed during the controlled child
interval. Therefore the previous DOS scheduling pass cannot prove cmdExec
reentry. It remains evidence only of a live DOS child plus Win16 progress.

SCHED.COM is an authored 16-bit COM fixture assembled from
`test/integration/multitask/win31-scheduling-child.asm`, not a Win95 component.
It writes a DOS receipt, waits for the host release file using INT 21h/28h,
then exits through INT 21h/4C00h. The wrapper copies DOSPRMPT.PIF and changes
only its arguments to `/c <absolute SCHED.COM path>` for this case. Read-only
inspection of the source PIF's original STDPIF startfile field (offset 0x24,
as declared in non-mvdm/public/internal/windows/inc/pif.h) reports COMMAND.COM.
The native control is different: it explicitly supplies
`/c <host cmd.exe> /d /c <native script>`.

Original COMMAND init.asm SetSSwitch sets SingleCom. tcode.asm ISNOBAT tests
SingleCom and copies that command directly to DOCOM1, which uses PRESCAN and
the ordinary parser/executor. In contrast, Do16BitPrompt's interactive
d16_run path checks DOSONLY and invokes SVC_CMDEXEC with AH=1 (cmd /c).
Thus a COMMAND /c COM fixture is not an equivalent test of typing the same
command at that interactive prompt. This is source/configuration evidence,
not an instruction trace or a claim to have identified the loaded COMMAND
image's runtime path. The replacement test must retain interactive submission
and observe inner Run16 plus independent progress in the same run.

editors24 failed with an absent KVM window and an empty 120x30 Console
snapshot; its inner Run16 command is EDIT.COM. This is a distinct unresolved
Win95 interactive case, not explained by the flawed /c scheduling fixture.

The authored startup controller now has an interactive scheduling variant:
launch an empty-argument COMMAND PIF, activate its named window, type
SCHED.COM through the same Win16 USER keybd_event ABI used by the editor
fixture, then run the independent heartbeat until host release/completion.
After completion it types EXIT. The PIF explicitly selects windowed/background
execution. The existing runtime runner requires the owned Run16 SCHED.COM
command during the live child interval as well as progress and normal exit.
No original guest component was modified or rebuilt.
dos-scheduling31c (PID 12172) failed before runtime at NASM argument handling;
the optional argument is now retained as an array. dos-scheduling31d
(PID 29300) is pending. Neither attempt is a scheduling acceptance result.

dos-scheduling31d subsequently passed (runner exit 0) both interactive rows.
With NTVDM64 off, the live-child interval records inner Run16 PID 27856,
parent 9792, command `SCHED.com`, and independent heartbeat 138 -> 873,
then 905 after release. With NTVDM64 on it records inner Run16 PID 5392,
parent 25492, the same command, and heartbeat 404 -> 2265, then 2269.
Both fixtures assert DOS result S14_OK, live child until explicit release,
correct enhanced/DOSONLY0 state, post-child progress and normal Windows exit.
This supersedes the insufficient /c fixture for the interactive Win31 route;
it does not establish Win95 behavior or physical RDP input.

The Win95 launcher matrix now reuses that same interactive controller, DOS
child, Run16 observation and heartbeat assertion. `dos-scheduling95` (PID
12052) failed both complete runs. The on-profile was observed with SCHED.RUN,
SCHED.END and heartbeat data, but the wrapper omitted MouseProbeRoot while
the reused controller waits for a later mouse interaction. Neither row is
accepted. `dos-scheduling95b` (PID 33112) supplies the existing private mouse
probe and retains heartbeat/route receipts even on failure. Its result is
pending; no production repair is inferred from the first fixture failure.

Cached NTVDM binary identity review: .text, .data and .reloc are byte-identical
to the S16 candidate. The sole .rdata difference is the IMAGE_DEBUG_DIRECTORY
timestamp at file offset 0x1A8A0C. Parsing both resource trees finds 14 versus
13 leaves: all 13 common resources are byte-identical, and only the staged
NTVDM_PACKAGE_ID resource is absent from the raw linker output. Resource-tree
layout bytes consequently differ; they are not changed guest/runtime code.
The whole-file hashes remain different and must not be called identical.

dos-scheduling95b completed with overall exit 1. Its independent scheduling
assertions pass in both rows: NTVDM64 off records Run16 PID 8836 executing
SCHED.com, heartbeat 10 -> 27 during the controlled child interval and 34
after release; on records Run16 PID 20196, heartbeat 117 -> 621 and 623 after.
The controller reaches its desktop/mode completion in both rows. Both root
observers exit 1, so normal-exit and the full matrix remain failed. These
receipts prove interactive DOS reentry can make guest progress on Win95;
they do not solve EDIT.COM's absent display or the final exit status.

editors25 completed with runner exit 0. In the Win95 DOSONLY=0 / NTVDM64-off
profile, both EDIT.COM and EDIT.EXE opened their mouse menus, read the seed
files, saved exact bbbseedB / aaaseedA contents and closed. The fixture now
switches the windowed DOS editor to fullscreen on both Windows families;
previously that operation was restricted to Win31 although both start from
windowed prompts. This repairs test synchronization, not production code.
The result still reports observerExit=0x00000001 and normalShutdownProved=false.
It is therefore editor-interaction evidence, not full profile acceptance.
editors26 reverses editors25's order (native first) and fails in DOS fullscreen
transition. Native menu/save/return receipts exist. DOS Enter-down was accepted
by Window thread 9340, Enter-up and Alt-down went to Console, and the next
Enter-down targeted Window thread 16208 but got sink-accepted=0. The test hook
increments that counter only when GWLP_USERDATA, component and input sink are
available and the sink returns true; zero does not distinguish a retired HWND
from an explicit sink rejection. Thus this is a demonstrated transition/input
probe failure, not evidence sufficient to change production routing or retry.
Runner exit is 1; original installation verification passed. Preserve this as
a failed order control, not a passing full-matrix row.

Exit provenance source follow-up: original COMMAND command1.asm Exec_Wait
calls DOS WAITPROCESS and stores AX in RetCode. tcode.asm cont_scs1 reads that
value and do_again clears AH before supplying the host command return record.
Together with the earlier cmdGetNextCmd termination trace this locates the
reported low-byte 1 upstream of host exit policy. It does not identify which
Win95 exit operation produced that byte or prove normal shutdown.
The single-event private input probe had a separate queued acknowledgment
message addressed to the same HWND. An accepted transition can retire that
window before the acknowledgment is handled. The test helper now signals a
single-event acceptance event directly after the production sink returns true;
it does not replay input or change production logic. Legacy burst-count users
retain their existing count barrier. This removes one known ambiguity but is
not yet proof that it caused editors26's failure. frontend-probe4 builds cleanly;
editors27 (PID 34652) reruns the same native-first ordering and is pending.

The installed O:/winnt/edit.exe PE machine is AMD64 (0x8664). Consequently a
DOSONLY=1 native capability rejection witness should name this exact image,
not infer that all PE programs are unexecutable in Win95. A guest-capable PE32
program would not be an equivalent negative case.
editors27 now acknowledges all transition keys but fails the second DOS
editor text-frame assertion. The final published frame is graphics 640x392,
Console snapshot blank, and inner Run16 still carries EDIT.COM. The new
single-event acknowledgment does not by itself fix the editor order failure.

The first DOSONLY=1 AMD64 rejection witness incorrectly expected EXEC error
11. Read-only inspection of the pinned EDIT.EXE shows a valid MZ entry at
CS:IP 0:0, header 4 paragraphs and bytes 0E1FBA0E00B409CD21B8014CCD21:
print the DOS refusal message, then INT21/4C01. Correct expectation is normal
DOS child termination with code 1, not necessarily failed EXEC. The revised
test verifies these exact stub instructions before launch and checks DOS
WAITPROCESS returns 0001. It does not alter the image. The original attempt
is not acceptance evidence; the corrected attempt remains pending.
native-refusal (PID 5672) completed, runner exit 0. Both DOSONLY=1 profiles
(NTVDM64 off/on; INSIGNIA fixed loaded) execute the pinned EDIT.EXE DOS stub,
observe WAITPROCESS AX=0001 and publish the PASS receipt. The AMD64 image SHA
is 5F54A0FA8CBF5423AEF8BE8502173A59183CF3ADBC913212B1066EF8FFF3D9C0.
Neither profile has an owned native CUI worker while the guest probe waits.
Each isolated copy is retired by the fixture and removed; this is capability
refusal/guest-execution evidence, not normal Windows shutdown evidence.
The earlier native-reject attempt ended at its incorrect error-11 expectation
and is not counted. No original guest image was modified or recompiled.
editors28 fails again after native completion, with no LibKvmWindow at the
final read; the optional surface snapshot therefore cannot capture a window.
The editor fixture still treated DOS reentry like a blocking native launch:
it removed both Enter keys and immediately toggled display after host Enter.
The fixture now retains DOS Enter in the guest sequence, uses the existing
guest progress/activation steps, then hands fullscreen interaction to the
frontend probe. Only native Enter remains host-submitted. Reverse ordering
now swaps complete action blocks rather than renaming their roles, preserving
those different scheduling contracts. editors29 (PID 29348) is pending.
These are test changes; no production repair or complete matrix is claimed.
editors29 completed with runner exit 0. Native-first then DOS now passes both
mouse File menus, exact read/modify/save contents and editor closure under
DOSONLY=0 / NTVDM64 off. The distinction between blocking native submission
and nonblocking guest DOS submission resolves this fixture's order failure
without product changes. The source installation remains unchanged. Root
observer exit remains 00000001, so normal Windows shutdown is not passed.
The reverse order is rechecked with these same final fixture mechanics.
Read-only exit-chain review of installed WIN.COM (22679 bytes, SHA256
56423f6c7328b3b587e400c4ab57e8fac1c6a943563d96e04f9886a9d6d0cfcd):
16-bit disassembly at COM offset 017D saves AX; 01E8 restores AX, 01E9 sets
AH=4C and 01EB executes INT21. Its EXEC helper at 034C issues 4B00 and then
4D to obtain the child's return status. These are static decoded sites, not
runtime breakpoint hits. Together with the recorded COMMAND/host return
chain they identify where to observe the remaining exit value; they do not
prove which WIN.COM branch ran, nor classify exit 1 as successful shutdown.
No original guest bytes were changed.
editors30 ends in startup failure, not an editor assertion: the existing
observer's timeout-live record captures the 16-bit subsystem dialog
"Error while setting up environment for the application." No editor request
was reached. It is retained as a failed startup attempt; it cannot be used to
invalidate or pass the corrected editor sequence. A repeat will use the
existing read-only thread/CCPU snapshot tools before its deadline if still
live, to distinguish command-environment retrieval from creation failures.
editors31 did not repeat the startup environment dialog. Its DOS-first
failure snapshot shows Prompt probe B at an empty O:/TMP/Q96/PATCH prompt,
not EDIT.COM. The process receipt has no inner Run16. Therefore this failure
is command delivery/readiness, not proof of lost editor presentation. The
next control selects the existing FrontendKeyboard option for the entire
command rather than guest USER synthetic keystrokes; no product changes.
editors32 uses the existing frontend keyboard route for every command key.
DOS-first now starts EDIT.COM, passes its published text/menu frames, saves
and exits back to graphics. The run times out at the original 90-second
observer deadline during later command input (key 92), so it is not a full
pass. The per-key route now selects the existing 180-second long-interaction
budget; this changes only the test deadline. A vanished frontend after observer
exit is reported as observer termination, not misclassified as missing NTCON.
editors33 completes with runner exit 0 using frontend keyboard for the entire
DOS-first/native-second sequence and the long interaction budget. Both editor
menus, distinct seed edits/save contents and closure pass; original installation
hashes remain unchanged. observerExit is still 00000001, so normal shutdown
remains unproved. The preceding editors32 deadline failure is not counted as
pass. Native-first previously passed editors29; neither run is a physical RDP
Raw Input claim.

The Win95 scheduling runner now has an explicit NativeScheduling control,
reusing the existing independent guest heartbeat/release fixture and host CMD
version receipt. Unlike the DOS interactive test, this PIF explicitly requests
host cmd.exe /d /c and requires a matching inner Run16. NTVDM64 off expects
blocked heartbeat; on expects progress. Native ownership receipts survive a
later shutdown failure. Runtime results remain pending.
Native scheduling assertions pass in both profiles. Multitask off: independent
heartbeat remains 121 bytes throughout the 3.26-second live native child,
then grows to 123 after release. Multitask on: 241 -> 751 during the child,
753 afterward. Each run records inner Run16 with explicit host CMD /d /c,
and both start/completion receipts match host Windows 10.0.26300.9818.
Both full runtime rows still fail normal-exit, so the wrapper exits 1.

The current cached builds staged an 11-image candidate successfully:
package 7b97a73d-08ce-4f0a-8a31-ed3519218703, candidate-final. This is not
publication. Full verification will use that candidate and exactly stamped
matching cache inputs. Documentation governance passed after indexing S17.

## Approved exit-code boundary and final delivery checks

The owner explicitly approved recording exit code 1 as an unresolved boundary,
with automatic exit, natural resource retirement and restart required before
closure. This supersedes the earlier requirement to attribute 1 first. The
actual return value remains visible; neither host nor original guest is changed
to return zero. No originating guest component is declared responsible.

The final candidate Full run completed with runner exit 0 and matching 11-image
package 7b97a73d-08ce-4f0a-8a31-ed3519218703. DOS4GW/NESticle was explicitly
skipped because its external fixture was unavailable, not reported as passed.
The separate four-profile retirement/restart run uses this same candidate.
Its natural-retirement records are collected before any failure-cleanup helper;
the observer must not have forced job cleanup enabled. Known exit 1 is accepted
only under an explicit Win95-only test option. All other failed startup,
interaction, timeout and cleanup assertions remain failures.
The first retirement/restart attempt passed DOSONLY=0 / multitask off twice,
then stopped before preparing its next row because the owner's live Win95 held
O:/WINDOWS/WIN386.SWP. No live user session was terminated. After the owner
closed it, retirement-final reruns all four rows against the final package.
The early attempt is partial evidence, not a complete four-row pass. An empty
survivor collection was initially serialized as [null]; the final fixture emits
an actual empty array. This is reporting-only, not a process cleanup change.

Final source review: no production host, addon, original guest or mirror source
is changed in this delivery. Mirror diff delta is zero. Tool changes are limited
to dynamic configuration and fixed INSIGNIA support. Probe changes add explicit
keyboard/frame witnesses and immediate per-input acceptance acknowledgment on
the existing private frontend test route; the old burst-count acknowledgment
remains for its callers. Such acceptance does not itself prove guest handling:
menu frames, saved file contents, scheduling heartbeats and child completion
supply the independent outcome checks. Test observation deadlines and polling
remain test-only; no production retry/timeout or alternate execution path was
added. The Win31 README's stale deferred-mouse statement is removed because
those repairs were already accepted in S16.

## Final verification and closure

The owner-approved closure condition passed on retirement-final: all four
DOSONLY (0/1) by NTVDM64 (off/on) rows started, exited automatically, retired
naturally, restarted from the same installation copy, and exited/retired again.
All eight roots returned 1; each natural-retirement record has
beforeScopeCleanup=true and survivors=[]. No forced process cleanup supplied
these passes. Individual retiring service exit values were not captured
(the JSON value is null); only their signaled process termination and absence
are established. The runner's interaction=pending/passed=false fields describe
that startup-only fixture, not the independently completed editor witnesses.

| DOSONLY | NTVDM64 | Editor/route witness | Automatic exit, cleanup, restart |
| --- | --- | --- | --- |
| 0 | Off | editors29 + editors33, native-first and DOS-first; native blocks, DOS reentry progresses | Passed, actual exit 1 retained |
| 0 | On | editors10; two host/DOS editors and transitions; concurrent progress | Passed, actual exit 1 retained |
| 1 | Off | editors13 two guest DOS editors; native-refusal0 | Passed, actual exit 1 retained |
| 1 | On | editors12 two guest DOS editors; native-refusal1 | Passed, actual exit 1 retained |

Every editor witness checks mouse menu actions, distinct read/modify/save
contents and editor closure. The three nonblocking editor cases repeatedly
alternate window/fullscreen. The native-blocking case verifies fullscreen
shell-out/graphics return and DOS window/fullscreen return in both execution
orders. DOSONLY=1 does not claim unsupported native EDIT.EXE runs: its immutable
DOS stub refusal is the expected result. These are private-desktop/frontend
and guest USER probes; physical RDP Raw Input remains outside this automation.
Earlier failed fixture attempts remain documented above, not counted as passes.

Cached x86/x64 builds passed; final Full passed 16/16 selected gates. The
external DOS4GW/NESticle fixture was unavailable and explicitly skipped.
Configuration generation, apply/select/unapply, failure rollback and command
safety controls passed. Win31 scheduling and fixed-INSIGNIA controls are retained
above. No original guest or production host source was changed in S17.

Publication-final verified all 11 manifest entries at both assets/release and
O:/winnt/system32, package 7b97a73d-08ce-4f0a-8a31-ed3519218703. The Win31 and
Win95 installed WIN.CMD files match their repository tools; PIF.EXE and both
addons come from and match assets/release. Existing SYSTEM.INI choices were
preserved at delivery; the next WIN.CMD invocation applies the user's choices.
No new backup/configuration debris was put in PATCH. Original winnt.zip SHA256
remains 92714715FC0FCE4CB33268F534E9E7D7D45AE612937249A2C2B2E2E013099947.
Task-owned O:/tmp/q95 copies were removed after each final test.

Exit code 1 is retained in TODO as an owner-approved unclassified boundary,
not a repaired guest defect. The source-owner attribution remains unproved.
S17 is closed under that explicit approval; T481 remains open for separately
admitted remaining work. The owner confirmed the two concurrently added
DOS identity probe files belong to another session; they are preserved and
excluded from this P. No claim is made that the shared worktree is entirely
clean while that research continues.

## Admission record retained at closure

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T481 S17, Ordinary Mode. |
| Admission And Approval | Owner accepted S16 and requested one Win95 WIN.CMD matching Win31, with DOSONLY and two-driver choices; four-matrix acceptance explicitly required. Owner subsequently approved fixed INSIGNIA.386 base support in enhanced mode, with only NTVDM64.386 selectable; apply the same rule to Win31. |
| Objective | Install both addons and PIF from assets; one installed WIN.CMD generates the selected profile and toggles only its two SYSTEM.INI device entries. |
| Non-goals | No new original-guest patches or general launcher framework; investigate and repair demonstrated four-profile host/addon defects if required. Preserve existing approved Win95 preparation. |
| Reference Baseline | S16 726a0341a; accepted Win31 dynamic launcher and coherent 11-image package. |
| Files And ABI Surface | tools/win95-launch, tools/win31-setup and corresponding tests/documentation; no planned product ABI change. |
| Applicable Rules | CMD and existing addon tools at package runtime; assets-only payloads, LF, source-policy, no Computer Use. |
| Verification | Four DOSONLY x multitask profiles; script configuration/failure/cleanup checks and real startup, mouse movement/clicks, alternating guest DOS window/fullscreen, two-VM EDIT.EXE/EDIT.COM interaction/save/exit under each applicable execution contract, routing/scheduling, return and normal exit; cached builds and applicable Full gates. |
| Expected Markers | Selected INI/NT/PIF settings, driver hashes, actual host versus guest execution, blocking versus concurrent progress, editor interaction/save and normal exit. |
| Asset Needs | Existing assets/release and isolated copies of authorized installed Win95; no new media. |
| Reporting Requirements | Separate script checks from guest execution; report each matrix row and uncovered boundaries. |
| Stop Conditions | No unproved guest semantic change or success inferred only from generated configuration. |
| Exit Criteria | Reviewed implementation, applicable cached build/test, coherent publication and tools delivery, commit/push, then owner acceptance. |
| Original Owner Request | One Win95 WIN.CMD selecting DOSONLY and whether to load both drivers, implemented as closely as possible to Win31; four-matrix acceptance. |
| Similar-Issue Sweep | Repeated selection, unrelated INI preservation, failed configuration without launch, stale generated files, legacy unapply and source-only asset delivery. |

S17 scratch: build/t481-s17; owner S17, 8-hour/20-GiB budget and 30-minute
no-progress review. Remove owned O:/tmp guest copies after testing. Preserve
owner sessions. No S17 closure or matrix success is claimed at admission.

S17 progress and explicit pending boundaries: [four-profile evidence](m0-t481-s17-win95-four-profiles.md).

Owner approved retaining historical Win95 exit code 1 as an unresolved boundary.
S17 closure requires verified automatic exit, natural process retirement and
restart in all four profiles; preserve the actual return code, not a fabricated
zero. Full gates passed on the final candidate; retirement/restart remains in
progress. This approval does not declare the source of exit code 1 resolved.

Owner scheduling clarification: enhanced DOSONLY=0 with NTVDM64.386 disabled
blocks guest Windows only for native CUI shell-out. DOS commands reenter the
existing worker and retain guest windowed VM scheduling. Verify those child
types separately; INSIGNIA.386 remains fixed base support in both cases.
