# M0 T448 S1 lifecycle source audit

## Question, inputs and method

At 7917ef25a, does every managed component obey service-only lifecycle authority,
and can service-loss cleanup preserve Console return without endpoint signalling?
Read current authorities, proposal and selected source functions; use rg exit,
shutdown and return call-site sweeps. No product run, guest mutation, build or
runtime fix in this audit. Source facts are not runtime acceptance.

## Verified chains and disposition

Paths below are repository-relative. Functions identify the reviewed baseline;
line numbers are not a substitute for source identity.

| Chain | Baseline evidence | Disposition |
| --- | --- | --- |
| Run16 startup | src/run16-exe/main.c connect_broker: 100 iterations, Sleep(50), create attempts at 0/20/60 | Replace iteration budget with true total 10s deadline; preserve singleton/version rules |
| Native admission | src/run16-exe/frontend_scope.c wait_worker_change waits worker/root handles; launch_native retries NOT_READY | Remove peer liveness decisions; service owns readiness/failure; no replay after acceptance |
| Native completion | native_commands.c FinishNativeRequest consumes result separately from frontend_registry.c WaitFrontendConsoleRestored | Join final eligibility in service, preserving real outcome separately from infrastructure error |
| DOS parent return | src/run16-exe/main.c launch completion calls run16_frontend_scope_resume_parent only when task_completed | Move coordination trigger to service-owned completion relation, not launcher-local branch |
| Frontend parking | src/ntcon-exe/session_service.c frontend_pump observes creator, queries usage and calls frontend_session_park | Service explicit park/return command; NTCON reports physical facts only |
| Management close | src/ntsrv-exe/opennt/source/lifecycle.c OpenNtBaseServiceTerminateManagementNode runs waits after accepting draining; background lifecycle also advances it | One service-owned operation independent of RPC caller, absolute deadline and phase-qualified ack |
| Return ack | frontend_registry.c FrontendConsoleRestored distinguishes preparation from final, but relies on root flags/event rather than per-close phase identity | Retain separation, make stale/duplicate semantics explicit and join result publication |
| Worker close dispatch | src/worker-base/input_watch.c delivers close callbacks as part of input watch; shutdown_close.c runs cleanup then TerminateProcess | Split generic lifecycle dispatch from input and keep executable-owned cleanup semantics |
| WOW missing receiver | src/ntvdm-exe/softpc/mvdm_standalone_worker.c skips begin_character_io for -w; win32/console_client.c alone creates close input_watch | Bind lifecycle reception independently; never initialize character frontend to repair WOW |
| NTVDM close timing | console_client.c console_input_console_close uses 5000; worker close uses INFINITE | Propagate remaining 10s operation budget, preserving original close handler distinction |
| NTVWM normal/fault exit | src/ntvwm-exe/main.c run_gui_launcher returns after local cleanup; fault callbacks TerminateProcess; CUI loop errors break into cleanup | Report delivery/fault, obtain service disposition; GUI target must not be coupled to launcher exit |
| Global broker death | base_rpc_client_core.c broker_lifetime_watch immediately TerminateProcess(...1722) | Transport publishes loss only; executable performs role-specific orderly cleanup |
| Frontend concurrent death handling | session_service.c independently waits broker; frontend_service_close joins pump/channel and restores | Shared hard-kill watcher can race local cleanup; eliminate competing exits and propagate failure |
| NTMON request | src/ntmon-exe/main.c close path uses common_rpc_close_management_node | Retain ordinary caller; acceptance must survive requester death, no local completion dependency |
| Reservation rollback | src/ntsrv-exe/opennt/source/base_reservation.c Abandon kills only unclaimed retained worker | Preserve startup rollback versus admitted lifetime distinction |
| Original exit policy | src/opennt-host/base/win32/server/srvvdm.c BaseSrvExitVDM dispatches DOS/WOW record cleanup | Retain original task result/record owner, not a substitute whole-process handoff barrier |
| Original host termination | src/mvdm/softpc.new/host/src/nt_reset.c host_terminate calls ExitVDM then ExitProcess; nt_event.c CntrlHandler handles Console close; wow32/wkman.c has task and ALL_TASKS exits | Management receiver alone cannot cover spontaneous/fatal exits; selected call-site/build binding audit must finish before S1 closure |

## Source-first decision

Original CntrlHandler retains Console-specific close policy; TerminateVDM retains
host cleanup ordering; BaseSrvExitVDM retains DOS/WOW task-record distinction.
These are not interchangeable callbacks. Reuse them behind existing project
bindings instead of moving guest scheduling into NTSRV/worker-base.
Original NT4 does not provide this product's independent NTCON and Run16 return
transaction. Its unavailable Console Server/CSR resource mechanisms are stopping
boundaries, not justification to import CSRSS. Existing frontend_registry and
lifecycle records are the smallest admitted project-owned carrier to consolidate.
The retained selected Ninja graph compiles nt_reset.c into the NTVDM original
host archive. ntvdm.exe.map confirms host_reset/host_terminate; wow32.dll.map
confirms WK32WOWInitTask, W32HungAppNotifyThread, WK32ExitKernel, WK32FatalExit
and WOWStartupFailed. Their original exit sites form a finite seven-call,
three-file hook budget described in the plan. nt_term.c is not selected by that
host archive; it is not another production implementation to patch.
nt_timer.c VdmUnhandledExceptionFilter is a selected catastrophic exception
path and must not wait for healthy-service orderly permission. A symbol/import
map is selection evidence, not runtime proof of cleanup or held-lock safety.

