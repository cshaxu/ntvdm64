# T448 S8 public-library import

## S14 synchronization and verification

Owner approves byte-exact synchronization of the selected SoftPC public
closure. Read-only upstream HEAD `7d3b711d0113992984cb7eeed613097ebf750168`
changes only selected `kvm-base/component.c` and `worker_interface.h` since
the S8 pin: the identity allocator becomes component-private, removing its
external declaration and internal null checks. No repository consumer outside
that component calls it. All 52 selected local blobs and manifest SHA-256s
match the new pinned Git objects. `libvterm` is a separate library.

Full build r1 failed during compiler detection (wrong CMake/PDB mismatch); r2
using Kitware CMake passed all 13 images and the second-build check with 1458
unchanged intermediates. Candidate package r1 matches all 13 built hashes.
Six x86 and six x64 frontend fixtures passed. CAF NaturalExit and SelfDelete
passed; ExternalClose cmd and ntmon passed, but command initially failed its
active-buffer pre-Enter echo assertion. The baseline comparison also failed
the same strict gate. Both candidate and baseline before-Enter snapshots show
`q` on the newly printed outer prompt at row 5, while the test recorded a
pre-injection cursor on row 4. The observer is corrected to check a newly
written character immediately before the current cursor against its pre-input
screen snapshot. This handles a parent prompt printed after input without
accepting an already-present character as echo. The corrected observer passes
all eight real NaturalExit, ExternalClose and SelfDelete cases for the
applicable cmd, command and ntmon targets. Original failed records are retained;
this does not prove that every historical echo problem was an observer race.
DOS Console 17 cases passed. The first WINMINE probe build failed because the
temporary harness removed its TEMP directory before compiling; a separately
run corrected-environment WINMINE gameplay witness passed. All tests use the
unchanged staged guest. The complete 13-image candidate is published to both
assets/release and O:/winnt/system32, with hashes checked against the candidate;
guest files were not changed. Disposable evidence and candidate remain under
`build/t448-s14`. The owner accepted S14 on 2026-10-22 local time. Negative injection for the
corrected observer and full-size SOL gameplay were not rerun in this stage.

## Scope and provenance

Owner admits S8 after the [S7 audit](m0-t448-s7-shared-library-import-audit.md).
Single implementer/reviewer; no guest changes, new wire protocol or peer
lifecycle authority. Published baseline remains d5d0dc243/protocol64 until all
delivery gates pass. This ledger records work in progress, not S closure.

`src/ntcon-exe/nxvm-import.json` pins52 selected files and18 translation units
from SoftPC0d9f1803ee04bd89bd6150306912a3e8674d0a83. The one-off import reads
Git blobs, not the sibling checkout or HEAD; it validates all old manifest
hashes before replacing files, resolves quoted includes, and rejects unexpected
removals. Selected bytes are unmodified; existing MIT license unchanged.
The old KVM keyboard implementation/header are retired in favor of Base.
No audio, storage, Console broker, Linux provider or upstream app is selected.

The narrow lifetime fixture derives from upstream
`test/lib/kvm-window/win32/kvm_window_capture_contract_smoke.c`, inspected
SHA256 `3FA8630062662A20FF13F87A49C4A7460040A6178E9334A4DE24392DAE23C80C`.
It retains the MIT attribution and isolates startup/join faults without
importing unrelated sizing/capture mocks. It includes the actual local imported
Win32 provider and links the production library; external source is not a
test/build dependency.

## Implementation boundary

- Window controller owns one exchange; frontend I/O writes only its unheld
  slot and commits after validation. Window consumes immutable slots.
- Text and RGB have distinct actual-size payloads. Removed NTCON text_frame
  staging files and frontend_session's separate full-frame allocation. No
  presentation copy remains in the controller.
- Existing indexed DIB wire converts once to RGB; text retains font banks,
  underline and two cursor spans. Existing160x96/font32 text wire acceptance
  remains independent of the library's dynamic extents. Window owns blink.
- Normal library destruction consumes the object even on cleanup error;
  controller clears the pointer and completes independent cleanup. Unsafe
  unjoined threads take upstream immediate fatal exit, not a new callback.
- Read-only test observers follow the pinned leaf's frame pointer and copy
  remote payload explicitly. Changing samples are rejected. This remains
  non-atomic observation, not a product IPC or proof of every concurrent schedule.

## Runs

