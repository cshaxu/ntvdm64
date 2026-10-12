# M0 T444 S1 — Native indexed-16 palette projection

## Delivered boundary

NTCON now treats the 16-entry `0x00RRGGBB` palette carried by a committed
text frame as Console-buffer state. It converts that wire ordering to the
host `COLORREF` table only at the NTCON Console boundary, applies it to the
logical surface before projection, and applies the logical surface palette to
the borrowed visible Console before copying indexed cells. This is neutral
between DOS and native workers: no worker identity branch, frame ABI, guest
binary or worker-base protocol changed for the palette repair. The separately
proved required-gate producer repair below adds one registered mirror guard.

The frontend records the caller's original visible palette at borrow time and
restores it on normal park and unparked teardown. Palette-only configuration
updates commit to the same logical surface and are projected even when no new
cell grid arrives. A notification failure rolls back a palette changed on a
prepared/logical surface before the publication is committed.

Windows Console supplies palette and cell operations as separate APIs, so the
existing frontend I/O lock serializes this presentation transaction; it cannot
promise a single host-compositor syscall for both operations.

## Evidence

- `build/M0-T444/S1/r004/frontend`: affected NTCON closure compiled with
  release `/O2`; the 12 requested targets completed with exit `0`.
- `build/M0-T444/S1/r004/frontend-text-handoff.txt`: focused fixture passed
  `48` checks with `0` failures. It proves a non-default frame palette, a
  palette-only update, normal text handoff, and original borrowed-palette
  restoration. The fixture also repairs its own previously invalid null reply
  buffer and invalid 1x1 Console geometry.
- `build/M0-T444/S1/r008/package/product-system32-manifest.json`: a coherent
  staged ten-image package selected the rebuilt NTCON and hash-checked every
  selected image.
- Deployment copied that tested `ntcon.exe` to `assets/release` and
  `O:\winnt\system32`; all release-manifest identities were compared after
  replacement.

## Delivery-gate limitation

The serial product matrix reached its WOW observer, but the retained private-
desktop observer failed before a product assertion with a null-object error.
Earlier runner fixes in this S removed output leakage and unavailable
`Get-FileHash` use in the test-only staging chain. The remaining observer
failure is retained at `build/M0-T444/S1/r014/product`; it is not counted as a
palette pass and requires separate harness diagnosis. Focused S1 evidence and
package identity evidence remain valid.

## Continued gate diagnosis — environment differential

The owner confirms the deployed `O:\winnt\system32` package starts interactive
COMMAND and executes VER normally. The automated failure therefore does not
identify a first-bad product commit. The prior CMD-parent experiment in
`build/M0-T444/S1/r038` actually recorded an NTVDM illegal-instruction dialog
(`CS:03ff IP:21c3 OP:63 68 61 69 6e`) before the prompt; it is not simply a
missing Console projection. Treat the earlier topology explanation as an
unproved hypothesis, not a root cause.

The reused r008 stage includes local SETVER configuration with absolute
`O:\winnt` references. Run r042 instead extracts `assets/winnt.zip`, overlays
the current ten released images, and copies only the three selected WOW
application files from the supplied installation. With the inherited execution
environment, COMMAND still times out and WINMINE reaches an illegal-instruction
dialog. Thus local SETVER configuration alone does not explain the failure.

Run r043 uses the same clean stage and observer with a child environment
containing only SystemRoot, windir, ComSpec, PATH, TEMP and TMP. Temporary
files use the owned short runtime path. `command /c ver` exits `0`; the Console
snapshot records `MS-DOS Version 5.00.500`. This proves an environment-dependent
difference, but does not yet identify the offending variable or establish an
environment-size limitation. Further differential tests retain those questions.