The existing ExitVDM binding alone cannot supply explicit worker-exit intent:
original BaseSrvExitVDM also handles individual task record cleanup. Do not
infer whole-worker death merely from its task number or call name. Preserve
those original calls and add the minimal explicit exit-intent seam at the
seven process-exit sites if no equally precise owned binding can compose.
The service may grant carrier exit before final Console restoration, while
holding the separate launcher completion; it never waits for the exiting
worker inside that worker's permission RPC. Source lock/reentry behavior is
an explicit implementation test obligation, not yet runtime-proven.

## Service-loss ordering counterexample

The same service process death wakes Run16 and NTCON. Schedule Run16 first and
delay NTCON: the present watcher kills Run16, its parent enters ReadConsole,
then NTCON restores/selects the canonical buffer. Mode bits may be correct while
the pending read belongs to another buffer. This is a source-supported possible
ordering, not a newly reproduced runtime trace or attribution of every CAF failure.

Without post-loss information from NTCON, Run16 cannot distinguish that schedule
from one where restoration already completed. A fixed sleep changes probability,
not correctness. A service-created fence signalled by NTCON would establish the
required ordering but is still cross-component synchronization. Owner explicitly
rejected that proposal: Run16 must return the original service-loss error without
depending on NTCON. The [working design](../operations/t448-lifecycle-plan.md)
now records that decision, not a pending exception. Loss return does not assert
that another component has finished restoring the Console.

## Original failure provenance and owner correction

Read-only OpenNT reference base/ntdll/csrutil.c CsrClientCallServer preserves
failed NtRequestWaitReplyPort Status in m->ReturnValue and returns it.
base/ntos/rtl/generr.c explicitly maps STATUS_PORT_DISCONNECTED to
ERROR_INVALID_HANDLE; public/sdk/inc/winerror.h defines that error as 6.
The selected vdm.c BaseSetLastNTError seam uses RtlNtStatusToDosError.
This is evidence for the disconnected-port API error, not a universal original
Run16 exit contract (OpenNT has no such launcher).

Current project broker_lifetime_watch instead hard-exits with 1722, and
base_rpc_client_launch_style.c rpc_failure drops its input status. Original
BaseCheckForVDM also has failure returns that do not set LastError. The finite
L07 repair therefore requires explicit failure propagation, not stale last-error
reads or a blanket replacement of all failures with error 6. No runtime change
or failure-injection verification is claimed in this source/design correction.

## Review verdict and remaining S1 boundary

The existing healthy-service design can be consolidated without new helpers or
guest changes. No runtime fix is delivered. The pending owner choice is resolved:
strict service-only control, independent loss cleanup and original error return;
no recovery fence. S1 design delivery still requires documentation verification
and reviewed commit/push, not product runtime acceptance.
The selected original exit call-site/build-binding audit now gives the bounded
three-file/seven-call design budget above; implementation still must prove
the lock/reentry and failure behavior before any runtime closure.

The ten finite L01-L10 groups in the working design retain all identified work.
Do not broaden this to unrelated WOW controls/messages. The existing package
and its T447 runtime evidence remain unchanged; documentation checks cannot
certify lifecycle behavior. Implementer/reviewer are the same person in two
passes, with no delegated agent.

The owner-correction documentation gate passed in background PID 32740,
2026-10-21 02:02:23-02:02:41 UTC: Verify-DocumentationGovernance and diff --check,
exit 0. This gate covers documentation only; no product build or publication
was needed for this zero-runtime-diff correction.

## Final owner-approved design disposition

Owner approved all six review corrections and execution through T pre-closure
acceptance. Finalization timeout returns Run16 through local failure cleanup,
not indefinite retention or a peer wait. Submission is single-attempt. Service
phases depend on actual scope, not a universal Console teardown. Task completion
cleanup remains original; whole-worker cleanup requires prior service authority
and cannot tear down the control connection before that decision. A received
close command is already authority. S2 includes basic fault exits; S3 extends
their full matrix. Win16 default completion is startup confirmation, so its later
task exit cannot release the already departed launcher. These are design closure
conditions, not measured runtime capabilities; the finite L01-L10 work remains.

Final design governance and diff check passed in background PID 22456,
2026-10-21 02:28:32-02:28:51 UTC, exit 0. Single-person reviewer checked the
six corrections, startup-only Win16 return and no peer-dependency rule against
the actual design diff. S1 reaches its design-only conclusion; runtime work
continues under sequential S2 admission, not a product acceptance claim.
