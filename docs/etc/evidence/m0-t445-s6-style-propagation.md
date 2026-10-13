# T445 S6 appearance policy propagation

Owner admits S6 after closing S5 without manual acceptance, then shortens the
options to --win16-style and --win16-style-all. No long-name aliases. Baseline
af8189e86 and the published S5 images remain the deployed product. Retain S1's
task-owned design with NTW16S as DLL owner; no automatic retro drawing yet.

Admission governance PID4748 (2026-10-13 08:55:20 local, deadline45s) passes.
Earlier Current size-check failures made no runtime changes. Preserve unrelated
WOW proposals and QUEUE. CLI parses style options in either order with --wait,
rejects duplicate/conflicting style options and preserves the exact target
tail, including command/c. Explicitness is separate from Off/Win16/All, so an
omitted option can inherit. r001 PID25244 at08:57:02, deadline60s, exit0:
x86/x64 each pass25 CLI cases.

Native wiring adds a validated scalar to native launch packets and the Hook
payload. Hook version3 has an explicit72-byte header; RPC46/product0.0.445
identify the changed wire contract. Run16 resolves explicit/native-inherited
policy; NTVWM copies it into each target's Hook context. Hook's existing child
context copy preserves policy when GUI launch clears Console handles. There
is no added global environment variable.

r002 PID5680 at08:59:31, deadline60s, exit0: both widths pass25 CLI cases plus
native packet round trips for Off/Win16/All, absent Console capabilities and
invalid policy values. Reproduce with tests/observation/verify-style-options.ps1
and a fresh build-owned BuildRoot. This is not real child-chain acceptance.

## Service and WOW task binding work

Check's copied envelope carries the request policy (VDM envelope version2).
The existing request connection stores it; WowStyle authenticates the worker,
requires an original dispatched WOW record, and matches its task plus original
parent receipt to that request. It is queried before InitTask publishes startup.
This is new owner-approved appearance policy, not a replacement for original
task selection, guest scheduling or BaseCheckVDM image classification.

r003 uses the x86 original Ninja graph with explicit bundled Node24.19.0.
Initial PID45460 at09:06:23 failed missing the explicit Node path. PID39464
at09:07:13 compiled, passed reservation, then exposed a protocol negative test
hardcoding version2 as invalid. It now derives invalid/current versions from
the protocol declaration. PID26676 at09:08:37, deadline600s, exit0: reservation
and protocol fixtures pass. None of these is the full service fixture.

r004 PID43164 at09:13:34, deadline600s, compiled the service fixture but failed
its restricted-token pipe creation before WOW tests. Retried outside that
restriction as PID23112 at09:15:00, deadline600s, exit0. Added assertions cover
All lookup, wrong worker/generation, pre-dispatch, wrong original receipt,
retired task, and a fresh Off request on the same WOW worker. Reproduce using
the original graph's basesrv-service-reservation-test.exe; its final lifecycle
PASS and the explicit WOW style PASS are required, not exit alone.

WOW task binding now copies policy into its existing lifecycle thread carrier.
Direct requests override parent inheritance, including Off. Local children
read original TDB_Parent through a bounded read-only protected guest lease,
validate TDB_sig, and select only the live original parent task under the
existing runtime lock. No additional task registry or guest mutation is used.
The retained src/mvdm/inc/tdb16.h declaration supplies offsets; it is included
unchanged, not copied into a new layout. Native pointers never escape the lease.
Production child/lifetime acceptance remains pending, including bootstrap and
separate-WOW classification; unit layout validation is not proof of those routes.

r007 PID6276 at09:19:23, deadline90s, exit0: x86/x64 pass25 CLI cases, native
policy packet tests, and wow_task_parent_test.c. The parent fixture checks root,
parent, selector bounds, bad signature, self-parent and acquire/release failures
against a test lease; actual guest mapping remains a separate runtime gate.

Background worker build r006 first exposed stale RPC45 interface identifiers;
they are updated with RPC46, including the identity-version test. The next
attempt exposed the new exported query's cdecl/stdcall mismatch; its declaration
and definition now use WINAPI, matching the WOW parent import contract.