Run r044 restores the full PATH alone and then all other inherited variables
with a short PATH; both COMMAND cases exit zero with the expected VER text.
Its minimal-environment WOW case records a visible `Minesweeper` window.
Run r045 adds a single `TEST_ENV_PADDING` value to the small environment:
4,096 bytes pass; 8,192 and 12,288 bytes time out. The 8,192-byte dialog reports
`CS:03f4 IP:1f8d OP:63 63 63 63 63`, matching the deliberately supplied `c`
padding. This is direct evidence of guest execution reaching environment
bytes. The write/relocation owner and original-versus-adapter attribution are
still open; controlled regression runs do not dispose of that defect.

Run r046 reaches a visible WINMINE desktop and the previously scoped SOL/WRITE
memory-error frontiers (read from the observer's final window snapshots).
Its Console matrix passes the first five cases, then rejects `native-eof`
because the reduced environment omitted PATHEXT and host CMD could not resolve
`more`. This is a test-environment error; the next controlled run retains the
normal Windows executable-extension contract. Empty intermediate WOW window
samples are not counted as application evidence.

The WOW comparison gate again requires the expected application signature in
its retained baseline. Mere worker liveness cannot substitute for an absent
WINMINE window or the selected SOL/WRITE execution frontier. The earlier r045
report selected as a candidate baseline exited `0x42B`; it is not an accepted
passing oracle. No complete-product pass or first-bad commit is claimed.

## Continued gate diagnosis — recovered attribution and capture repair

The environment finding is not new: the existing TODO and
[T420 S35 direct-copy witness](m0-t420-s35-xms-capability-progress.md#direct-environment-copy-witness)
already establish the original COMMAND INIT-lifetime defect. The current
staged COMMAND SHA-256 is
`908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43`,
matching the independently repeated T422 evidence. The r045 failure at
`IP:1F8D` matches original INIT `Alloc_error`, with the synthetic environment
bytes at execution. This corroborates that existing immutable-guest debt,
not a newly identified host regression. No product environment truncation,
guest change or invented first-bad commit is justified. The bounded test
environment is recorded explicitly and does not claim that large environments
are repaired.

Run r047 passed 16 Console cases; EDIT was absent from the clean ZIP stage.
Run r048 added only the supplied EDIT.COM/EDIT.HLP/QBASIC.EXE/QBASIC.HLP,
with input hashes in `supplement.json`. EDIT reached its UI, but after returning
and submitting MEM it intermittently raised `The device is not ready`.
Run r049 passed an independent Console EDIT case; that does not erase r048.
Run r051's five independent Console EDIT cases passed 1/3/4/5 and failed 2
with the same dialog after MEM submission. This remains an unresolved
production-path failure, not an observer timeout waiver or retry-to-pass.

Run r049 exposed a separate Window observer defect: input readiness was read
from NTCON's actual Window frame, but per-command evidence was captured from
its parked Console. The MEM snapshot was blank despite successful observed
Window echo/completion. `console_startup_observer.c` now captures that same
owned rendered text frame while the Window exists, falling back to the restored
Console after closure. Assertions are unchanged. The r050 AMD64 observer build
and input-milestone negatives passed; r051's full Window matrix then passed all
17 cases, including nested MEM and EDIT-return-MEM. The failed first r050
compile (`/WX` uninitialized structure warning) is retained alongside the
successful corrected build.

Run r049 WOW snapshots independently show visible Minesweeper, SOL's `Out of
memory` dialog and WRITE's `Not enough memory for Write to complete this
operation` dialog. The latter two are retained application frontiers, not
functional success; Minesweeper window creation alone is not gameplay proof.
No liveness-only comparison is accepted. Full delivery is still pending the
EDIT/MEM failure, complete gate reconciliation and publication/commit checks.

## EDIT return failure: diagnostic call-site isolation

The r052 diagnostic relink reuses the release-identical NTVDM inputs from
`build/M0-T443/S16/r033/x86` and changes only the error-reporting translation
unit. It is not a published product image. Run r053 completed ten EDIT-return-
MEM cases without reproducing; r054 stopped at the first failure, case 14,
after thirteen passes. This is a reproducibility investigation, not a
retry-to-pass acceptance rule. The case again reached EDIT, exited it,
submitted MEM, then displayed the device-not-ready dialog. Its timeout report
retains the live thread stacks and dialog text before owned cleanup.

The first diagnostic used `fprintf`, which the historical host build redirects
to `mvdm_host_fprintf`; its absent file log is not evidence that the error
handler was bypassed. Run r056 uses direct Win32 file logging instead, preserving
the original error and last-error state. Run r058 failed at case 5 and recorded:
`pid=36744 tid=36568 error=21`, source `nt_graph.c:814`, in
`publish_text_update()` after `NtvdmConsoleUpdateText()` returned false.
Thus the failing caller is now identified, but local snapshot readiness versus
transport/publication failure still requires distinction. Neither suppressing
ERROR_NOT_READY nor reverting the entire S14 error propagation is justified.
Run r059 instruments only those two call boundaries for the next owned replay.

Source review also finds that original `SuspendTimerThread()` explicitly does
not wait for the heartbeat to become blocked. This contradicts a broad reading
of the adapter's quiescence comment, but is not yet proof of this error's cause;
the failing publication branch and actual producer thread must be established.

WOW gameplay remains open. The r055/r057 private-desktop screenshot probe found
the Minesweeper window but could not capture its client area. Adding a nonzero-
client readiness check and explicit private-desktop binding in r060 did not
reach that predicate within its bounded wait. These are failed diagnostic
observations, not evidence of playable WINMINE or a classified product defect.
The probe never switched the user's desktop or altered the guest binary.

Run r061 stopped at failing case 14. The r059 text-boundary diagnostic records
`pid=31248 tid=28476 phase=publish error=21`; no describe failure was recorded.
The next r062 diagnostic adds only activation and failed-send records under the
existing client lock. Run r063 stops at case 2 and proves the exact boundary:
PID 17224/TID 17200 successfully releases the channel at tick 407544640
(`active=0 error=0 pipe=0 failure=0 generation=0`), then attempts video send at
407544687 on the same thread and still-empty channel. `ntcon_worker_video`
therefore returns ERROR_NOT_READY locally, before any frontend request. This
excludes missing text backing and frontend rejection for this reproduction.

The repair target is producer quiescence across the original blocked/reentrant
COMMAND interval, not transport-error suppression. Original
`CNTRL_VDMBLOCKED` is set before the adapter releases I/O and cleared after
reacquisition in `nt_resume_event_thread`; investigate using that existing
state to gate the added software presentation work while preserving pending
dirty state and the pre-release final paint. No new lifetime owner, connection
state or error waiver is justified. The exact tick/flush entrance and minimal
guard still need focused verification. These diagnostics are not deployed;
no first-bad commit or completed repair is claimed.

## Blocked software tick repair and final non-diagnostic candidate

The producer uses original `CNTRL_VDMBLOCKED`: original `nt_event.c` sets it
before releasing the frontend and clears it after reacquisition. The final
pre-release paint happens before that flag is set. The project-added software
branch of `nt_graphics_tick()` now returns while blocked, before route sync or
presentation work. This preserves pending dirt, including cursor/register-only
updates, for the original resume path. Real send errors are still propagated.
There is no transport error waiver, new state, scheduler or lifecycle owner.
The executable mirror addition is one guard, registered under DIV314.

Runs r064/r065 instrument the guard: twenty independent EDIT-return-MEM cases
passed, and five worker processes recorded the guarded interval. The diagnostic
image is not a release input. Run r070 rebuilds the exact production nt_graph
object and relinks against release-identical formal x86 inputs; inputs.json
records their hashes. Candidate NTVDM SHA-256 is
`2E68CA2633B6A911A857A56AFD93148648DE4C137551DE9CA8E35ADE0F083FE1`.
Run r072 passes all Console 17 and Window 17 cases with that non-diagnostic
candidate and the same guest/configuration/input assertions. No first-bad
commit has been established; the direct runtime trace, not a speculative
commit attribution, identifies this failure boundary.

`tests/observation/verify-blocked-video-tick.ps1` extracts the actual current
production functions into a focused test carrier. Run r071 proves blocked
ticks emit no transport call, retain dirty/cursor/mode/stream state, resume
normally, ignore unrelated shell-count bits, preserve final pre-block paint,
and still report actual ERROR_NOT_READY/ERROR_BROKEN_PIPE failures. Earlier
r066/r067 host-module lookup and r069 fixture macro failures are retained;
they are not passing builds. The final candidate has not yet replaced the
deployed NTVDM.

## WOW gameplay observation repaired, without changing WOW production code

The r055--r068 zero-client observation was not sufficient to claim a product
geometry defect. Run r073 records `iconic=1`, a zero client, then an explicitly
restored test-owned Minesweeper window with a 221x181 client. The private
desktop remains unswitched. Capture now shows the real board. Run r075 posts
ordinary mouse/key messages and captures flag/reset/open results; visible
counter changes from 10 to 9 and open cells show numbers.

The reproducible entry is `tests/observation/verify-wow-mine-gameplay.ps1` and
its native `wow_mine_gameplay_probe.c`. Run r077 uses the r072 candidate package
and passes flag plus counter change, F2 reset to the original covered-cell
image, left-click cell change, normal WM_CLOSE, and `run16 --wait` completion
with exit zero. Each action has its own pre-operation pixel reference and
bounded observation, not an old prompt or a liveness-only criterion. Four BMP
captures are retained. The English guest hash is
`633C1328922B4092351E0019B1BA1C6D56C0F0FEA83824CC8B7E2E731B15E80B`.
This proves those gameplay/close operations, not comprehensive WOW rendering,
fonts, every game action, SOL or WRITE functionality. Prior-P and historical
three-application frontier reconciliation remains a separate gate.

## Native worker fixture repairs

The lifetime fixture's file variable `log` collided with the CRT declaration
on its AMD64 build; it is renamed `log_file`, with unchanged assertions. The
presentation fixture still expected five records for a text publication now
carried in a single variable-length data record. It now requires exactly four:
rejected barrier, VIDEO_BEGIN, VIDEO_DATA, successful explicit barrier. The
peer additionally verifies that exact operation order; it does not tolerate
implicit activation, extra records or rejected error propagation.

Run r079 passes presentation (530 checks) and input return (689 checks), but
its lifetime run fails because the standalone build directory lacks required
sibling Hook DLLs. `execution.c` actually calls the production suspended-child
installer; these are dependencies, not optional mocks. Run r081 stages the
same fixture executable with released Hook32/64 and run16, preserving all
assertions, and passes 1084 checks with zero remaining handle growth.

The native Worker generator now includes the existing presentation target.
`verify-native-worker-fixtures.ps1` stages and hashes those exact dependencies
and runs all three fixtures on test-private Consoles. Run r083 builds through
the official generator and passes all three. r076/r078 fail before compilation
on a Windows PowerShell path/module environment mismatch; r080 fixes only the
runner environment and successfully builds. No production behavior was changed
to obtain these fixture passes.

## Independent WOW comparison with the preceding committed package

Run r084 archives the ten release images from HEAD
`643d292a45c12a696df5e3cde5cce89644bcf547` without changing the worktree, overlays
them on the same clean guest/configuration package, and runs each of WINMINE,
SOL and WRITE separately before repeating with the r072 candidate. All six
observations completed. Removing only volatile HWND values, the recorded
window-class/visibility/text sets are identical for each pair: WINMINE has its
Minesweeper window, SOL reports `Out of memory`, and WRITE reports `Not enough
memory for Write to complete this operation.` Those two errors remain non-pass
functional outcomes; matching them proves only the retained frontier, not an
absence of WOW defects. No retry-to-success, guest change or diagnostic product
image is involved.

The baseline also passes the same flag/counter/reset/open/close/zero-exit probe
at r084/baseline-gameplay. The historical WINMINE gameplay requirement in
[E53](m0-t422-s2-user-client-lifecycle-ledger.md#e53-user-observed-winmine-gameplay-and-visual-receiver-split)
included actual open cells/flags and already recorded small monochrome drawing.
Current captured gameplay retains those witnessed operations; it does not
claim that the separately queued visual/font limitations are repaired.
The first r082 comparison stopped before running any product because its
restricted PATH lacked Git. r084 pins the actual Git executable and completes;
the failed attempt is preserved, not treated as a product regression.

## Gate signature and governance verification

The aggregate WOW gate previously recognized only the Chinese WINMINE/SOL
signatures. It now also recognizes the observed English images, but still
requires the exact selected baseline signature in the last sample and preserves
the timeout/frontier distinction. Run r088 extracts the actual pattern block
and passes all three r084 baseline/candidate comparisons plus a negative where
an old valid sample is followed by a last sample with only process liveness.
That negative is rejected; old window content cannot satisfy readiness.

Governance checks exposed pre-existing missing Current Work/proposal fields,
an unindexed active evidence record, historical numbered planning mixed into
Queue, and an over-escaped runtime path in the preceding closure evidence.
Those are corrected without changing any candidate row or its order. The
former queue text and its audit-family map are retained verbatim apart from
relative-link relocation in the indexed queue-planning history. Run r088
passes both documentation governance and relative-link checks; r085--r087
retain the earlier failures. Full Control gates, complete historical WOW
frontier reconciliation and final ten-image publication/commit/push still
remain. No all-tests or completed-delivery claim is made.

## Coherent version-444 package and aggregate-gate repairs

Run r089 rebuilds the complete ten-image package with application version
0.0.444 (wire protocol unchanged), the registered blocked-tick guard and the
canonical WOW32 build. Its final build/staging exit is zero; the exact inputs
are in r089/package/product-system32-manifest.json. Earlier failed build
attempts retain their logs; they are not passing builds. This candidate has
not replaced the published ten-image set.

The initial r091 Control run exposed an observation-staging mismatch:
startup-rejection fixtures intentionally require a build-owned executable
path before creating adversarial peers, while the aggregate runner moved them
to its short O:/tmp package. verify-s7-rpc-fixtures.ps1 now stages only the
fixture and exact NTSRV input under a fresh build-owned System32 directory,
checks both hashes, and retains the C-side safety guard. Run r092 passes all
seven RPC cases, six native-GUI cases and five version-negative cases. It then
fails the strict DIR prompt observation. The proposed terminal observer repair
uses ReadConsoleOutputW character cells instead of dividing a variable-width
character stream into fixed-width rows. All prompt, summary, cursor and dirty
tail assertions remain; r095 is its separate runtime verification.

Run r093 retains all three WOW frontiers but fails the first ordinary DOS
input milestone: the current row ends in `NC4E>exitut`. The test has typed
`exit` and not yet sent Enter. The extra `ut` comes from the SHORT_HISTORY
fixture's seeded `prior shell output`; that environment variable leaked from
the preceding WOW observation into the ordinary command matrices. The
aggregate runner now ends that fixture before those matrices, without changing
the strict echo predicate or its negative tests. Strict DIR remains a separate
dirty-screen gate; this change is not evidence that arbitrary seeded-screen
residue is repaired in the product.

Run r094, using identical r089 binaries and the unchanged r050 input observer,
passes Console 17, Window 17 and all three retained WOW frontier comparisons.
Timings are 52,627 ms, 48,656 ms and 66,712 ms respectively; aggregate product
time is 172,262 ms. This separates the harness-fixture leak from a COMMAND/VER
product regression, consistent with the owner's deployed-package observation.

The following r094 Minesweeper gameplay check fails reset against its initial
reference. Its saved `mine-before.bmp` is uniformly grey, before board paint;
the reset cell hash d3f62a0f instead equals the actual covered-cell hash in the
passing r084 preceding-package gameplay. A visible restored HWND and a WM_NULL
reply do not prove WM_PAINT completion. The probe now requires patterned cell
and counter pixels, equal neighbouring covered cells and two agreeing
observations before arming the first operation. It retains all flag/counter,
F2, open-cell, close and launcher-result assertions. No product or guest code
changes are involved. Subsequent runs must verify this repair; r094 gameplay
itself remains a failure, not a functional pass.

## r095--r106: complete package gate and remaining frontend fixture

r095 proves the corrected cell-grid observer passes strict DIR (12,264 ms).
Its eleventh native interactive relaunch instead reaches a real worker error:
input error 1237 (`ERROR_RETRY`) followed by launcher 1067. NTVWM had already
dequeued keyboard records before requiring an output snapshot to stay stable
across scrolling. A viewport change therefore terminated input processing.
The optional READ_TEXT_CONFIGURATION error 1168 is not this fatal cause.

The project-owned `ntvwm_presentation_input` now reads geometry only for a
mouse record, once per batch; keyboard delivery requires no output snapshot.
Mouse geometry uses one Console info observation, not a stable pixel capture.
Input order, real errors and partial-write failures remain asserted. The new
`verify-native-input-geometry.ps1` compiles the actual function with controlled
boundary failures: repaired r098 passes; the HEAD function in r100 fails the
same assertions as expected. This is a deterministic fixture, supplemented by
r099's three independent twelve-round real interactive CMD runs, all passing.
No first-bad COMMAND/VER commit is established by these results.

r096 validates the corrected Minesweeper readiness probe: actual flag/counter,
reset, open-cell, close and launcher-zero assertions pass. Native fixtures and
29 service fixtures also pass. r099 subsequently stops at an obsolete
build-directory-only test guard, before starting that scenario. Management and
broker-retirement tests now use the shared isolated-package ownership scope;
r101 passes selected-worker close, independent Console survival, both worker
types' frontend-death receipts, nested Window handoff and native fixtures.

r102 runs `Invoke-ProductVerification.ps1 -Suite Full` using the coherent r098
package (only NTVWM differs from sealed r089), then Minesweeper gameplay,
native presentation/input-return/lifetime, and service29. Retained owner is
PID 15780, start 2026-10-11 20:07:11 -07:00, deadline 900 seconds. The actual
owned process terminates with exit 0. Full selected gates take 344,602 ms,
including Console17, Window17, three WOW frontier comparisons and all control
gates; service29 takes 3,446 ms. SOL/WRITE's recorded error frontiers remain
non-functional outcomes, not full WOW success. Package manifests, exact
arguments, per-gate timing and output are retained under r102.

Additional frontend coverage is separate. r103 is not-started under the
restricted environment; r104's first three fixtures pass, but its full-fault
entry lacks the documented private-desktop font setup. r105 uses the correct
`--private-desktop-full` entry, passes allocation/revision/projection-failure
cases, then times out waiting for a reply to a non-final video chunk. Current
production deliberately replies only to the final chunk. The fixture now
sends both chunks, asserts no premature reply, and reads the final result.
This is a test contract repair, not a production protocol change.

r106 builds that fixture in a fresh build root and passes the prior timeout,
EOF rollback, sole-channel ownership/input isolation and stable handle counts.
It subsequently fails parked reacquisition at console_channel_lifetime_test.c
line 1354. The result remains failed and unclassified; publication/closure
must not treat the aggregate gate as proof that this additional case passed.

## r107--r112: classify and repair native intermediate viewport bounds

r107's diagnostic runner has a parsing error before build/test; despite its
wrapper's zero exit, stderr makes this explicitly not-started, never a pass.
r108 records successful parked binding (`bind=0`) with a 120x60 canvas and
viewport 0,36--119,59; the following explicit text conversion fails. r109's
test-only API wrapper identifies the exact rejected intermediate window:
0,0--119,23, while `dwMaximumWindowSize` is 106x46. Error is 87.

The production `prepare_logical_surface` already distinguishes logical viewport
from native viewport, but its intermediate shrink bounded only current and
logical dimensions, not the native maximum. It now also bounds each axis by
`dwMaximumWindowSize`. Logical storage, copied cells and the worker's requested
dimensions are unchanged. No mirror, worker discrimination, channel protocol,
connection ownership or lifecycle policy changes. Diagnostic wrappers were
removed after retaining r109's output.

r110 passes reacquisition and reaches teardown, where the fixture itself
attempts a one-row shrink retaining the same over-limit width. The fixture now
chooses a legal new native viewport and still asserts exact viewport/cursor
preservation plus original cursor-shape restoration. r111 builds the final
non-diagnostic frontend and passes the entire `--private-desktop-full` fault
fixture, actual owned runner exit 0. This includes 102 channel lifetimes,
allocation/revision/projection failures, EOF rollback, sole-owner isolation,
park/resume input mode, cancellation and stable process handle counts.

r112 stages the new frontend with the r098 worker and the other eight unchanged
r089 images, then runs frontend palette/video/dispatch fixtures and the complete
serial ProductVerification Full, WOW gameplay, native and service gates. Its
background owner, command, deadline and logs are retained under r112. Its
three frontend fixtures pass; governance then rejects an escaped path in this
evidence document, before product testing starts. The notation is corrected;
r113 resumes governance and product tests using the unchanged r112 package.
The product gate is in progress; r102's passing package is not substituted
for this changed package's required evidence. Publication remains pending.

## r114--r116: palette-only updates must preserve geometry

Closure review adds a missing invariant to frontend_text_handoff_test: after a
configuration-only palette change, buffer dimensions, viewport and cursor must
match the pre-update frame. r114 fails this assertion (49 checks, one failure)
with otherwise passing colours. `SetConsoleScreenBufferInfoEx` consumes
exclusive right/bottom boundaries unlike its getter; NTVWM's existing
set_hidden_window/size code already accounts for that contract, but the new
NTCON palette writer did not. The minimal fix increments the returned right
and bottom before the setter, without changing storage or requested geometry.

r115 rebuilds the non-diagnostic frontend: 49 palette assertions pass and the
entire private-desktop frame-failure/lifetime suite exits zero. r114 remains
the negative witness; no product output deduplication, worker-type branch or
protocol change is introduced. r116 waits for the exact retained r113 process
identity before staging this new frontend with the unchanged nine images and
running the full gate. This preserves serial use of the global BaseSrv endpoint.
Its deadline includes the prior-run wait; publication remains pending until
the final package itself passes. No result for r116 is claimed yet.

r113 subsequently completes with actual runner exit 0: Full takes 368,549 ms,
followed by successful WINMINE gameplay and native fixtures; service29 takes
3,407 ms. r116 observes that process's actual termination and starts its own
serial run; staging, three frontend fixtures and documentation governance
pass. Its product/WOW gates remain live at this update. The ten-file-only
publication procedure is prepared at r117 but has not run; it requires the
final gate's success, retains separate prior deployed/repository backups and
verifies each file against the tested manifest before updating release identity.

## Delivery audit before final publication

| Requirement | Authoritative check / current evidence | Status at audit |
| --- | --- | --- |
| Actual indexed palette, handoff, configuration-only change and caller restoration | frontend-text-handoff-test, r115: 49 assertions, including unchanged geometry/cursor on palette-only update | Pass |
| Allocation/revision/projection failures, final frame, EOF and ownership/lifetime | console-frame-failure-test --private-desktop-full, r115: actual production dispatcher, real pipes/Console APIs and 102 lifetimes | Pass |
| Keyboard input independent of a scrolling output snapshot | verify-native-input-geometry.ps1, r098 positive and r100 old-function negative; r099 three independent 12-round real runs | Pass |
| Late video tick retains unpublished state while COMMAND is blocked | verify-blocked-video-tick.ps1, r071; r065 real EDIT/MEM repetitions and subsequent Console/Window matrices | Pass, final-package matrix ongoing |
| DOS Console and Window interaction, output, exit and cleanup | Invoke-ProductVerification.ps1 -Suite Full, r116 Console17/Window17 | Inspect final aggregate before delivery |
| WOW accepted application milestones | Independent WINMINE/SOL/WRITE comparisons to r084 preceding-P/historical evidence; actual WINMINE flag/counter/reset/open/close through verify-wow-mine-gameplay.ps1 | r116 frontiers pass; final-package gameplay pending |
| RPC, launch failures, versions, native GUI/CUI, ownership and retirement | r116 Full control gates plus verify-native-worker-fixtures and verify-service-fixtures | Ongoing |
| Mirror diff and documentation | r118 compares the sole changed mirror nt_graph.c bytewise and newline-normalized against original OpenNT; both differ semantically, not formatting alone. DIV-314 registered. Governance, links and diff check exit zero | Pass |
| Coherent ten-image delivery and clean pushed change | r117 publication procedure plus final Git/identity review | Not yet executed |

The WOW contract retains the owner's prior boundary: WINMINE must reach a
usable interface (this packet additionally exercises gameplay); SOL/WRITE must
not regress below their separately evidenced deepest milestones. Their existing
memory-error dialogs are not relabelled successful application operation.
Queued WOW capability work is not silently treated as implemented by this gate.

## Final verified delivery

r116 completes with actual owned runner exit 0. Its Full gate takes 347,227 ms
(preparation 2,187 ms); every recorded gate has Passed=true. The same package
then passes real WINMINE gameplay, native presentation/input-return/lifetime,
and all 29 service fixture cases (3,374 ms). This supersedes the pending rows
in the pre-publication audit above; earlier failed attempts remain evidence.
The actual gameplay witness changes the flag cell and mine counter, restores
the covered-cell hash on F2, changes a cell on left click, closes the owned
game window and receives launcher exit zero.

r117 publishes exactly the ten files from r116's tested manifest to both
assets/release and O:/winnt/system32 and verifies each SHA-256 at all three
locations. Prior repository and deployed copies are recoverable in r117's
previous-assets and previous-system32 directories; unrelated runtime files,
guest binaries and configuration are not copied or changed. APP_VERSION is
0.0.444 and protocol version remains 45. Publication process exits zero.

r119 additionally runs the actual deployed run16 on an owned private Console
for interactive COMMAND and native CMD, independently typing VER then EXIT.
Both show version output and return zero; only processes created by this smoke
under the exact deployed component paths are explicitly cleaned. This uses
the recorded bounded observer environment, not an assertion that the known
original guest large-environment defect has been repaired.

Final scope review retains one mirror insertion (the registered DIV-314
blocked-state guard) and project-owned frontend/worker fixes, with no guest,
protocol, NTSRV lifecycle or worker-type rendering redesign. Observer repairs
preserve output, order, completion, fault and cleanup assertions; they repair
stale framing, screenshot readiness, fixture environment and character-grid
observation contracts rather than treating failures as success. Publication is
complete; S1 remains awaiting the owner's manual acceptance, without admitting S2.

r120 performs the final completion audit: it requires all 14 named Full gates,
all 29 service cases, 49 palette assertions, the full fault/lifetime receipt,
actual WOW gameplay, successful publication/smoke runs and all ten three-way
hash identities. Documentation governance, relative links and diff check pass;
the actual audit process exits zero. This is the bounded current delivery/test
contract, not a claim that all queued future WOW capabilities are implemented.

## Owner acceptance

On 2026-10-11 the owner confirmed “验收通过，准入S2”. S1 is closed at
`fb7650ce0e449e58732ea7e7d4bc3d442aeab779`, pushed to main with a clean
worktree before admission. The accepted ten-image release remains 0.0.444,
protocol 45; the documented guest/WOW limitations remain explicit. CURRENT
now admits S2 capability discovery, not S3 or a new terminal-query responder.
