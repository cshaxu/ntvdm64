# T434 S1 — Read-only worker task trace design

## Status and baseline

Owner admits the former Queue head as T434 and requests design. S1 is audit/
design only; no implementation or product delivery is claimed. T433 closure
47d78555e and S7 package d1aa8336c/31a1487e4 remain APP433/RPC41/I/O25,
six AMD64/four I386 images at O:/winnt/system32. Do not bump versions or replace
runtime files for this design. S2 requires design review/admission.

## Two independent authorities

Execution remains existing DOSRECORD/WOWRECORD/native Direct Win32Records:
admission, original/native waits, results, BUSY/READY/EMPTY, I/O and retirement.
Observation is one NTSRV-private sidecar associated with the existing worker/
service instance. Direct rows are projected from authoritative records; only
observed facts/history/gaps live in the sidecar. No observed node is appended
to an execution record chain or completed into a run16 receipt. No second stack,
scheduler, kill policy or worker-residency counter is created.

NTCON is unchanged. NTMON's ordinary tree, Direct STACK/TASK, kind labels
DOS0/WIN16=1/WIN32=2/WIN64=3 and Delete/ESC behavior remain. Enter on a worker
opens a separate read-only detail modal; ESC closes that modal. Observed rows
have no Delete action. Do not count observed descendants as Direct depth.

## Current-source audit and proposed ownership

| Logic | Confirmed source/current behavior | Planned change |
| --- | --- | --- |
| Native create/inject/resume | nthook32-dll/create_process.cpp: finish holds actual PROCESS_INFORMATION and installs before ResumeThread; A/W entrypoints and TLS recursion/debug exclusions. | Add creation publication at this existing successful boundary. No failed Create/injection rollback node, no additional suspension, no replacement process or changed handles/flags/waits. |
| Native report protocol | common/protocol/native_hook.h is bootstrap only; service.idl has no child-create report. | A new copied common/protocol trace/report contract and finite client are required; no claim that delivered Hooks already report creation. |
| Reporter authentication | ntsrv-exe/transport/rpc_security.c: session/logon privacy authentication plus RPC actual PID and real caller process. | Reuse it; verify reporter against a pinned Direct root/previously accepted observed process. Never trust bare PID or worker key alone. |
| Root process identity | native_commands.c BindNativeTarget validates the real suspended target; CUI does not currently retain its process for observation. | Optional trace owner takes its own minimally privileged reference at binding, before target resume. Original Direct completion remains with NTVWM. |
| Exit notifications | Existing NTSRV process waits and GUI waits are event-driven. | Sidecar owns observation-only waits and cleanup, not execution waits or receipts. |
| DOS successful entry | dos/v86/doskrnl/dos/msproc.asm:1371 SVC_DEMENTRYDOSAPP before transfer; DEM dispatch to demmisc.c demEntryDosApp. | Prefer existing VDD user callback registration, not another guest BOP or moved original algorithm. |
| DOS ordinary termination | msctrlc.asm:977 SVC_PDBTERMINATE; demsrch.c demTerminatePDB invokes VDDTerminateUserHook before normal cleanup. | Observe through the same original callback; copy bounded PSP/parent/image fields only. |
| Existing VDD mechanism | nt_msscs.c VDDInstallUserHook/Create/Terminate are present in actual ntvdm.exe map; IsFirstCall suppresses some callbacks. | NTVDM-owned collector registers one local callback instance and deregisters safely. Prove root correlation/initial entry coverage before promising zero mirror diff. No VDD DLL/helper is needed. |
| TSR | msctrlc.asm skips PDB termination for EXIT_KEEP_PROCESS; no proved terminal host event. | Expose terminal uncertainty. Do not time out into EXITED or pretend residency/foreground-return is fully observed. |
| Monitor projection | management.c uses Direct records for native STACK/TASK; ntmon/main.c uses service keys and snapshots. | Query copied trace by existing generation-safe management key, not a new PID-only selector or local enumeration. |

## Native event flow

1. Existing NTVWM Direct bind supplies the root's real process reference to
   NTSRV. Sidecar binds it to the existing Direct identity; no second receipt.
2. Controlled parent Hook already has a real successfully created suspended
   child. After compatibility installation succeeds and before its own resume,
   publish the typed child process attachment, actual creator and copied facts.
3. NTSRV authenticates the real RPC reporter and matches its pinned process
   instance in the sidecar. It registers the child relation and wait before
   acknowledgement; a quickly exited child is still identified by its handle.
4. Hook preserves original CREATE_SUSPENDED. Without that flag it resumes by
   its existing transaction. Report failure never becomes execution failure.
5. NTSRV observes the registered process signal and records observed exit.
   Windows parent waits/results and NTVWM Direct completion are unchanged.

Prefer stateless finite observation RPC for injected reporters: bind/authenticate
to the existing broker, not control Connect/register/GetNext. A persistent
execution connection would itself affect empty-service retention. No new
observation token is initially required: pinned root identity and pre-resume
parent registration authorize the next known reporting process. Reassess only
if implementation proves an unavailable boundary; do not add a capability
framework merely for resemblance to the frontend bootstrap.