Build chronology: r005 PID1092 at09:18:08, deadline120s, exit1 because the
parent import library had not been built. r006 PID39452 at09:18:39,
deadline1800s, exit1 on the stale RPC identifier; PID45780 at09:20:29,
same deadline, exit1 on the export decoration. PID29672 at09:21:32, same
deadline, exit0: complete ntvdm.exe link and VdmTib ownership audit pass,
followed by compilation of the three changed WOW binding translation units.

r008 PID13808 at09:23:09, deadline1200s, links the full WOW32.DLL but exits1
on the lifecycle fixture's obsolete link list, missing worker-base/common
production libraries. The retained runner now links those exact current
dependencies, with no replacement provider. PID15024 at09:24:37, same
deadline, exit0: task-lifecycle and native-send both report errors=0 and
WOW_FIXTURE_OK. New checks cover invalid policy, missing parent, direct task
policy and the child task inheriting its live parent during real host-thread
message callouts. The final native-send build also includes the subsequently
added post-unbind query rejection assertion; the earlier task-lifecycle build
predates only that assertion. Existing /FORCE:multiple fixture bindings and
page-domain stub remain explicit fixture limitations, not real guest evidence.
No src/mvdm or src/opennt-host diff was introduced.

## DOS delivery and launcher relay

Get's project envelope now carries validated policy plus a presence bit
(120-byte prefix, inside the already unreleased version2 envelope). NTSRV
reads the last BUSY record after original srvvdm.c has committed its stack
transition. The request is matched by original parent receipt and Console;
the first new-Console record with no event uses its authenticated reservation.
There is no observation-label dependency, secondary task stack or guest-field
reuse. A non-delivery does not overwrite policy; a successful resume with no
active task explicitly clears it. The client checks the original request's
WOW/PIF/environment flags because original Get overwrites returned state.

r009 PID13376 at09:29:34, deadline600s, exit1: the initial guard incorrectly
ran DOS metadata lookup on the WOW Get result. The original output had cleared
the query flag. Restricting the service edge to the authenticated DOS worker
and using the client's saved request flags fixes that error. PID38796 at09:30:38,
same deadline, exit0: complete service lifecycle passes with the added Win16
policy delivery and PIF absence assertions.

The existing COMMAND native-child adapter installs the existing Hook payload
into its suspended child before resume. The installer's pinned-image check
chooses context-only mode for Run16; actual native targets get their width's
Hook. It copies the exact DOS policy and existing Console capabilities without
adding a style environment variable or altering user arguments. Native caller
suspension is preserved; failed installation terminates the uncommitted child.
NTVDM links the existing installer/context libraries rather than a new injector.
r010 PID37696 at09:32:11, deadline1800s, exit0: worker and VdmTib link audit pass;
affected WOW binding objects compile with the unchanged parent import contract.

r011 PID13012 at09:33:51, deadline600s, exit0: full service fixture plus
--reenter-nested-return, --reenter-after-return and --reenter-before-increment.
All use the original service dispatch. Tests verify that a reused DOS record
receives Off instead of the prior request's Win16 policy and that completed
reentry clears the policy when no busy task remains. This does not yet prove
real guest nested On/Off parent restoration; that product scenario is pending.

r012 initially failed a fixture header path (PID3908 at09:35:36) and then a
missing original RTL link dependency (PID2136 at09:36:31). Both exit1, deadline120s.
PID19872 at09:37:18, same deadline, exit0: verify-dos-style-relay.ps1 links the
production COMMAND adapter and Hook installer/decoder. Six actual child
processes verify Off/Win16/All with and without caller-requested suspension;
each sees launcher-only copied context and no injected Hook module. A seventh
invalid-policy child is rejected and its handles cleared. The incoming DOS
policy and absent Console capabilities are fixture-supplied, so this is real
child transport evidence, not a claim that a guest COMMAND chain was exercised.

## WOW native child edge

