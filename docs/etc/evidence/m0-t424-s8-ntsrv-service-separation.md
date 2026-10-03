# T424 S8 NTSRV private service separation

## Question and accepted boundary

Separate project-added service responsibilities without relocating original
OpenNT/MVDM execution, scheduling, completion or cleanup. This is a physical
source reorganization of one existing provider, not a new service framework.
The accepted baseline is S7 production `d83d2b212`, application 0.0.424,
protocol/RPC32. The owner approved six private modules and a retained original-
interface adapter; there is no public ABI or lifecycle policy change.

## Inputs and procedure

The S7 lexical inventory was reviewed against the complete 4,545-line service
carrier and pinned original `srvvdm.c`, `srvinit.c` and client `vdm.c`. Each
definition receives one owner below. All 137 complete bodies move unchanged;
signatures change only `static` linkage for actual private cross-module calls
and the process-watch callback. The eight existing private state layouts are
lifted unchanged into `service_internal.h`; the local VDM admission structure
stays in `worker_registry.c`. Comments/includes/prototypes move with their
implementation; the guarded USER hook definition moves to service core.

`tests/component-integration/verify-ntsrv-service-separation.mjs` compares every
body and signature with the accepted commit, verifies eight exact state layouts,
original/public contracts and one fixture provider. Its negative controls
reject changed bodies/signatures, duplicate providers and missing modules.
The service fixture no longer includes `base_service.c`; its two queue/take
hooks call the actual linked production archive. The production build selects
all seven units in the existing opennt-base-bindings archive. No old duplicate
implementation survives in the carrier or test translation.

The detailed source/body hashes, baseline line numbers and dispositions are
retained under build/M0-T424/S8/r001/service-dispositions-r002.json. The durable
table below records every definition, origin boundary and target owner.

## State, resources and lock contract

| Block | Owner and reason |
| --- | --- |
| Service instance / connection / native record / frontend route / worker watch / management label / Console context / resource binding | Existing eight layouts, private header only. One explicit instance and one unchanged recursive service lock; no duplicated policy state. |
| Authentication and connection rundown | service_core; admission remains atomic under the existing lock. The worker-claim block stays inside Connect, avoiding a second admission authority or changed locking. |
| Worker reservation, process handles and watches | worker_registry; process watches retain their own handles. Callback calls original resource cleanup unchanged. |
| Frontend identity and capabilities | frontend_registry; pinned process identities are Console discriminators, never task/child observations. Existing attachment and Console-return acknowledgement remain unchanged. |
| Native payload, start/result events and direct receipt | native_commands; ownership moves through existing typed replies. Only actual target completion and final I/O can complete the existing receipt. No new scheduler or descendant tracking. |
| Retirement deadlines / shutdown request | lifecycle; existing ten-second service authority and explicit management termination remain here, not in projection or common. |
| Original DOS/WOW heads, records, locks and algorithms | src/opennt-host/base/win32/server/srvvdm.c remains owner. Management reads them under original locks; native records remain the project-owned direct counterpart. |
| Original-shaped resource/dispatch hooks and DOS results | retained base_service.c; calls original Check/Update/Get/ExitCode/Reenter/Exit and preserves parent result before original cleanup. Do not extract the original policy into a shared/new service module. |
| Connection watch drain | Connection rundown releases service lock before blocking UnregisterWaitEx; service stop remains after RPC/rundowns/watches drain. No new lock/wait policy. |
| Cross-owner lock order | Existing service lock before original DOS/WOW lock where both are needed. Helpers retain caller-held lock contracts; recursive calls do not acquire a second service state. |

Twenty-six existing helpers need private external linkage, including two fixture
queue/take seams and the function-pointer process-watch callback. This finite
boundary is not a public service/client API. Header consumers are only seven
service units and the test hook fixture; no common or worker dependency is added.

Accounting: 113 unchanged definitions move into six modules (9 core, 18 worker,
38 frontend, 19 native, 13 lifecycle, 16 management); 24 stay in the DOS/WOW
interface/resource carrier. Its raw size falls from 4,545 to 893 lines because
code/state moves, not because capability is deleted. No function body is added,
deleted or rewritten. Added private prototypes/comments/include organization
are not recovered original diff. Original mirror diff recovered/expanded: 0/0.

