# T425 S9 test throughput audit

## Question and admission

Owner accepts S8 90e57fe09 and admits S9 on 2026-10-03 to reduce per-stage
verification time. Initial P 51f09f7d1 delivers initial S9 work. S9 r022 is published
as a coherent eight-file package with S8 recovery retained. T425 stays open
for owner acceptance. Other-session Queue/proposal changes are excluded.
Owner subsequently continues S9, without closure, to replace hard test waits
with fresh observed milestones and improve keyboard delivery. The continuation
below supersedes the initial pending-acceptance disposition, not its evidence.

## Inventory and retained contracts

| Group | Actual path / contract | Isolation and present wait disposition |
| --- | --- | --- |
| Service fixtures | tests/adapter-basesrv/base_service_reservation_test.c and base_service_fixture.c; production archive registration, capability negatives, release/resume, completion, rundown, WOW late result | In-process service, PID-qualified pipes, private Desktop; safe bounded concurrency, no global RPC endpoint. |
| Retirement policy | Same fixture --frontend-authority and ntsrv lifecycle.c | Two real ten-second waits replaced by explicit time at the existing decision owner; production wrappers still use GetTickCount64. |
| Real retirement wiring | tests/observation/verify-broker-retirement.ps1 | Global endpoint, serial. Still repeats workerless and empty grace; not yet optimized. Must retain real shutdown/receipt acknowledgement separately from clock assertions. |
| Product Console17/Window17 | tools/audit/Verify-CommandExitStatus.ps1 | Global endpoint, serial. Existing output/exit, nested guest, failure assertions retained. No coverage removed. |
| Relaunch and cooked return | verify-frontend-rapid-relaunch.ps1, verify-frontend-rapid-interactive.ps1, verify-frontend-relaunch.ps1 | Global endpoint, serial; retained immediate launch ordering. Fixed input pacing remains under review. |
| Independent sessions and nested handoff | verify-ntvwm-management.ps1, verify-broker-io-handoff.ps1 | Real broker/session separation and final output barriers; retain. |
| Input and producer units | softpc_relative_mouse_queue_test.c, softpc_relative_mouse_bridge_test.c, ntvwm_presentation_test.c | No global endpoint; eligible for parallel isolated execution with source/toolchain identity checks. |
| WOW frontier | observe-wow-frontiers.ps1 | Retain original WINMINE/SOL/WRITE limits and baseline comparison; not gameplay passes. |
| Build/package setup | Existing M0-T424/S2/r001 Ninja cache, S8 r046 package | Reuse only dependency-selected source/toolchain identity. No unconditional full rebuild or unrelated DLL rebuild required. |

No test is deleted. This inventory is not a claim that all groups have already
been optimized or timed. No production deadline, wire version, guest/library,
worker scheduling or frontend behavior change is admitted.

## Implementation and measurements

New tests/observation/verify-service-fixtures.ps1 replaces the disposable S8
serial driver with a tracked bounded runner. It owns each observer, uses exact
process completion rather than pacing sleeps, requires actual zero fixture exit
and PASS assertions, and records hashes, per-case time and total elapsed time.
Parallel cases do not compete for BaseSrv. A CHECK failure's suspended children
are cleaned only by exact test image and owned fixture parent PID.

| Run | Cases / concurrency | Wall time | Result |
| --- | --- | --- | --- |
| S9/r001/service-serial | Original service16 / 1 | 7321 ms | PASS |
| S9/r001/service-parallel | Same service16, same executable hashes / 4 | 2132 ms | PASS; 70.9 percent less wall time. |
| S9/r002/service-parallel | service17 including deterministic frontend authority / 4 | 2709 ms | PASS |
| S9/r003/service-parallel | Repeat service17 / 4 | 2394 ms | PASS |
| S9/r004 and r005 | Added event assertions | Not a pass | Fixture observed capability rather than frontend-state event; early CHECK exit left test children and blocked one relink. Exact two children were removed; test observation corrected. |
| S9/r006/service-parallel | service17 with new-work cancellation and actual state event / 4 | 2884 ms | PASS; authority case 411 ms. |

