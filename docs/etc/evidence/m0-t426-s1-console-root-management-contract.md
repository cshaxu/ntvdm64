# T426 S1 — Console-root management contract audit

## Question, inputs and procedure

Owner continues the admitted NTMON Console-root worker-tree package. This S
is source/design-only: freeze the smallest production change and exact tests,
not implement or claim a working tree UI. Baseline: main 5ed674c7c, delivered
T425 closure 8cbc1c997, unchanged published eight-file S9/r022 package;
APP 0.0.425, service/RPC 37, worker I/O 25.

Read AGENTS and its execution/design/rule/contribution set, source policy,
Roadmap and the [approved proposal](../../proposals/proposal-ntmon-console-worker-tree-001.md).
Inspect current production owners, not historical component names:

```powershell
git status --short
Get-Content src/ntsrv-exe/opennt/include/service_internal.h -Raw
Get-Content src/ntsrv-exe/opennt/source/management.c -Raw
Get-Content src/ntsrv-exe/opennt/source/lifecycle.c -Raw
Get-Content src/ntsrv-exe/opennt/source/frontend_registry.c
Get-Content src/ntsrv-exe/opennt/source/service_core.c
Get-Content src/ntsrv-exe/opennt/source/worker_registry.c
Get-Content src/ntsrv-exe/opennt/source/native_commands.c
Get-Content src/ntsrv-exe/main.c
Get-Content src/ntmon-exe/main.c
Get-Content src/common/rpc/management.c
Get-Content tests/adapter-basesrv/base_service_reservation_test.c
Get-Content tests/app/common_management_test.c
Get-Content tests/observation/monitor_layout_test.c
Get-Content tests/observation/verify-monitor-layout.ps1
```

Source conclusions below are high-confidence inspection, not newly executed
runtime results. No build, test session, live process, guest or O:/winnt change
is needed for this audit. Unrelated work remains preserved.

## Existing owners and necessary changes

