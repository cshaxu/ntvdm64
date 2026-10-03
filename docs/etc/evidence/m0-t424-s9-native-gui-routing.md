# T424 S9 native GUI routing

## Scope and baseline

S9 follows accepted S8 d30f33870, APP 0.0.424 and RPC/protocol32,
x86 /MT CCPU40. Its containing production P delivers native GUI routing and
the verified RPC/protocol33 eight-file O:/winnt package. Earlier increments
below retain their actual unpublished status at the time. T424 remains open.
GUI default startup-only and explicit --wait must remain unchanged; UNBOUND
display and frontend naming are separate stages.

## Initial source and ownership audit

| Mechanism | Source/current owner | S9 disposition |
| --- | --- | --- |
| Broad DOS/WOW/native discovery | Original OpenNT client vdm.c through existing classifier and run16 dispatch | Retain original discovery and DOS/WOW ownership; do not expand mirror diff. |
| Native GUI/CUI metadata | Original base/win32/client/vdm.c, NtQuerySection SectionImageInformation; current project query in run16/main.c | run16 image_classification.c retains launcher-side OS section/query ordering and error mapping. Reject DLL/non-GUI-CUI images; no PE parser or old host-machine restriction against supported modern native targets. |
| Direct command authentication/transport | NTSRV native_commands.c, common RPC/codec; NTVWM next_command.c | Reuse existing authenticated copied-command path; frontendless native admission still to implement. |
| Worker registration/creation | NTSRV worker_registry.c and reservations | Reuse broker creation/registration. GUI request release is not target/carrier termination. |
| Frontend binding | run16 scope currently precedes native submit; NTVWM main binds all incoming commands | Classify before binding. CUI retains route; GUI must not create a character frontend. Full launch routing remains pending. |
| GUI target creation | run16/main.c local launch_gui | Remove after broker/worker path and failure contracts are connected, not before. |
| Native process materialization | Existing restricted native_launch codec/binding and NTVWM execution | Reuse authenticated standard resource handling; GUI must strip character capabilities. |
| GUI lifetime beyond worker request | Not present; text record currently owns direct receipt only | NTSRV must retain restricted process handle and event-driven exit cleanup independently of worker occupancy/launcher. Pending. |
| --wait result | Current launcher waits GUI process directly | Move to authenticated service result, retaining real exit code; default must not wait GUI lifetime. Pending. |

Recovery ladder: original vdm.c owns image metadata, but its complete client
unit also depends on historical BaseClient/CSR and original guest dispatch.
Compose its existing section/query dependency at the launcher boundary;
original DOS/WOW units remain selected in their own graph. No external source
import/intrusion or mirror edit is required. GUI lifetime and service admission
are project-owned architectural extensions, not claims that original WOW held
a native per-task process handle.

## First implementation increment

The initial unshipped worker-classifier candidate was rejected by the owner's
classification-order correction. It is removed from NTVWM. The existing native
image-section query is factored into run16/image_classification.c, called before
service admission/frontend acquisition, matching DOS/WOW's discovery before
BaseCheckVDM. No worker preflight/resubmission is introduced. The old local GUI
creation route remains until actual broker routing is connected; this is not
S9 completion and no candidate is published.

The classifier test links the production implementation and uses a controlled
GUI-subsystem probe, a CUI image, a DLL, invalid paths and null arguments,
repeated 32 times with process handle-count equality. This checks classification
and resources, not full GUI routing or desktop UX.

On 2026-10-02 the isolated x86 /MT production-function fixture passed:
`build/M0-T424/S9/r001/classifier-test.exe` with
`build/M0-T424/S9/r001/gui-probe.exe`, `C:/Windows/SysWOW64/cmd.exe` and
`C:/Windows/SysWOW64/kernel32.dll`: 227 checks, zero failures, zero handle
delta, exit 0 (`classification-r001.log`). Compiler flags come from the
generated run16 entry edge; outputs are separate from the active S2 cache.
The extraction initially missed base_classifier.h and its RTL/TEB link closure;
failed isolated attempts r001-r003 are retained, not counted as passes.
The corrected link selects the existing support.obj and original RTL library,
the same providers as run16. No new historical declaration or replacement
error mapper is introduced. This does not prove native GUI routing.

Documentation governance and git diff --check pass at this increment.
The initial Ninja run was stopped before compilation after the graph inputs
were corrected. The corrected graph's `run16.exe`, `ntvwm.exe`, classifier
test and GUI probe build succeeds (`build-classification-r002.log`, exit 0).
The graph-linked fixture also passes 227/0 with zero handle delta
(`classification-r002.log`, exit 0). Full S9 routing/runtime gates remain open.

## GUI routing implementation increment (unpublished)

