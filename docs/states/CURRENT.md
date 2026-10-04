# Project Status

## Current Work

**Active: M0 T425 S9 (Ordinary Mode).**

The preceding reviewed M0 T425 S9 P delivers initial test-throughput optimization.
S8 is owner-accepted at 90e57fe09. S9 build, retained regression, coherent
publication and postpublication smoke pass. No next S is admitted; T425
remains open. Owner approves continuing S9 with observation-driven input,
EDIT transitions, stage timing and identity-pinned cleanup; no S9 closure yet.

## Active Packet

| Field | Contract |
| --- | --- |
| Identifier Mode | M0 T425 S9, Ordinary Mode. |
| Admission And Approval | Owner continues S9: replace hard waits with observed operation milestones and reliable keyboard delivery; retain typeahead and all assertions. |
| Objective | Reduce normal product-test overhead with fresh input/output observations, EDIT state waits, explicit phase timings and pinned cleanup. Measure against the same r022 product; 60 seconds is a target, not a promised result. |
| Non-goals | No production/protocol change, helper, guest/library edits, weakened assertions, complete-matrix concurrency, lost cold/independent-session coverage or implicit shortening of WOW longevity observation. |
| Reference Baseline | Initial S9 P 51f09f7d1; unchanged published r022 eight-file package; r029 Product 301534 ms and retained S8 evidence. |
| Files And ABI Surface | Test observation/runners/build recipes and this packet/plan/evidence only; no production or wire ABI change. |
| Applicable Rules | Current execution, architecture, coding, document and source-policy authorities. |
| Verification | Same-product repeated Console17/Window17 plus retained WOW; fresh-echo and cleanup identity negatives; supplemental typeahead; stage timings, source/artifact seals and governance/diff review. |
| Expected Markers | Explicit case assertions, nonzero failure propagation, timing/coverage records, owned-process cleanup and governance pass. |
| Asset Needs | Existing x86 CCPU40 cache, S8 immutable guest/runtime inputs; no new imports. |
| Reporting Requirements | Per-suite contracts/dependencies/waits, coverage mapping, measured savings, repeatability and remaining slow/unsupported boundaries. |
| Stop Conditions | Shared endpoint collision, uncertain coverage equivalence, production deadline change or regression; preserve unrelated work. |
| Exit Criteria | Measured wait reduction with retained assertions, no routine real ten-second policy waits, build/test/review evidence, required coherent publication if production changes, commit/push. |
| Original Owner Request | Continue S9, not close it: replace hard waits with observed milestones and reliable keyboard delivery without weakening retained assertions. |
| Similar-Issue Sweep | Repeated package builds, fixed sleeps, idle-grace cleanup, competing BaseSrv users, obsolete/no-assertion scenarios and duplicated gates. |

S9 [audit and timing evidence](../etc/evidence/m0-t425-s9-test-throughput.md):
same service20 image measured 7435 ms serial versus 2160 ms four-way (70.9
percent less); repeated final service20 passes. Existing-owner time inputs
verify expiry/cancellation/rearm without changing the production ten-second
grace. Real worker shutdown/receipt wiring, RPC/GUI/version negatives,
DIR/EDIT/cooked return, immediate relaunch and independent sessions pass.
Corrected nested Window checks ordered output, not unpromised history.
Complete r029 Console17/Window17 and WOW pass in 301534 ms. r030 publishes
the identical eight files; r031 published Console/Window smoke and hashes pass.
Intermediate failed controls and withdrawn experiments remain non-pass in
the ledger. The speedup percentage is not a whole-suite claim.

Observation-driven continuation r035/r036 repeats all 34 product cases and
three retained WOW longevity/frontier gates with the identical product. Total
208148/204913 ms, about 31-32 percent below r029; not the 60-second goal.
Bounded four-character input requires fresh full echo before Enter; EDIT uses
actual dialog/menu/shell states. Typeahead keeps continuous delivery. Fresh
echo and cleanup-identity negatives pass; detailed timings/limits are in the
same ledger. The current 20-second-per-WOW longevity contract is not shortened.
Supplemental Console DOS typeahead passes; native/Window typeahead's existing
contiguous-snapshot assertion fails with both new and immutable old observers.
Those checks remain non-pass in r037-r039; neither their assertions nor their
continuous input policy is weakened. Test-contract investigation remains open.