Policy time inputs are private service_* functions in the existing lifecycle
owner, not a global fake clock or runtime override. Public production wrappers
pass actual monotonic time. The authority fixture verifies no retirement before
expiry, the exact expiry state/notification, new admitted work cancelling old
grace, root-loss shutdown notification and authentic startup failures. Further
rearm/empty-service timing coverage is still required before S9 closure.

Commands (PowerShell 7, private unswitched Desktop):

```powershell
cmd.exe /d /c build\M0-T424\S2\r001\run-ninja-parallel.cmd basesrv-service-reservation-test.exe ntsrv.exe
pwsh -NoProfile -File tests/observation/verify-service-fixtures.ps1 -Observer build/M0-T425/S6/r005/observer.exe -Fixture build/M0-T424/S2/r001/basesrv-service-reservation-test.exe -LogRoot build/M0-T425/S9/r006/service-parallel -Concurrency 4
powershell -ExecutionPolicy Bypass -File tools/governance/Verify-DocumentationGovernance.ps1
git diff --check
```

Build passes with existing historical header warnings. Governance passes after
packet-field spelling correction; diff whitespace check passes. Unrelated Queue
and untracked proposal remain outside this work.

## Initial closure work (historical progress checklist)

- Finish policy cancellation/rearm and empty-service controlled-time coverage.
- Separate real shutdown wiring from repeated grace waits; explicitly clean
  owned test sessions between product scenarios.
- Audit/consolidate other runner setup and meaningful overlapping coverage;
  repeat the selected schedule and check residual processes/handles.
- Update execution guidance only to the proved schedule.
- Retain production-change regression/publication gate for the tiny lifecycle
  time-input refactor; current compiled candidate is not yet published.
- Review, commit/push S9 only after its exit criteria; T425 stays open.

## Subsequent implementation and non-pass controls

This chronological journal retains intermediate pending/failure states. The
final delivery section below is authoritative for S9's completed disposition.

The existing NTSRV empty-service decision now has explicit-time private
functions in idle_policy.h; main.c retains its lock, real waitable timer and
service ownership. The production grace remains exactly 10000 ms. The x86
idle_policy_test passes before/at expiry, Connect cancellation, no deadline
renewal, rearm, occupied service, stopping and overflow. The production archive
fixture additionally covers frontend worker-loss rearm and unbound native
shutdown event delivery without killing its controlled child.

The final service runner retains the original 16 cases and adds authority,
unbound retirement, native command and native worker cases. The last two are
in-process archive fixtures, not users of the global RPC endpoint. S9/r017
passes all service20 assertions four-way in 2274 ms. S9/r015 measured the same
service18 fixture hashes at 8640 ms serial and 3232 ms four-way. Deliberate
runner-failure in r009 fails nonzero and removes its owned suspended child;
the subsequent r010 positive run succeeds. This is failure cleanup evidence,
not a passed product scenario.

Routine retirement checks retain real frontend death, worker shutdown and
1067 direct receipt assertions; repeated ten-second policy waits move to the
deterministic owner tests. -FullDeadlines retains the old real-timer diagnostic.
The GUI runner explicitly cleans its owned package between cases rather than
waiting for ordinary idle grace. S9/r020 passes all five actual GUI cases,
including startup-only return, actual exit 37, fresh text routing, target
survival after launcher release, and text parent resumption. This routine
survival case does not claim real ten-second carrier retirement; that remains
the full-deadline diagnostic. r018 failed because its report directory did not
exist; r019 failed because its long assertion wrapped at the observer's 53
columns. Directory creation and separate short assertion markers correct those
runner errors without discarding actual exit/survival assertions.