All commands below run in hidden owned background processes with logs below
`build/t448-s8`; stable intermediates stay under build/.ninja and build/.ccache.

| Run | Actual evidence | Outcome |
| --- | --- | --- |
| r1 | PID35752;2026-10-22T18:08:00Z–18:10:37Z;build-r1-result.json and build-r1/steps.json | Failed x64 observer compilation: old embedded-frame layout/fixed constants. x86 build passed. No publication. Earlier PID35576 produced no result and was verified absent before replacement. |
| r2 | PID1180;18:23:13Z–18:26:29Z;build-r2/result.json,steps.json and build-r2.stdout | Full13 and no-change second build passed; library, keyboard and mouse fixtures passed. Controller fixture compiled but was not run. Not runtime/package acceptance. |
| r3 | PID23536;18:28:49Z–18:32:28Z;build-r3-result.json and logs | Full13/incremental and prior focused fixtures pass; joined startup rollback, consumed cleanup-error return and four immediate-fatal child cases pass. |
| r4 | PID31156;18:33:37Z–18:37:02Z;verify-r4-result.json | Full13 and1435 unchanged intermediates pass. Fixture runner then fails before executing tests: x86 worker has run-ninja-parallel.cmd, not run-ninja.cmd. No product failure or publication. |
| r5 | PID38608;18:37:32Z–18:37:44Z;verify-r5-result.json | x86 non-UI fixtures pass, including allocation/lifetime faults. Desktop controller fails before assertions. No publication. |
| r6 sandbox | PID37152;19:00:01Z–19:00:10Z;verify-r6.stdout/stderr | Repeated x86 non-UI checks pass; added diagnostic proves CreateDesktop error5. Execution permission failure, not product acceptance. |
| r6 host | PID17388;19:05:50Z–19:08:45Z;verify-r6-host.stdout/stderr | Both widths' six fixtures and real channel/text pass. Seven CAF cases pass; cmd SelfDelete times out before NTMON confirmation. No publication. Reuses verified r4 full13. |
| runtime r4 | PID38340;19:09:04Z–19:11:50Z;runtime-r4-result.json | Independently runs package-r6: DOS17 and Minesweeper gameplay pass; WRITE four-sample startup frontier passes; SOL is visible/alive at all four samples, startup observation only. |
| review | PID26548;19:11:10Z–19:11:50Z;review.json | All52 upstream Git blob identities/local SHA256 and license match; no OpenNT/MVDM or guest changes. Documentation governance and whitespace pass. Earlier sandbox read failed before comparison; no false pass. |
| CAF r7 | PID27552;19:12:45Z–19:13:09Z | Exact cmd SelfDelete passes with diagnostic-only observer changes, unchanged product; not proof that r6 failure was repaired. |
| CAF r8 | PID17056;19:14:15Z–19:14:59Z | Fixed three-case cmd SelfDelete diagnostic group passes all three, with no retry-to-pass rule or relaxed assertion. |
| title r9 | PID22548;19:17:12Z–19:17:23Z | New ACP caption regression fails controller assertion319: short non-ASCII title rejected by unadapted UTF-8 boundary. |
| title r10 | PID22388;19:18:53Z–19:19:14Z | Both-width controllers pass after NTCON converts ACP to UTF-8; actual GetWindowTextW equals the ACP-decoded source. Subsequent shadow-variable cleanup requires next build. |
| build r11 | PID15120;19:20:26Z–19:23:53Z | All images compile/link, but second-build comparison fails on window_controller.obj because the agent edited a header comment during this check. Comment reverted; this is invalid incremental evidence, not a product regression. |
| build r12 | PID17232;owned session17576 | Repeats full13/incremental with product/test/build inputs frozen, retains r11 failure. Running; no publication. |

r6 SelfDelete retains `cmd.txt.caf.txt`: visible frontend38456 and correct input
owner, but self-ntmon-confirmed=0; its Window title already names ntmon. Thus
the run does not establish a failed Console return, nor does it establish
correct self-delete. COMMAND and direct NTMON pass. NaturalExit cmd/command
and ExternalClose cmd/command/ntmon independently pass pre-Enter echo.

The host lifetime tests prove joined startup rollback, consuming cleanup-error
returns and four immediate-fatal child boundaries without atexit. The payload
test runs a concurrent held reader through32 allocation failures and succeeds
with65 allocations/65 releases, peak3 live payloads. Neither proves arbitrary
thread schedules. Sandbox diagnostics remain in this ledger even though the
host invocation subsequently passes isolated desktop creation/controller tests.