Original WK32LoadModule32 in src/mvdm/wow32/wkman.c retains command building,
show-state rules, native creation flags, wait order and error mapping. Its one
CreateProcess edge is bound at compilation to wow_native_create_process_a by
a header forced into that translation unit only. No mirror edit is needed.
The project wrapper reads the already-bound current WOW lifecycle task's
policy, creates the native child suspended, calls the parent worker's existing
Hook installer, then preserves the caller's requested suspension/resume state.
The worker-local NtvdmInstallChildStyle seam grants no frontend capabilities
and stores no process-wide WOW policy. Failed installation retires the
unreturned child; failure without a bound task is explicit, not inferred Off.

r013 PID19792 at09:41:39, deadline1800s, exit0: worker/parent import rebuild and
VdmTib audit. r014 PID17628 at09:43:46, deadline1200s, exit0: full WOW32 link,
task-lifecycle and native-send fixtures both errors=0. Each now creates a real
native child from the bound WOW task through the production creation wrapper
and worker installer. A build-owned same-image run16.exe child verifies the
copied Win16 policy and launcher-only/no-Hook mode, then returns101, asserted
by the parent. These are real native children, but their WOW parent binding is
the host fixture, not yet a real Win16 application launching a native program.
The existing original printf/puts selection does not emit the new puts marker;
the final stderr error count and explicit child exit assertions cover it.

r015 PID46808 at09:45:42, deadline60s, exit0: dumpbin of production wkman.obj
shows its unresolved call to _wow_native_create_process_a@40, with no remaining
CreateProcess reference. This confirms build wiring only, not guest execution.

Native production child-chain tests, real DOS relay/restore acceptance, real
WOW child/lifetime tests and title-only DLL API remain incomplete.
No worker-global fallback is allowed.
Full package/deployed regression, release and commit/push remain pending;
all thirteen existing published images are unchanged.

## Title-only DLL boundary, r016/r017

The new Ntw16sTitleInstall/Remove API is presentation-only: it changes a
caption, not geometry, drawing or process policy. HWND ownership remains on
the calling GUI thread. Per-window subclass state retains accepted source and
display captions, prevents duplicate library installation/round-trip prefixes,
and releases itself on destruction. The module owner must retain the DLL
while any such subclass is live. No Hook or WOW consumer is connected yet.

Reproducible entry: tests/observation/verify-title-marker.ps1 -BuildRoot
build/M0-T445/S6/r017. It builds the real DLL and test in both widths, then
runs tests/app/title_marker_test.c on a private desktop with a 15-second
per-fixture bound. Build/test runs are hidden owned processes with a 120-second
outer diagnostic deadline. No user desktop input or global settings change.

- r016 PID38412 at09:53:42: both builds succeeded, but the restricted token
  rejected desktop creation (error5). The initial runner incorrectly treated
  an unavailable process exit code as zero. This is a failed run, not a pass;
  the runner now pins the process handle, rejects missing/nonzero exit status
  and rejects nonempty failure output.
- r017 PID26368 at09:54:22, elevated, exit1: a real assertion exposed the
  distinction between SetWindowText success and the window's WM_SETTEXT
  return. Installation now sends WM_SETTEXT explicitly and checks its result.
- r017 PID6044 at09:55:00, exit0: both widths pass A/W updates, exact caption
  round trips, caller-authored prefix text, empty captions, foreign-thread
  rejection, reentrant setters, removal/destruction during a setter and
  normal cleanup. No live prior process remained before cache reuse.
- r017 PID26096 at09:55:35, exit0: adds rejected installation and destruction
  during installation; both widths still pass all assertions.

These are actual DLL/HWND tests, not an end-to-end Run16 acceptance result.
The title consumer integration, complete real descendant route matrix and
all-package publication/commit/push remain pending. No S6 binaries have been
published and no S6 acceptance is requested.

## Native creation integration, r018 onwards