| Requirement / source | Current fact | Smallest implementation boundary |
| --- | --- | --- |
| [management.c](../../../src/ntsrv-exe/opennt/source/management.c), OpenNtBaseServiceSnapshot | Enumerates worker_watches only, sorts by sequence/task. Root connections and independent gui_records are absent. | One expanded management projection; roots from authenticated existing connections, workers from watches, GUI targets from existing records. No new task graph. |
| [service.idl](../../../src/common/protocol/service.idl), DTASKMGR_WORKER / TaskSnapshot | PID-only wire identity; internal sequence and service epoch are discarded by Server_TaskSnapshot. | Copied management node DTO with service instance, node category and stable generation/key; retain PID as display only. |
| [base_process.c](../../../src/ntsrv-exe/opennt/source/base_process.c), OpenNtBaseRegisterProcess | Nonzero monotonically assigned process sequence, overflow rejected; pinned actual process handle. | Reuse sequence with service-instance identity; never authorize by current PID alone. |
| [frontend_registry.c](../../../src/ntsrv-exe/opennt/source/frontend_registry.c), RegisterFrontendRoot / RegisterFrontendLease | Validates NTSRV-created exact process/event, stores Console lease and creator generation; exit watch signals existing service event. | Sample root age from its authenticated process; projection never invents launch ancestry. |
| Same file, service_clear_frontend | Deletes Console contexts/routes before connection free. Native native_root remains generation, but DOS no longer has a root descriptor after rundown. | Capture explicit management association at authenticated bind and retain a minimal missing-root descriptor before destructive rundown. Never keep a freed root pointer or preserve a live transport. |
| [service_core.c](../../../src/ntsrv-exe/opennt/source/service_core.c), worker Connect; [worker_registry.c](../../../src/ntsrv-exe/opennt/source/worker_registry.c), native registration | Watches pin worker process/creation time; DOS uses authenticated Console/context matching, native binds native_root; shared WOW excluded. | Persist management root generation on the existing watch at these proven binding transitions, including later route binding. Do not equate a released I/O channel with lost association. |
| [lifecycle.c](../../../src/ntsrv-exe/opennt/source/lifecycle.c), FrontendUsage / service_root_has_worker | Reads admitted pending/native activity and original DOS records; root/worker census excludes shared WOW. | Read-only management BUSY/IDLE projection, not a new readiness/retirement policy. Combine admission, original active records and handoff facts. |
| management.c, original record readers | DOS depth counts BUSY/TO_TAKE; READY PermCom and returned receipt-only records are not running children. WOW dispatched/task list remains original. Native Direct records/inflight determine native depth. | Retain these readers and locks. Blocked parents remain BUSY; unknown/startup/control-binding gaps must not silently become IDLE. No descendant count or Console-member stack. |
| [native_commands.c](../../../src/ntsrv-exe/opennt/source/native_commands.c), BindNativeTarget / NativeStarted | Actual GUI process is pinned while suspended; record moves to service gui_records after startup. Carrier occupancy released; GUI exits independently. Handle currently query/synchronize only. | Top-level GUI_TARGET row, kind=2, depth=1 while live; process age from actual target. Stable GUI key must survive carrier exit. DEL requires a narrowly retained target terminate right at authenticated bind, never reopening a displayed PID or killing carrier. |
| lifecycle.c, TerminateWorker | Native session stop and closed ack then actual carrier exit; DOS/WOW existing termination branch. PID-only selector. | Stable-node selector resolves exact watch; preserve owner-specific close and failure result. No new recursive termination. |
| [session_service.c](../../../src/ntcon-exe/session_service.c), frontend pump; frontend_registry.c, Console-return ack | Existing retire event means park/return lease, not necessarily root death. Broker frontend_closing/state wake determines full retirement. | Root DEL sets NTSRV's existing closing barrier and signals state wake; do not just set lease-retire event and claim root closed. Report success only after actual root exit and required borrowed-Console restore ack. |
| [main.c](../../../src/ntmon-exe/main.c), selection/render/input | selected_pid and confirm_pid, flat list, no STATE column; title correct. Footer UP/DOWN=Select DEL=Kill F3=Exit and VK_F3 disagree with approved contract. | Stable selection/confirmation, tree/columns, approved exact three controls; no UNBOUND heading, MISSING DEL no RPC. ESC cancels confirmation, otherwise exits. |

All implementation above belongs to project-added adaptation. No original
MVDM or opennt-host source relocation/change is required. Common supplies
copied declarations/client mechanics, not root policy; worker-base changes
are not needed for this management package.

## Frozen copied contract and identity

Use one management snapshot and one typed close selector, not a parallel
monitor registry. Proposed node fields (fixed-width, with bounded image):

- service_instance: existing nonzero management epoch, strengthened at service
  initialization to a fresh random instance identity; fail startup on entropy
  failure. The current time/PID-derived value is not a collision-free promise.
- category: FRONTEND, WORKER, GUI_TARGET, WOW_TASK. No UNBOUND heading or
  synthetic grouping record. Independent workers and detached GUI targets
  are top-level; absence of a frontend is not an error for window programs.
- generation and object_id: FRONTEND/WORKER reuse registry sequence with
  object_id=0. GUI uses the authenticated original worker generation plus its
  monotonic request id, retained in that same GUI record after transfer.
  Neither PID nor task id alone is sufficient. Zero/unknown values rejected.
- root_generation: explicit authenticated management association, zero only
  for truly independent nodes. WOW_TASK also carries its actual worker parent
  identity. Frontend association is separate from execution stack.
- process_id, worker_kind, management_state, started_filetime, task,
  stack_depth, image: copied presentation facts. DOS/Win16/Win32 remain 0/1/2;
  frontend category ignores worker_kind and displays CONSOLE.

Management states BUSY/IDLE/MISSING/CLOSING/UNKNOWN are presentation only.
Pending startup/handoff or active blocked task maps BUSY. An idle registered
worker with no active task maps IDLE only when its owner state proves it;
unready/disconnected unknown does not. Root BUSY aggregates its own admitted
work and associated busy/transitional workers, not unrelated GUI or WOW.
MISSING has no live elapsed time; live rows use actual process creation time.