Local run16 GUI CreateProcess is removed. Both native kinds use existing worker
admission and copied request/receipt mechanisms. GUI takes the frontendless
branch; run16 still classifies before submission. NTVWM creates the real target
and binds its identity before resume. NTSRV retains the same record and restricted
process handle independently of worker request occupancy, with event-driven
actual-exit handling. --wait uses the service result. GUI release does not kill
the target or shared worker. Startup notification failure does not commit release.

Protocol/IDL major are both 33; MIDL and six x86 EXEs build in
`build-gui-r001.log` and `build-gui-r002.log` (notification-order correction).
Actual RPC client/native-command/native-registry fixtures pass in `rpc-r001-*`.
GUI startup/default returns 0 and --wait returns 37 in `gui-startup-r001.txt`
and `gui-wait-r001.txt`. Gated survival passes in `gui-survival-r003.txt` and
its assertion-bearing console snapshot: launcher returns while GUI is alive;
explicit release produces real exit 37. The first survival attempt used a
nonexistent PowerShell path and never started. r002 failed its exit assertion
because the fixture did not pin the target handle before release; observer exit
zero alone is not a pass. r003 pins the process and checks actual assertion output.
Shared-text/nesting, authentication/failure/rundown, full regressions and
publication remain unproved. O:/winnt remains S8.

## Second verification increment (not final publication)

The first complete candidate passes all 34 Console17/Window17 cases
(`matrices-r001.log`), modern EDIT return, cooked-CMD relaunch, independent
sessions and four fault-retirement cases (`retained-r001.log`). Five real
version-negative variants reject launcher and NTVDM with 1306; the existing
three WOW frontiers remain limits, not usability passes (`versions-wow-r001.log`).
All eleven actual RPC cases pass (`rpc-full-r001.log`). Classification is 227/0.

`gui-routing-r002.log` proves startup-only/real wait 37, GUI -> fresh text,
carrier retirement without GUI death, and text -> GUI -> text with output
restoration and outer result 19. `dos-gui-r001.txt` plus target-created S9GUI.OK
prove DOS -> actual GUI -> DOS MEM; a failed dispatch cannot satisfy this marker.
New regression scripts and the controlled probe are in tests/observation.

Adding real missing-image launch initially fails the resource fixture; r002-r004
are retained failures. A pure Windows CreateProcess control reproduces first-
failure handle growth without the product execution path. This initialization
is measured before the baseline, as the first successful launch already was.
Eight subsequent product GUI failures keep 125 handles unchanged. r005 passes
1073 checks, zero failures and zero net handles. No Sleep or weakened aggregate
equality is introduced. The common codec/RPC/client units also pass.

Final review then moves GUI exit-wait registration into BindNativeTarget while
the actual target is suspended, making allocation failure a pre-resume rollback.
`build-gui-r003.log` builds this service-only refinement; GUI rerun is r003.
Earlier full gates precede that final refinement and are not frozen-package
evidence. Final rerun/freeze/publication/commit remain open. O:/winnt stays S8.

## Final candidate revalidation

The service wait-before-resume refinement passes the full retained gates in
`matrices-r002.log`, `retained-r002.log`, `rpc-full-r002.log` and
`versions-wow-r002.log`. These are still candidate evidence, not publication.

The GUI probe is then strengthened to create and destroy a real, unshown HWND
on its private desktop. A GUI subsystem header alone is insufficient proof of
window creation. `gui-routing-r004.log` passes all five actual-process cases:
startup-only zero, --wait real 37, GUI -> fresh text, GUI survival after carrier
retirement, and text -> GUI -> text with restored output and outer exit 19.
This proves window-resource creation, not visible interactive GUI usability.

Regenerating the build graph rewrites generated/base_vdm_config.c and causes
five product EXEs to relink (`build-gui-window-r001.log`); their hashes change.
The candidate runtime is resynchronized, WOW's incremental graph reports no
work, and all eight products must receive a new frozen manifest. The previous
freeze and earlier run evidence are retained, not silently overwritten. Full
artifact-matched regression is running as r003; publication remains pending.

## Production ownership and reuse review

Both native kinds now use run16's `launch_native()` and
`scope_launch_native()`, the same copied native-launch codec, authenticated
Submit/GetNext/Bind/Startup service path and NTVWM `launch_request()`.
The former local `launch_gui()` CreateProcess/wait implementation is deleted.
`--wait` also uses the same service receipt/result mechanism as native text;
only text performs character-route restoration. No launch parser is changed.

Differences retained deliberately:

- run16 performs subsystem discovery before admission, rather than forcing
  original DOS/WOW execution to adopt a native worker classifier.
