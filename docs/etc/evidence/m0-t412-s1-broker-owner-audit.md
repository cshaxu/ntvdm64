# T412 S1 broker owner audit checkpoint

## Status, question and inputs

This is a source-audit checkpoint, not S1 closure or an implemented design.
The 2026-09-13 request admitted execution of S1. Inspection found a direct
contradiction in inherited worker event evidence, triggering the active packet's
stop condition. Production code, binaries and runtime processes were unchanged.

Question: which selected D01--D07/D10 implementations duplicate original OpenNT,
what can be restored, and what finite mechanisms/prerequisites remain?

- Repository input: `08b33351b1c341bb433f27ab517ee9a4e0bc3e0c`.
- Read-only OpenNT input: `O:/repos.external/OpenNT`, revision
  `5c5b979ec08c17d3ca2eb70e8aad62d26515d01c`.
- Both `build/output/ntvdm32.exe` and `O:/ntvdm64/ntvdm32.exe` have SHA-256
  `C0771B414F93B1F1644765447F89439860E23E6F9FB67A1319222955B267652F`.
  Equality establishes artifact identity only, not a build-to-HEAD or runtime
  success claim. No new guest test was run in this checkpoint.
- Formal composition reviewed: `tools/build/New-T310OriginalSoftpcNinja.ps1`,
  particularly source lists at 217, 244, 259 and final link at 1089.

Method: read actual selected providers and matching original functions; inspect
the formal generator; use `rg -n` to trace callers and dependencies; count
physical source lines with `Get-Content`; compare pairs with
`git diff --no-index --ignore-space-at-eol --numstat`; hash artifacts with
`Get-FileHash`. Nonzero `git diff --no-index` denotes differences, not a test
failure. Ignoring line-end whitespace is necessary: the server file otherwise
appears wholly changed by line-ending differences.

## Actual product route

`app/entry.c` -> `app/launch_declaration.c` ->
`base_vdm_broker_publish/deliver` -> process-local `broker_base_vdm_record` ->
`base_vdm_local_publish` -> original MVDM `GetNextVDMCommand` callers ->
autonomous `base_vdm_client.c` -> autonomous `base_vdm_local.c`.

The app assigns broker ID 1 and immediately copies between two local stores.
This is not an existing service process to which IPC merely needs attaching.
The selected Base client `opennt-host/base/win32/client/vdm.c` contains only
the environment subset, not GetNextVDMCommand/GetBinaryTypeW. The original
server file exists but is not in this formal product source list.
`adapter-opennt-host/README.md` explicitly records no implementation bodies yet.

`broker.c` is a neutral in-process registry fixture. Comparing a message's
user_key to stored bytes is not OS peer authentication. Its README requires
a future transport to authenticate first; do not treat this as a discovered
remote vulnerability in an already-running server.

## D-row findings

| Row | Current versus original | Source-first disposition and required proof |
| --- | --- | --- |
| D01 | `base_vdm_local.c:get_next_command` has one available command and simplified local DOS state. Original `srvvdm.c:130` selects DOS linked records or WOW records, delivers metadata/streams and releases VDMINFO. Current normal response sets standard handles to NULL and discards PIF/desktop/title/reserved lengths. | Restore original server selection/copy/free policy plus CheckVDM/UpdateVDMEntry; test busy/reentry, capacity retries, optional captures, fields and streams. Local record is not the final broker policy. |
| D02 | `fill_pif_info` uses application name for title and returns early on short capacity. Original `BaseSrvFillPifInfo:2722` prefers explicit title, initializes outputs, accounts for Reserved, and publishes required lengths even after failure. | Recover original body and capture fields; test explicit-title precedence, empty fields and short-buffer output/length behavior without consuming the command. |
| D03 | Local `first_vdm_available` initializes per session and is read/cleared without its record lock. Original `BaseSrvIsFirstVDM:472` owns one service flag. | Use original service-instance ownership; serialize this call in service dispatch, verify concurrent clients and broker restart. Keep host boot-file mutation disabled. |
| D04 | Local directory setter allocates before freeing the old value and has no lock around set/get. Original `BaseSrvSetVDMCurDirs:486` locks and frees the previous value before allocation; original getter at 706 consumes on successful copy. | Restore original ordering and locking, including allocation-failure outcome, short buffers and one-shot consume. Do not silently call the current failure behavior equivalent. Client zero-length special cases remain client-owned. |
| D05 | Local exit ignores WOW and completes its session with zero. Original `BaseSrvExitDOSTask:1698` wakes parent waits, closes service/worker resources and frees Console/task records; WOW has its own exit owner. | Recover server cleanup and original client ExitVDM return boundary; task completion is not unconditional worker termination. Verify parent result and every cleanup/failure path. |
| D06 | Local accounting gates decrement wake on count reaching zero and a new native-child-pending field. Original `BaseSrvSetReenterCount:2529` signals the VDM event on decrement when it exists. | Restore original count/wake behavior with service-owned records; remove local begin/cancel scheduling only after its original replacement is selected and tested around thread creation. |
| D07 | The 64-line local client shallow-copies VDMINFO, dispatches locally and waits on one private event. Original client at upstream vdm.c:340 allocates captures, preserves original result/failure field rules, waits and retries with cleared exit code. | Restore original BaseClient routines and finite capture/transport/wait bindings, in the requesting process. Test short buffer versus other failures, cancellation and wait/retry result ordering. |
| D10 | Local `mvdm_image_classify_path:101` accepts any NE signature as Win16 and any PE prefix as native before system fallback. Original upstream GetBinaryTypeW:93 maps NT image-section status, validates DLL/machine/subsystem and uses BaseIsDosApplication. | Restore original client classifier/control shape, not the simplified signature parser. Its underlying image-status provider needs a separate finite-boundary proof below. Search/CLI policy is not binary classification. |