r2's library assertions exercise lazy initial payloads, same-size reuse,
text→graphics→larger-text while old processing/displayed readers remain held,
generation invalidation, RGB decoding, all retained glyph/style pixel checks,
font32/split-cursor geometry, native viewport and malformed publication rules.
They do not replace concurrent allocation-fault or desktop lifecycle tests.

## Remaining fixed acceptance

1. Finish x86/x64 library/lifetime verification, including allocation failure
   and concurrent retirement; real isolated Window controller/capture/reopen.
2. Frontend channel/text and eight CAF pre-Enter echo cases against coherent
   staged full13; DOS17 and retained independent WOW frontiers. Existing SOL
   layout and WRITE startup-only limits remain explicit.
3. Final source/hash/dependency review, exact custom/imported/mirror delta,
   documentation governance, full13/incremental identity, coherent publication
   to assets/release and O:/winnt/system32, commit/push and clean checkout.

No original OpenNT/MVDM mirror or guest file changed so far. Removed historical
local library divergences are superseded by byte-exact upstream sources, not
silently attributed to original mirrors. Final inventory remains pending.

## Additional compatibility finding: title encoding and capacity

Source review traces GetConsoleTitleA in frontend_session.c and NTVWM
presentation.c through the existing128-byte copied title into the controller.
The prior imported native API was ANSI; the selected new Window API explicitly
validates UTF-8 and uses Unicode host calls. Host ACP is1252. A caf-e-acute
fixture proves the unadapted controller fails, rather than merely displaying
mojibake. NTCON now performs ACP-to-wide-to-UTF8 conversion on initial and
updated captions before library calls; stored controller text is UTF-8.
No producer wire or upstream file changes. Both-width r10 retains the native
caption's decoded characters and all existing controller lifecycle assertions.

The remaining boundary is independently concrete:127 ACP1252 e-acute bytes
become254 UTF-8 bytes plus NUL. Both upstream title and control payload
capacity are128, including NUL. A conversion alone cannot preserve that input.
Read-only check of current upstream22cdeb7f confirms the same limit. No fallback,
replacement characters, hidden HWND manipulation or silent truncation is added.
The owner has been offered upstream capacity expansion (preserve content and
unchanged import) versus explicit UTF-8-boundary truncation. This is a newly
discovered observable compatibility decision, not a repeated permission ask.
Publication remains pending. The earlier full13 package and runtime results
are historical pre-title-fix evidence, not verification of final source.

Read-only upstream follow-up at22cdeb7f reports no changes since the selected
pin in the four imported component directories. Thus updating the pin alone
cannot fix the title capacity boundary. As a provisional, non-final accounting,
NTCON-owned C/header changes are144 additions/211 removals (net-67). Imported source, provenance metadata,
test/tool additions and documentation are separately accounted at final review;
no claim of a total autonomous-code reduction follows from the product-only sum.

The smallest content-preserving upstream candidate is bounded capacity
expansion, not a dynamic-title framework: both Window title and generic control
payload must cover the converted128-byte ACP carrier. For example512 bytes
covers127 three-byte UTF-8 characters plus NUL; changing only the title constant
violates the existing compile-time payload assertion. It adds12672 bytes to
the33-entry control ring, not per-frame graphics storage. Upstream should prove
initial/update exact Unicode captions, boundary acceptance, overflow/malformed
rejection without modifying the prior title, and normal destruction. This is
a design proposal only, not authorization to modify the read-only upstream or
an undisclosed exception to unchanged import.

Current link-map inspection finds no Base task/sleep/yield/event-wait/reset
symbols in NTCON. The optional default notifier and event-create code still
link through the upstream selection API; Window selects its native notifier.
No claim is made that every unused API byte has disappeared. Keeping the
selected upstream files exact is distinct from enabling another runtime loop.

## Latest-source verification continuation

- r12 PID17232, 2026-10-22 19:24:32--19:28:20 UTC: full13 build and
  no-change incremental pass,1447 intermediate identities unchanged.
  `build/t448-s8/build-r12/result.json` retains all13 image hashes.
- r13 PID13756,19:29:24--19:31:39 UTC: both architectures' six frontend
  fixtures pass, including ACP title conversion, real isolated controller,
  consumed destruction/fatal boundaries and concurrent payload retirement.
  Channel/text pass. All eight CAF NaturalExit/ExternalClose/SelfDelete
  cases pass against newly staged package-r13 with before-Enter echo checks.
  `verify-r13-result.json` and `r13-*` retain the results. This does not erase
  the earlier unassigned r6 intermittent failure or prove its cause fixed.