- CUI requests keep their authenticated frontend and final I/O fence. GUI
  requests have no character frontend and strip its capabilities/Console
  standard handles, while retaining actual redirected file/pipe handles.
- NTVWM waits on a text target and reports real completion. GUI startup
  transfers the existing Win32Record to service ownership; NTSRV holds a
  restricted actual GUI process handle and event wait, independent of carrier
  occupancy. This is not a second task record or process-tree policy.
- GUI default returns startup success; --wait returns the target's actual
  Windows code. Existing Win16 startup and DOS original task completion remain
  original-owned. A Win16 task does not acquire an invented process handle.

The wait callback only signals the service-owned event and never takes the
service lock or stores a record pointer. The service drains the wait before
closing its handle/freeing a record. Bind/register failure precedes target
resume; successful handoff is never rolled back by killing the running GUI.
Service startup acknowledgement succeeds before worker occupancy is released.
The carrier-survival fixture confirms the GUI survives that later retirement.

`git diff --numstat -- src/mvdm src/opennt-host lib` is empty. No guest,
shared-library, system-registry, Job, helper or monitor display change is part
of this increment. Classifier rerun r004 passes 227/0/zero handle delta.
Execution test r006 was invoked without its required report argument and
returned its usage status; this is not an assertion run. Correct r007 passes
1073/0/zero net handles, including eight real failed CreateProcess requests.

## Final gates and publication

All reports below are under build/M0-T424/S9/r001. The reused x86 /MT CCPU40
cache is build/M0-T424/S2/r001; unchanged WOW and VDMREDIR products retain
their verified input/artifact identities. MIDL emits v33_0 client/server
interfaces matching APP_PROTOCOL_VERSION 33; five incompatible variants are
rejected before delivery with 1306 (`versions-wow-r003.log`).

| Requirement | Final actual evidence |
| --- | --- |
| Console17 / Window17 | run-matrices.ps1 -Run r003; matrices-r003.log, 34 PASS |
| EDIT, relaunch, isolation, four faults | run-retained.ps1 -Run r003; retained-r003.log; actual EDIT/Ctrl+Q/CMD echo/DOS MEM, outer CMD 19, independent session 23, both worker kinds and frontend loss |
| Actual RPC, auth/startup/completion/lifecycle | verify-s7-rpc-fixtures.ps1; final-rpc-dos-gui-r003.log, all 11 cases; fixtures rebuilt against final service libraries |
| GUI startup / --wait / nesting / carrier survival | verify-native-gui-routing.ps1, gui-routing-r004.log, all five cases; probe creates actual unshown HWND |
| Actual DOS -> GUI -> DOS | verify-dos-native-gui.ps1, dos-gui-r002.txt + line-02 snapshot + target-created marker; MEM and launcher completion |
| Classification / resources | classification-r004.log 227/0/zero handles; execution-lifetime-r007.txt 1073/0/zero net handles |
| Retained WOW | versions-wow-r003.log and per-application window reports: WINMINE main window; SOL/WRITE known memory dialogs; not gameplay/usability acceptance |
| Frozen source/products | release-source-manifest-r002.json, release-candidate-final-manifest-r002.json; 143 input identities and eight cache/runtime hashes rechecked |
| Publication / actual published smoke | publication-r001.log, published-manifest.json, postpublication-r001.log; outer CMD 19, GUI startup 0, --wait 37, all eight published hashes unchanged |

Publication preserves accepted S8 in accepted-s8-recovery with its manifest,
then replaces only the eight selected products. No guest/config overwrite,
test executable or helper is added to O:/winnt. The published set is identical
to the final tested manifest. Temporary Z: mapping is removed in finally.

The other session's owner-added S10 rapid-relaunch/lost-wakeup plan is included
as planning only. Its source-proven possible event-reset interleaving is not
repaired by S9, and the 1600-ms relaunch probe is not evidence for rapid-launch
race freedom. S10 admission/implementation remains separate. Monitor/UNBOUND
display and frontend renaming are not delivered here. The containing S9 P
provides the reviewed commit/push; final clean/synchronization are checked
against Git after that delivery, rather than inventing a self-referential hash.

## Acceptance

- [x] Frontendless native admission and NTVWM GUI launch via NTSRV.
- [x] NTSRV independent GUI handle/exit registration and startup-only release.
- [x] Broker --wait result, failed startup/rundown races and no target kill.
- [x] Retain launcher classification order; remove local run16 GUI creation
  after its broker/worker replacement is connected and verified.
- [x] Direct/nested GUI, shared-worker preservation and fresh text isolation.
- [x] Full retained build/RPC/Console17/Window17/EDIT/WOW gates and review.
- [x] Coherent eight-file publication and smoke; containing P delivers commit/push/clean tree.