window_titles.cpp adds the ten planned native A/W window/dialog entry wrappers
to the existing Hook transaction, only for WINDOW_STYLE_ALL. Win16-only policy
still propagates but leaves native captions unchanged. The exact same-width
NTW16S DLL is lazily loaded beside the copied package Hook path, not CWD/PATH.
Its reference lasts through process exit because installed HWND callbacks may
outlive creation wrappers. DllMain only establishes API interception.

For the title-only stage, the finite thread hook observes successful WM_CREATE
and WM_INITDIALOG returns, rather than installing an early CBT subclass: title
publication needs completed creation data, not early geometry. This is the
smaller S6 binding; the S1 CBT proposal remains relevant to actual S7 rendering.
Nested calls retain scoped state and idempotent HWND installation. Failure
rolls back a returned modeless window; modal initialization failure ends its
dialog before exposing a successful result. No process/thread enumeration,
renderer installation, persistent GUI-thread hook or guest/mirror edit.

Reproducible native fixture is tests/observation/verify-native-titles.ps1,
with WorkerBuildRoot=r003, Hook64BuildRoot=hook64, StyleBuildRoot=r017 under
build/M0-T445/S6. It compiles the authored native_title_hook_test.cpp/.rc,
injects the real production Hook into child processes, and asserts copied
policy and actual captions on private desktops. No Run16/service/guest
end-to-end acceptance is implied by this installer-level entry.

- r018 PID20688 at10:00:32 exit1: Windows PowerShell module auto-discovery
  did not find Get-FileHash. The owned runner explicitly imports Utility.
- r018 PID26848 at10:01:04 exit1: Hook32 built; Hook64 graph incorrectly
  required a fixture-only launcher for DLL-only generation. That eager check
  is removed; Ninja retains the stage dependency for fixtures that need it.
- r019 PID18164 at10:02:28 exit0: actual Hook32/Hook64 build/link completed.
- r020 PID29020 at10:03:41 exit1: x86 passes Off/Win16/All windows and
  indirect dialogs; x64 fixture lacked the original selected RTL support.
- r020 PID15844 at10:04:43 exit0 after linking that support: both widths pass
  three policies, A/W windows, modeless/modal indirect dialogs, dynamic title
  update and later GUI threads. These cases exercise actual injected DLLs.
- r021 PID26176 at10:06:14 and PID24532 at10:08:48 exit1: expanded named-dialog
  and cross-width suite exceeded its GUI-thread 10-second budget in the Off
  baseline. PID6348 at10:07:20 also failed its child-process budget. Stage
  timing shows forward progress (about16 seconds per isolated dialog group),
  not proof of a product deadlock. A message-aware fixture wait does not remove
  that delay. The rerun uses explicit diagnostic bounds30/45/90 seconds for
  GUI thread/grandchild/direct child; production waits are unchanged.
- r017 PID31980 at10:11:33 exit0: title DLL now also rejects an initial setter
  that returns success without accepting decorated text, and removal checks
  WM_SETTEXT's actual result. Both widths pass the added negative case.

Expanded r021 PID6456 at10:10:26 completed exit0 within its660-second outer
deadline. Both x86-to-x64 and x64-to-x86 actual Hook-created children pass
Off/Win16/All payload and caption assertions, all ten window/dialog entrypoints,
main/later GUI threads, dynamic captions and normal exit. All twelve tested
child/grandchild processes complete their assertions; no native-title-test
process remains. Child waits remain bounded; the isolated-desktop timing is
a diagnostic limitation, not a product performance pass or failure attribution.

This run uses its copied pre-hardening style DLLs, not the later r017 rebuild.
Exact SHA-256 identities (both fixture directories contain the same DLL set):

| DLL | SHA-256 |
| --- | --- |
| nthook32.dll | 3DE8B72C7C9938E5CFC4D500C3861167C2735087F74DFF0D15930FAE758EB3F1 |
| nthook64.dll | 2769DCE34233BE0742C6CB5221DDF3DC104A0A0ABD654BCFA5F87B04FB364C14 |
| ntw16s32.dll | 19DE92DB4FC7673B5737BD0F88C34437961114180DB510C2174E4B78CC569E78 |
| ntw16s64.dll | 9B2E2763BA0250A6115E4412510E79EDC1546AC8B4DA822E4E66EA0D3A035C54 |

