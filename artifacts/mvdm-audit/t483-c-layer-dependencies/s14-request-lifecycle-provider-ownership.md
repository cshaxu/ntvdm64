# Request and worker lifecycle provider ownership

## S14 P48

Baseline db7168805. Eight complete bodies: nthook-dll/hook_classification.c;
ntsrv-exe/opennt/source/base_interactive.c, base_process.c, base_request.c;
ntvdm-exe/monitor/source/monitor_printer.c; worker-base/connection.c;
ntvdm-exe/win32/worker_lifecycle.c; ntvdm-exe/softpc/mvdm_standalone_worker.c.
service_core.c was only partially read and is NOT counted.

| Provider | Ownership decision and migration constraints |
| --- | --- |
| Hook classification | Original Base classifier stays C. The three suffix descriptors consumed by it are composition storage, not hook behavior; consolidate original-owned descriptors independently of nthook/Run16 entry policy. Native process-image query uses common/native_image and the hook selection policy remains F. Do not replace the classifier with extension guessing. |
| Interactive scope | Visible-windowstation/process-token lookup and explicit LUID scope are B-compatible native mechanics, without NTSRV task knowledge. Authentication and scope admission remain outer. Preserve rejection of SYSTEM LUID and absent/invalid bindings; do not substitute a wire-provided LUID. Public SDK types suffice for the native resource portion; do not introduce B dependency on the current F header. |
| Process registry | Lock and duplicated-handle resources can live B, but the body directly updates original CSR_PROCESS fields, ListLink and SequenceNumber, declared by C ntcsrsrv.h. That original-record manipulation needs a C owner, not a private copied structure in B. Authenticated registration and role/task selection remain outer. Keep lock held from CsrLockProcessByClientId through CsrUnlockProcess, generation matching and same-access handle duplication. |
| Request context | Borrowed thread-local binding does not authenticate or allocate a CSR thread. Its independent storage can use B's existing context; typed interpretation remains C and trusted bind/restore remains outer dispatch. Preserve nesting/restore on every exit, without creating a second global request registry. |
| Printer state | The three functions manipulate original VDM_TIB printer fields and call NtVdmControl on direct-access close. This is printer state, not hardware-V86 CPU emulation. Existing C nt_lpt.c is a consumer; do not reimport the excluded monitor tree. C state ownership and the selected direct-close provider must be resolved together; buffer bounds and VDD ownership are not changed by this review. |
| Worker connection | This is E orchestration over D authenticated BaseClient: acquire grant, wait for frontend, close endpoints, then release acknowledgement. It cannot move to B. A failed granted connection must not claim release; a host-handle helper alone cannot own the protocol transition. |
| Worker lifecycle | NTSRV close instructions, broker-loss handling, task-ticket dispatch and sole cleanup claim stay F/E. Original CntrlHandler/TerminateVDM remain C cleanup. Authorize-exit waits for service authority, joins VM I/O before resource disposal and prevents competing cleanup threads; do not move this state machine into B or replace authorization with a timeout. |
| Standalone worker | Startup and teardown composition stays F. CPU, observer, memory, registry, frontend/executor and WOW resources have distinct owners. Stop/join producers before destroying frontends; retire WOW while guest memory remains available; only afterward unbind/dispose. CsrPortHeap storage is consumed by original C separately from the main entry. A single file target does not justify moving the whole worker core inward. |

Supporting evidence: srvvdm.c calls CsrLockProcessByClientId; exports.c calls
NtUserTestForInteractiveUser; nt_lpt.c calls printer initialization; nt_cpu.c
calls NtvdmActivateLifecycle. base_server.h replaces original request-thread
lookup; CSR layouts remain in non-mvdm/public/sdk/inc/ntcsrsrv.h. The current
worker startup invokes original main under its termination escape and cleans up
after exit authorization. Fixture absence of lifecycle owner is an explicit
current branch, not proof that production authorization can be omitted.

Migration design must separate independent resource operations from original
record interpretation and outer dispatch. Existing C carrier selection and
mirror cost remain required for record/state operations; moving declarations
or callbacks is not closure. No product code changed, no runtime tests run,
and no ledger IDs eliminated. Eight bodies cover fifteen open symbol records.


## S14 P49: complete service, launch transport and Run16 bodies

Baseline abaf38657. Full reads now include ntsrv-exe/opennt/source/service_core.c
(previously partial), common/base_srv/base_rpc_client_launch.c and
run16-exe/main.c. This adds three provider files/six symbol records, not
eliminated dependencies. Production and mirror changes remain zero.

| Chain | Concrete migration decision |
| --- | --- |
| Service core | Keep NTSRV connections, reservations, authenticated frontend association, process watches and shutdown orchestration outer. The one direct-ledger symbol in this file is not authority to move the service core. UserNotifyProcessCreate is a null original optional hook: recover its original storage ownership separately, without importing NTSRV management into C/B or inventing a callback. |
| Connection versus process lifetime | Disconnect abandons connection work but does not call original VDM cleanup for a still-running resident worker; the retained process watch owns that event. Registration derives PID/generation from retained handles. Route matching prefers exact authenticated process grant before Console fallback. Preserve late-binding close transactions, not just list membership. |
| RPC launch transport | Request/reply generation checks, typed stream attachments, parent receipt and broker protocol remain D/outer. Original BASE interpretation should be separated at C's call boundary, not move this whole RPC file to B. Independent byte/handle mechanics can reuse existing A/B providers, with no second authenticated connection. |
| Attachments and waits | Preserve stdout/stderr alias receipts and revoke each distinct receipt once. Unconsumed returned handles are closed even on cancellation. WaitObjectForVDM duplicates are retained as one ConsoleRecord event until ExitVDM, distinct from a launcher's parent event/receipt. Receipt consumption on exit-code retrieval must remain tied to its matching parent handle. |
| Command delivery | observation_direct is stored on actual delivery and is one client-level value, not a permanent identity for simultaneous guest programs. command_ready executes only after a successful actual command delivery. Neither field can become completion authority during architecture migration; earlier multi-task identity limits remain explicit. |
| Run16 | CLI resolution, frontend selection, broker bootstrap, submission and receipt waits stay F. Original BaseCheckVDM and environment projection remain C. Suffix descriptors and CsrPortHeap are separately identifiable original-facing storage; their ownership can be moved without moving the CLI entry. |
| Launch sequencing | Preserve matching ANSI/Unicode environment projection and child-only WIN16DIR, leaving host SYSTEMROOT intact. Existing resident-worker reply waits on parent receipt rather than creating a worker. After service startup is attempted, NTSRV owns rollback; before that boundary the launcher uses original undo-creation. Do not duplicate both cleanup paths. |
| Completion and readiness | WOW startup readiness is re-read after waits to handle completion racing publication; task exit zero is not automatically InitTask success. Worker-process death is not a substitute for a task receipt. Bootstrap retry is bounded before submission; it cannot be reused to replay an admitted request. Numeric task exit codes remain distinct from latched transport failure. |

Implementation order: separate original-owned storage from product entry units;
retain C classification/record algorithms; retain authenticated transport and
request lifecycle outside; design the narrow C-to-host operation boundary
without importing the broker or hiding an outer callback in B. The present
bodies prove these responsibilities, but do not yet provide a reviewed patch
or measured mirror cost. Those remain required before relocation. No new
runtime or failure-path verification is claimed by this source review.