S9/r008 full package attempt passes the retained WOW frontiers, Console17 and
the preceding Window cases, but Window EDIT times out. It is not a full pass.
S8/S9 EDIT control in r013 passes both packages with the original observer.
An attempted menu-readiness change in the observer was withdrawn: Window
presentation cannot be validated by waiting for that menu in the visible
Console. r014 and short-path r016 fail before initial COMMAND prompt, not at
EDIT return; r014 contains an environment-setup dialog. Those failures do not
prove an EDIT production regression or a complete long-path root cause.
No acceptance assertion is weakened to hide these results.

Invoke-ProductVerification.ps1 is a new, still-under-validation entrypoint for
serial global-endpoint groups. It checks cache/package hashes and x86 inputs,
records each gate's elapsed time, owns only exact isolated package paths,
cleans them between groups and releases Z: in finally. Parallel in-process
service tests remain separate from this serial group. Its complete coverage
mapping, repeated execution, production publication and closure remain pending.

The first consolidated Control run r021 retains non-pass status. RPC7 passes
in 30031 ms, GUI5 in 8959 ms, all five actual version-negative peers in
63660 ms, and strict DIR in 12264 ms. Modern native EDIT fails before the
initial COMMAND prompt and has an environment-setup dialog; no line-02 snapshot
exists. Total to that failure is 159093 ms, not a full-suite timing. The strict
DIR group's environment variables now restore immediately before subsequent
groups. This correction still requires a fresh complete schedule run. The new
cleanup also revalidates pinned process image identity before killing it.

The idle-policy fixture is now a reproducible basesrv-idle-policy-test.exe
target in the existing Ninja graph and passes from that graph. Regeneration
relinks the service against current binding archives; the older r008 candidate
is therefore no longer the final cache identity. A new coherent candidate is
required before publication. Upper/lowercase guessed WOW DLL target names
were rejected without building anything; WOW32 belongs to its separate build
island, not this graph. These command errors are not successful build evidence.

The formal cache builds run16/ntsrv/ntcon/ntvdm/ntvwm/ntmon/VDMREDIR.dll
successfully after regeneration; unchanged WOW32 retains the S8 build island
input. S9/r022 stages this coherent set. Its short Z: observer and explicit
disposable 80-column Console geometry pass the real modern EDIT return fixture.
That test setup does not resize an inherited user Console. The consolidated
entrypoint uses the same explicit fixture geometry; r024 has passed modern
EDIT after RPC7, GUI5, version negatives and strict DIR, and is still running
remaining controls. No complete-control pass is claimed yet.

S9/r023 measures the same final service20 image at 7435 ms serial and 2160 ms
four-way (70.9 percent less). After explicit fixture mutation lock corrections,
r025 passes service20 again in 2507 ms. Production locks, deadlines, ABI and
retirement authority remain unchanged. Execution guidance now records the
proved concurrency/cleanup/time-input constraints without waiving the existing
runtime publication gate. Documentation governance and diff checks pass.

The r024 consolidated Control run passes ten groups, including cooked return,
12 rapid native/DOS pairs, 12 interactive relaunches, two-session isolation
and both workers' real frontend-loss receipt 1067. Its last nested Window
group times out (247184 ms total to failure); it is not an overall pass.
The standalone r026 stable-title observation returns actual exit 23 and
captures DOS banner, MEM and executed parent echo, but the old final-screen
assertion fails because the earlier banner is no longer on the final CMD page.
The contract does not promise host history. The fixture now requires the DOS
banner at line-01, MEM at line-03 and executed parent echo on the final screen,
plus delivered input, actual exit 23 and actual CAF confirmation. This checks
execution order instead of retaining an old banner indefinitely.