The final combined candidate still needs a fresh integrated pass. WOW window
consumers and real guest ancestry/override cases remain open, as do full-package
delivery gates. Source review/diff-check finds no mirror modification.

r023 documentation-governance PID46300 at10:15:49, deadline120s, exit0;
git diff --check passes. These structural gates are not S6 delivery/closure.

## WOW title bindings and first complete candidate build

WOW registration wrappers now read the bound task's immutable policy before
ordinary/modeless creation, preserve the original creation/provider chain,
and decorate only the returned caption HWND. The existing modal facade uses
a finite nested same-thread creation scope and marks its exact dialog after
WM_INITDIALOG, before its modal loop. The library reference is process-wide;
the enable decision is never worker-wide. Off does not load the style DLL.
The worker image's directory supplies ntw16s32.dll, with no CWD/PATH fallback.
No original guest or mirror source changed.

Separate WOW's InitTask shared ID is deliberately zero in original wkman.c.
The new worker-local NtvdmSeparateWowStyle checks original nt_reset.c's
fSeparateWow classification, then requires policy from an actual separate-WOW
GetNext delivery. Parentless shared bootstrap remains Off; a separate worker
without a delivered record fails explicitly. Local guest children retain the
original TDB parent lookup/inheritance. This closes the source-level zero-ID
gap, but successful real separate-WOW launch acceptance is still pending.

- r024 PID41176 at10:21:09, deadline900s, exit1: WOW32 link succeeds and the
  lifecycle fixture reports errors=0, but PowerShell treats its stderr summary
  as a terminating NativeCommandError. The fixture runner now captures both
  streams with an owned process, pins/validates exit status and has a60-second
  fixture deadline instead of interpreting stderr text as exit status.
- r024 PID18496 at10:22:28 exit0: task-lifecycle, native-send and new
  style-policy fixture all errors=0. The new case uses the production policy,
  DLL loader and modal binding with actual private-desktop HWNDs. It checks
  bound Win16 policy, A/W modal titles, dynamic titles and explicit Off.
- r025 PID22724 at10:25:07 exit1: same-runtime Off peer and parent policy
  assertions pass, but later scheduling assertions fail because the fixture
  used a host thread wait without restoring original WOW cooperative ownership.
- r025 PID23556 at10:26:49, deadline240s, exit0: fixture re-enters through
  production lifecycle_yield after peer retirement, as the existing cross-task
  fixture does. Both policy isolation and all subsequent lifecycle assertions
  pass, errors=0. No product scheduler or test state fields are modified.
- r026 PID33136 at10:30:02, deadline1200s, exit0: rebuilt NTVDM/import surface
  and complete WOW32, then all three fixtures pass. Style case also verifies
  non-separate classification and missing-delivery refusal; it does not fake
  successful separate-WOW metadata. VdmTib ownership audit passes.
- r027 PID44180 at10:31:04 exit1: runner passed an absolute path to the native
  generator's relative BuildRoot contract. PID10392 at10:31:24, deadline900s,
  exit0 after correcting the runner: AMD64 Launcher, Service, Worker, Frontend
  and Monitor all compile/link. This is build evidence, not runtime acceptance.

Stage-System32ProductPackage.ps1 now optionally takes the three style artifacts
as an all-or-none set, validates build-owned inputs and correct x86/x64 machine
types, and includes them in the coherent package manifest. Existing ten-file
callers retain their prior input contract. Input read errors name their source.

## Security-product interruption during staging

r028 PID40536 at10:34:20, deadline240s, exit1. Baseline media is expanded only
from assets/winnt.zip; only missing WINMINE/SOL/WRITE are copied individually
from the owner-approved installed media. No whole O:/winnt copy occurs.
The build-owned partial r028-runtime receives run16/ntsrv, then Windows rejects
reading native-frontend/ntcon.exe with a virus/PUA error. That source file is
subsequently absent. This partial tree is not a testable/accepted package.