- r14 PID27936,19:31:16--19:31:52 UTC: seven affected observer/measurement
  translation units compile with W4/WX; input-milestone executable links and
  passes fresh-echo, stale-prompt, partial-input and foreign-window negatives.
  Other six diagnostic tools were compile-checked, not each runtime-qualified.
  Cache is build/.ninja/msvc-x64-release/observation-checks; evidence is
  build/t448-s8/observers-r14-result.json.
- r15 PID38032,19:31:55--19:34:30 UTC: latest package-r13 DOS17 and Minesweeper
  flag/counter, reset, open-cell, WM_CLOSE and Run16 completion pass. WRITE
  retains its four-sample main/document/pageinfo startup frontier, not editing
  or normal-exit acceptance. SOL is startup-observation-only with its existing
  layout limitation. runtime-r15-result.json and runtime-r15-* retain evidence.
- r16 PID12840,19:33:59--19:34:26 UTC: all52 upstream blob/local SHA identities,
  license,18 selected units, original mirror/guest unchanged, governance and
  diff whitespace checks pass. review-r16.json records provisional deltas:
  imported +1825/-745; product/metadata +248/-281; tests/tools +505/-216;
  documentation +223/-15 before this final run-result note. No release assets
  changed and no product publication or delivering commit is claimed.

Owner reiterates autonomous execution and no repeated authorization requests.
Read-only upstream inspection uses that standing O: authorization. Imported
sources remain exact; no silent title truncation or upstream write was made.

## Title-boundary follow-up

Read-only upstream recheck still reports22cdeb7f8df1d6cb05c4c7331a1616f5324c982a
and128-byte title/control payloads. No external source update resolves the gap.
r17 PID21240,19:36:44--19:36:57 UTC, passes both-width controller tests with
new initial/update boundary assertions. On the verified ACP1252 host,127
e-acute characters occupy128 source bytes including NUL but255 UTF-8 bytes.
Both operations return ERROR_INSUFFICIENT_BUFFER; creation leaves NULL and
the actual existing HWND caption remains unchanged. This proves safe rejection,
not preservation of the formerly supported title. Other code pages derive
their source length by actual encoding rather than assuming single-byte ACP.

Caller audit narrows impact: production creates its controller with an empty
caption; present_loop deliberately ignores cosmetic refresh_window_title
errors. Thus this long-title path retains an old/empty caption, rather than
proving a session crash or failed Console handoff. No broader failure is claimed.

The audit also finds an input/output capacity coupling: refresh_window_title
read ACP bytes using KVM_WINDOW_TITLE_CAPACITY. Simply expanding that upstream
constant would expand raw input as well, reintroducing UTF-8 overflow. Its local
read buffer now uses the existing CONSOLE_IO_TITLE_BYTES128 producer boundary.
This changes no wire limit or current observable truncation rule and introduces
no new protocol, cache or library divergence. The library's output capacity
remains independent. r18 PID23896,19:37:58--19:38:44 UTC, passes the affected
x64 frontend and x86 session-fixture builds, real isolated Console handoff,
governance and whitespace checks. boundary-r18-result.json retains the result.

## Delivery blocker audit

Following the verification continuation and title-boundary follow-up, the same
capacity blocker persists on a fresh read-only upstream check: clean checkout,
HEAD22cdeb7f, Window title128 and control payload128. Recorded r17/r18 and
runtime/review processes are no longer live; their captured results are terminal.
There is no outstanding process to wait for and no further independent change
needed to demonstrate this finite boundary. Previous turns made concrete test
and input-boundary progress, but neither removed the incompatibility.

Current authorization selects unchanged import and does not select silent
truncation, a private HWND bypass or modification of the read-only upstream.
The content-preserving exit is an upstream capacity update as specified above;
an explicitly changed compatibility/import contract is the alternative. Do not
manufacture delivery by treating safe rejection as compatibility acceptance.
After resolution, refresh provenance as applicable, verify the full source
candidate including the final input-boundary correction, publish all13,
review/commit/push and verify cleanliness. These steps remain pending. All
working changes and existing release/guest images are preserved.

## Approved title resolution