r026/nested-owned passes these ordered assertions. The private observer's
initial Console title is explicitly "NTVDM observation", independent of build
path; this is fixture setup, not a product title fix or proof of a path root
cause. Title-specific fixtures continue setting their own explicit titles.
An earlier disposable inline command suffered PowerShell variable expansion,
so that attempt is non-pass. Its missing Z-alias cleanup left a broker and
four exact children; report PID/parent/creation-time evidence identified them
before recovery. The tracked nested runner now owns both physical and Z: paths,
cleans its pinned image-verified processes before unmapping and restores its
environment in finally. The fresh owned rerun passes and does not leave that
broker. The consolidated Product matrix is running separately in r027;
publication and S9 closure still remain pending.

r027 completes WOW comparison and Console17, then rejects Window nested-MEM
because Merge-ConsoleTextSnapshots cannot prove contiguous output history
across repainted pages. The guest exits with the expected code 1. This is
still a failed run (252088 ms), not a completed Window matrix. The nested-MEM
and MEM-repeat cases already require each MEM result after its own command
in snapshots 3/5/7 and 1/2 respectively. Those strong ordered assertions,
final markers, delivered input and real completion remain unchanged; only
the redundant history-merge prerequisite is omitted for these two cases.
Other cases retain the exact merge/count rules. The unchanged snapshot unit
still passes once-only counting, failed capture, discontinuity rejection and
partial-row cases. A fresh complete Product repeat runs in r029.

r028 additionally compares r027's three independent observations with S8,
S7 and historical S2/S3/S5 frontiers; all 15 comparisons pass at their declared
observation depth, not gameplay. No guest, producer, renderer or mouse code is
modified to cure these test setup/assertion failures.

## Final delivery

r029 passes all Product groups: WOW observation comparison 66249 ms,
Console17 115157 ms, Window17 116018 ms; total 301534 ms with setup/cleanup.
All established cases, including nested/repeated MEM, guest exit7 and EDIT
return to MEM, remain. r030 publishes this identical eight-file set to
O:/winnt with recoverable S8 backups. r031 passes published COMMAND, MEM,
EDIT and native VER in Console and Window and rechecks all eight hashes.

| Contract | Retained S9 evidence / routine entrypoint |
| --- | --- |
| Original service16 and added authority/unbound/native cases | verify-service-fixtures.ps1: r023 same service20 image 7435 ms serial / 2160 ms four-way; r025 repeat20 2507 ms. |
| Exact ten-second decision, cancellation, rearm, notification | basesrv-idle-policy-test.exe and service20 authority/unbound cases; production constant/owner unchanged. |
| Real peer death and direct receipt1067 | r024 retirement-wiring both workers, 8501 ms. |
| Authenticated RPC startup/failure/completed-result/monitor | r024 RPC7, 30414 ms; actual ten-second startup-timeout retained. |
| GUI routing, actual exit and target survival | r024 GUI5 9493 ms, repeated r020; -FullDeadlines retains real idle-timer diagnostic, not claimed by explicit cleanup. |
| Legacy/application/protocol negatives | r024 five real negative peers, 59565 ms; actual 1306 rejection. |
| Strict DIR / modern EDIT / cooked return | r024 12237 / 23800 / 21537 ms; real screens, Ctrl+Q, parent echo/MEM and outer exit19. |
| Rapid12 pairs / interactive12 / independent sessions | r024 8925 / 7516 / 9396 ms; actual output/exit, selected close preserves other session. |
| Nested native/DOS Window | r026/nested-owned: actual CAF, ordered DOS banner/MEM, parent echo and exit23; owned alias cleanup. |
| Product / historical WOW / publication | Complete r029; r028 fifteen frontier comparisons; r030/r031 publication and smoke. |
| Unchanged producer/mouse/cursor units | Accepted S8 ledger and unchanged runtime C/header/test source identities; not a new physical RDP pass. |

No test is deleted. Native-command/native-worker archive cases move from
the serial RPC-labelled driver into service20. Removed routine waits are
test-owned idle-grace cleanup and repeated real-clock policy decisions;
explicit time assertions and actual shutdown wiring remain. Removed history
prerequisites do not synthesize a transcript: each MEM must follow its own
command, and merge/count negatives remain for cases requiring continuity.

