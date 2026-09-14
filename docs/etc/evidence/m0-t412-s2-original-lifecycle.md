# T412 S2 original BaseSrv lifecycle execution

## Composition and reproduction

Run `node tools/audit/Verify-BrokerOriginalLifecycle.mjs`. Each invocation
compiles the complete unchanged `opennt-host/base/win32/server/srvvdm.c`, a
test host, and the original RtlConvertLongToLuid body extracted from the
accepted original declaration carrier. The inline storage specifier is removed
only for that generated test object; the algorithm is not rewritten.

The test links native NT event, duplication, token, loader and heap interfaces
using the installed SDK import libraries. Its finite host supplies the current
process heap and one request context for the test's actual process/thread.
CSR lookup accepts only that process. It implements no VDM queue, selection,
capacity, reentry, first-query or completion policy. UserNotifyProcessCreate
remains null through its original guard; this fixture does not exercise USER
input-idle or interactive-token acceptance.

The existing declaration probe and unused-DFS umbrella exclusion remain
test-only. They are not the final product header boundary. All outputs are in
`build/M0-T412/S2/original-lifecycle`; the driver checks x86 PE machine, unchanged
owner bytes and the link map's srvvdm.obj ownership of key service functions.
No deployed EXE is changed.

## Passing original-body assertions

1. First-VDM returns true once, then false, and remains false after a DOS
   record is created and removed in the same service instance.
2. Original CheckVDM creates the DOS Console record with copied command and
   VDM_TO_TAKE_A_COMMAND, reporting VDM_NOT_PRESENT.
3. Original directory set/get reports the required size on a short buffer
   without consuming the stored data. Successful retry copies and consumes;
   another query reports zero remaining bytes.
4. Original UpdateVDMEntry acquires the test worker process reference and
   creates a real unsignaled paired parent event.
5. Original GetNextVDMCommand rejects a short command buffer without dispatch,
   then copies the command and sets VDM_BUSY on retry. Parent wait stays pending.
6. Reporting completion with exit code 7 wakes the parent, preserves the code
   and creates the original no-work worker wait. Both are real NT events.
7. Original reentry increments twice and decrements to one; that decrement
   signals the worker wait even though the count is not zero. The function's
   original success result is TRUE, not STATUS_SUCCESS. The initial test
   incorrectly expected zero and was corrected; the source was not changed.
8. Original DOS exit removes the record and releases the server-side waits;
   the test closes its remaining client references. Repeated exit is rejected.
9. Empty shared-WOW acquisition succeeds without a wait handle and clears the
   command length, preserving the original nonblocking branch.

## Limits and next implementation group

This is execution of the real original server owner, not a policy simulation.
It remains a same-process fixture: it does not prove RPC receipt ownership,
cross-process authentication, Console binding, original BaseClient retry/copy,
WOW task submission/notification, fault-injected allocation rollback or product
selection. The existing product still selects its old local service provider.

S2 remains active. Restore the finite product declaration/request/resource
bindings and original client routines, extend negative/coherent lifecycle
coverage, and select the recovered provider before retiring the local policy.
No net production-code reduction, S2 closure or three-product build is claimed
by this evidence checkpoint.

## Original BaseClient restoration continuation

The complete original GetNextVDMCommand function is now restored in
`opennt-host/base/win32/client/vdm.c`, under OPENNT-HOST-014's explicit
command-cohort selection. It contains 390 original lines; its LF-normalized
SHA-256 is `47cf285cbc505d9d43a50af16e283ea51d1d9ef8d2ff093795086d0ca327addd`.
Comparison with `O:/repos.external/OpenNT/base/win32/client/vdm.c` at revision
`5c5b979ec08c17d3ca2eb70e8aad62d26515d01c` proves an unchanged function body.
Its original copyright notice remains in the mirror file. The four finite CSR
declarations retain signatures from the same tree's `public/sdk/inc/ntcsrdll.h`.
No CSR implementation or full Base DLL was imported.

The current default environment cohort still compiles independently. The
focused graph selects OPENNT_BASE_CLIENT_VDM_COMMANDS and checks that
GetNextVDMCommand links from client.obj while BaseSrv functions link from
srvvdm.obj. It uses the existing original RTL error.c/table with that source's
actual formal per-edge compiler flags, not a replacement error mapper.

Additional passing assertions use the original client to query first-VDM,
receive a short-buffer failure and ERROR_INVALID_PARAMETER, retry with the
returned required size, copy the real command, and retrieve empty shared-WOW
results. Capture allocations are drained after both failure and success. The
test transport supplies capture storage and dispatch only; the restored client
supplies capacity propagation, error mapping, result copy and cleanup policy.

The finite PEB/TEB used here is test-owned and supplies the same local Console
key and status carrier. It is not a modern PEB reinterpretation, registered
cross-process Console proof or a production authentication implementation.
The restored wait loop is source-identical but a wake-and-retry through the
client still needs its own asynchronous test. Other BaseClient lifecycle
routines and product bindings remain S2 work; the old selected product policy
has not yet been retired. Adding original source reduces the missing-source
scope, but is not a claimed net line deletion or product rollout.

## Coherent client lifecycle and real wait continuation

The same original source cohort now includes ExitVDM,
SetVDMCurrentDirectories, GetVDMCurrentDirectories, CmdBatNotification and
RegisterWowExec: a contiguous 320-line original block, LF-normalized SHA-256
`2936de23dcbbdec23cf9ea23416606a7a3faeb96473d407e4bfaee00f89ac3aa`, from the same
pinned source/revision. Together with GetNextVDMCommand, 710 original lines
have been restored; this is restored source volume, not net code deletion.
The driver validates both source hashes and each selected client symbol owner.

Additional passing tests now cover:

- Original client directory size query, retained data on short capacity,
  successful copy/consume and capture cleanup.
- Original batch start/terminate notifications create/remove the actual
  server BAT record for the fixture's Console key.
- Original RegisterWowExec registers a real test-owned message-only window
  through native USER loading and original PID/sequence checks. Clearing the
  invalid registration removes the saved HWND. This does not prove guest
  WOWEXEC message delivery or shared-WOW task execution.
- The original client waits on the real server-created event. Only after the
  server returns that wait handle does a test thread submit NEXT.COM through
  original CheckVDM. The original queue/event code wakes the client. Exactly
  two client dispatches occur; the retry carries exit code zero rather than
  the initial 99, and the returned command and drained captures are asserted.
- Original ExitVDM routes through original BaseSrv cleanup, signals the
  pending parent and closes the returned worker-side wait reference. The test
  verifies the closed reference is invalid before any further allocation.

The first directory capture implementation in the test needed its allocation
capacity rounded to the original four-byte message-pointer alignment. This
was a test-transport correction, not an edit to the original directory code.
No new outgoing interface shape or service policy was required for this group.
Native program classification, launch/update client routines and product
resource/authentication bindings remain unfinished; S2 stays open.
