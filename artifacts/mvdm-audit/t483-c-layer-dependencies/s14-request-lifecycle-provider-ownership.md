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