The r024 Control invocation is not relabelled as an overall pass: ten groups
pass there, the corrected nested group passes separately in r026. Product
r029 is a complete invocation pass. The 70.9-percent reduction is service20,
not the whole S. Complete Product still costs about five minutes. Normal input
pacing, actual startup timeout, WOW limitations, native 30ms sampling and S8's
extreme mouse/RDP limits remain. No runtime deduplication is restored.

Reproduce global groups with Invoke-ProductVerification.ps1 using validated
RuntimeRoot/BuildCache/Observer and fresh build LogRoot: -Suite Product runs
WOW+Console17+Window17, -Suite Control runs serial controls, -Suite Full combines
those selected global groups, not every repository unit. Run service20
independently; build basesrv-idle-policy-test.exe via the cached Ninja graph.
Use -FullDeadlines for timer mechanism changes/investigation. Never count
explicit cleanup as actual timer expiry. The unchanged snapshot unit passes
once-only counts, capture-error and discontinuity rejection.

Production review finds only NTSRV-local decision extraction and private
explicit-time wrappers: real locks/timers, 10000-ms grace, registration,
execution/completion, shutdown authority and frontend/worker I/O are preserved.
No original mirror, guest or imported library changes. MSVC14.43/SDK22621/x86
/MT/CCPU40 cache, wire37/I/O25 and APP0.0.425 remain. Existing header/resource
warnings are not new failures. S8's seal differs for documentation-only
src/mvdm/README.md; its listed runtime C/header/producer/test inputs are unchanged.

### Published SHA-256

| File | SHA-256 |
| --- | --- |
| run16.exe | CBE4117FF5E6D61D892A61CA0D7775913B8389E7C32106A2D0D4FF5582EE986B |
| ntsrv.exe | C91F632FB8902A318BCE3C256CEC0284EC6F2315F97FF3E8CAF6A52814161E24 |
| ntcon.exe | 93701C974EB9B6F762FE0499D1B260FF5E701F1CC9B1CEC51F0AE11E6B16DA46 |
| ntvdm.exe | EDCC09BB991F6754F6439309EBFBC94421A031FF501C7A56BDE11459D09C3955 |
| ntvwm.exe | A2CF70DFD817A8F765D205A369885023CB2F21A314DE160E3FDBEA5FFFAAC7A3 |
| ntmon.exe | 9FDC95C114DBE2473D46415043AD5193CCB66DFA4B97D56A09B6351D49757EF2 |
| WOW32.DLL | BEB688FE832921D788DB297787B2D1062D7FDD095AF1A304824F31DE7F19D0C1 |
| VDMREDIR.DLL | 771D04B12366613762BD71E39DC04C503A01093372879B16D1EB836117F0DC0C |

Recovery files/hashes remain in S9/r030/recovery and published-manifest.json.
Final source/build seals are S9/r032/build-source-inputs.json and
retained-s8-source-inputs.json. The observer's final subsequent edit is only
comment wording; the tested stable-title implementation is unchanged.
After r031 smoke ended at 21:16, read-only inspection found new O:/winnt
NTSRV PID10444 created at 21:18:40 and WOW NTVDM PID26036 at 21:20:03.
These postdate the owned smoke run and are not attributed to its cleanup;
they were left intact rather than terminating a possible owner side-test.
The containing sequential reviewed commit/push delivers S9. Other-session
Queue/proposal changes are excluded. T425 remains owner-open; no next S is
admitted and owner validation is next.

## Owner-approved observation-driven continuation

### Scope and source disposition

This continuation changes only project-authored tests/runners/build recipes and
their governance/evidence. No product, original mirror, guest, imported library,
wire ABI, startup/retirement deadline or target scheduling behavior changes.
The eight r022 product images equal r029 and the published r030 hashes above.
No new product publication is necessary for this test-only P; O:/winnt remains
the same accepted package. Queue/proposal edits from the other session remain
excluded. S9 is still active; this is not its closure or a 60-second claim.

