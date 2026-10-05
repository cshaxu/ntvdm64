# T432 S6 single-worker reconstruction

## Request and inputs

Owner rejects superseded dual-worker implementation in main. New target:
one x86 ntvwm.exe, nthook32/64.dll,16/32/64 execution and handoff, NTSRV
actual64 task identity and NTMON WIN64 display. Accepted baseline is
T431 S2 P2,12160c65725d986a59519f479458fbe35cb5a1f1.

## Procedure and results

An alternate Git index below build/M0-T432/S6/r001 captured all tracked and
untracked candidate files using read-tree/add/write-tree/commit-tree/update-ref.
Evidence branch evidence/t432-dual-worker-superseded-20261005 points to
3d5bc9466cf7208112ea53b9ff3ebf97534b0354. Snapshot/worktree comparison passed.
Main and normal index were not changed. Other-session proposal/TODO preserved.
Applied baseline reverse diffs to49 tracked src/tests/tools files; removed
four archived untracked candidate source/test/build files. All are recoverable
from the evidence branch. No destructive Git history reset was used.
`git diff --exit-code 12160c657 -- src tests tools` passed with empty output.
No build, process termination or publication occurred at this step.

## Interpretation and remaining gates

Baseline source identity is proved, new Hook64/WIN64 behavior is not. Archived
S2–S5 test successes cannot prove reconstruction; their native-zero failure
remains a non-pass. Next individually design/reimplement required shared
Hook/native metadata without width-selected workers, build under S6, run
matching/cross-width actual child, DOS/WOW, fault/isolation/product gates,
publish verified ten-image manifest and review/commit/push. T stays open.

## Other-session documentation audit and S6 P1

Owner requests joint audit/submission of the other-session proposal and TODO.
Proposal's2026-10-04 queue promotion is historical; T428 closure owns its
later admitted disposition. Correct the present-tense queue-head and old
active-packet claims, not implement the old inventory again.
TODO's20ms output capture and event-driven input description matches delivered
T429 S7/S8 and current ntvwm main's20ms-active/INFINITE-idle wait. The reported
mysmb16 performance issue remains unmeasured debt, not an attributed defect.
Removed Bochs obligations concern a retired provider; shell-out research has
delivered broker/Hook successors. Removed old ConPTY wheel debt concerns the
replaced backend; its strict historical failure remains indexed, not a claim
that horizontal-wheel capability now passes. No source behavior is changed.

Normalized src/tests/tools diff against accepted baseline and HEAD is empty.
Some transient Git status entries reflect stat/line-ending refresh, not source
changes. Commit must contain documentation only. Governance, relative links
and diff checks are required. S6 P1 records reconstruction/admission and joint
review; it does not close S6, prove Hook64, or replace the published package.

## Shared native metadata reimplementation

From restored baseline, extract its project-owned SEC_IMAGE metadata query
to common/native_image. Run16 and Hook select one implementation, not another
filename resolver/PE parser. Actual-process queries use PROCESS_NAME_NATIVE
and GLOBALROOT to avoid x86 System32 reopening into SysWOW64. The owned ABI
header uses SIZE_T stack sizes for native-width consumers; mirrors unchanged.
No target-machine worker selection/reservation or additional worker build.
Existing classification fixture adds optional actual suspended32/64 CMD checks
with exact-owned cleanup, retaining all legacy assertions.

S6/r001/generate.log retains the first generator rejection: default Node is
not22. generate2.log explicitly uses local Logi node22 v22.21.1; the generated
formal graph contains one ntvwm.exe, no32/64 worker product targets. Reused
cache M0-T427/S2/r001 compiles the affected closure with native MSVC x86 /MT;
metadata-build.log passes Run16, Hook32 and classification fixture links.
metadata-build2/3.log record diagnostic and final fixture-only rebuilds.

native-metadata-test.log and test2.log retain failed first-CreateProcess handle
measurement (6/7 startup handles, stable on second iteration), not product
leak proof or passing runs. Final test3.log preserves the original file-query
handle gate and separately measures each production actual-process query over
32 repetitions while its real suspended32/64 child is held:402 assertions,
zero failures, file/query handle delta0; creation bootstrap99->106->106.
Arguments are cache nthook-gui-test.exe, C:/Windows/SysWOW64/cmd.exe, current
cache nthook32.dll, C:/Windows/Sysnative/cmd.exe. Fixtures never resume these
CMDs; terminate/join only the exact newly created test children. The first
creation-only increment is disclosed, not silently normalized as a full
process-creation no-leak result. No published files or user processes changed.