Read-only Defender inspection confirms detection at2026-10-13 10:34:24-07:00:
ThreatID2147848358, Trojan:Win64/Disdroth.EM!MTB, ActionSuccess=true,
DidThreatExecute=false, IsActive=false; exact resource is
O:/repos.hobby/ntvdm64/build/M0-T445/S6/native-frontend/ntcon.exe.
No exception/exclusion, restoration, retry intended to evade detection or
protection change is performed. Owner security review is needed before this
artifact can be used. The detection is not assumed to be a false positive.

Published assets/system32 are untouched; no S6 commit/push or acceptance.
Other remaining gaps include real Run16/DOS/WOW route tests, successful
separate-WOW delivery, full13-image gates, final review and delivery. Fixture
tests alone do not prove those routes. The goal remains active and incomplete.

## DOS detached-launch policy snapshot

Source audit finds the initial S6 read timing incorrect: original cmdexec.c
cmdExec32 starts cmdCreateProcess on a detached thread, then immediately calls
GetNextVDMCommand. command_process_compat.c must not read the mutable current
DOS policy from that detached thread. This is a source-proven race opportunity,
not a claim that the owner observed policy leakage in a running guest.

The existing project-owned mvdm_command_native_child capture now includes one
DWORD policy, copied on the guest thread with command/environment/handles.
The process adapter requires that active capture and consumes its value before
original cmdCreateProcess finishes the capture and resumes the child. Finish
and abort clear it; absent capture returns ERROR_INVALID_STATE before process
creation. No guest, mirrored source, environment, scheduler or new task table
changes. This extends the existing copied-input seam, not original execution
policy. The original source does not own this new product appearance value.

- r029 PID39424 at10:43:06, deadline900s, exit1: x86/x64 CLI, packet, parent
  and new snapshot tests pass; subsequent graph generation fails because the
  runner omitted the explicit Utility module needed for Get-FileHash. No
  worker build pass is claimed from stale r006 output.
- r030 PID29368 at10:44:04, deadline900s, exit0: explicit runner module import,
  complete NTVDM link and VdmTib audit pass, provider bindings compile, real
  child payload relay passes all three policies, suspension, invalid policy
  and missing-capture rejection. The source snapshot test deliberately changes
  the delivered task policy before a real detached reader runs; it also covers
  both guest/host command capture, finish, abort and failed guest lease.

Snapshot tests mock session/guest-memory boundaries; relay tests supply the
incoming snapshot value. These are complementary focused evidence, not real
COMMAND nesting acceptance. The blocked NTCON is not restored or rebuilt;
publication and full route verification remain pending.

## Actual Run16 native GUI entry and nested override

The native-title fixture now has explicit launcher and relay modes. Neither
controller installs a test Hook payload: they execute the actual Run16 CLI
on a private desktop. Original child assertions decode the real installed
context and check actual HWND captions, dynamic setters and child exit codes.
GUI-subsystem fixture variants prevent accidental text-frontend admission.
The CompileOnly verifier mode compiles these without calling the older direct
installer tests. It is build-only and does not write a test pass marker.

r031 uses a deliberately incomplete, build-owned GUI-only runtime containing
Run16/NTSRV/NTVWM, both Hooks and both style DLLs, all copied from the S6
builds. Exact input hashes are retained in r031-inputs.json. It does not use,
restore, replace or rebuild NTCON; it is not a coherent full-package delivery.
An existing broker is refused before the test; cleanup targets only the owned
runtime. No global environment or owner desktop changes.

- r031 PID41500 at10:47:54, deadline960s, exit0: six actual CLI cases pass,
  Off/Win16/All times x86-to-x64 and x64-to-x86 native GUI descendants. Each
  child covers ten A/W window/dialog creation APIs, dynamic captions and a
  later GUI thread. All six launcher exit codes are zero. A read-only process
  sample confirms Run1645592 -> NTSRV5812 -> NTVWM22436 -> x86GUI27324 ->
  x64GUI34084 for the first Off case, with exact r031 image/command paths.