The findings establish differences, not that each caused the historical WRITE
failure. That causal claim would require matched runtime evidence.

## Original selection and resource boundaries

Original `BaseSrvCheckDOS:1105` has READY/RETURNED and BUSY/TO_TAKE branches.
The busy branch allocates another DOS record and can wake the VDM when
`nReEntrancy` is set. It reports PRESENT_AND_READY, not automatically
NOT_PRESENT. Therefore neither a generic idle-worker pool nor a rule that
every busy worker forces a new process preserves original behavior.
Only source-defined acquisition/reentry may deliver the queued child; arbitrary
guest command injection is not allowed.

Original `BaseSrvUpdateDOSEntry:1527` duplicates the created worker handle and
implements partial/full creation rollback. `BaseSrvCreatePairWaitHandles:2353`
creates an event and duplicates it into the caller; it is not intrinsically a
kernel event-pair subsystem. The finite binding still must preserve both owners,
signaling, close order and failure cleanup.

Required adapter groups are capture/marshal, caller process/thread identity,
process-local event/resource duplication, Console identity, and WOW notification/
interactive-token checks. The original source contains CSR process lookup and
sequence-number validation. A claimed PID alone cannot replace authenticated
caller identity. Original USER interactive-token and WOWEXEC dependencies are
not yet proven composable; no success stub is admitted by this checkpoint.

## Provisional process design and unresolved gates

The selected policy direction remains three programs: run16 is original-client
composition, basesrv hosts original service policy, ntvdm is one original worker.
Proposed modern mechanics are a local authenticated endpoint and copied,
versioned request/reply records. Wire IDs are distinct from process-local HANDLEs.
The detailed OS resource-transfer protocol is not frozen yet; it must not send
raw duplicated handle values simply because both processes are x86.

Use original CheckVDM/UpdateVDMEntry selection and rollback. Broker launch
reservations protect creation/registration races but do not replace the original
DOS/WOW task lists. Worker events and command completion remain separate facts.
Default idle eligibility requires completed tasks, no queued/reserved work,
no active interactive guest/reentry/parent wait and drained resource ownership;
quiet COMMAND or EDIT is never evidence of eligible idleness.

D10 has a deeper dependency than merely importing GetBinaryTypeW. Upstream
`base/ntos/mm/creasect.c:MiVerifyImageHeader` distinguishes Win16, OS/2 and
DOS-bound NE using executable type, imports and extender signatures. Current
two-byte NE detection bypasses those decisions. The original logic exists;
this checkpoint has not proved modern NtCreateSection emits all needed original
statuses, nor audited a composable header-verification slice. Do not import the
memory-manager shell, invent a parser, or call the classifier restored until
that boundary is resolved. This remains S1 work after the stop disposition.

CLI search/quoting, batch/PIF behavior, transport authentication/resource
transfer, Console identity and the exact executable publication migration must
still be finalized. Existing ntvdm32.exe publication remains unchanged. The
six-stage working plan is not evidence that these design gates already passed.

