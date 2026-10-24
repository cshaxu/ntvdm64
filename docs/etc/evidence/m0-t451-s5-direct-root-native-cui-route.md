# M0 T451 S5 — direct-root native CUI route

## Scope and source finding

This delivery repairs the common direct-root native-CUI delivery failure behind
`run16 command -> VER`, NTMON and the installed native `EDIT.EXE`.  It does
not special-case a shell token, alter COMMAND.COM/BOP/BaseCheckVDM, or modify
guest media.

The original `BaseCheckVDM` path gives a direct DOS root a distinct,
broker-authenticated execution Console.  NTSRV already records the relation
between that context and the visible frontend root in
`service_root_console_matches`.  `service_take_native_command`, however,
discarded that proof and required raw `frontend->console == caller->console`
at delivery time.  The NTVWM-CUI worker consequently received no queued native
command.  It is a delivery-time adapter defect, not a short-lived-command or
final-render defect.

The repair keeps the existing authenticated relation: delivery admits the
visible root Console itself or a broker-issued context belonging to that root;
an unissued/foreign locator remains rejected.  Host Console membership is
still required and `BindNativeTarget` retains its machine/subsystem check.

## Final-frame candidate

The pre-existing final native-CUI release candidate is retained as a separate
ordering improvement.  It adds a versioned `CONSOLE_IO_PRESENT_BARRIER` used
only after NTVWM's final capture.  NTCON's presentation thread, not the
producer, advances the exact `(video pointer, serial)` commit frontier.  A
replacement frame, stop or thread loss fails the barrier; ordinary publication
remains asynchronous.  This does not substitute for the NTSRV delivery repair
above.

## Build and focused evidence

| Check | Result |
| --- | --- |
| x64 service/frontend/worker/launcher/monitor/hook and x86 worker/WOW32 Ninja graph | Passed; the x86 worker safely rebuilt its invalidated Ninja log and linked current `ntvdm.exe`; x64 `ntsrv.exe` relinked from the repaired source. |
| `basesrv-service-reservation-test.exe --native-command-distinct-context` | Passed: an issued distinct context delivers through the actual `service_take_native_command` routine, while a foreign locator returns `ERROR_PIPE_NOT_CONNECTED`. |
| `console-channel-lifetime-test.exe --present-barrier` | Passed: the complete final native frame reaches the production presentation commit frontier before the peer receives the reply. |
| coherent stage | Passed: `build/t451-s5/package/product-system32-manifest.json` verifies all ten explicitly selected images against the immutable extracted `assets/winnt.zip` baseline. |

## Superseded runtime limitation

An earlier restricted agent-token invocation exposed no interactive desktop.
Both the new direct-root observer and the existing private-desktop RPC observer
therefore failed before a visible NTCOn window or Console snapshot existed.
These are retained as historical failed attempts under
`build/t451-s5/direct-ver.txt` and `build/t451-s5/control-r2/`; they are not
claimed as successful guest acceptance.  The product-verification driver also
contained stale removed fixture labels (`startup-timeout`,
`workerless-grace/cancel`); S5 removes them so the current fixture's declared
contract is the single authority.  Its subsequent desktop-unavailable failure
is distinct from that repaired invocation defect.

