# Project Status

## Current Work

**Active: M0 T425 S9 (Ordinary Mode).**

The containing reviewed M0 T425 S9 P delivers test-throughput optimization.
S8 is owner-accepted at 90e57fe09. S9 build, retained regression, coherent
publication and postpublication smoke pass. No next S is admitted; T425
remains open for owner acceptance.

## Active Packet

| Field | Contract |
| --- | --- |
| Identifier Mode | M0 T425 S9, Ordinary Mode. |
| Admission And Approval | Owner: close S8, admit S9 to reduce excessive per-S test wait; approved T425 S9 plan applies. |
| Objective | Audit test coverage and waits; simplify setup, reuse identity-validated builds, run isolated checks concurrently, replace repeated real retirement waits with deterministic policy tests. |
| Non-goals | No lifecycle policy change, new component/helper, guest/library edits, display deduplication or weakened acceptance. |
| Reference Baseline | S8 90e57fe09; published r046 eight-file package and retained S8 evidence. |
| Files And ABI Surface | Test runners/fixtures, existing lifecycle policy test seam only if required, execution guidance and this packet/evidence; no wire ABI change intended. |
| Applicable Rules | Current execution, architecture, coding, document and source-policy authorities. |
| Verification | Inventory retained assertions; measure before/after elapsed time; deterministic expiry/cancellation/rearm tests; repeat isolated schedule; preserve product and failure gates. |
| Expected Markers | Explicit case assertions, nonzero failure propagation, timing/coverage records, owned-process cleanup and governance pass. |
| Asset Needs | Existing x86 CCPU40 cache, S8 immutable guest/runtime inputs; no new imports. |
| Reporting Requirements | Per-suite contracts/dependencies/waits, coverage mapping, measured savings, repeatability and remaining slow/unsupported boundaries. |
| Stop Conditions | Shared endpoint collision, uncertain coverage equivalence, production deadline change or regression; preserve unrelated work. |
| Exit Criteria | Measured wait reduction with retained assertions, no routine real ten-second policy waits, build/test/review evidence, required coherent publication if production changes, commit/push. |
| Original Owner Request | Close S8 and admit S9 to optimize tests because each S takes too long. |
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
deduplication or change original mouse/VGA algorithms. Normal input pacing,
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

T425 remains owner-open, not a closed T. The containing reviewed P delivers
S9 after accepted S8 90e57fe09, updates evidence/plan/execution guidance and
preserves unrelated Queue/proposal changes. Documentation governance and
actual diff/requirement review apply before commit/push. The S9 packet is
delivered pending owner acceptance; no further implementation is admitted.