This proves the x86 shared metadata slice only. Hook64 compilation, actual
cross-width installation, Run16 direct64 admission, actual WIN64 service
projection and complete product/fault/reuse/publication gates remain open.

## Fresh matching-width Hook64 slice

Rebuild the accepted shared Hook source family for AMD64 only; no64 worker
target or width-selected worker registration is restored. Context V2 keeps
the64-byte header and carries pinned Hook32/64 paths, with recipient-width
validation. Shared actual-process metadata chooses the matching DLL. The
isolated original classifier translation unit retains its legacy NE branch
through a local build facade, without editing its mirrored source body.

S6/r001/hook64-build.log retains the missing original status-mapping link;
hook64-build2.log passes after including the baseline original error.c binding.
hook64-fixture-build.log retains a missing GUI CRT entrypoint; build2 passes
using the same explicit wmainCRTStartup entry as the existing x86 fixture.
hook64-fixture.log passes93 assertions: actual matching-width CMD and immediate
children, A/W calls, GUI/CUI transitions, handles/environment/CWD/streams,
malformed context and installation failure. Accepted run16.exe is copied only
as pinned identity input into S6/r002-hook64; this folder is not a product
package and no published artifact changed.

Cross-width interception still deliberately fails closed in this intermediate
slice. No claim is made for cross-width propagation, fresh x86 Hook V2
regression, broker WIN64 projection, coherent ten-image publication or S6
functional closure. The evidence branch is not a source/build dependency.

## Fresh cross-width transaction and production integration

Freshly review/admit only DetourProcessViaHelperDllsW in the indexed helper
register before editing the restored pinned source. The normalized source diff
is confined to that routine; current SHA256 is
0A9802622AE4A58E2A2E19E4C326A5A0C04FE31DBB347176B38C5B748DEB5472.
Ordinal1 uses the official helper completion; helper payload identification
bypasses target interception. No worker64 target, private loader or resident
installer is selected. Creator retains the actual child and suspension.

Actual matching/opposite-width CUI and GUI fixtures first pass115 assertions
per width in hook32-cross-test.log/hook64-cross-test.log. Extended tests add
controlled callback create failure, held helper timeout and exact target death.
hook32-failure-test.log/hook64-failure-test.log pass141 assertions each:
ERROR_ACCESS_DENIED, ERROR_TIMEOUT at10015/10032ms and ERROR_PROCESS_ABORTED;
owned helpers are joined before return, other failures leave the target's
suspend count unchanged, and the creator aborts/joins only its own target.
This is installer/Windows-child evidence, not DOS/worker product proof.

Remove the old nthook_target32 execution gate/helper; the sole x86 NTVWM now
uses the shared installer for actual32 and64 targets. Run16's unchanged legacy
classification has a positive native SEC_IMAGE fallback, not a new search or
shell parser. NTSRV records verified actual process machine at existing direct
binding; management selects WIN64 from that record, not carrier width. No
worker-width reservation, Observed task or second registry is added. Candidate
APP0.0.428/RPC40/I/O25 avoids reusing the rejected candidate's RPC39 contract;
MIDL40 and all server/client interface symbol references are synchronized.

Build failures retained: missing RTL/NTDLL/TEB fixture link closure, a system
library mistakenly treated as a Ninja file dependency, and old v38 interface
symbol references after MIDL40 regeneration. hook32-context-test.log was
mistakenly run after a failed build, using a mixed old fixture; it is invalid
candidate runtime evidence, not a product pass. Subsequent commands enforce
build success before runtime. hook32-context-test3.log passes97 assertions
including exact unpublished-target rollback. single-worker-production-build3/4
pass affected formal x86 links. native-lifetime.txt passes1077 checks,48
completions,16 cancellations and zero remaining handles. service-reservation-
test.log passes the retained original DOS/WOW/native binding/completion and
management lifecycle fixture. These trusted fixtures do not prove actual
cross-process RPC40 or WIN64 projection yet.

Still required: actual alternating native descendants/DOS return, production
WIN64 text/GUI snapshots, protocol mismatch, single-worker reuse/isolation,
full ten-image build/package gates, Console17/Window17, independent WOW
frontiers, coherent publication and production P. O:/winnt is unchanged.

### Formal runtime and actual-machine probes

