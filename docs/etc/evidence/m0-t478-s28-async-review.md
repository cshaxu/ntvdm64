# T478 S28: asynchronous completion bounded disposition

Parent: 13e182da6. The owner explicitly accepts the original allocation-exhaustion limit. This is a bounded disposition, not a product fix.
No production source or release package has changed in S28.

## Established facts

The exact original queue/raise/dequeue/dismiss fixture uses real critical
sections, controlled LocalAlloc and an ICA observer. Two normal callbacks retain
FIFO and finish with no queue allocation and interrupt count -1. A forced failed
allocation followed by the existing unconditional raise leaves an empty queue;
the original dequeue raises C0000005. All nine reproduction assertions pass;
this means the defect is reproduced, not that production passes a safety test.
Artifacts: build/t478-s28/queue-baseline and initial-findings.json.

The five common queue/interrupt/uninitialize release bodies and the empty
VrTerminateNamedPipes release body match both pinned original editions after
comments, line directives and debug-only content are excluded (12 comparisons
in build/t478-s28/original-comparison.json). This is an original allocation and
lifecycle limitation, not missing original code or a project-created regression.

All producers must be considered: Netbios32Post, named-pipe completion and two
DLC posting sites queue a common disposition then raise an interrupt. Provider
work has already been queued separately. Merely returning failure from the
common enqueue and suppressing the IRQ would strand that work and cannot be
called a complete repair. Merely returning from empty dequeue likewise does
not restore the missing callback. Any repair must account for completion count,
provider ownership and original ordering without adding generic retry/polling.

The existing production-adapter publication fixture passes 24 assertions at
build/t478-s28/existing-publication. It verifies payload/error/count publication,
thread binding, stale epoch, cancelled session and ended guest-memory rejection.
It is controlled-memory evidence, not a real pipe callback or queue drain test.
It does not cover NetBIOS/DLC, nor prove that VrUninitialize waits for callbacks.

## Lifecycle distinction

DOS NetResetEnvironment reaches service 0x0f / VrTerminateDosProcess, which calls
mailslot and named-pipe task cleanup; the latter is empty in original release
code. Whole redirector service 0x01 / VrUninitialize instead deletes four
critical sections and clears initialized state. The current guest redir sources
contain no active named SVC_RDRUNINITIALIZE caller; this does not prove arbitrary
private BOP callers are impossible. DLL detach also calls VrUninitialize, then
TerminateDlcEmulation. The latter closes open adapters, not the generic queue.

Selected successful LoadVdmRedir keeps the module; its visible FreeLibrary is
failed-load cleanup, not per-DOS-task unload. Session disposal rejects live
thread bindings, but the named-pipe completion binding is not a global provider
callback shutdown contract. Do not report existing session checks as a proof of
whole-module quiescence or conflate DOS exit with DLL detach.

## Owner decision and closure

The owner explicitly chose: “登记原版内存耗尽限制，保持正常路径不变（推荐）”.
The original exhaustion crash and potentially stranded provider completion are
therefore retained and named, not reported repaired. No emergency pool,
preallocation redesign, retry, empty-dequeue masking or altered callback order
is introduced. This resolves finding 62 within the approved semantic ceiling.

For lifecycle, no selected successful-loader path was found that unloads the
redirector between DOS tasks. Explicit private uninitialize with live callbacks
and task-loss drain remain original limits; the investigation does not assert
these paths are safe. Session memory checks are retained and their 24-case
fixture passes, but do not establish provider-wide quiescence. No real late
callback incident or project regression has been established by this audit.

The committed async-s28-results.json contains the original comparisons,
reproduction hashes, production-adapter results and the verified unchanged
ten-image release manifest. Product remains the S27 package
c08075be-80dc-453c-b178-b0377ff9bb14. Since no production source changed, no
redundant build, Full or republication is claimed; S27's completed gates remain
its evidence. This S commits tests/documentation only, locally, without push
or O:/winnt writes. S29/finding 65 follows.