Owner explicitly selects display-only truncation and confirms implementation.
This supersedes the preceding capacity blocker:512 bytes was a proposed
implementation size, not an original OpenNT requirement. NTCON converts its
existing ACP input through UTF-16 into a bounded temporary UTF-8 buffer, then
copies the longest complete prefix fitting127 bytes plus NUL. It never changes
the caller's string, Console title, wire limit or imported library. No native
HWND bypass, new API, callback, dynamic allocation or upstream patch is added.

r19 PID22424,20:14:47--20:15:01 UTC, passes both architectures' controller
fixtures: short non-ASCII text preserved; exact127-byte ASCII retained;
two-/three-byte output truncated only at character boundaries; initial creation
and subsequent update succeed; actual native caption matches an independent
UTF-16-prefix oracle; caller source unchanged. Host ACP1252 covers both wider
UTF-8 cases. Other host code pages are derived by actual conversion, not claimed
as separately exercised locales. Product caption buffer is large enough for
three bytes per input UTF-16 unit; surrogate pairs need at most four bytes per
two units. Trimming examines the first excluded byte and backs over continuation
bytes, so it never copies a partial UTF-8 sequence.

Final qualification starts with frozen-input build-r20 PID17676. Historical
failed runs remain recorded; their results are not overwritten by later passes.

## Final candidate gates

- build-r20 PID17676,20:15:30--20:19:32 UTC: all13 components and1454 unchanged
  intermediate identities pass. Sources/build inputs stayed frozen throughout
  both builds. Cache: build/.ninja/msvc-{x86,x64}-release with optional ccache.
- r21 PID12968,20:19:46--20:21:58 UTC: both-width six frontend fixtures pass;
  real channel/text pass; all eight CAF cases pass, including cmd/command/ntmon
  self-delete and actual pre-Enter echo. Package-r21 hashes match build-r20.
- review-r23 PID33668,20:18:37--20:19:09 UTC:52 pinned upstream blobs, license,
  18 units, original mirrors/guest unchanged, governance and whitespace pass.
  Source review retains one writer/exchange owner, joined destruction before
  release, no decoded-failure publication, and title conversion only at the
  frontend presentation boundary. No imported-file patch or new product timeout.

Source accounting before final documentation/publication: imported +1825/-745;
owned product plus provenance metadata +258/-284; tests/tools +541/-216.
NTCON-owned C/H alone is +154/-214 (net-60). Including tests/tools and metadata
gives a net increase299, not an overall autonomous-source reduction. Added tests
cover the changed ABI, allocation/held-reader failures, lifetime faults and title
encoding; reducing them merely to claim a smaller total would remove evidence.
Original mirror diff delta remains zero. Documentation and release binaries
are separate from these source counts.

## P1 publication and acceptance handoff

Runtime-r22 PID33748,20:22:20--20:24:57 UTC, passes DOS17 and Minesweeper
flag/counter, reset, open cell, WM_CLOSE and Run16 completion. WRITE retains
four-sample main/document/pageinfo startup evidence; editing and normal exit
remain unverified. SOL retains startup-only observation and the accepted layout
limitation. No original guest change or bypass was introduced.

Observers-r24 PID16404,20:25:56--20:26:32 UTC, rebuilds all seven affected
diagnostic translation units against the final source with W4/WX and passes
the linked input-milestone assertions. Six tools are compile-checked only;
this does not claim additional runtime observation coverage.

Publication-r20 PID17840,20:25:13--20:25:37 UTC, verifies all prerequisites,
checks no installed project processes, backs up the prior complete package,
then synchronizes all13 hashes to assets/release and O:/winnt/system32. Both
destinations match package-r21/build-r20. release-manifest.json is the committed
identity authority; NTCON SHA256 is
9971B301AC8D8AA06EEA3752812F410B1C7C0052DFA89D86A9CF9A1CA8A99AF7.
Backup: build/t448-s8/publication-r20-backup. Runtime tests used the same exact
candidate images; hash equality is not claimed as a second installed-session
runtime test. No user processes needed termination for this publication.

The delivering P1 commit carries sources, provenance, tests, this record and
the complete release manifest/assets. Verify push and clean status at handoff;
owner acceptance is not inferred. Keep bounded scratch and rollback evidence
until acceptance disposition; do not clear valid Ninja/ccache caches. The r6
readiness incident is retained as unassigned test evidence, not a claimed fix.

## Delivered identity and final T-scope reconciliation