The accepted graph lacked worker-identity-version-test.exe; the first full
runtime request therefore failed before compilation (full-runtime-build.log).
Add the retained fixture to the formal graph and replace its stale v36 symbol
with v40. generate9/full-runtime-build2 pass the real RPC fixture, NTVDM and
VDMREDIR links; r004-wow32 composes all77 original WOW32 bodies and links
against this actual NTVDM import library. rpc40-identity.log passes current
connection plus previous APP0.0.427, incorrect application protocol and the
accepted RPC38 launcher rejection. Only its created service is terminated.

Stage-System32ProductPackage now accepts optional Hook64 only alongside
Hook32, requires AMD64 for that DLL and I386 for the other nine images, and
records each machine/hash. r003-runtime is a ten-image integration package,
not publication. Existing baseline media/configuration is retained. The new
r005 observer builds the current source. chains32.log passes all ten retained
native CMD-to-legacy routes, including interactive MEM and parent output order,
under the previously measured short-history private Console profile. This
does not establish the default private-geometry frontier.

Do not count chains64.log/r007 as AMD64 evidence: read-only production
snapshots revealed System32 reopening by the x86 launcher into SysWOW64.
The first machine-projection attempt r008 also had invalid trailing-backslash
command quoting and never launched its target. r009 retained the actual
WIN32 snapshot, exposing the width-selection error rather than hiding it.
Correct the fixture to use the public Sysnative alias through the x86 launch
boundary, after verifying the real System32 image's machine.

r010-projection64/projection64-3.log now proves an actual System32 CMD direct
task through the authenticated production NTSRV snapshot: one x86 NTVWM row
has kind3, stack1 and C:/Windows/System32/cmd.exe; the direct request completes
with exit0 and ordered MACHINE-BEGIN/MACHINE-END output. NTMON's production
label maps kind3 to WIN64. This is actual service DTO/runtime proof, not a
visual NTMON observation or proof of every nested cross-width case. All these
fixtures remove subst Z: and clean only their owned package scope.

The corrected r011-chains64/chains64-sysnative.log passes all ten retained
legacy routes with Sysnative selecting actual AMD64 CMD. Assertions are the
same as I386: real guest markers, interaction, direct completion and ordered
parent output. No retries, deleted cases or relaxed results are introduced.
Still outstanding are alternating native32/64 descendants, GUI WIN64
projection, full Console17/Window17 and independent WOW frontiers,
single-worker reuse/isolation, package preflight/recovery/publication and
production commit. S6 and T432 remain open.

### Alternating native children and detached GUI

The shared installer fixture now executes ordinary Windows
32-to64-to32 and64-to32-to64 descendants. Every layer verifies its matching
loaded Hook/context; original returned handles, suspended-child count,
real Windows wait/exit and existing negative assertions remain unchanged.
alternate32-test.log/alternate64-test.log each pass147 assertions, including
real10016ms helper timeout and exact owned-helper cleanup. This is distinct
from broker-mediated DOS handoff and does not create another worker/receipt.

The real machine-projection fixture also covers a held AMD64 GUI image.
r012 initially stopped observation when run16 returned, before detached
registration; its first snapshot caught the temporary worker-owned record and
is not a GUI projection pass. r013 continues read-only observation after the
startup receipt and passes: category4/kind3 actual GUI target, category2/kind2
single x86 carrier back at EMPTY, exit0 startup-only receipt, and authenticated
exact management close. gui64-2.log/snapshot JSON retain these states.

The misquoted r008 observer wrote MACHINE-BEGIN and its geometry report at
the repository root. Their contents/paths were verified as this failed probe's
not-started result and moved to r008 as misquoted-observer.txt and
misquoted-observer.geometry.txt. Nothing was discarded; no test artifact
remains a repository-root deliverable.

The first full Product request was correctly rejected for cache/runtime
run16 hash mismatch: later dependency rebuilding relinked run16 after the
r003 snapshot. Reconcile all x86 production links in pre-product-build.log
and stage fresh r015-runtime; r016-product compares that exact ten-file set.
The first requested secondary WOW baseline path was also corrected to the
actual T431 S2/r011-full, alongside accepted r030-product. These failed
prerequisites are retained, not counted as product passes. Product tests remain
serial because of the global endpoint and subst Z:.

Audit detects APP0.0.428 still inherited from the earlier baseline sequence,
where version.h and Verify-ProductVersions require the admitted T identity.
Before release it must become0.0.432 with affected rebuild and renewed
runtime/version proof; do not weaken that assertion or represent428-package
results as tests of the future432-package.