## Original mirror provenance

Read-only pinned upstream inputs:

| Original input | SHA256 |
| --- | --- |
| OpenNT/base/win32/server/srvvdm.c | c1e2177c6c00679d85cfa475f620841f6736b0e56d8dbf790b71afe33e1ed80b |
| OpenNT/base/win32/server/srvinit.c | f53d4ca6f7d3eee8945d94fc237f73b892fb0f509da8a47256043180b48bfc70 |
| OpenNT/base/win32/client/vdm.c | 3f03d0dbb08e0163f2d9cf415daad0981e42e1b1855f6f48a3b59022b7374173 |

The first and third selected mirrors have pre-existing substantive differences;
srvinit is byte-exact. S8 neither recovers nor expands that historical diff.
`git diff --exit-code d83d2b212 -- src/mvdm src/opennt-host` passes: all original
mirror changes in this P are zero. This statement does not turn three upstream
comparisons into a complete historical mirror audit. No guest/configuration,
shared KVM library, polling interval or wire version is changed.

## Complete function dispositions

Origin codes: **A** = project authentication/resource binding to original DOS/WOW
interfaces, retained in the carrier; original algorithms remain original-owned.
**P** = project-added registry/Windows resource/frontend/native receipt/projection
adaptation, moved physically. Calling an original helper does not make its
caller an original algorithm. No original algorithm is moved or reverse-called.