Snapshot copy and selector validation occur under service->lock. Existing
DOS/WOW readers additionally acquire their original locks, in that order.
Return one bounded coherent copied snapshot (including epoch), with checked
counts/allocation sizes and deterministic ordering. Avoid the current RPC
count-then-copy race by allocating/copying atomically at the service owner;
do not retry forever while lists mutate. RPC serializer sees no live pointers.
Selection identity is category/instance/generation/object_id; RPC caller
authentication remains mandatory. Identity is not a transferable permission.

Wire change: advance APP_PROTOCOL_VERSION and service.idl major together to
38; task identity becomes APP 0.0.426 at the first production delivery.
Worker I/O stays version 25: this package changes no worker data protocol.
Generate MIDL under build and relink affected consumers. Reject protocol 37,
old application identity, wrong RPC interface, wrong instance, malformed kind,
unknown/stale keys and foreign-logon/session callers explicitly.

## Association, tombstone and close lifetime

Live roots are existing registered connections, not a second live registry.
Add only missing-root descriptors under the service owner: generation, old
PID and minimal display metadata. Watch associations are set only by existing
authenticated registration/bind transitions and retained through pipe release.
Before root/context destruction, preserve a descriptor if associated watches
remain. After watch removal/rebind, remove descriptors with no references.
Retired connection memory, process handles, routing events and transport pipes
are not kept alive for display. No retained history after last association.
An actual new root generation never adopts old children by matching a PID.

Do not change shutdown merely to make MISSING visible. Existing NTSRV loss
notifications still close associated workers; brief intermediate states may
require a deterministic controlled fixture. S2 must verify each actual bind
and rundown hook rather than assume initial worker Connect covers all paths.

For actions, authenticate and resolve a stable key under the service lock,
pin exact process/event references, set the existing closing/requested state,
then wait outside the lock. Re-resolve only by that key, never current PID.
Frontend action uses existing frontend_closing and lifetime state notification;
completion distinguishes request accepted, restoration acknowledged and root
actually exited. Timeout/failure is not success or permission for a tree kill.
Worker action retains native Console-session ack and original DOS/WOW close.
GUI action targets only the registered actual GUI process; preserving its
identity/query/terminate capability must not replace native completion owner
or its startup-only/explicit --wait semantics.

### Final owner clarification: no UNBOUND; preserve real ownership

The earlier proposed UNBOUND peer layout is superseded by the owner's final
approval. Win16 workers appear at the same level as NTCON; their actual
Win16 tasks appear underneath them. Registered Win32 GUI targets whose carrier
occupancy has been released appear at top level independently; never force a
carrier to remain alive or attach its later text work to an old GUI program.
Original WOWRecord execution, dispatch, completion and cleanup still belong
to NTVDM/WOW. Service-owned parent identity reflects that actual ownership,
not process ancestry reconstructed by NTMON. Known former frontend roots
remain MISSING while associated watches survive, not independent nodes.

The original [srvvdm.c](../../../src/opennt-host/base/win32/server/srvvdm.c)
allocates WOWRecord with iTask, not a per-task Windows process handle.
BaseSrvGetWOWTaskId wraps and avoids currently live IDs, not historical IDs.
WOW_TASK therefore needs a stable admission identity retained with the
original record lifetime in project-owned management metadata, not task ID
alone. Do not change guest task IDs or original completion ownership.
Do not invent a task PID, process age or independent process capability.
Render unavailable process fields explicitly; retain executable labels before
original command delivery releases lpVDMInfo, as management metadata only.
Worker rows keep their real PID and original task depth; task rows do not
multiply that depth or count as additional workers/frontend users.