r016-product completes successfully on the reconciled ten-image428 candidate:
WOW67345ms, Console17 62271ms, Window17 66906ms, total201755ms. All existing
assertions pass; WINMINE's visible-main-window frontier and SOL/WRITE's
independent retained error frontiers match both actual T431 baselines. This
is not full SOL/WRITE functionality or gameplay acceptance.

Correct application identity to0.0.432 (RPC40/I/O25 unchanged).
version432-build.log rebuilds all affected x86 targets and real RPC clients;
WOW's graph verifies no additional work against the unchanged import-library
identity. rpc432-identity.log passes previous APP/protocol/RPC38 negatives.
Stage fresh r017-runtime and admit renewed serial full Product proof at
r018-product/product432.log; its result must be read before claiming a pass.
The earlier428 tests are retained evidence, not silently reused as432 runtime
proof. Production scan finds no ntvwm32/ntvwm64, worker-width policy or original
mirror-body diff. O:/winnt remains the accepted T431 package.

### Final application-identity runtime proof

r018-product on r017-runtime (APP0.0.432/RPC40/I/O25) completes with all
ten runtime hashes unchanged: independent WOW frontiers69277ms, Console17
85980ms, Window17 82699ms, total244091ms. These are retained frontier
comparisons, not newly claimed SOL/WRITE functionality or WINMINE gameplay.
r020-chains64 repeats all ten native/legacy routes through actual AMD64 CMD;
r021-machine64 proves authenticated kind3/direct completion/output order.
The first renewed GUI command named a nonexistent test file and never ran;
the corrected nthook-gui-test.exe input passes r022-gui64, including detached
kind3 and startup-only receipt plus exact management close.

r023-handoff explicitly selects verified AMD64 CMD via Sysnative. Both
nested-console and nested-window pass real DOS/MEM output, parent-return
marker, actual exit23 and (Window only) CAF confirmation. Existing assertions
remain intact; all ten package images are now hash-pinned by this fixture.
native-lifetime432-final.txt passes1077 checks, failures0 and remaining
handles0. r024-service-fixtures passes all29 existing service scenarios,
including reentry, parent resume, cancellation and management; elapsed23950ms.

Retained control probes still assumed pre-System32 flat packaging. Their
launcher/path bindings now use the existing Get-PackageBinaryRoot while
preserving CWD and all assertions; version-negative peers use the equivalent
System32 binary binding. BuildConsoleRegression requires both project/library
include roots and C11 for existing shared-header static assertions. Three
failed compiler attempts remain in terminal-observer-build1/2/3 logs; the
corrected build4 succeeds. These test-tool changes do not change production.

Final graph review fixes the displaced base-owner flags for the thin run16
image-classification wrapper. Its affected production closure is rebuilding;
do not publish r017 until rebuilt identities and renewed required gates agree.
An abandoned October4 ntvdm-only Ninja wait had no compiler/linker child;
its exact PID3208/command/parent/date were checked before ending it. Its parent
CMD exited automatically, so the subsequent parent-stop attempt correctly
found no process. No runtime session or unrelated process was terminated.
Control verification, build-input closure, coherent recovery/publication and
production commit remain open. S6 and T432 are not closed.

The first final Ninja invocation used VDMREDIR.DLL instead of its case-sensitive
target VDMREDIR.dll and was rejected before building. Corrected build2 passes
the affected x86 closure, including VdmTib owner verification. WOW's dependency
check reports no work. Fresh r025-runtime/r026-full starts serial full gates.
Hook64 input audit detects stale generation-time source hashes (installer.cpp)
after later source implementation; that old manifest is not a current build
identity proof. Preserve it and regenerate the Hook-only graph, check Ninja
dependencies/rebuild and compare tested artifact hashes before publication.
The initial dependency snapshot incorrectly accepted only absolute paths;
Ninja's actual relative header paths must resolve against its build directory.

hook64-final-build.log reports no work after manifest regeneration; final
Ninja dependency resolution pins43 source/header/definition/graph/compiler
inputs in hook64-build-inputs.json, with generation-time source hashes checked
against every current body. Hook64's DLL hash equals the staged/tested
r025-runtime manifest, so no post-staging Hook64 artifact change occurred.
The incomplete absolute-only dependency snapshot is retained separately as
hook64-build-inputs-initial-relative-path-miss.json. This records the actual
project dependency closure and MSVC14.43 compiler identity; the supported
SDK22621/toolchain profile remains the formal build prerequisite.

