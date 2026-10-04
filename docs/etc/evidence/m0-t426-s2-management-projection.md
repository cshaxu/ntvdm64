# T426 S2 — service-owned management projection implementation

## Scope and inputs

Owner approves the no-UNBOUND hierarchy and implementation. S1 contract is
delivered at ff3221711. S2 is active, not closed. Published product remains
the unchanged T425 S9/r022 eight-file package. This record distinguishes an
initial verified production change from the complete management capability.

The existing project-owned NTSRV snapshot RPC performed count/allocation/copy
across two independently locked calls. A concurrent registration/rundown could
change the required count. The first implementation connects owner-side
SnapshotCopy to Server_TaskSnapshot: the existing recursive service lock now
covers count, allocation and original projection together. Original DOS/WOW
traversal and lock ordering stay in their owners. Checked size arithmetic,
empty output on failure and caller-owned heap release are explicit. No wire
change is introduced by this first increment; the eventual DTO change still
requires synchronized APP/RPC revision, regeneration and negative tests.

## Procedure and actual observations

Generate the selected formal graph under build/M0-T426/S2/r001:

```powershell
$taskNodeExecutable = (Get-Command node.exe).Source
powershell -NoProfile -ExecutionPolicy Bypass -File tools/build/New-T310OriginalSoftpcNinja.ps1 -Architecture x86 -BuildRoot build/M0-T426/S2/r001 -NodeExecutable $taskNodeExecutable
```

The first generation attempt omitted NodeExecutable and failed its prerequisite
check; the explicit runtime path above succeeded. The first restricted Ninja
invocation stalled before compiler output; only its exact owned Ninja process
was stopped after process-command identity verification. The same build was
then run with the required host-tool access, completing all 61 selected steps.
No product or other-session process was terminated.

Build through the retained MSVC x86 environment wrapper:

```text
build/M0-T425/S9/r033/msvc-x86.cmd <installed-Ninja> -C build/M0-T426/S2/r001 basesrv-service-reservation-test.exe ntsrv.exe -j 8
```

MSVC Win32/x86 /MT, original selected CCPU40 graph. Both formal targets link;
existing imported-header/source warnings remain visible, not warning-clean
claims. No mvdm or opennt-host mirror source changed.

Run the built production-provider fixture from that build root:

```text
basesrv-service-reservation-test.exe
basesrv-service-reservation-test.exe --native-worker
basesrv-service-reservation-test.exe --io-authority
basesrv-service-reservation-test.exe --frontend-authority
```

All four return 0 with their explicit PASS assertions. Default additionally
checks empty atomic snapshot, populated copied snapshot equal to the existing
original projection, and invalid-service output cleanup. Retained default
tests cover original DOS Check/Update/Get/Exit, WOW character-I/O exclusion,
startup/completion latches and reused-WOW receipt identity. Other cases retain
native authentication/root-loss rundown, both I/O close acknowledgements,
resume grant and broker-controlled retirement assertions. These in-process
fixtures do not occupy the global BaseSrv RPC endpoint.

## Final production implementation and provenance

The first increment above is retained chronology, not the current completion
claim. The final provider uses one locked traversal with checked growable
caller-owned storage. Existing DOS/WOW source lists remain authoritative.
Project-only metadata preserves authenticated root generation/PID and WOW
admission labels before original GetNext frees VDMINFO. No original mirror,
guest, device, execution, completion or library file changes.

Production TaskSnapshot now returns versioned keys, parent keys, depth,
display state and close permission. Common owns declarations and the typed
authenticated client; NTSRV alone projects relationships. NTCON roots precede
their bound workers, independent WOW workers precede actual WOW tasks,
detached registered GUI targets are top-level. No UNBOUND or observed task.
Missing roots derive only from surviving authenticated association metadata;
no tombstone registry or lifetime extension. Rebuilt roots cannot adopt an
old generation's children. Unknown task identity/time is not fabricated.

CloseManagementNode replaces PID-only wire termination. Service instance,
category, worker/root generation and object identity are validated while the
registry lock is held; the actual process/event objects are pinned before
unlocking. Waits and termination happen outside that lock. Worker close retains
the existing native Console-session acknowledgement or DOS/WOW close path.
Frontend close uses existing service shutdown notification and waits for its
actual exit. GUI close targets only the registered GUI process, not its former
carrier or descendants. WOW task rows are explicitly read-only: no original
individual-task close contract was proved. The local PID compatibility API
remains only for existing provider fixtures, not production RPC selection.

NTMON consumes server order/depth, retains selection/confirmation by full key
and clears stale confirmations on refresh/error. Original title and
UP/DOWN=Select Task DEL=End Task ESC=EXIT remain. S3 still owns final tree
column/selection polish and its full acceptance; this is a compatible wired
consumer, not an S3 closure claim.

Wire evolution synchronizes APP0.0.426/RPC38, regenerates x86 MIDL and rebuilds
every affected consumer. I/O25 remains unchanged. The unchanged WOW32.DLL
comes from the immutable delivered package; its code/export dependencies did
not change. New source is project management adaptation, not imported source.

## Final verification ledger