| Definition | S7 carrier line | Origin | Current target (opennt/source) |
| --- | --- | --- | --- |
| service_native_root_selectable | 201 | P | worker_registry.c |
| service_signal_frontend_states | 223 | P | lifecycle.c |
| service_signal_worker_states | 233 | P | worker_registry.c |
| service_delete_console_context | 241 | P | frontend_registry.c |
| service_root_console_matches | 251 | P | frontend_registry.c |
| service_delete_win32record | 273 | P | native_commands.c |
| service_release_console_identities | 280 | P | frontend_registry.c |
| service_copy_execution_console_members | 300 | P | frontend_registry.c |
| service_clear_pending_win32record | 336 | P | native_commands.c |
| service_clear_win32records | 343 | P | native_commands.c |
| service_launcher_connected | 375 | P | native_commands.c |
| service_release_launcher_results | 384 | P | native_commands.c |
| service_win32record_depth | 398 | P | management.c |
| service_next_win32record | 407 | P | native_commands.c |
| service_query_native_image | 415 | P | native_commands.c |
| service_delete_frontend | 428 | P | frontend_registry.c |
| service_clear_frontend_channel | 436 | P | native_commands.c |
| service_clear_frontend | 448 | P | frontend_registry.c |
| service_prune_cancelled_frontends | 492 | P | frontend_registry.c |
| service_preserve_parent_results | 508 | A | base_service.c |
| service_worker_terminated | 538 | P | worker_registry.c |
| service_wait_deliver | 601 | A | base_service.c |
| service_stream_deliver_to_connection | 617 | A | base_service.c |
| service_stream_deliver | 628 | A | base_service.c |
| service_worker_stream_deliver | 634 | A | base_service.c |
| service_wait_revoke | 638 | A | base_service.c |
| service_duplicate_resource | 648 | A | base_service.c |
| service_close_resource | 714 | A | base_service.c |
| service_resources_init | 728 | A | base_service.c |
| service_resources_release | 738 | A | base_service.c |
| service_wait_resolve | 743 | A | base_service.c |
| service_abandon_launch | 752 | A | base_service.c |
| OpenNtBaseServiceStart | 795 | P | service_core.c |
| OpenNtBaseServiceConfigureEmptyNotify | 841 | P | service_core.c |
| OpenNtBaseServiceStop | 854 | P | service_core.c |
| OpenNtBaseServiceIsEmpty | 887 | P | lifecycle.c |
| service_copy_management_text | 902 | P | management.c |
| service_copy_management_image | 910 | P | management.c |
| service_same_management_wait | 914 | P | management.c |
| service_clear_management_labels | 920 | P | management.c |
| service_find_management_label | 929 | P | management.c |
| service_prune_management_labels | 943 | P | management.c |
| service_capture_management_label | 968 | P | management.c |
| service_capture_management_label_from_info | 984 | P | management.c |
| service_capture_initial_management_labels | 991 | P | management.c |
| service_find_management_watch_for_console | 1006 | P | management.c |
| service_capture_checked_management_label | 1019 | P | management.c |
| service_copy_management_record | 1045 | P | management.c |
| service_sort_management_records | 1106 | P | management.c |
| service_copy_win32record | 1120 | P | management.c |
| OpenNtBaseServiceSnapshot | 1139 | P | management.c |
| OpenNtBaseServiceTerminateWorker | 1184 | P | lifecycle.c |
| OpenNtBaseServiceConnect | 1237 | P | service_core.c |
| OpenNtBaseServiceDisconnect | 1363 | P | service_core.c |
| OpenNtBaseServicePeer | 1420 | P | service_core.c |
| OpenNtBaseServiceAttachStream | 1428 | P | service_core.c |
| OpenNtBaseServiceRevokeStream | 1443 | P | service_core.c |
| OpenNtBaseServiceRetainPeer | 1454 | P | service_core.c |
| OpenNtBaseServiceRegisterFrontendRoot | 1472 | P | frontend_registry.c |
| OpenNtBaseServiceAcquireFrontendRoot | 1534 | P | frontend_registry.c |
| broker_frontend_admit | 1636 | P | frontend_registry.c |
| broker_frontend_clear_admission | 1660 | P | frontend_registry.c |
| OpenNtBaseServiceFrontendStartupResult | 1675 | P | frontend_registry.c |
| OpenNtBaseServiceStartFrontend | 1707 | P | frontend_registry.c |
| OpenNtBaseServiceReturnFrontendConsole | 1822 | P | frontend_registry.c |
| service_console_return_ack | 1848 | P | frontend_registry.c |
| OpenNtBaseServiceWaitFrontendConsoleRestored | 1859 | P | frontend_registry.c |
| OpenNtBaseServiceFrontendConsoleRestored | 1881 | P | frontend_registry.c |
| OpenNtBaseServiceCancelFrontendRootReservation | 1894 | P | frontend_registry.c |
| service_frontend_exited | 1908 | P | frontend_registry.c |
| OpenNtBaseServiceRegisterFrontendLease | 1917 | P | frontend_registry.c |
| OpenNtBaseServiceFrontendJoinCandidate | 1993 | P | frontend_registry.c |
| OpenNtBaseServiceFrontendJoinDecision | 2009 | P | frontend_registry.c |
| OpenNtBaseServiceFrontendLeaseReady | 2024 | P | frontend_registry.c |
| OpenNtBaseServiceRetainFrontendRoot | 2046 | P | frontend_registry.c |
| OpenNtBaseServiceRegisterNativeBackend | 2111 | P | worker_registry.c |
| OpenNtBaseServiceCompleteWorkerChannel | 2175 | P | native_commands.c |
| OpenNtBaseServiceCompleteNativeRequest | 2181 | P | native_commands.c |
| OpenNtBaseServiceBindNativeTarget | 2223 | P | native_commands.c |
| OpenNtBaseServiceNativeExitCode | 2264 | P | native_commands.c |
| OpenNtBaseServiceAcquireConsoleContext | 2296 | P | frontend_registry.c |
| OpenNtBaseServiceBindConsoleContext | 2354 | P | frontend_registry.c |
| OpenNtBaseServiceRetainCommandWorker | 2384 | P | worker_registry.c |
| service_attach_frontend | 2454 | P | frontend_registry.c |
| OpenNtBaseServiceAttachFrontend | 2514 | P | frontend_registry.c |
| OpenNtBaseServiceRequestFrontend | 2528 | P | frontend_registry.c |
| OpenNtBaseServiceFrontendUsage | 2580 | P | lifecycle.c |
| OpenNtBaseServiceRetireWorkerlessFrontend | 2626 | P | lifecycle.c |
| OpenNtBaseServiceWorkerShutdownEvent | 2641 | P | worker_registry.c |
| service_root_has_worker | 2662 | P | lifecycle.c |
| service_root_retirement_deadline | 2687 | P | lifecycle.c |
| service_root_workerless | 2712 | P | lifecycle.c |
| OpenNtBaseServiceFrontendLifetimeChanged | 2717 | P | lifecycle.c |
| OpenNtBaseServiceNextFrontendDeadline | 2721 | P | lifecycle.c |
| OpenNtBaseServiceRetireExpiredFrontends | 2736 | P | lifecycle.c |
| OpenNtBaseServiceFrontendStateChanged | 2787 | P | frontend_registry.c |
| OpenNtBaseServiceWorkerStateChanged | 2805 | P | worker_registry.c |
| OpenNtBaseServiceRetireFrontend | 2825 | P | lifecycle.c |
| service_frontend_idle | 2840 | P | frontend_registry.c |
| service_queue_native_command | 2853 | P | native_commands.c |
| service_native_result_record | 2930 | P | native_commands.c |
| OpenNtBaseServiceNativeStartupResult | 2949 | P | native_commands.c |
| OpenNtBaseServiceSubmitNativeRequest | 2996 | P | native_commands.c |
| OpenNtBaseServiceFinishNativeRequest | 3078 | P | native_commands.c |
| service_take_native_command | 3119 | P | native_commands.c |
| OpenNtBaseServiceGetNextNativeCommand | 3204 | P | native_commands.c |
| OpenNtBaseServiceFrontendRequest | 3240 | P | frontend_registry.c |
| OpenNtBaseServiceAttachFrontendRequest | 3277 | P | frontend_registry.c |
| OpenNtBaseServiceWorkerFrontendCapability | 3317 | P | frontend_registry.c |
| OpenNtBaseServiceTakeFrontend | 3347 | P | frontend_registry.c |
| OpenNtBaseServiceWaitFrontend | 3388 | P | frontend_registry.c |
| OpenNtBaseServiceFirst | 3407 | A | base_service.c |
| OpenNtBaseServiceRegisterWowExec | 3432 | A | base_service.c |
| OpenNtBaseServiceCreateReservation | 3466 | P | worker_registry.c |
| OpenNtBaseServiceCreateNativeReservation | 3478 | P | worker_registry.c |
| OpenNtBaseServiceSelectNativeWorker | 3506 | P | worker_registry.c |
| OpenNtBaseServicePrepareWorker | 3546 | P | worker_registry.c |
| service_retire_completed_root | 3567 | P | lifecycle.c |
| service_vdm_process_completed | 3623 | P | worker_registry.c |
| service_vdm_update | 3645 | P | worker_registry.c |
| service_vdm_admit | 3663 | P | worker_registry.c |
| OpenNtBaseServiceStartVdmWorker | 3720 | P | worker_registry.c |
| OpenNtBaseServiceStartNativeWorker | 3795 | P | worker_registry.c |
| OpenNtBaseServiceReleaseReservation | 3822 | P | worker_registry.c |
| OpenNtBaseServiceWorkerReservation | 3843 | P | worker_registry.c |
| service_bind_existing_console | 3853 | P | frontend_registry.c |
| OpenNtBaseServiceReportConsoleMembers | 3924 | P | frontend_registry.c |
| service_allocate_console | 3983 | A | base_service.c |
| OpenNtBaseServiceWowStarted | 3998 | A | base_service.c |
| OpenNtBaseServiceWowStartup | 4033 | A | base_service.c |
| OpenNtBaseServiceCheck | 4058 | A | base_service.c |
| OpenNtBaseServiceUpdate | 4214 | A | base_service.c |
| OpenNtBaseServiceExitCode | 4305 | A | base_service.c |
| OpenNtBaseServiceReenter | 4354 | A | base_service.c |
| OpenNtBaseServiceGet | 4389 | A | base_service.c |
| OpenNtBaseServiceExit | 4486 | A | base_service.c |
| OpenNtBaseServiceReleaseCommandReply | 4543 | A | base_service.c |