Report calls must have a bounded/cancellable failure contract and no unbounded
retry. Freeze a measured low-latency budget during implementation review, not
the product's ten-second launch grace. Monitoring failure must preserve launch
success and mark coverage gaps on the next accepted publication/query. Missing
reports cannot be magically reconstructed; report loss itself may be uncertain.
Never publish RPC or perform waits from DllMain/under the loader lock; use only
the existing successful CreateProcess transaction outside that boundary.

Creator relation is distinct from a Windows effective parent override. If
PROC_THREAD_ATTRIBUTE_PARENT_PROCESS chooses another parent, do not describe
the reporter as a proved OS inheritance parent. Unsupported/debug launch APIs,
uninjected processes, missing reports and security boundaries are gaps. No Job,
process-tree scan, Console member sampling or DLL-detach completion fallback.

GUI descendants keep observation association if admitted, but never receive
text I/O authority because of observation. Their existing GUI execution remains.
Native-to-legacy redirected run16 is a launch bridge, not a fake duplicate guest
process: preserve the existing Direct admission as a separately proved boundary;
do not invent cross-worker parent links from matching names/timestamps.

## DOS and WOW event flow

DOSONLY=0 uses existing broker Direct admissions where actually present. Its
entry event is correlated to that Direct root without duplicating it. DOSONLY=1
uses original INT21h EXEC within the worker; successful child entries and
ordinary PDB termination become OBSERVED facts, never new Direct records.

Use worker identity plus PSP plus occurrence, because PSPs are reused. Parent
links require a copied valid parent-PSP and known occurrence. Image identity
requires a proved original source/environment/path, otherwise unknown. Do not
hold guest pointers across callbacks or RPC; callbacks copy under a bounded
guest lease and enqueue into a bounded worker-owned outbox. Shared encoding/
publication mechanics may live in worker-base; PSP extraction and original
execution stay NTVDM-owned. Queue overflow is an explicit coverage gap, never
guest blocking, allocation-driven scheduling or guessed completion.

Entry-only nodes have last-confirmed entry and unproved terminal state; an
unreported TSR must not be presented as definitely still executing forever.
Failed EXEC, overlay, load-only and COMMAND builtins do not become running nodes.
Confirm abort/control-break source behavior independently. Existing first-call
suppression and Direct/PSP correlation are explicit S4 proof obligations.

WOW Direct tasks can be projected from existing WOW records. Do not treat DOS
PSP callbacks as Win16 task identity or promise internal WOW task tracing from
them. Unproved WOW-only observed transitions stay unavailable for future WOW
source work; this T does not rewrite the WOW scheduler.

## Snapshot, ordering and cleanup

Query uses existing service-instance/category/generation/object management key.
NTSRV allocates opaque trace node IDs and one snapshot revision; PID/PSP are
display/source facts, not selectors. Fields: proved parent/creator, Direct or
Observed, existing kind, last-known state, image, optional PID/PSP, timestamps,
source, revision and coverage/uncertainty flags. Native reporter sequences are
per reporting process; no fictitious total chronology across concurrent threads.
Server commits CREATE before enabling its exit callback publication. Retained
process identity prevents PID reuse/reopening errors.
Process existence does not prove that it is running rather than suspended;
preserve caller-requested CREATE_SUSPENDED as a fact, not a guessed run state.

Callback only signals/queues an observation update; snapshot mutation remains
under the sidecar/service lock. Cleanup detaches under lock, drains registered
callbacks outside that lock, then closes references. Never close a pending wait
handle or synchronously unregister while holding a lock the callback needs.
Refs, nodes and copied strings are bounded; truncation/gaps are explicit. Dead
worker snapshots are stale/unavailable and cannot expose replacement workers.
Late observed exit may complete trace history after its Direct root returns,
without keeping the worker/frontend/service alive.

Windows wait details checked against
[RegisterWaitForSingleObject](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-registerwaitforsingleobject)
and [UnregisterWaitEx](https://learn.microsoft.com/en-us/windows/win32/api/threadpoollegacyapiset/nf-threadpoollegacyapiset-unregisterwaitex).

## Stage sequence and proof

| Stage | Bounded delivery |
| --- | --- |
| S1 active | Source/contract design; honest missing reports, VDD reuse/TSR/first-call limits, identity/failure/lock boundaries. No runtime claim. |
| S2 | Direct-only NTSRV copied trace query and minimal monitor access, tied to existing keys/records; stale/forged/cross-worker/root and lifecycle invariance tests. |
| S3 | New controlled Hook creation reports and NTSRV observation waits, both widths, short-lived/concurrent/suspended/forced-exit/surviving children and missing-report negatives. No launch/result/lifetime change. |
| S4 | NTVDM original callback collector, DOSONLY on/off, nested EXEC, parent correlation/reuse and truthful TSR/unsupported states; prove mirror budget before any intrusion. |
| S5 | Read-only NTMON modal, source/revision/gap hierarchy, multi-session faults, full retained mixed batches/Console/Window/WOW and coherent delivery audit. Stop before owner T acceptance. |

Actual copied protocol changes synchronize APP protocol/IDL major and regenerate
MIDL; I/O version changes only if its unchanged frame/input protocol changes.
Application advances to434 at first production delivery, not S1 design. Each
code-bearing S connects its slice to production, builds the correct native/
I386 consumer islands, tests failure and cleanup, publishes the coherent package
and commits/pushes. No disconnected implementation accumulated until S5.
