# T449 S1 — WOW execution continuation audit

## Question, baseline and method

Determine which transferred T447 execution obligations remain on the actual
T448-delivered source and package, and identify complete implementation groups
without treating old partial passes as whole-feature proof. Baseline is
`1f2f968e8` (product release `55566cadb`). This first source pass reads the
T447 [execution plan](../operations/t447-wow-execution-plan.md), its
[S4 message ledger](m0-t447-s4-message-source-audit.md), the
[S3 control ledger](m0-t447-s3-control-source-audit.md), the
[S2 closure](../../history/m0-t447-s2-foundation-closure.md), the T448
[integrated acceptance](m0-t448-s5-integrated-acceptance.md) and current
selected sources. It does not execute a test or change guest, product source,
an exception budget or a compatibility boundary. The table is an audit in
progress, not a closure of S1 or a substitute for the detailed inherited rows.

The T448 source comparison `git diff 4d4ef4d47..HEAD` over the WOW32 mirror,
WOW adapter, service management/lifecycle and NTVDM worker lifecycle finds
changes in `wow32/wkman.c`, `wow32/wow32.c`, service `management.c` and
`lifecycle.c`, and the NTVDM worker lifecycle; no files in `ntvdm-exe/wow`
changed in that restricted comparison. `wkman.c` and `wow32.c` changes are
registered DIV-356 whole-worker exit authorization, not an individual-task
termination implementation. This is a file-level fact, not proof that all
dependent build inputs or later behavior are unchanged.