## Verification progress

Artifacts are under build/M0-T424/S8/r001; unchanged dependency cache is
build/M0-T424/S2/r001. The graph generator is invoked with explicit Node22;
build-products-r002 and build-tests-r003 pass x86 /MT CCPU40 links. The WOW
sub-build reports no dirty source/dependency; VdmTib storage check passes.
The eight candidates are staged from the same build plus immutable S7 media.
125 source/test/build inputs are frozen in release-source-manifest.json.

source-separation-r001 and ownership-r001 pass exact bodies/layouts, private
header consumers, seven actual archive providers and link-leak negative controls.
rpc-r002 passes all eleven real process fixtures: client, bootstrap, startup
rejections/timeout, native worker failure/completed worker loss, workerless
grace/cancel, monitor RPC, native command and native registry. All require
actual exit zero and assertion output, not merely observer termination.

matrices-r002 passes Console17 and Window17, 17/17 each, with original output
and exit assertions. retained-r001 passes DOS/native/DOS relaunch plus outer
cooked CMD exit 19, two independent Console sessions and all four worker/root
loss retirement cases. edit-return-r001 proves actual modern EDIT menus,
Ctrl+Q, CMD echo, DOS MEM and launcher completion. No synthetic delay or
redraw was introduced in production.

units-r001 records native RPC108/0, frontend RPC149/0, worker RPC164/0,
management72/0 and binding42/0, zero outstanding tested handles/allocations.
GetNext failure/disposal ownership passes. Execution lifetime850/0 retains
target survival and remaining-handles=0; frame415/0 and input677/0 pass.
Common I/O normal, broken-pipe and close-hang retain the authenticated original-
shape close callback and its existing bounded forced-close failure contract.