The current management close endpoint terminates the actual WOW worker,
not an individual guest task. Per-task DEL must not silently invoke it.
Unless S2 proves an existing supported targeted WOW-task close path, expose
the task row as read-only with an explicit unavailable action. This audit
does not authorize a new guest task-kill mechanism or execution policy.

## Exact downstream verification inventory

These are required future assertions, not passing results of this S.

| Test entrypoint / proposed case | Required assertions |
| --- | --- |
| tests/adapter-basesrv/base_service_reservation_test.c --management-tree (new) | Two roots, DOS/native mixed children, top-level WOW worker with executable children, independent GUI row, pending/blocked BUSY and resident IDLE; no UNBOUND, fake per-task PID, stale wrapped-task identity or mutation of task waits/results; released pipe retains association. |
| Same fixture --management-missing-root (new) | Controlled root rundown while exact watch lives; old PID MISSING with child, new generation isolated, final watch removal prunes descriptor; allocation failure/rollback and service Stop leak checks. |
| Same fixture --management-close (new) | Stable worker/root/GUI selectors; stale instance/key/PID replacement rejected without signaling replacement; root restoration and process-exit barriers; native close failure; GUI DEL leaves carrier and unrelated GUI/WOW alive. |
| Existing fixture default, --native-worker, --native-command, --io-authority, --frontend-authority, --wow-start-late-query | Existing actual DOS/WOW record, native receipt, root authentication, blocking/reentry and stop/closed tests remain; do not weaken old assertions when row counts expand. |
| tests/adapter-basesrv/monitor_rpc_test.c | Actual RPC populated/empty snapshot, generation/type/instance negative selectors, malformed/version/application mismatch; peer/session authorization, no management-only connection pins. |
| tests/app/common_management_test.c | Production common client preserves RPC error/exception, empty outputs and allocation cleanup; forwards full stable key unchanged; malformed input invokes no RPC. |
| tests/observation/monitor_layout_test.c | Real renderer cells: hierarchy/STATE/elapsed, exact title/footer, long paths/scrollbars, missing roots, independent WOW/GUI nodes without UNBOUND, stable selection after reorder/removal, no stale confirmation. Add production input-dispatch cases for UP/DOWN/DEL/ESC and no-op missing roots. |
| tests/observation/verify-monitor-layout.ps1 | Repair stale src/interface IDL and missing common-rpc link dependencies before use, or replace with formal Ninja fixture target; all products/intermediates under build. Existing script is not a valid current build command. |
| tests/observation/verify-ntvwm-management.ps1 and verify-native-gui-routing.ps1 | Extend actual serial package checks to distinguish GUI target from carrier; native direct completion/reuse unchanged, two Console sessions cannot cross-control. |
| tools/audit/Invoke-ProductVerification.ps1 plus existing lifecycle/RPC probes | Retain Console17/Window17, WOW frontiers, nested DOS/native both ways, loss/cleanup and version negatives; coherent eight-file package hash/publication. No parallel global-endpoint/Z product matrices. |

Monitor fixture currently includes its production main.c, not a copied fake
renderer. Preserve this selected real renderer or move a bounded UI mechanic
to an NTMON-private testable module; do not duplicate the service/provider.
Common client unit mocks prove transport/error contract only. Missing-root
fixture is controlled lifetime evidence, not proof of visible RDP behavior.

## Stage disposition and limitations

S1 completes this bounded source contract/inventory. S2 implements service
projection, minimal tombstones, stable action/client/IDL and focused negatives;
S3 implements NTMON layout/input/selection, S4 actual integration, full retained
gates, coherent publication and handoff. Each stage is admitted only in CURRENT.
Current proposal's src/interface wording is superseded by delivered
common/protocol ownership, not permission to recreate interface.

No production defect fixed or runtime capability newly verified here. No
original mirror touched, so there is no new mirror diff to normalize. Existing
GUI termination lacks retained rights; existing PID-only action remains until
S2 wires the stable selector. Unknown transitional states, snapshot race and
hotkey discrepancy are covered requirements, not silently accepted behavior.
Future task traces, hooks and naming unification remain separate candidates.
