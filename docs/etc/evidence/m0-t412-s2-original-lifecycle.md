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