- r032 PID35396 at10:52:08, deadline180s, exit0: both-width relay fixtures
  compile. This is only build evidence.
- r033 PID18548 at10:53:42, deadline540s, exit0: eight nested actual Run16
  cases pass. Each initial policy1/2 and cross-width direction tests omitted
  (inherited) policy and the opposite explicit policy. Child and grandchild
  check real context plus a created/updated caption. The parent creates another
  window after the nested launcher exits to prove that child override did not
  mutate its own policy. All eight launcher exits are zero. The r031 runtime
  is reused only after checking every copied image against its retained hash.
  Cleanup also includes this run's exact fixture image paths.

These results strengthen native entry/propagation evidence; they do not claim
CUI, real DOS nesting, real shared/separate WOW task isolation, or complete
product/publication acceptance. The Defender interruption remains unresolved.

## Real WOW bootstrap investigation

The read-only frontier observer accepts an optional Off/Win16/All CLI policy.
Physical staging accepts an explicitly enumerated runtime-image subset for
cleanup identity; its default five-image behavior is unchanged. Focused WOW
stages contain the selected S6 images and original assets/winnt.zip guest media,
not NTCON. This is not full-package acceptance or a security-product bypass.

- r034 PID16560 at10:57:31, deadline240s, exit1 before guest observation: the
  isolated runner removed its own process execution-policy environment while
  trimming the test environment. The runner now retains that existing launch
  setting; no system execution policy or Defender setting changes.
- r035 PID28844 at10:58:19, deadline240s, observer exit0, **functional failure**:
  all three policies return launcher exit0 too early and no app windows are
  observed. Observer completion is not counted as application acceptance.
- r036 PID34560 at11:00:09, deadline90s, observer exit0, same failure: existing
  opt-in registration trace reaches UserRegisterWowHandlers and RegisterHung,
  then seven failed InitTask calls and cleanup.
- r037 PID26444 at11:01:15, deadline600s, exit0: rebuilt WOW and all three
  lifecycle/native-send/style-policy fixtures pass, but real guest still fails.
  Added opt-in scalar diagnostics show task039F, shared IDFFFFFFFF, parent023F,
  InitTask error0490 (ERROR_NOT_FOUND). Diagnostics retain the initialization
  error across trace writes. This narrows the failure to the new parent lookup.

Root cause: original kernel31/task.asm inherits curTDB into TDB_Parent;
ldboot.asm explicitly creates and later deletes a bogus boot task. The initial
WOW shell therefore has a nonzero guest parent that was never a USER lifecycle
task. The initial S6 implementation incorrectly demanded a live policy owner
for that parent. Original wkman.c::W32Thread already identifies this shell by
assigning gptdShell and CURRENTPTD before InitTask. The project binding now uses
that exact original TD identity (opaque pointers, no copied TD layout) to avoid
inheriting from the bogus boot task. Ordinary local children still require their
real parent; direct broker requests still override with their own copied policy.
Separate WOW's initial shell still requests its delivered separate policy.
No filename test, empty-list heuristic, guest or mirror source modification.

r038 PID20128 at11:06:12, deadline600s, exit0: rebuilt complete WOW32 and
task-lifecycle/native-send/style-policy fixtures all pass. Real immutable
WINMINE now reaches a visible Minesweeper window under all three policies.
Each policy has four read-only samples (4/8/12/16 seconds): Off has exactly
`Minesweeper`; Win16 and All have exactly `(Win16 Style) Minesweeper`. The
shared WOWExec shell remains unmarked in all three runs. Twelve application
caption samples are checked explicitly, not inferred from observer exit0.
Original task initialization now succeeds. The observer intentionally ends
each open application at its bound; this is title/startup observation, not
normal-exit or gameplay acceptance. The marked short physical stage is removed
after owned-process cleanup. Mixed simultaneously active application policies,
guest-created descendants and complete product delivery still remain open.

Further mixed-task and modal coverage is retained in the
[real-chain follow-up](m0-t445-s6-real-chains.md).
