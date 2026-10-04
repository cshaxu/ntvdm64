# T426 S4 — real management integration and owner handoff

## Scope, provenance and final implementation

Owner-approved sequence enters S4 after S3 3908fccb7. The complete UI is
already in the published S3 package. This stage adds only test observations
and verifies the actual product; no production, protocol, worker, guest,
OpenNT/MVDM mirror or library is changed. APP0.0.426/RPC38/I/O25 is retained.
Original execution/completion and NTSRV ownership remain at their owners.

The existing monitor RPC fixture now optionally emits a copied JSON snapshot
with full stable keys/parents and closes a selected category/PID by resolving
its full key from that snapshot. The wire call still passes the full key;
PID is only the test CLI's row selector. It never enumerates or registers tasks.
Empty JSON snapshots are valid observations, not a successful live-worker
assertion; original --existing/--terminate assertions remain intact.

The new serial private-desktop integration script starts actual launchers and
uses the production common RPC client. It records actual service replies,
pins exact process objects before close, gates fresh survivor input and checks
its output/exit code. Its observations do not implement management policy.
Existing native management tests gain -FrontendClose and use the established
identity-checked cleanup helper for remaining package processes, replacing a
PID-only final cleanup loop. No product helper or production state is added.

## Exact verification and limits

All new outputs are below build/M0-T426/S4. Existing MSVC14.43/SDK22621 x86
/MT cache S2/r001 builds monitor-rpc-test.exe: two affected compile/link steps
pass without new warnings. Frozen product files are copied from S3/r001/runtime
without modification; test executables reside only in the isolated build copy.

| Requirement | Exact entrypoint / evidence / actual result |
| --- | --- |
| Actual mixed tree and independent roots | tests/observation/verify-monitor-tree-integration.ps1, r003/run.ps1 and mixed-tree.json: PASS two distinct NTCON roots with actual DOS/NTVWM children, top-level WOW worker with a real executable-labeled task, and independently top-level registered GUI. WOW task has PID0, depth1 and no close action. Full parent keys are checked, not guessed from PID ancestry. |
| DOS root close acknowledgment and pruning | Same r003: PASS authenticated frontend close, actual NTCON/NTVDM exit before success, direct unfinished DOS receipt1067, old root/worker absent from subsequent service snapshot. Independent NTVWM/root, WOW and GUI remain live. |
| Detached GUI close isolation | Same r003: PASS GUI actual process exit; native/WOW survive. Release survivor's input gate only after management close; actual fresh TREE-INDEPENDENT-OK and direct exit23 prove continued interaction, not an old prompt. |
| Native root close / independent Console | verify-ntvwm-management.ps1 -TwoSessions -FrontendClose, final r007: PASS actual frontend, worker and attached CMD exit after RPC acknowledgment; second native Console receives fresh ISOLATED-SESSION-OK and returns23. r005 passed before final cleanup strengthening; r007 verifies final script. |
| Original worker DEL path | Same script -TwoSessions without added switch, r008: PASS selected worker/target termination and independent Console input/exit23; existing assertions preserved. |
| Missing root / generation / stale actions / WOW / reentry | verify-service-fixtures.ps1 -Concurrency4, r004: PASS all22 retained fixtures in2242ms. Controlled frontend-rundown assertions show MISSING under the exact old identity, replacement isolation and final prune; management close validates instance/generation and readonly WOW. This is controlled transitional-state proof, not an artificially prolonged physical window. |
| Actual original RPC negatives after fixture extension | monitor-rpc-test.exe --empty via existing private observer, r006: PASS actual empty snapshot, protocol mismatch and absent service-instance close rejection. S2 r007's five interface/application/version negatives retain unchanged production inputs. |
| Real title/footer/keys/layout and refresh | S3 r001/r002: actual renderer plus production key dispatch, stale/reordered/removed selection and Console/Window NTMON ESC. Full title/footer and columns checked in actual 80-cell Console fixture; readonly WOW DEL sends no close. |
| Full package regression / publication | S3 r003: Console17/Window17/three retained WOW frontiers pass,209612ms; S3 r004 coherent publication and recovery, r006 published smoke pass. S4 changes only tests/docs; r009 passes all eight integration/published hash checks against S3, zero production source diff, no owned process or Z mapping, and records source/artifact manifests. |

The new mixed-case integration is one service with concurrent controlled
participants, not concurrent complete product matrices. Z: is its sole alias
and removed in finally. Test cleanup is explicitly owned and identity checked;
cleanup of still-live WOW/other test processes is not evidence of normal
retirement. Process inspection in test setup/cleanup does not enter NTMON or
NTSRV task projection.

Two initial test failures remain evidence: r001 omitted observer WorkingDirectory
and therefore did not resolve COMMAND from the package; r002's scripted input
parameter collided with PowerShell's automatic input variable, so survivor
input was never delivered. Fix the test causes, retain both reports and run
fresh r003 with all assertions. Neither failure is silently treated as a pass
or repaired with product delay/retry. Existing tests were not weakened.

## Final requirement audit

- NTSRV is the only graph/action authority; common carries typed copied RPC
  data, NTMON only renders copied rows and submits stable-key close requests.
- Frontends precede their authenticated children. WOW workers are independent
  top-level nodes with actual original WOW tasks below; detached GUI targets
  are independent. No UNBOUND or MEMBERS display, observed descendants or Job.
- DOS=0/Win16=1/Win32=2, exact title and UP/DOWN/DEL/ESC remain. Unknown task
  PID/time stays explicit; unsupported individual WOW task close stays readonly.
- Reordering retains full-key selection; removal chooses a nearby row; stale,
  failed or permission-lost snapshots cancel confirmation. Closing another
  generation cannot become a PID-only kill. Timeout is not success.
- Provider uses copied data under its lock and pins actual close objects before
  waiting outside it. UI never owns a worker, original record or lifecycle.
- Source review finds no new mirrors, protocol revision, helper, scheduler,
  sampling task graph or worker/front-end policy branch in S3/S4.

Retained boundaries: physical RDP/display/focus is not newly proved by these
private-desktop tests; SOL/WRITE keep their previously accepted frontiers,
not gameplay acceptance. WOW task DEL remains explicitly unavailable.
Missing-root intermediate display uses deterministic service fixtures because
normal cooperative shutdown may remove the association too quickly to see.
S4 adds no historical process trace or deeper guest capability.

The final owner-verifiable package remains the coherent S3 package at O:/winnt.
S4 test/evidence delivery and final governance/link/diff review complete this
implementation sequence. T426 stays open until the owner accepts the product;
do not admit the next queued T or represent owner acceptance as already given.