The initial audit identified 53.6/53.8 seconds of explicit ordinary keyboard,
line and EDIT sleeps per Console/Window matrix. Those are nominal inserted
waits, not measured exclusive CPU/runtime costs or a guaranteed saving.
Repeated CIM discovery/cleanup and cold-start overhead were initially unquantified.
WOW's three serialized 20-second longevity observations are a separate retained
contract, not interchangeable with first frontier appearance.

### Changes and completion predicates

- Invoke-ProductVerification opts only its ordinary 17-case shell matrices into
  MVDM_OBSERVER_MILESTONE_INPUT. Other control/non-echo probes retain their
  own contract. InputPolicy=Observed is default; Paced retains legacy input for
  a same-product A/B diagnostic. Environment is restored on all exits.
- Ordinary input has at most four outstanding characters. Before more input
  or Enter, the actual current cursor row must contain the complete new command
  prefix and a matching cursor, with no dirty tail. WriteConsoleInput record
  count / Window message return is only delivery, never consumption. Missing
  echo fails; keys are not reinjected or retried to success.
- Each command is armed by its full pre-Enter echo. Completion requires a new
  empty current prompt, EDIT's actual Untitled surface, or the real direct
  launcher process completion. The existing verifier separately retains exact
  output, counts, per-operation MEM snapshots, ordering and exit-code checks.
  Historical prompts elsewhere cannot acknowledge this operation.
- EDIT observes welcome-dialog dismissal, File menu Exit, and return to the
  shell, replacing its unconditional 500/1500-ms sleeps. Its four post-EDIT
  MEM operations in the short-history fixture are unchanged. Console Alt-F
  and Window's actual bare-ALT then File input routes remain distinct.
- Window observation copies the actual selected text-frame context prefix,
  following window_text_geometry_probe's existing test-only mechanism and
  compiling against the actual library declaration. It validates the expected
  frontend PID, pins the process, checks two matching copies/dimensions/cells,
  and rejects a torn or non-text snapshot. Cursor blink phase is not an input
  milestone. It does not use the potentially stale visible Console mirror to
  acknowledge Window keys/menu state. This is not a new production API/channel.
- Explicit zero-line-delay/typeahead keeps its original continuous delivery
  policy; it is not serialized on prompt consumption. Ordinary milestone
  observation uses bounded 10-ms sampling plus direct process-handle waits,
  not a claimed pure event notification that the public display lacks.
- The old five-second Window lookup is not nested inside post-exit waits:
  target death is checked before the nonblocking HWND lookup. Startup/window
  delivery discovery deadlines otherwise remain unchanged.
- Package cleanup makes one ownership/ancestry discovery, pins all selected
  process handles, validates image and creation time, kills only those objects,
  waits on their handles and checks residuals. It no longer re-enumerates CIM
  per PID or polls process disappearance. WOW shares these identity checks.
- Timings now include driver preparation, gate body, gate cleanup, per-case
  body/cleanup and observed-input wait totals. Cleanup failure marks a gate
  failed even if its assertions succeeded. No case, timeout, output assertion,
  cold-start, independent-session or typeahead contract is removed.

### Runs and retained intermediate evidence

- r033: standalone observation Ninja graph; MSVC14.43/SDK22621 x86 /MT,
  C11 /W4 /WX; actual header dependency invalidates the copied Window layout.
  input-milestone-test passes partial/missing echo, stale prompt/current-row,
  cursor/tail and wrong Window-PID negatives. verify-cleanup-identity passes
  stale creation time and unowned path negatives without killing its process.
- Initial restricted Ninja invocation did not reach compilation; only its
  exact r033 processes were stopped. Escalated compile exposed the headers'
  C11 requirement; the recipe was corrected, then compilation passed. These
  predecessors are not passed builds.