Final-package r026-full passes independent WOW frontiers, Console17 and
Window17, but its control group fails before bootstrap because the old
flat-cache fixture paths do not resolve the System32 product layout. This
is not a full-suite pass. The control harness now copies and hash-checks its
four test clients beside the isolated candidate binaries and uses the actual
package root; these clients are not publication images.

r028-control passes all seven RPC cases, five GUI cases and five version
negatives. Its monitor fixture's fresh-parent zero-byte resume assertion is
corrected from NOT_READY to INVALID_STATE, matching the unchanged accepted
service_prepare_parent_resume contract; null reply assertions remain. Strict
DIR then fails with entered-dos=0: the observer sends old Z:/run16 instead of
Z:/system32/run16.exe and reads the outer CMD directory. The observer path is
corrected without removing its DOS-entry/output/clean-prompt assertions;
renewed execution is still required.

Fresh Hook32 and Hook64 installer logs each report147 passing assertions.
metadata-final-test2.log reports402 checks, zero failures and zero steady-state
handle growth. The first metadata renewal omitted required arguments and is
not evidence of a pass. An additional WIN64 renderer assertion builds, but
direct launch cannot establish its80x25 private-buffer geometry on this host;
monitor-win64-layout.log is a fixture failure, not a renderer pass. The new
r030-reentry32 probe observes both actual machine phases on the same worker
and a real exit23; its final Console snapshot contains CHILD-DONE but not
PARENT-DONE, so its output-order gate fails and remains under investigation.
No publication or production commit is authorized by these partial results.

The reentry failure is now source-verified: end_io unconditionally ended and
disposed the shared worker presentation when any admitted request completed,
even with a live outer request on the same actual Console. The fresh fix keeps
that endpoint while another admitted request remains, without changing the
broker's explicit cross-worker release event or process-retirement authority.
Affected x86 NTVWM relinks; fresh coherent r031-runtime replaces r025 only as
the current candidate, not the published package. r032-reentry32 and
r033-reentry64 both pass: one worker PID, actual child/parent kinds3/2 and2/3,
ordered CHILD-DONE/PARENT-DONE output and real outer exit23. Each direction
has separate authenticated snapshots and owned cleanup; Z: is removed.

monitor-win64-private.txt runs the actual renderer fixture in an owned private
Console, exits0 and prints its retained passing marker. Its new exact WIN64
character/color assertions pass alongside unchanged WIN32, tree, footer,
selection and action assertions. The inherited-terminal geometry failure
remains recorded; it is not a production renderer failure or waived assertion.
r034-full is now running against r031. Read its terminal result before release.

Owner raises a further distinction: worker process retirement must remain
NTSRV-directed; a local completion is not permission to exit a carrier. The
existing common worker_base_io_close also authenticates RELEASE_BEGIN before
physical handle closure and acknowledges RELEASED afterwards, and both worker
adapters use it. However, the provider currently accepts the release without
an independent remaining-parent test. The local admission-reference repair
above proves shared-Console continuity, not that all release-policy decisions
have moved to NTSRV. No second worker-base task registry or unapproved new
release protocol is introduced by this S6 fix.

r034-full terminates at Strict DIR after passing Console17/Window17, three
independent WOW frontiers, seven RPC fixtures, five GUI cases and five version
negatives. Its raw VT log proves the corrected launcher was invoked but the
bare `command` guest was not found from the package root. The subsequent DIR
was native CMD output; entered-dos=0 correctly fails the unchanged assertion.
The fixture now independently resolves the packaged System32 COMMAND.COM,
with flat-package fallback, and quotes both paths. It does not add package
directories to application search or alter production launch syntax.
terminal-guest-path-build.log records the observer rebuild; r035-control renews
the control gates on the same ten-image r031-runtime. No production artifact
changed for this fixture repair; failure evidence remains retained.

### Owner correction: I/O release authority

Owner explicitly includes physical I/O channel closure, not only carrier
process retirement. S6's active brief is expanded accordingly. The local
NTVWM users==1 repair is therefore interim evidence, not the final design.

Source audit: frontend_pump already closes its channel only on authenticated
FrontendRequest's release instruction, then reports FrontendIoDisconnected.
worker_base_io_close currently asks RELEASE_BEGIN and then closes/acknowledges.
The provider accepts that request without a service-owned completion decision;
NTVWM end_io uses local users to decide whether to ask, and NTVDM's adapter
asks on the original execution pause boundary. These are the exact remaining
authority gaps; original DOS/WOW execution remains in its own mirror owner.