## Measured replacement envelope, not net deletion

Physical lines include comments and blanks. These disjoint files identify the
current implementations to retire or replace, not deletions already performed.

| Current file | Lines |
| --- | ---: |
| adapter-mvdm-host-out/basesrv/source/base_vdm_local.c | 665 |
| adapter-mvdm-host-out/basesrv/source/base_vdm_client.c | 64 |
| adapter-mvdm-host-out/basesrv/source/base_vdm_broker.c | 167 |
| broker/base_vdm_record.c | 172 |
| **Four C files** | **1068** |
| basesrv/include/base_vdm_local.h | 145 |
| basesrv/include/base_vdm_broker.h | 41 |
| broker/base_vdm_record.h | 95 |
| **Associated headers** | **281** |

Total replacement envelope: **1349 physical lines**. Part of these files is
still-needed copy/resource binding and must survive in thinner form; the total
is not a promised 1349-line net reduction. Classifier file is another 264 lines,
but only its classifier body is a direct D10 replacement; resolution/launch
helpers need individual disposition. `app/launch_declaration.c` is 365 lines
of mixed composition/record construction, not entirely removable code.
The separate generic broker/wire four files total 166 lines and are also not
included in the 1349 figure.

Normalized server mirror diff is **0 added / 0 deleted**; restoration here
means selecting existing original code, not reducing a nonexistent server diff.
Client mirror currently measures **+136/-3242** against full upstream vdm.c;
these include legitimate package crops. Recovering selected client routines
reduces missing-source scope but cannot justify importing the full Base shell
or promising every crop will disappear.

No private-overlay deletion is yet counted. The confirmed duplicate record
policy above is in adapters/broker, not a private overlay. MVDM hooks needing
review include cmdexec DIV-197 local pending state, cmddisp DIV-191 local state
observations and cmdmisc DIV-230 absent process-command-line fallback. Do not
erase another same-number marker in dpmi32/modesw.c by tag alone.
DIV-196 asynchronous payload copy, DIV-109 thread ABI/binding, DIV-149 child
stream binding and guest leases are not made unnecessary merely by adding IPC.
Their actual replacement/lifetime proof remains mandatory.

## Stop finding: inherited RESET-consumption claim contradicted

At current `mvdm-host/softpc.new/base/ccpu386/c_main.c:291`,
`c_cpu_take_event(mask)` explicitly clears the bit by CAS. HALT at line 4073
calls it for RESET and breaks. The later normal reset handler at line 4422
calls it again before invoking `c_cpu_reset`.

Upstream HALT at line 3987 only observes `cpu_interrupt_map & RESET`.
The normal handler at line 4338 is the place that consumes the bit and resets.
Thus, with one pending RESET and no second producer raise between the two
checks, the local HALT path has already removed the request needed by the
normal handler. This is a high-confidence static control-flow difference;
there is no claim of a newly reproduced deployed-binary failure here.

[T411 S4 evidence](m0-t411-s4-ccpu-event-profile.md) claimed the atomic helpers
preserve original consumer order and explained the lost-RESET issue solely as
an original RMW race. That blanket equivalence claim is not supported by this
path. [T411 S2](m0-t411-s2-cvidc-vector-closure.md) explicitly used inert provider
symbols for vector publication; that fixture cannot prove reset consumption.

Disposition: keep S1 open and record the stop; do not change CPU source or
silently expand broker ownership. Proposed bounded follow-up is an explicitly
admitted original-event-consumer audit/repair: preserve atomic producers, recover
the original observe-versus-consume distinction, verify HALT/normal RESET,
timer and interrupt acknowledgement ordering as one coherent profile, then
run original-provider focused tests and the formal x86 regressions. Do not
reopen instruction decoding or invent a broker reset state machine.

## Verification and remaining work

This checkpoint used read-only source/build-definition inspection and artifact
hashing; no fresh compile, link or guest execution occurred. Documentation
governance and diff checks apply to its delivery, not runtime correctness.
No production lines were removed. S1 is not closed and S2 is not activated.

After owner disposition of the stop: finish the finite boundary audit, especially
D10 status classification, Console identity, WOW private callbacks and resource
transfer; complete exact per-hunk/overlay removal counts and the source-backed
test/design matrix. Then report S1 completion honestly before source restoration.