The [S8 ledger](../etc/evidence/m0-t425-s8-worker-frame-deduplication.md) retains
the owner brief, original-source dispositions, failed predecessors, exact
release/recovery hashes, commands, assertions and limits. Original mouse
draw/erase algorithms now receive natural route/handoff events; periodic
mouse_refresh_pointer is removed. Software FULLSCREEN preserves simulated
VGA, cursor and final-paint ownership without enabling hardware takeover.
NTCON and worker-base do not suppress emitted display events. Native Console
sampling derives actual dirty rows; explicit native frame sends are unfiltered.

Owner accepts restored EDIT performance and normal blinking. Extreme
downstream-injected 1000-motion retirement remains unstable diagnostic
evidence, not a normal-path gate; physical RDP observation is not claimed.
Original CRTC bit-5-only hide behavior is recorded in [TODO](TODO.md).
Mild DOS mouse choppiness has no proven CPU/driver attribution. No new
scheduler, helper, component, channel or lifecycle policy is introduced.

The S8 accepted cursor/register/route and input ordering capabilities retain
unchanged producer/input C/header source identities. S9 does not reintroduce
deduplication or change original mouse/VGA algorithms. Diagnostic/typeahead pacing,
native 30ms sampling, real startup timeout and physical RDP limits remain.
Owner's transient CAF report was withdrawn as working; no speculative patch.

Other-session Queue and worker-control proposal changes remain preserved and
outside this P. They do not authorize another active task. The [T425 plan](../etc/operations/t425-worker-neutral-frontend-plan.md)
owns the approved stage sequence; detailed prior status is retained in the
indexed stage ledgers and repository history, not duplicated here.

## Current Technical Baseline

Published tested S9: build/M0-T425/S9/r022/runtime copied coherently to
O:/winnt; publication/recovery manifest is build/M0-T425/S9/r030.
Eight files: run16.exe, ntsrv.exe, ntcon.exe, ntvdm.exe, ntvwm.exe, ntmon.exe,
WOW32.DLL and VDMREDIR.DLL. Recovery retains the previously published S8 files.
MSVC14.43 / SDK22621 Win32/x86 /MT CCPU40, APP 0.0.425,
RPC/protocol37 and I/O25. Source/build-input seals and all eight SHA-256 values
are in the S9 ledger; guest/configuration and imported libraries are unchanged.

WINMINE, SOL and WRITE independently retain preceding S7 and historical
S2/S3/S5 observation frontiers. Existing dialog/environment limitations are
not repaired or counted as gameplay passes. Host scrollback is not promised.
Five intentionally changed mirrors are provenance-reviewed registered project
hooks; original execution/device algorithms and OpenNT-host mirrors remain.
The wider mirror sweep's pre-existing format-only differences are not a
whole-tree byte-clean pass or a scope expansion.

## Recent M0 Closures

| Task | Outcome and retained evidence |
| --- | --- |
| T424 | Owner-closed at audited ff50ff588, production S12 46abdb554. [Closure](../history/m0-t424-component-control-closure.md). |
| T423 | Owner-accepted S40 f64559086. [Closure](../history/m0-t423-console-window-runtime-closure.md). |
| T422 | Initial WOW32 and planning milestone. [Closure](../history/m0-t422-initial-wow32-closure.md). |

## Recent Governance

T425 remains owner-open, not a closed T. Initial P 51f09f7d1 delivers
initial S9 after accepted S8 90e57fe09, updates evidence/plan/execution guidance and
preserves unrelated Queue/proposal changes. Documentation governance and
actual diff/requirement review apply before commit/push. The S9 packet is
delivered; this test-only continuation does not close S9 or admit another S.
Test-only work uses build/M0-T425/S9/r033 and later fresh run directories.