Implementation boundary: report a typed completion/pause checkpoint through
the existing NTSRV control RPC, and obtain an explicit retain/release decision.
For native completion NTSRV authenticates the direct record and determines
remaining admitted work from its existing records, not worker users or a new
Console census. For DOS pause the original execution owner reports its pause;
NTSRV authorizes the associated route transition, without scheduling the guest.
An unsolicited RELEASE_BEGIN must no longer create that authority. Existing
broker-directed cross-worker releases use the same decision/acknowledgement
mechanism. worker-base executes the decision and owns common endpoint cleanup;
NTCON remains an instruction follower, not a second route registry.

Ordering requirement: service release decision precedes worker final paint and
input return, which precede frontend disconnect notification and both physical
close acknowledgements. Only after these may a release-dependent completion
receipt or new owner proceed. Retained same-worker completions still flush and
confirm their output without deactivating the parent's channel. Failure cannot
silently complete a request or reacquire ownership. New tests must assert
unsolicited-close rejection, retained-parent continuity, ordered both-endpoint
closure, rollback/disconnect and both native widths. Protocol/build identity
and affected product evidence require renewal after implementation; r031 is
not published or certified against this newly approved authority requirement.

Owner subsequently confirms this checkpoint/instruction design. The RPC41
candidate adds WorkerIoCheckpoint(reason, direct-request, decision) on the
existing authenticated service connection; common/rpc and worker-base share
client mechanics. DOS reports its original pause boundary, and native reports
the exact admitted direct request. NTSRV examines its existing native records
to retain a live same-worker parent. One route flag distinguishes a service
release order from the later final-I/O-ready transition; it is not a token,
task registry, scheduler or additional peer connection. RELEASE_BEGIN without
that order now fails. New admissions wait on the existing broker condition
during the ordered release, rather than starting a target on a closing pipe.
NTCON still consumes the service's release instruction after final I/O.

NTVWM no longer tests users==1 to decide closure. A KEEP instruction performs
an output flush/barrier without mouse leave or unused-input return; RELEASE
uses the existing final frame/input-return/dual-ack cleanup. Local users remain
only input/capture admission state. NTVDM's project adapter requests the same
typed decision before its existing pause barrier/close; no original mirror
body is changed. Shared close mechanics remain worker-base-owned.

io-authority-build.log fails on missing protocol constant declarations;
build2 additionally finds the same declaration omission in the service entry.
Both are retained failures. Explicit protocol includes correct the omissions;
build3/build4 pass the affected x86 links and VdmTib ownership gate. Hook64 and
remaining protocol41 clients/DLL dependencies rebuild successfully in their
separate formal caches. No candidate is published.

io-checkpoint-rpc-test.log:220 checks, zero failures and zero handle delta,
including RETAIN/RELEASE, malformed decision and RPC failure/exception.
io-checkpoint-lifetime.txt:1084 checks, zero failures,48 completions,
16 cancellations, target survival and zero remaining handles. The first
argument-less invocation exits2 and is not a pass. io-checkpoint-provider-test
adds unsolicited-close rejection, authenticated exact-record validation,
live-parent retention, delayed frontend-close notification and admission
exclusion during release; retained double-ack/acquisition barriers pass.
r041-service passes all29 retained service fixtures. Reports and exact tests
remain under build/M0-T432/S6; these supplement, not replace, real-package I/O.

r035-control (sealed RPC40 r031) passes RPC, GUI, version negatives and strict
DIR with real DOS entry. modern-edit-return exits not-started because the
fixture still launches flat PackageRoot/run16.exe; its missing line snapshot
is correctly a failure. It now uses Get-PackageBinaryRoot for the launcher,
COMMAND.COM and MEM.EXE; editor, output, return and completion assertions remain.
r036/r037 independently prove actual AMD64 CUI and GUI kind3 with output/real
completion or detached startup/management close on sealed r031, not RPC41.
r040-runtime is the fresh ten-image RPC41 candidate; both native reentry
directions pass in r042/r043: I386 parent/AMD64 child and AMD64 parent/I386
child reuse the same worker PID, preserve child-before-parent output and
return actual outer exit23. Publication and full gates remain open.