That limitation does not apply to the documented modern PowerShell
desktop-capable token.  The rule is now explicit in
[Execution](../../rules/EXECUTION.md#private-desktop-test-token): a
`result=not-started` record from a restricted executor is a launch error, not a
desktop or product verdict.

## Terminal-root retirement correction

The direct-route work exposed an adjacent lifecycle regression after a native
and DOS carrier were used in one Console root.  The first terminal-root draft
let *any* retained child launcher start the root's terminal retirement.  Thus
an inner `MEM` completion could mark the root terminal while its outer
`cmd.exe`/NTVWM carrier was still alive; later `exit` could not finish the
normal NTSRV-coordinated handoff.

NTSRV now admits this transaction only from the root creator.  A nested Run16
still retains the root for authenticated routing, but its completion has no
authority to retire it.  Once the creator completes, NTSRV waits for active
carrier work to finish, sends the normal worker-close instruction to an idle
NTVWM-CUI carrier, and advances to the existing frontend return only from that
carrier's actual worker-watch exit.  Run16, NTCON and workers acquire no new
direct lifecycle link; guest files remain unchanged.

| Check | Result |
| --- | --- |
| x64 actual service fixture, desktop-capable modern `pwsh`, 40 cases | Passed in 6975 ms. The new shared-carrier cases prove an inner completion leaves the root admissible, and both native-root/DOS-root terminal completion wait for real idle-carrier exit. |
| Short physical-stage `COMMAND → installed native EDIT → CMD → MEM → exit` | Passed. It proves real native-to-DOS return, visible EDIT/CMD/MEM markers and root launcher completion. |
| Short physical-stage `CMD → nested COMMAND/DOS → parent return → exit` | Passed. It proves the inverse route's actual input/output and direct exit 23. |

The private-desktop cases use the checked runner under the configured modern
`pwsh.exe`; the short stage is the checked `New-PhysicalRuntimeStage` helper,
not a drive alias.  No desktop automation was used.

## Publication

The final cached full-component run at
`build/t451-s5/terminal-retire-r13/full-component-build/result.json` passed:
all ten selected images built from the stable Ninja trees, all 1,488 checked
intermediates remained unchanged on the second build, and the final
`ntsrv.exe` SHA-256 is
`46E153D87EC815A9F5609B5BED79458F46DED3669AAAA0A5515F2692E3A7D61F`.
The final private-desktop product probes at
`build/t451-s5/terminal-retire-r13/final-probes.result.json` passed against
that package before publication.

The staged ten images were copied, after recoverable prior copies were made,
to both `assets/release` and `O:\winnt\system32`.  Candidate, release and
installed SHA-256 values were compared entry by entry in
`build/t451-s5/terminal-retire-r13/publication/published-manifest.json`.
`assets/winnt.zip` and all guest files remain unchanged.

The final product probes prove both cross-carrier directions through real
Console input/output; remaining owner acceptance is visual/usability evidence,
not a substitute for those lifecycle assertions.

## Root Run16 exit regression

The former root-only hang was not a COMMAND, `VER`, `MEM`, or shell-token
failure.  It was an NTSRV retirement ordering error.  Once the creator's root
transaction completed, `service_advance_terminal_root_retire` treated only an
idle NTVWM-CUI carrier as closeable.  A live DOS carrier was therefore left
open.  In the inverse shape, it final-closed the frontend before the creator's
already-defined `WaitFrontendConsoleRestored` exchange.  NTCON then took its
final-close path rather than PARKing and acknowledging restored Console mode,
leaving the root launcher waiting indefinitely.

The service now owns both actions consistently:

1. only the root creator can begin terminal retirement;
2. the service sends `WORKER_CLOSE_WORKER` to either textual carrier kind
   (`DOS` or `NATIVE`/NTVWM-CUI); WOW and GUI-launcher observations are not
   Console carriers;
3. after their observed exit, it clears the root retirement transaction but
   preserves the frontend lease; and
4. the creator's existing restoration RPC lets NTCON PARK and acknowledge
   Console return before Run16 exits.

No component gains a new direct lifecycle link; no guest file, BOP, COMMAND
or BaseCheckVDM logic changes.

`tests/observation/verify-root-carrier-exit.ps1` is the fixed executable
regression.  It uses a physical isolated package, a clean deployment-like
environment, and result-gated input—not typeahead.  Against
`build/t451-s5/root-exit-r12/package`, all of the following passed:

| Root form | Command | Result required |
| --- | --- | --- |
| `run16 command` | `VER`, then `EXIT` | visible host `VER`; direct launcher exits 0 |
| `run16 command` | `MEM`, then `EXIT` | visible MEM result; direct launcher exits 1 (guest MEM's documented result) |
| `run16 cmd` | `VER`, then `EXIT` | visible host `VER`; direct launcher exits 0 |
| `run16 cmd` | `MEM`, then `EXIT` | visible MEM result; direct launcher exits 0 |
| `cmd → run16 command` | `VER`/`MEM`, then inner and outer `EXIT` | visible result; outer launcher exits 23 |
| `cmd → run16 cmd` | `VER`/`MEM`, then inner and outer `EXIT` | visible result; outer launcher exits 23 |

The service fixture also passed after rebuilding the affected x64 service
target.  It now asserts the mixed DOS/NTVWM-CUI case directly and verifies
that terminal rundown preserves—not prematurely closes—the frontend lease.

The tested ten-image package was published after recoverable copies to both
required destinations.  `build/t451-s5/root-exit-r12/publication/
published-manifest.json` records entry-by-entry equality.  The new `ntsrv.exe`
hash is `3FE8AE82825DB4EC747063E4975FAF326FA5D31A397676B3E60C40B652C87872`.
`ntvdm.exe` and `ntvwm.exe` were refreshed as part of the required coherent
package, not because this repair changes their source.  `assets/winnt.zip`
and every guest file remain unchanged.

## 2026-10-24 revalidation — acceptance remains open

The owner subsequently reported `run16 command` and `run16 cmd → command`
hangs. Earlier root-return claims above must not be read as proof of the
Explorer/sole-initial-Console-member branch: inspection of
`console_startup_observer.c` shows that it creates the product with flags zero
while the observer owns the Console. The prior eight rows therefore establish
borrowed-Console behavior, not that distinct direct-root branch.

The direct observer now waits for the exact staged NTCON process before
attaching to the newly created Console. Run16 takes its initial membership
snapshot before requesting that process, so the observer cannot change that
decision. It accepts either initial carrier and does not require the service
to be a newly created child. It counts newly produced output markers to avoid
accepting the native CMD startup banner as the result of `VER`.

Evidence root: `build/t451-s5/revalidate-r1/`, owned by this S. The first
`direct-baseline.ps1` execution used the sealed complete package matching
`assets/release/release-manifest.json` (Run16 SHA-256
`33F16671D41937D3F576A32710A2C7EB17870293AA42D5AAB4B0072FC3087547`).
It ran under the desktop-capable modern PowerShell token, created an unswitched
private desktop and a fresh product Console, and reported:

```text
attached=1 attach-error=0 initial-ready=1 submitted=1
console-observed=1 observed=1
exit=0
observer-exit=0
```

This is one passing `COMMAND → VER → EXIT` control, not a claim that the
reported failure is resolved. It cannot prove Terminal/ConPTY behavior.

The preceding unverified Run16-only deployment was also found to disagree
with the committed ten-image manifest. `restore-coherent-baseline.ps1` first
validated all ten sealed source hashes, rejected any live installed project
process, and backed up both destinations. It then restored the complete
manifest set to `assets/release` and `O:\winnt\system32` and checked all twenty
destination hashes (`coherent-baseline-restored.json`). This is recovery of
the recorded baseline, not a new fix release. Guest files were not copied or
changed. Speculative RPC bootstrap edits lacking failure evidence were removed.

The first complete cached build in `components/result.json` passed for all
ten images; its second pass preserved all 1,488 intermediate files and output
hashes/timestamps. That run predates the following candidate correction and
is not final acceptance evidence for it:

- The optional-HWND candidate passed zero from Run16 but NTCON's shared hex
  parser rejected zero before frontend registration. This is a deterministic
  candidate defect, separate from the owner's original hang report.
- Window metadata now accepts zero; inherited process/event resources still
  reject zero, malformed strings and overflow. The argument fixture verifies
  this distinction. The authenticated service fixture adds absent/different
  HWND cases while retaining wrong-capability/generation rejection.
- The real bootstrap fixture requests absent window metadata and must still
  receive an authenticated frontend created by NTSRV. A forged resource is
  not accepted by weakening the metadata parser.

The subsequent `components-fixed/result.json` passed all ten images and
preserved all 1,488 intermediates on the second build. The candidate is
staged, not published. Its argument parser fixture passed 52 checks; the
actual-service suite passed all 40 cases (`service-all.stdout.txt`).

`direct-results.json` records six passing independent-Console cases, including
both VER/MEM orders for CMD and COMMAND and `cmd /d /c command`. The revised
`eight.stdout.txt` additionally passes the owner's four true-root cases and
four inherited-CMD controls. Root rows use the separate direct observer;
all require both new command results and actual launcher termination.
`dos-regression.stdout.txt` records all 17 ordinary Console and all 17 window
cases, including stream/redirection and EOF checks. These are distinct
evidence scopes, not a claim that all product gates have passed.

The borrowed-Console identity-loss fixture needed an explicit live process
handle and distinct Console-close/worker-close events: the old synthetic
watch no longer modeled a live carrier. The revised fixture checks that
Console-close is signaled while worker-close remains unsignaled. No product
shutdown rule was weakened to make this fixture pass.

Actual RPC controls now pass all six selected cases: client, bootstrap,
startup-rejections, native-worker-failure, native-completed-worker-loss and
monitor-rpc (`control-rpc.stdout.txt`). Bootstrap uses absent HWND metadata.

The next lifecycle batch stopped before EDIT: the existing modern-EDIT
script did not observe COMMAND's initial prompt and delivered no input.
Its report is `control-modern-edit.txt`; the same short-history invocation
against the sealed baseline also fails (`edit-baseline-history.txt`). A
candidate without short-history setup likewise fails
(`edit-candidate-normal.txt`). Thus geometry alone is not a demonstrated
cause, and neither this comparison nor the earlier bare-COMMAND passes
waives the failure. A single-variable follow-up now separates it from target
path spelling: `path-bare.txt` and `path-absolute.txt` both fail before the
prompt with the inherited 2,457-character developer PATH; changing only the
test child's PATH to its staged System32 and host System32 makes both
`short-path-bare.txt` and `short-path-absolute.txt` show ready=yes and exit zero.
This matches the already-controlled ordinary matrix's environment boundary
and the retained immutable COMMAND large-environment TODO; it does not fix
that guest defect. The EDIT fixture and control-suite runner now explicitly
use the same limited test lookup environment, with no product environment
truncation. The renewed `control-lifecycle-r3` passes actual EDIT Ctrl+Q,
CMD echo and DOS MEM return; same-Console relaunch; 12 rapid mixed relaunch
pairs; 12 interactive CMD relaunches; independent-Console management; and
both DOS/native worker-loss and frontend-loss retirement paths.

The nested-window run completed its command/exit assertions and observed
`caf-visible-window=1`, but exposed a stale test report contract: the observer
printed an unrelated last-error 203 on success and the verifier still expected
the old line without a stage field. Success now reports zero and the verifier
requires the explicit `verify-owner` stage. The GUI gate's first three rows
passed before its test helper tried resolving pwsh through the deliberately
limited guest PATH; it now uses the current PowerShell installation directly.
Both fixes are test-only and require fresh runs (`control-followup`).

Current-source window-reader observations also pass all four SOL and WRITE
startup samples (`wow-frontiers.pass.txt`). These retain S4's startup-only
boundary and do not claim SOL Options or complete gameplay acceptance.

The current-source ConPTY observer now has a stable Ninja target in
`Generate-ObservationNinja.mjs`; its x64 build passed, rather than relying
on an old copied test executable. The remaining full regression, coherent
candidate publication, commit and push remain pending; no S5 closure is claimed.

### Final-artifact verification boundary

The corrected CAF probe passed actual nested-window CMD/DOS input, parent
return and launcher exit 23. GUI startup, explicit wait, GUI-spawned text,
one-shot carrier residency and text/GUI/text return passed; the fresh ConPTY
observer passed repeated DIR with `final=0 entered-dos=1 dirty-prompt=0`.
The separate current-candidate WINMINE probe passed flag/counter, reset,
open-cell painting, WM_CLOSE and normal launcher completion.

The next version-negative gate stopped before compilation because
`APP_VERSION` still read `0.0.448`. The shared header explicitly requires the
admitted T version; it now reads `0.0.451`, with protocol major unchanged.
This metadata correction invalidates affected artifact identity, not the
above diagnosis. Final coherent build and repeated acceptance therefore use
a fresh `build/t451-s5/revalidate-r2/` package. Earlier passing r1 reports
must not be substituted for final-package results.

Source review finds the implementation confined to project-owned frontend
admission, NTCON argument/bootstrap handling, Run16 host observation and
shared metadata. No original mirror source or guest binary is changed by
this delta; original-mirror diff increase is zero. The service keeps HWND
as optional lookup metadata, authenticates the created process and event
objects, and clears explicit registration state on both failure rollbacks.
The registered frontend prevents idle retirement before worker admission.
These review conclusions supplement, rather than replace, the positive,
negative and lifecycle fixtures. Documentation governance and whitespace
checks passed before final delivery-record updates; those updates require
the gate to be rerun.

### Final negative checks and bounded follow-up (2026-10-24)

The r2 package passed the full Control selection, both 17-case DOS matrices,
the corrected eight-case root/inherited matrix and the independent WOW
frontiers. Its supplementary real Console Ctrl+C case returned 1067 before
VER/MEM. Source inspection found `controls_live` is never assigned and the
two NTCON control events are never consumed, including in older revision
`0e1f3f501`. The callback therefore declines the event, allowing default
process termination. This is an existing host-side disconnect, not guest
behavior or proof that the main root-return correction failed.

The bounded candidate removes the dead enable state, wakes the existing
presentation loop for control events and serializes canonical key records
onto the existing input queue. No RPC, lifecycle command, guest or timeout
is added. Parked or returned presentation discards these gestures rather
than delivering them to a later owner. Real Ctrl+C/Break and full-package
regression remain required; implementation is not yet a verified repair.
The complete cached build and focused control runner is
`build/t451-s5/revalidate-r1/verify-control-repair.ps1`, PID 21648,
with fresh immutable candidate/evidence under `revalidate-r3`.

The r2 redirection run produced the correct redirect/append/pipe/VER files
and outer CMD exit 23, but its wrapper expired before the observer returned.
The observer's `write_console_input_text` sleeps 100 ms per character in
addition to line pacing; the wrapper's 55 seconds omitted that input cost.
The corrected diagnostic budget includes character count, five paced lines
and the target's existing observation budget. The old run is not counted
as passing. This changes only a test budget, never product timing policy.

### Owner root-exit rejection and retained Console (2026-10-24)

The owner rejected all four standalone mixed chains and additionally reported
`run16 cmd → exit`. Treat the root acceptance rows as open. A detached-before-
wait probe captured one r3 Run16 STILL_ACTIVE result; subsequent cold runs
returned zero, so that single wait failure is not yet fully attributed.
More importantly, the successful run in `revalidate-r3/root-exit-stacks`
still retained NTCON and NTSRV after Run16 returned. Thus launcher exit alone
was an insufficient oracle for the user-visible root Console lifetime.

Source: `OpenNtBaseServiceFrontendLeaseReady` acknowledged restoration and
unconditionally made the frontend idle. `service_advance_terminal_root_retire`
correctly delayed frontend destruction until carrier exit/Console return,
but no subsequent final-close decision existed for a non-borrowed root.
The r4 candidate preserves that ordering, disallows PARK during terminal
carrier retirement, and after restoration selects idle only for borrowed
Consoles; a self-created Console instead receives service-owned final close.
No new control protocol, timer or guest change is involved.

The checked-in direct probe now excludes frontend processes older than its
launcher, detaches before waiting for return, and requires NTCON process
exit as well as launcher exit. The matrix adds independent CMD/COMMAND
EXIT-only cases without removing the eight requested mixed chains. A service
fixture asserts borrowed-idle versus self-created-final-close after the same
restoration acknowledgment. Cached full build runner PID 38288 owns r4;
none of these candidate changes have been published or declared passing.

The r4 service40 fixture passed and independent CMD EXIT now proves both
Run16 and NTCON exit. Independent COMMAND EXIT remained open. Its retained
failure in `revalidate-r4/root-command-stacks` contains Run16 PID 39232,
NTSRV 15660 and NTCON 29416, with no remaining NTVDM. The NTSRV frame at
module RVA 0x983c maps into `OpenNtBaseServiceWaitFrontendConsoleRestored`
(start RVA 0x95f0), called by `Server_WaitFrontendConsoleRestored`.

Source closure: `service_vdm_process_completed` called root retirement before
signalling `parent_wait`. Retirement's pending-work check therefore observed
its own unfinished receipt. If worker-watch rundown had already executed,
there was no subsequent advancement after the event was set. The correction
publishes the exact completion event before root retirement under the same
service lock. Neither callback order can now strand this completion behind
its own unsignalled event. This explanation is source-backed; final repeated
runtime verification remains required.

The r4 real Ctrl+C test also improved from 1067 to normal task exit but showed
`cver`: DOS needs actual Ctrl scan-key transitions, not only native Console
modifier annotations. The candidate emits Ctrl-down, key-down/up, Ctrl-up
through the same existing input queue. The channel fixture checks all four
records; real DOS Ctrl+C/Break must still prove VER/MEM and normal return.
Full cached r5 build plus focused validation is owned by runner PID 13784.

### Final r5 regression reconciliation

The r5 candidate is version 0.0.451 with unchanged protocol 65. The complete
ten-image cached build passed and a second build preserved 1489 intermediate
hashes. Service40 and the ten root/inherited cases passed: all eight mixed
chains plus independent CMD and COMMAND EXIT-only. Each standalone case
waits for a newly created frontend, detaches the observer before retirement,
and requires both launcher and frontend process exit. This supersedes the
earlier launcher-only oracle, not the owner's rejected installed result.

The same candidate passed all eleven Control gates, both seventeen-row DOS
Console/Window matrices, WINMINE interaction and four-sample SOL/WRITE startup
frontiers. These frontiers do not certify SOL Options or complete editing.
Actual Ctrl+C and Break preserve DOS VER/MEM and return; a separate real
Console fixture verifies the four input records delivered to the active
queue. Keeping OS control-dispatch initialization in that separate process
does not relax the broad channel fixture's exact handle-count equality.

The initial supplementary redirection run inherited WINDOW_INPUT from the
preceding DOS Window suite and failed before command submission while looking
for a KVM window belonging to an ordinary host CMD. The checked-in redirection
runner now clears inherited observation-mode variables in its child only.
The fresh `redirection-clean` reports prove `>`, `>>`, `|`, `cmd /c ver`,
file content and outer CMD exit 23. Service-death injection also passed:
Run16 returns ERROR_INVALID_HANDLE and the real carrier/target/descendant
exit before test containment. No product timing policy was changed.

All 71 non-product candidate files match the sealed baseline. No original
mirror or guest file is changed. These fixes are in the existing owned
NTSRV/frontend bindings; no new private USER dependency, control transport,
polling, timeout or independent lifetime owner is introduced. The existing
adapter exception remains the boundary: NTSRV owns identity and retirement;
Console HWND is only observed presentation metadata. The unrelated queued
WOW32 governance edits were reviewed as documentation-only admission gates;
they neither admit implementation nor change queue order.

The complete channel and failure variants also pass, retaining exact handle
counts 427/427 and 424/424 after repeated handoff, binding and failure cases.
No handle-count assertion was waived. The separate control delivery fixture
passes with real Ctrl+C/Break, not merely synthetic queue writes.

Publication preflight and Apply both passed. All ten images were replaced at
both `assets/release` and `O:\winnt\system32`; the twenty SHA-256 values match
the tested candidate and the tracked `assets/release/release-manifest.json`.
Backups and the verification list are retained under
`build/t451-s5/revalidate-r5/publication`. The installed Run16 SHA-256 is
`35834D01F550C8D0B4FD55AAE5FF07C5B3DC0FD7218E34F037E82B769C75A812`;
NTSRV is `18AB4B13F6A4F0BE6C42098B489B1E05A3997C26A754F80D2ADE51EFA5D3148F`.
This is the first publication of the r5 root-lifetime corrections; the earlier
owner rejection applied to the prior installed package. Owner acceptance of
this complete delivery remains pending.

The reviewed delivery was committed and pushed to main as `60367a2b5`.
The production delta is +130/-62 lines, tests/tools +376/-96, and the included
evidence/queue governance documentation +656/-24 before this final receipt.
Original mirror diff remains zero. Final documentation governance and
`git diff --check` passed. This records delivery, not owner acceptance.