| Stable obligation | Selected chain and observed status | Existing evidence and remaining boundary |
| --- | --- | --- |
| M1, C01--C03/C09/C10: send, post, retrieval, filters and quit | `mvdm/wow32/wumsg.c` and message thunks retain original conversions and native USER transport; `wow_user_message_bridge.c` services the native send boundary, not a replacement queue. | `tests/observation/verify-win16-message-probe.ps1` asserts ordinary same-worker send/nesting/reply/post/filter/quit and has distinct cross-worker and external-peer modes. T447 recorded bounded passes; the script itself records unproved implicit focus, full converted structures and other variants. Do not infer complete M1 from one mode. |
| M2: mixed receive, reply and continuation | Original `WU32InSendMessage`/`WU32ReplyMessage` use native USER. Current `wow_user_task_lifecycle.c::native_call_begin/end` uses an execution scope; it does not manufacture a received SMS. | The old opening table in the S4 ledger describes a *superseded* target-based `psmsSent` candidate. The later ADAPTER-WOW-077 record and current source show it was removed. Keep actual mixed/implicit receive, duplicate/no-pending reply and reentry evidence separate from this source observation; full M2 is open. |
| M3: scheduling and exceptional waits | `opennt-host/.../taskman.c` retains original task scheduling; lifecycle scope supplies actual native-stack readiness. `mvdm/softpc.new/host/src/nt_eoi.c::BlockWOWIdle` remains the one-bit interrupt owner. | S4's final source disposition rejects speculative unconditional idle toggles. A real original `W32Dispatch/W32Exception` Ignore/Close and subsequent task/hardware-wakeup witness is not established by the normal callback fixture. Preserve original behavior until the whole exception path is proved. |
| M4: conversion and data lifetime | Original `wumsg.c`, `wmsg16.c`, `wmdisp32.c` and `wdde.c` own conversions; DIV-350--354 and existing call-local acquisition records are present in current source. | T447 source-body alias/queue fixtures and a real classic Execute/ACK witness are bounded evidence. They do not prove DATA/POKE/ADVISE/INITIATE, failed transfer, all output structures or complete cross-worker lifetime. Verify source and actual selected build identity before reusing an old candidate result. |
| M5: ordinary failure and retirement | Existing callback/borrow/terminal owners and original task cleanup remain distinct from requested individual deletion. T448 changes the service/worker close boundary and must be included in current chain review. | T447 S2 was closed under the explicitly accepted isolated common-dialog risk; S3 was closed with bounded control coverage. Those closures do not prove all stale-handle/reentry/receiver-death paths. Review actual changed boundary and retained raw-fault risk rather than reopen closed rows wholesale or dismiss the risks. |
| M6: integrated execution | Real two-task Win16, native x86/x64 peers, production WOW/CCPU and immutable guest remain required. | T448 final preclosure covers DOS17 in both presentation modes, game interactions, WRITE startup frontier and lifecycle/CAF; it is not the full inherited M1--M6 matrix. `verify-win16-message-probe.ps1` and its mode-specific assertions are the existing real-guest starting point, not a blanket pass for every mode. |
| Individual WOW task deletion | `management.c::service_append_worker` projects `WOWRECORD.iTask` into a management row and deliberately does not advertise targeted close. `lifecycle.c::OpenNtBaseServiceCloseManagementNode` accepts worker/frontend and refuses other categories. Original `wkman.c::W32Thread` has both `VDMInfoiTaskID` and `htask16`; `W32HungAppNotifyThread` selects by `htask16`. | This is a source-confirmed missing control path, not a failing test claim. The management sequence/request is not an HTASK. The admitted [termination requirements](../operations/t447-wow-execution-plan.md#approved-individual-wow-task-termination) require incarnation-safe identity, guest-launched/separate-WOW coverage, deletion/exit races and surviving sibling. `W32DestroyTask` signals the hung event before cleanup; that event is not completion. Original hung handling can escalate to another task/whole worker and must not do so without the already required separate confirmation. |
| Basic controls/dialog/menu dependencies and app witnesses | Original selected control/dialog/MENU-01 owners and bounded S3 fixes remain; all unrelated W02/W03/W06/W09 family completion stays with its existing candidate unless a specific execution prerequisite is demonstrated. | T447 control-interaction proof covers listed controls and three style modes, not all UI. T448 verifies WINMINE/SOL gameplay and only the WRITE startup frontier; WRITE editing/exit and intermittent direct-game long-play failure remain unproved. Actual owner SOL geometry and environment-dependent witnesses must not be silently reclassified as defects or passes. |

## Specific identity and lifetime observation

Original `src/mvdm/vdmdbg/vdmdbg.c::VDMTerminateTaskWOW` enumerates the
shared process record then starts `W32HungAppNotifyThread` in that worker with
the actual `htask`. Reusing its *contract and original in-worker algorithm*
does not authorize copying its remote function pointer into an NTSRV RPC or
introducing cross-component direct control. In the current service the
management label is keyed by a worker generation and a service-generated
incarnation sidecar for a `WOWRECORD`; its displayed `record->iTask` is an
admission/request identity. `W32Thread` writes the actual guest `wTDB` to
`td.htask16` and separately stores `iW32ExecTaskId` in `td.VDMInfoiTaskID`;
guest-created tasks may lack that request identity. A safe group therefore
must establish actual worker-owned binding and its lifetime, including task
reuse, before supporting Delete. The original `ptd` search and later guest
access also need a concrete race/ownership review; a search hit is not a
proof that an unprotected pointer remains live.

The current service display is also explicitly conditional: both
`service_capture_wow_labels` and `service_append_worker` only traverse
`WOWHead->WOWRecord` when the one `WOWHead` sequence matches the selected
worker. The original `srvvdm.c` record is a BaseSrv command/wait list, not
an enumeration of every original `TD`. `wkman.c` assigns `VDMInfoiTaskID=-1`
on its `W32Thread`-related internal task path. This source comparison means
the current projection cannot itself establish complete guest-task coverage,
particularly for internally created tasks and separate WOW workers; it does
not by itself establish which of those tasks actually appear in NTMON at
runtime. Resolving the observable task set requires tracing worker publication
and retirement through original TD/shared-task ownership and NTSRV's existing
RPC controls, not casting an `iTask` or duplicating a remote TD pointer.

At the current worker edge, `worker_base_controls` exposes the authenticated
server, worker-shutdown and Console-close signals; NTVDM's
`worker_lifecycle.c` maps them to the existing close routines. Neither this
signature nor `OpenNtBaseServiceCloseManagementNode` currently conveys a
task identity. A complete design must cover NTSRV's accepted task request,
worker-base dispatch and NTVDM/WOW32 execution/acknowledgement together using
the project's established RPC/I/O boundaries. Merely setting the existing
worker-shutdown event or adding a management UI item cannot implement the
targeted operation, and the shared worker-base must not interpret WOW policy.

## Original task publication and retirement chain

`wkman.c::AddProcessSharedList` registers a per-process shared-list object;
`AddTaskSharedList` first reserves an HTASK with module and thread unset,
and the later `W32Thread` call fills the module, current thread, module name
and image path. `vdmdbg.c::VDMEnumTaskWOWEx` walks the selected process's
task list and only invokes the callback for entries with a nonzero module.
This is a different original inventory from BaseSrv's launch/wait records.
It establishes a useful source for published HTASK identities, not that an
NTSRV launch record and every task have a one-to-one relationship.

The shared table is explicitly best-effort: `AddTaskSharedList` permits a
task to continue when the mapping is unavailable or its finite table is full.
Its reserved entry has no ready module or thread. Therefore an empty
enumeration, a reserved entry, or a successful launch cannot prove that a
particular live task is safely targetable. Any service-side projection must
report coverage honestly, bind an identity to the actual worker generation,
and reject stale/reused task identities; it must not treat missing inventory
as proof of task termination or pretend that a reservation is a live task.

Normal `WK32KillTask` calls `W32DestroyTask` before
`RemoveTaskSharedList`, which removes the current thread's record. The
`W32Thread` InitTask-failure and unexpected-return paths visible in this
file call `W32DestroyTask` and exit without the same explicit per-task
removal; a process-level shared-list cleanup or another path may still remove
them. This is an audit candidate, not a proven stale record: resolve those
paths and ownership before relying on list disappearance as completion or
adding another independent registry. A repository search found the
`RemoveProcessSharedList` definition and declaration but no direct call; it
does not establish that all process-death reclamation is absent, since the
shared-memory manager or another process might reclaim it. Trace that manager
and actual worker death before making the determination. `sharewow.h` itself
maps a named section and serializes it with a mutex; its lock function does
not perform per-process reclamation. The section can disappear when its last
handle closes, but that alone does not prove cleanup while another process
keeps it open. Also note that the original debugger
enumerator uses shared memory and callback-based observation; it is not an
authorization to let NTSRV directly control a worker or dereference its
private pointers. The eventual control remains through NTSRV and worker-base.

## DDE evidence chronology correction

The late S4 ledger describes DIV-350--354 as a source candidate awaiting
delivery. `git log -S DDEBeginConversion -- src/mvdm/wow32/wdde.c` identifies
`3eae6c637` as the commit that incorporated the group; `git diff --stat
3eae6c637..HEAD` over `wdde.c`, `wdde.h`, `wmsg16.c`, `wmdisp32.c` and
`wumsg.c` is empty. T448's final cached build selected and released the
current WOW32 provider. Thus the earlier statement that this source has never
been built or delivered is stale. This does **not** upgrade old candidate
fixture results to proof for the final release: the recorded final T448
regression did not include the classic-DDE message-probe mode, and neither
the current final build nor a hash comparison proves real DDE exchange,
failure rollback or exactly-once payload retirement. The inherited real
Execute/ACK and source-body evidence retain their exact run/package identities;
the remaining M4 matrix must be reconciled by caller and ownership family.

One concrete failure-path candidate in the current source is the 16-to-32
posted DDE path: `WU32PostMessage` converts first, calls host `PostMessage`,
and invokes the ordinary unthunker even when posting fails. For POKE, DATA,
ADVISE and EXECUTE, `ThunkWMMsg16` can create an HDDE association and native
payload before that call; `UnThunkWMMsg16` has no corresponding cases, and
`wow_message_unwind16` only releases the explicitly listed text/COPYDATA
resources. The conversion-local rollback is used for construction failures,
not visibly carried to the post-result branch. This is a source-level
ownership risk, not yet a proven leak: the native packed-lParam contract,
the original HDDE retirement paths, retry via `WOWPRIVATEMSG`, and borrowed
versus newly created pairs must be traced and tested together before changing
cleanup. In particular, freeing everything on a failed post without proving
ownership can break reused pairs or cause double release. Include this path in
the finite M4 failure matrix; do not claim that DIV-354 already covers it.

Source identities (SHA-256) for this pass: `wkman.c`
`49BE04B3E5959F175D72B63A87239EA4F325CFDD38B051932DF2176D5527E860`;
`vdmdbg.c` `11CAD596254D09929987CF110F5B492862972D06077C5FECC100DC79B087E1D2`;
service `management.c` `A944E17AFD2E0403D1E8A2E1DC4757E9A14C776A1CF01A2FC492398B33AAD2A7`;
service `lifecycle.c` `4B9847C3399F8B05B8D37E24CE8D0CE7620030FFE2E30A9DD4755128921FB28C`;
`wow_user_task_lifecycle.c` `02068CECA2890AF3DE2A9FCE85C6FFBD259E0FA9625757D43D888BEB772576F0`;
selected `taskman.c` `FCCAB963DCDE796091AAE9972C86B874C899B81D2F91727B7081A3ACFD5A1898`.

## Remaining S1 audit

Complete the per-entry M1--M6 conversion/failure denominator, callback/object
responsibility reconciliation, and the task registration/identity/retirement
source chain against current production selections and test assertions. Then
specify dependency-ordered groups with exact original reuse, duplicate removal,
failure/lifetime rules and acceptance. Do not treat the individual-task gap as
the only open row or begin isolated repair before this audit is reconciled.