The first RPC41 interactive AMD64-to-DOS probe times out before Enter:
the injected absolute launcher/guest command wraps beyond its53-column
current-row echo milestone. Shortening only the launcher spelling preserves
the absolute guest path and allows Enter. The resulting inner NTVDM reports
the original environment-setup error before a DOS prompt. The identical
probe on sealed RPC40 r031 reports the same dialog and timeout. This rules
out attributing this particular observation solely to the new checkpoint,
but does not prove its cause or pass the handoff gate. Relative guest spelling
also failed and is retained as an invalid substitute for that control.
Reports are handoff64-console, handoff64-console-short,
handoff64-console-absolute and r031/handoff64-console-control. The operation39
ERROR_NOT_FOUND log is the permitted absent text configuration at initial
seed, not proof of the DOS environment failure.

r044-full completes with exit1, not a full pass: independent WOW frontiers,
Console17, Window17, RPC7, GUI5, version negatives5, strict DIR and modern EDIT
return pass. Cooked-return fails with outer exit0x2331; the captured page shows
bare COMMAND/MEM could not resolve from the package root, so no DOS entry was
proved by that fixture. Owner confirms this is normal search behavior, not a
product defect. Set the cooked-return and rapid-relaunch fixture cwd to the
package's System32 directory and retain bare COMMAND/MEM, matching actual
usage. This supersedes the interim explicit-path fixture correction; preserve
all output/order/exit assertions and production PATH/search syntax. Later
gates in r044 were not reached.

Final authority review also closes a pre-drain acquisition loophole: after
NTSRV orders release, the current worker's ACQUIRE now returns ERROR_BUSY,
even while its existing pipe remains usable for final I/O. The route cannot
be regranted before both endpoint acknowledgements and a new execution
admission. r045-service passes all29 fixtures including this additional
negative; current RPC client220 and native lifetime1084 checks pass, zero
failures/handle delta. The first final-fixture build invocation names a
nonexistent native-image-test target and fails; corrected targets build.

r046 staging precedes a dependency rebuild; r047 control preflight correctly
rejects its run16/cache hash mismatch, before tests. Fresh r048-runtime seals
the current ten images. r049-control is running, not passed. Hook64's43-input
manifest is renewed as hook64-build-inputs-rpc41.json; the old RPC40 manifest
is retained, not silently overwritten. Original mirror source diff against
12160c657 remains empty; no new package is published and S6 remains open.

r049-control passes RPC7/GUI5/version negatives5, strict DIR, modern EDIT,
System32-cwd cooked return,12 immediate DOS/native pairs,12 immediate native
interactive relaunches and session isolation. It then fails retirement before
DOS creation: that fixture also starts bare COMMAND from package root. Its
captured page proves ordinary name-resolution failure, not worker death or
registration failure. Set its cwd to launchBinary/System32 without changing
guest search or the death/1067/cleanup assertions.

r050-io passes both real frontend-loss worker/receipt cases and all four
I386/AMD64 Console/Window native-to-DOS/parent-return cases, actual exit23.
These use the existing observer's checked80-column disposable Console
profile, now explicit in the handoff fixture. The earlier default private
desktop53-column/environment-error controls remain independent failures;
do not claim they are repaired or silently count them as passing reruns.

Current Hook32 and Hook64 installer logs each pass147 assertions, including
ordinary alternating-width descendants, native identity/suspension and
installer-only helper timeout/early-death cleanup. Current metadata-test2
passes402 checks, zero failures/handle growth; metadata-test incorrectly
supplied CUI/EXE files for GUI/DLL roles and fails162 assertions, retained as
an invalid invocation, not a product failure proof. Hook64 generation-time
source identities match current bodies; its renewed43-input closure is
separate from the retained old manifest.

r051-full ends with exit1 on sealed r048. All preceding retained groups
pass; final nested-window-handoff times out with an actual NTVDM illegal
instruction dialog: CS03f4 IP20f7 OP63 6f 72 72 65. This is not the earlier
unresolved-name or environment-setup dialog. APP/RPC41 identities agree;
no dual-worker production names/reservations remain, and original MVDM/
OpenNT-host mirror diff against12160c657 is still empty.

r055-contrast reproduces the same I386 Window handoff on sealed RPC40 r031
and current RPC41 r048 under the full runner's version-test environment.
Both fail with CS03f4 IP1f96 OP63 61 6c 5c 54. Its wrapper exit0 means
evidence collection completed, not that either case passed. This excludes
an RPC41-only explanation but does not prove the cause or an immutable guest
defect. Four version-negative configuration variables remain present until
the runner's outer finally; source search finds their consumers only in
Verify-ProductVersions.mjs, not product code. They nevertheless enter the
inherited environment. The absent/version-only/build-only contrast r056 is
prepared under build but not run: elevated execution's automatic approval
review returns401 authentication and executes nothing. Do not bypass this
failure or report the proposed contrast as evidence.