Formal product-programs and focused fixture targets link under
build/M0-T426/S2/r001 using MSVC14.43/SDK22621/Win32 x86 /MT CCPU40. The full
cold graph and subsequent affected incremental closure both pass. Imported
warnings remain, not a warning-clean claim.

| Gate and entrypoint | Actual result and boundary |
| --- | --- |
| verify-service-fixtures.ps1, r006, concurrency 4 | PASS all 22 retained/provider cases; includes management-gui and management-frontend-close, original DOS/WOW, stale identities, missing/rebuilt roots and native close acknowledgement. 2258ms fixture time. |
| common-management-test.exe | PASS 73 checks, zero failures and allocations; typed snapshot/close version/error and ownership assertions. |
| monitor-layout-test.exe via private-desktop console-startup-observer, r004/layout-final | PASS real Console cells/frame/colors/25 rows, overflow, horizontal extent, shrink/empty/confirmation and stable keys/WOW unknown identity; inherited pseudo-Console direct launch could not resize and is not counted as passing. |
| verify-s7-rpc-fixtures.ps1, r004/rpc | PASS client, bootstrap, startup-rejections/timeout, native-worker-failure/completed-worker-loss, workerless-grace/cancel, monitor-rpc, native-command/registry; actual authenticated RPC and original completion assertions. |
| Invoke-ProductVerification.ps1 -Suite Product, r003 | PASS Console17, Window17 and three retained WOW frontiers; 210794ms. Exploratory prior candidate only, not final publication identity. |
| Frozen final r004/runtime, Product r005 | PASS identical eight-file package; Console17 63202ms, Window17 73055ms, WOW frontiers 67642ms; total 210446ms including preparation/cleanup. |
| Verify-ProductVersions.mjs, r007/version-negative | PASS old application, wrong protocol, wrong reply application/protocol and legacy RPC interface; both launcher and worker return ERROR_REVISION_MISMATCH without retry/task delivery. |
| verify-native-gui-routing.ps1, r008/gui | PASS actual GUI startup, wait exit37, GUI-to-text, carrier retirement and text-GUI-text with resume marker/exit19. Existing completion/transfer assertions retained. |
| verify-native-monitor.ps1, r010/actual-monitor | PASS actual NTMON through NTVWM in Console and Window: authoritative CONSOLE/WIN32 tree visible, no UNBOUND/MEMBERS, ESC delivered and direct exit0. Full 80-cell title/footer checked by the renderer fixture. |
| verify-ntvwm-management.ps1 -TwoSessions, r010/isolation | PASS authenticated selected-worker close/actual Console-session acknowledgement; two roots/worker rows, independent Console remains responsive and returns23. |
| r009 publication | Eight verified hashes copied to O:/winnt; recoverable previous coherent package and both hash sets retained under recovery/ and published-manifest.json. |
| r011 published-smoke.ps1 | PASS actual published Console and Window COMMAND/MEM/EDIT/native VER and all eight hashes equal verified source. Z: removed and owned test processes cleaned. |

The first r008 actual-monitor attempt did not reach its complete-title marker:
the isolated physical viewport was 43 columns, clipping the centered 80-column
title. Its raw timeout is retained as non-pass. The corrected observation gates
on the actual CONSOLE tree row, verifies the visible title prefix/kinds and
ESC exit, and retains exact full-title/footer cell checks in the formal layout
fixture. This does not change production layout, input pacing or timeouts.
The failed comma-delimited native-shell RPC Cases invocation executed no cases;
the corrected PowerShell string-array invocation is the passing r004 proof.

Fixtures supplement rather than replace actual GUI routing/close, independent
sessions, frontend lifetime and protocol-negative package evidence. A test
actor acknowledging service frontend close proves notification/wait/lock
ordering, not actual frontend Console cleanup. WOW frontier observations are
not gameplay or SOL/WRITE functional passes. Physical desktop/RDP observation
is not newly verified.

## Review, delivery and remaining boundaries

Review confirms common/protocol is declaration-only; NTSRV owns all projection
and close policy; NTMON consumes copied rows only. Existing original critical
sections/order and real task completion remain untouched. All changed mirror
paths are absent: no new mvdm/opennt-host difference requires normalization.
GUI target termination rights are purpose-specific, not generic handle transfer.
No process observation, Job, helper, recursive kill or worker/frontend policy
is introduced by management. The unused ad-hoc monitor-layout build script
remains historical: the formal Ninja fixture replaces its invalid build path;
S3 can remove/redirect it when finishing test entrypoint/UI organization.

S2 source/provider/client/protocol gates and published smoke pass. Source/test
hashes and reviewed diff are retained in r009/final-inputs.json and
reviewed-source.diff (SHA256
68AAB40F0F1548C5F17BB09FD9FA04515E0EC60BC8427244A2B62A8A87285869).
Final governance/link/diff review gates the containing P. S3
layout/selection polish and S4 actual mixed-kind/multi-root integration remain
separate admitted work, not claimed complete here. Actual short-lived missing
roots are provider-fixture proof, not a physical desktop observation. Individual
WOW-task close remains deliberately unavailable; close its worker explicitly
to use the established supported boundary. T426 remains open for owner review.