S8 P1 source/release commit is b3ee596e4ac85e1a223af24a5ccd410369d6bce3.
Post-push verification found origin/main equal to HEAD, no staged/unstaged or
untracked files, and all13 candidate/assets/installed SHA-256 values equal.
The staged52 imported blobs also matched the previously verified raw source
bytes. Git reported unreachable loose objects during its automatic packing;
no manual prune or unrelated repository cleanup was performed.

This follow-up is documentation-only. It does not refresh binaries or claim
new runtime evidence. Against the accepted S6 source d5d0dc243, no changes
exist under Run16, NTSRV, worker-base, NTVWM, common protocol/transport,
adapter-mvdm-host-out or original MVDM/OpenNT mirrors. NTCON frontend_session
changes are the frame writer/size/present ABI, removed staging allocation,
consumed-on-return destruction cleanup and the ACP title bound. No lifecycle
RPC owner, command/ack or reconnect policy changed. The S5 nine-row source/
assertion reconciliation and S6 closure audit remain applicable to those
unchanged owners; S8's channel/CAF and allocation/lifetime tests cover its
changed frontend boundary. This is not a fresh exhaustive schedule proof.

| Remaining closure group | Disposition |
| --- | --- |
| S1-S6 lifecycle/polling/monitor scope | Prior accepted S6 audit and S5 nine-row reconciliation retained; no new source changes to their control owners. |
| S7 compatibility audit | Bounded audit concluded and handed its selected closure to S8. |
| S8 import/integration | Full13/incremental, both-width fixtures, real channel/eight CAF, DOS17 and retained WOW frontiers, exact import/publication hashes, commit/push/clean verification delivered. |
| Title capacity | Resolved under explicit owner-approved display truncation; no outstanding upstream expansion request. |
| Owner acceptance/T closure | Owner reports functional acceptance but identifies the NTVWM CAF title issue below; T closure is not claimed. |

No further implementation S was identified within the fixed import scope. Preserve
SOL layout, WRITE editing/exit and intermittent direct WOW exit as their
existing limitations/TODOs. Preserve the r6 readiness incident as unassigned,
not repaired; later passing matrices do not identify its cause. The queued
WOW32 continuation is not absorbed into T448. Keep current rollback/evidence
until acceptance and exact safe-disposal review. Do not repeat successful
product tests or add cleanup features merely while waiting for acceptance.

## Owner functional acceptance and title observation

The owner reports functional acceptance and observes `ntvwm.exe` as the CAF
caption after `run16 cmd`. On the delivered package-r21, an isolated private
desktop observation recorded the visible LibKvmWindow and NTVWM hidden Console
with the same carrier-path title. A separate `cmd /k title TITLE-AUDIT-EXPLICIT`
observation recorded that explicit title in both windows. DOS `COMMAND.COM`
instead displayed the isolated outer Console title `NTVDM observation`. A
generated PIF with title `PIF-TITLE-AUDIT` displayed that title in CAF; its
read-back NT31 flags were zero. These are title observations, not normal-exit
or general PIF compatibility tests: the observer terminated the held launcher
after sampling, and only its own isolated package was cleaned up.

Relevant retained run files in the S8 scratch root are
`title-audit2-cmd.txt.timeout-live.txt`,
`title-audit3-explicit.txt.timeout-live.txt`,
`title-audit4-dos.txt.timeout-live.txt`, and
`title-pif-audit.txt.timeout-live.txt`. The initial short timeout was rejected
before launch; two incorrectly formed DOS argument attempts did not reach the
intended target and do not count as evidence. The PIF test did not prove the
requested NT31 flag was set. No production change, release or fix is claimed.

NTVWM captures its hidden Console title, while the existing frontend title
query reads the attached native Console. Active worker titles can differ from
that native title during nested handoffs, so blindly seeding every acquisition
from the latter may overwrite active or restored titles. A title correction
must preserve explicit title changes, nested handoff and restoration; its
design and runtime verification are not established by these observations.

The owner subsequently requests a fix, admitted as S9. The release-side
title and new-channel title must be distinguished: the current frontend
clears its active title on unbind but retains a per-channel title for later
reactivation. NTVWM seeds the screen before target execution, and can reuse
an already-presenting carrier for nested launches. Any implementation must
avoid reinitializing that live carrier or replacing a returning channel's
own title with its child's title. A frontend native-title query alone cannot
provide the prior active worker's copied caption. These are source-level
constraints; no selected implementation or verification is claimed here.