Current native certification independently passes: r052-hook-I386/AMD64 each
prove ten legacy routes and visible WINMINE startup; r053-reentry-I386/AMD64
prove reciprocal actual-width children, the same x86 worker PID,
child-before-parent output and outer exit23; r054 GUI proves actual64
detached kind3, startup-only receipt0 and authenticated management close.
These narrow successful results do not override r051/r055 failures.
No publication or S6 closure is claimed.

Approval service recovery permits the prepared r056 contrast. Identical
r048 I386 nested-window handoffs yield absent=PASS, build-only=PASS,
version-only=FAIL, the latter with CS03f4 IP20f7 OP63 6f 72 72 65, matching
r051. Thus the version-negative configuration environment is a demonstrated
trigger under this fixture; the internal immutable COMMAND/CCPU failure
mechanism is not yet established. Do not infer general environment immunity.

Fix only the runner's test-variable lifetime: its version-negative gate saves
the four incoming values and restores them in its own finally, including
exception/nonzero paths and truly absent values. Incoming owner values are
preserved, not stripped; guest, product search, protocol and assertions stay
unchanged. tests/observation/test-product-version-environment.ps1 extracts and
executes the actual runner AST block with a finite fake Node boundary; all
six absent/present x success/nonzero/exception cases pass, proving setup,
restoration and failure propagation without a product process or endpoint.
Current r057-control-scoped is running the entire affected serial Control
group on unchanged r048. r051's successful Console17/Window17/WOW input hashes
remain applicable: this correction changes only the subsequent control
version-negative block, not those group bodies, observers or ten-image set.
The earlier failed observations remain failures, never relabelled passes.

### Final S6 delivery gate

r057-control-scoped exits0 in212391ms. Every Control row is true: RPC7,
GUI5, version negatives5, strict DIR, modern EDIT return, cooked outer CMD,
12 immediate DOS/native pairs,12 immediate interactive native relaunches,
independent session isolation, both worker frontend-loss/receipt1067 cases,
and actual I386 nested-window handoff/parent-return/exit23. Its final handoff
takes5956ms, not a timeout. This is the corrected runner on the identical
sealed r048 ten images, not a production modification or weaker assertion.
The successful r051 Console17/Window17/WOW groups have unchanged package,
observer, guest and case inputs and precede the modified Control-only block;
they are reused by dependency identity, not relabelled as a successful full run.

r029-publication/publish.ps1 publishes the coherent r048 ten-image set plus
MIT notice to O:/winnt/system32, preserving the old nine images and current
NTVDM.REG/CONFIG.NT/AUTOEXEC.NT as recovery. Guest/configuration are not replaced.
published.json matches every tested image hash. The first r058 attempt uses
the isolated-build handoff helper on O:/winnt and is correctly rejected
before any product starts; it is not a failed product execution. Its cleanup
restriction remains unchanged. The separate publication smoke uses the owner's
approved exact deployed paths, pinned handles and creation-time checks.
r059 proves actual deployed DOS MEM output/receipt0, I386 CMD output/receipt0
and AMD64 CMD output/receipt0; all ten deployed images plus notice retain
their hashes afterward. Smoke explicitly cleans its package processes and
does not claim that cleanup as proof of normal retirement.

Final source review retains one worker, original search/flags/native-child
handles and suspension, copied architecture-neutral Hook context, official
finite installer-only helper, actual-task projection and NTSRV release policy.
No MVDM/OpenNT-host mirror change exists against the accepted source baseline.
The new test-variable fixture covers the actual runner block's setup,
restoration and failure propagation, not an alternate test implementation.
Governance, relative links and diff checks pass before production P formation.
T432 still awaits owner acceptance; default private-desktop and arbitrary
large-environment behavior are not claimed repaired, nor are SOL/WRITE playable.

P2b97041e748772dce813fb963e7527781c7321909 contains the reviewed implementation,
tests and publication evidence, pushed to main; HEAD and origin/main match
and the worktree is clean immediately afterward. Documentation-only P3
records that observed delivery and S6 implementation closure. T432 remains
open for the owner's actual product acceptance; no successor work is admitted.