- r034: all eight selected Console/Window empty, nested-empty, mem-repeat and
  EDIT cases pass. Initial post-exit Window lookup still inserted five seconds;
  this measurement is retained, not represented as the final optimization.
- r035: complete same-product Product pass, 17 Console + 17 Window plus all
  three retained WOW longevity/frontier observations. Driver total 208148 ms,
  including 1607-ms preparation; WOW 67031 ms, Console17 62911 ms, Window17
  72569 ms. Gate cleanup separately totals 2540 ms. Per-case body/cleanup
  details are in the matrix summary JSON, not conflated with group overhead.
  Compared with r029's 301534 ms this is about 31.0 percent less. Old total
  excluded preflight, so this is not a precisely equal stopwatch boundary.
- r036 final repeat passes all 34 matrix cases and three unchanged WOW
  observations: total 204913 ms; preparation 1254, WOW 66769, Console17 62134,
  Window17 70549 ms; separate gate cleanup 2843 ms. This is about 32.0 percent
  below r029, with the same preflight-stopwatch caveat. r035/r036 are two full
  passes, not retries hiding a failed full invocation.
- r037 supplemental Console nested-mem-typeahead passes all three distinct
  MEM execution witnesses. Console dos-native-typeahead exits with expected
  code/input-delivery but fails the existing contiguous-snapshot merge assertion.
  Its milestone-waits=0 proves it did not opt into serial consumption waits.
- r038 uses the immutable r026 old observer on that same dos-native-typeahead
  and product: it fails the same contiguous-snapshot assertion. Window's
  nested-mem-typeahead also fails that existing merge assertion. These are
  retained failures, not product/queue passes or waived assertions. This
  continuation does not alter Merge-ConsoleTextSnapshots or typeahead delivery.
  Further test-contract investigation is required; S9 remains open.
- r039 confirms the same Window nested-mem-typeahead assertion failure with
  the immutable r026 old observer. The matched failures establish that the new
  milestone policy did not introduce this assertion failure; they do not prove
  every queue/handoff property or authorize weakening the test. Raw reports and
  verdicts remain in r037-r039. Final input/identity negatives, script parsing,
  documentation governance and diff checks pass; final test input/artifact
  identities are retained under r033/final-inputs.json.

Reproduce with Invoke-ProductVerification.ps1 -Suite Product, RuntimeRoot
build/M0-T425/S9/r022/runtime, BuildCache build/M0-T424/S2/r001, Observer
build/M0-T425/S9/r033/console-startup-observer.exe, WindowObserver the sibling
worker-window-snapshot.exe, GuestFixture build/M0-T425/S9/r008/G7.COM,
WowBaselineRoots build/M0-T425/S9/r029 and a fresh repository-build LogRoot.
Use InputPolicy Observed or Paced with otherwise identical package, cases,
deadlines and assertions. Compare preparation, case/gate bodies and cleanup
separately; retain failed/timeout reports rather than retrying them into a pass.

### Remaining boundary

The current 20-second WOW longevity contract alone exceeds the 60-second whole
Product goal when retained serially. A first-frontier quick check could be
separate, but must never replace or be called equivalent to longevity. Further
shared-environment work must preserve explicit cold launches, independent
sessions, actual completion codes and restoration/fault assertions. No warm
reuse or reduced matrix has been silently substituted in this continuation.
Shell echo observation is intentionally opt-in: password/non-echo applications,
wrapped command input and graphics surfaces are not falsely acknowledged.
Physical keyboard focus/RDP behavior is not established by private-desktop
message injection. The broader Control suite is not a new pass in this P.

### Owner closure

Owner closes S9 on 2026-10-03 after pushed 817fe636b. The measured 31-32 percent
Product reduction, unchanged eight-file package, repeated full passes and
explicit supplemental non-pass/60-second limitations are accepted as the
bounded stage outcome. T425 remains open; repeated shared-WOW launch diagnosis
is admitted separately as S10, not a reopened throughput implementation.
