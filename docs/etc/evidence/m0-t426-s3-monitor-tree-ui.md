# T426 S3 — monitor tree layout and selection

## Request, boundary and inputs

Owner approves implementing the no-UNBOUND NTMON interface and sequential
stages. S2 provider delivery a8f356a22 and S3 admission 2cc2761d2 establish
the starting contract. NTSRV remains the only relationship/snapshot/close
authority. This stage changes NTMON-private UI and its test entrypoints only.
Original execution, MVDM/OpenNT mirrors, worker-base, libraries and service
protocol remain untouched. APP0.0.426/RPC38/I/O25 do not change.

The existing project renderer and service consumer are reused. New bounded
UI helpers reconcile copied rows and dispatch the existing keys, not another
service/registry or generic framework. Server ordering/indent depth remains
authoritative; NTMON never enumerates processes or reconstructs associations.

## Implemented behavior

- PID's fixed 12-cell field includes child indentation and all ten decimal
  digits of a 32-bit PID. KIND/STATE/ELAPSED/STACK/TASK stay aligned for roots,
  workers, WOW tasks and detached GUI rows. Unknown WOW PID/time/stack remains
  explicit rather than inventing process data. Horizontal scrolling remains.
- Selection uses the complete service/category/generation/object key. Reorder
  preserves the selected identity; removal chooses its previous visual index,
  clamped to the surviving last row. Empty snapshots clear selection. This
  positional fallback is UI only, never execution identity or association.
- Snapshot removal, replacement, loss of close permission, bind error or RPC
  failure clears confirmation. The service independently validates every
  eventual close, protecting the snapshot-to-keypress race too.
- The production loop invokes one handle_key path for UP/DOWN, DEL,
  confirmation Y/N/ESC, exit ESC and existing horizontal arrows. Read-only
  rows send no close call; cancel ESC cancels confirmation without quitting;
  unconfirmed ESC exits. Close errors remain visible, not retried into success.
- Removed unused confirm_task_count state and misleading worker-only private
  close naming. No component lifecycle, completion or close policy changed.
- Title NTVDM Task Monitor and exact footer
  UP/DOWN=Select Task DEL=End Task ESC=EXIT remain unchanged.

## Reproducible verification

Declared S3 outputs: build/M0-T426/S3/r001 and later S3 run roots. Reuse the
validated formal x86 cache build/M0-T426/S2/r001, not a new cold rebuild.
MSVC14.43/SDK22621 Win32 /MT CCPU40. Build product-programs and
monitor-layout-test.exe through the retained x86 environment wrapper and
installed Ninja. Four affected compile/link steps pass; no new compiler
warnings. Seven unchanged runtime files retain S2 hashes. Freeze the complete
package in S3/r001/runtime; only ntmon.exe is replaced before verification.

The retired ad-hoc verify-monitor-layout.ps1 recipe referenced src/interface
and omitted common/RPC dependencies. It now builds the formal Ninja target
and runs the existing private-desktop observer with fresh evidence, checked
actual exit/PASS assertions, and environment restoration. Example arguments:

```text
verify-monitor-layout.ps1 -BuildRoot build/M0-T426/S2/r001
  -Observer build/M0-T425/S9/r033/console-startup-observer.exe
  -ReportPath build/M0-T426/S3/r001/layout.txt
  -MsvcWrapper build/M0-T425/S9/r033/msvc-x86.cmd -Ninja <installed-Ninja>
```

| Requirement | Exact entrypoint and current result |
| --- | --- |
| Full renderer cells, hierarchy and alignment | monitor_layout_test.c wmain/cell assertions via formal target and r001/layout: PASS exact title/footer/attributes, fixed KIND columns for six mixed rows, MISSING state, full-width PID, unknown WOW fields and executable label. |
| Existing scroll/overflow/empty behavior | Same actual renderer: PASS 25 rows, frame/colors, four arrows, overflow selected row, horizontal limit/last cells, shrinking/empty clearing and confirmation footer. |
| Actual selection/refresh/input algorithm | selection_and_input in that fixture includes production main.c, mocks only RPC boundary: PASS UP/DOWN clamps, reorder stable key, nearby removal, empty selection, permission/stale-generation/error confirmation invalidation, read-only DEL no call, Y/N/ESC and close error/selector forwarding. |
| Typed transport and authority remain correct | S2 r006/r004/r007/r010 tests retained by unchanged common/service/protocol and seven-artifact identities; UI mock is not RPC authentication or actual process close proof. S4 retains real mixed-kind close/integration work. |
| Product NTMON Console/Window main loop | verify-native-monitor.ps1, r002: PASS actual frontend/NTVWM pipeline, CONSOLE/WIN32 rows, no UNBOUND/MEMBERS, ESC delivered and direct exit0 in both modes. Narrow physical viewport may clip title; full 80-cell title proven by renderer fixture, not falsely inferred from the clipped observation. |
| Final package non-regression | Invoke-ProductVerification.ps1 -Suite Product, S3/r003, final r001/runtime and S2/r005 WOW comparison: PASS Console17, Window17 and all three retained WOW frontiers; 209612ms total (67881ms WOW, 64040ms Console, 71447ms Window). |

No source or test result is represented as physical RDP observation. WOW task
DEL remains unavailable rather than invoking whole-worker close. No observed
descendant, Job, helper, new channel, polling policy or task record is added.
The inherited 750ms monitor snapshot refresh remains unchanged.

## Publication and review

Frozen r001/runtime passes the full retained gate above. r004/publish.ps1
checks r003's passing gates and tested runtime manifest before replacing all
eight O:/winnt files; preserves the old coherent package under r004/recovery,
verifies every published hash and rolls back all eight on publication failure.
Actual publication passes. r004/published-manifest.json records candidate,
recovery and published hashes. Seven binaries retain the S2 published identity;
only ntmon.exe changes, with APP0.0.426/RPC38/I/O25 unchanged.

Published Console/Window smoke passes under r006: COMMAND exit, native VER,
MEM and EDIT in both modes, expected results and all eight identical hashes.
r005 also passed those interaction cases, but its final PowerShell5 JSON-array
enumeration failed in the publication script, not the product. Retain that
failure; r006 explicitly enumerates the parsed array and repeats the same
cases with fresh reports rather than declaring the failed script a pass.
Final governance/link/diff checks pass before delivery. Reviewed the
production diff: bounded copied-snapshot state, full-key dispatch, no additional
registry/ownership, one allocation/free per snapshot and no new transport or
worker-policy branches. No other-session modification is staged.

S4 real mixed-kind/multi-root final integration and owner T acceptance remain
separate; this record does not claim them from fixture-only evidence.