Preserved attempts: first product build found the cross-unit function-pointer
callback missing from the private declarations; fixing its declaration/linkage
did not alter its body. The initial fixture target list named observer.exe,
which is an existing separately built tool, not a graph target. The next link
was refused because an old console-client-test process still held that exact
binary; after checking its full path it was ended under owner authorization,
then r003 passed. rpc-r001 had CLI array-argument parsing failure before any
fixture; r002 supplies the actual array. matrices-r001 was refused by the
foreign-package guard while RPC fixtures still owned the global endpoint;
r002 ran after their completion. These are retained failures, not passes or
reasons to loosen guards/assertions.

versions-wow-r001 passes five actual mismatched-server variants (old app,
wrong protocol, wrong reply app/protocol, legacy RPC interface), each rejecting
both launcher and NTVDM with 1306 before delivery. WOW private-desktop window
samples match S7 frontiers: WINMINE visible guest main class/title, SOL's memory
dialog and WRITE's not-enough-memory dialog. All three observation timeouts
are explicit non-completion bounds, not usability passes. Foreground gameplay,
physical mouse/RDP acceptance and new GUI routing are not claimed.

publication-r001 validates the frozen inputs and all eight x86 candidate hashes,
backs up accepted S7 under accepted-s7-recovery, then publishes the exact set
to O:/winnt. published-manifest.json matches release-candidate-manifest.json.
No guest/configuration overwrite, alias rename, extra executable or component
occurs. Failure rollback restores the prior coherent eight-file set.
Postpublication r001 failed: the second `run16 command` was actually read by
outer CMD as `un16 command`; subsequent MEM ran outside DOS and CMD exited 216,
not the required 19. This is a real failed observation, not an observer timeout.
Unchanged probe/1,600-ms line rhythm and all original assertions pass in r002
and three consecutive r003-r005. The untouched S7 package passes three Z:
controls and three strict O:/winnt controls; its eight baseline hashes are
verified. The temporary exact-path control restores the entire S8 package in
finally and checks every published hash. No production code, test rhythm or
acceptance assertion was changed to obtain these passes.

The initial input loss is not attributed solely to a harness or to production;
controls do not reproduce it and do not prove it repaired. Its causal follow-up
is retained in TODO rather than hidden or broadened into this source-only
reorganization. Observer source sends each character separately and uses a
fixed inter-line rhythm rather than a per-command readiness barrier; this is
an investigation lead, not the demonstrated root cause. No permanent delay,
retry, injected refresh or input workaround is added.

Final gates recheck exact source/private boundaries, frozen inputs, publication,
relative links, documentation governance and staged diff. 113 bodies move and
24 adapters stay; all 137 bodies and eight layouts remain exact to accepted S7.
This P delivers the bounded source separation; original behavior and the
unattributed observation are reported separately. T424 remains open. S9 GUI
routing, frontend rename, and final audit remain separate, unimplemented stages.
