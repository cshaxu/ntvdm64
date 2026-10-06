# T434 S3 — Native observation candidate

## Scope and baseline

S2 production e5769fcc3 / closure d2b928396 is APP434/RPC42/I/O25 at
O:/winnt/system32. S3 adds read-only controlled native process facts, not
execution records or authority. Published S2 remains unchanged.

## Candidate implementation and ownership

NTSRV-private task_observation.c retains actual process references on successful
Direct bind; a bounded sidecar uses distinct opaque IDs. Hook finish publishes
typed actual child/reporter references after installation and before its existing
resume, outside DllMain. Common RPC carries a connectionless authenticated
report, with a250ms per-call cap and no registration/retry. Parent scope derives
from pinned process objects; documented ProcessBasicInformation checks actual
inherited parent without enumeration or PEB reads. Parent override/unsupported
query is a gap, not a guessed relation. An actual duplicate object report is
idempotent; a PID number is never the registration identity.

One-shot process waits record true signal/exit facts. CREATE is inserted under
the service lock before its callback can publish EXIT. Stop cancels/drains waits
outside that lock before closing processes/freeing the service. Sidecar data
does not enter IsEmpty, Direct receipt, worker state or retirement. Copied query
projects original Direct rows and read-only historical/Observed facts; missing
native image stays UNKNOWN, never DOS. Root observation failure is optional.
Source/report coverage and capacity/rundown semantics still require review/tests.

No Job, Console-member sampling, process-tree scan, new runtime helper, guest or
mirror algorithm change. RPC/IDL advances43 together; APP434 and I/O25 remain.
Common C APIs gain C++ linkage guards for actual shared Hook use, not duplicate
implementations. Hook64 builds the same generated native RPC client and existing
authentication-scope mechanism; MIDL allocation has no loader-lock activity.

## Actual evidence so far

| Run | Procedure | Actual result |
| --- | --- | --- |
| r001-formal | New-T310OriginalSoftpcNinja -Architecture x86 | Graph includes production observation module; no x86 runtime claim yet. |
| r002-service/build2.log | New-NativeWorkerNinja -Component Service; run-ninja ntsrv.exe basesrv-service-reservation-test.exe | AMD64 compile/link pass after respecting RemoveHeadList macro statement shape. |
| r002-service/fixture.log | Existing production archive reservation/lifecycle fixture | Exit0; original Direct/worker/WOW cleanup assertions pass with added root waits. No child-observation acceptance claimed. |
| r003-hook64/build2.log | New-NativeHook64Ninja; run-ninja nthook64.dll | AMD64 Hook candidate links after common C linkage declarations were made explicit. |

native_observation_probe.cpp is authored test material: real event-gated root /
child and independent root37/child23. Its unhooked host retains the borrowed
Console until explicit test release. Both widths build under r008-probes;
Hook32 dependencies on generated RPC header/client and existing scope archive
are connected in the formal graph, not duplicated. Native43 consumers and
WOW32 build; r010 is an unsealed candidate, not a published package.

r011 failed on a read-only PowerShell PID parameter. r012 proved actual CREATE
but exposed the new callback's reversed boolean: FALSE is a process signal,
TRUE a timeout. Rename it timed_out and skip only TRUE. Existing worker/GUI/
frontend callbacks already ignore that argument in their infinite waits.
Failed reports remain; no delay/repaint was added.

r013-child64, r014-x64-RootFirst, r015-force64, r016-child32, r017-root32 and
r018-force32 pass actual creation/parent and real exit/code. Child-first/forced
exit never completes Direct; root-first returns37 while child survives, then
child23 is observed separately. Main Direct stack remains1. An intervening
case was blocked before execution by the existing-broker precondition; it is
not counted. Both actual runs resume only after the endpoint was verified empty.
The common client passes137 checks with zero retained allocations, including
observation rejection/exception outputs. Short/concurrent/suspended, negatives,
capacity/rundown and full publication gates remain unproved.

r020-short64/r025-short32 wait for the parent to obtain actual child23 before
the first query; both preserve CREATE/EXIT without sampling live membership.
r023-suspended64/r026-suspended32 prove caller CREATE_SUSPENDED is retained:
no child-code entry before explicit resume, requested flag4 in the trace, then
actual23 while Direct remains37. r021 exposed a test-only MainModule lookup
before user-mode LDR initialization; use the unique CreateProcess witness and
authenticated retained-object image instead, retaining the failed attempt.
r024-concurrent64/r027-concurrent32 create16 children from16 real threads,
wait for all actual23 exits, then query:16 distinct matching CREATE/EXIT nodes,
correct parent and no premature Direct completion. These are bounded controlled
cases, not a claim of universal launch API coverage.

r007-monitor/negative-rpc.log runs real43 NTSRV: an unregistered reporter with
its actual child, a different-process reporter attachment, and old protocol
are rejected with node0 and no ordinary management rows. Production archive
native-observation-fixture.exe in r002-service additionally seeds trusted fake
worker watches using real process objects (unit only) to prove actual-object
idempotency, unsupported parent rejection, unchanged count2, observations not
preventing IsEmpty, and cancellation/drain of pending waits without terminating
either suspended process. This does not stand in for Bind/RPC authentication.
The x86 production archive fixture and client in
r002-service/fixture32.log/client32.log also pass (137 client checks, no
allocations). Source loss/timeout, capacity/history/reuse and actual CMD
nesting/full package remain required before S3 closure.

r030-cmd64/r031-cmd32 prove actual CMD -> CMD -> probe -> child with four
distinct real-process nodes and sourced parent edges, Direct stack1 and
Direct37. r029 was blocked before execution by an external W service. Under
the owner's standing related-process termination approval, only pinned
W:/system32/ntsrv.exe PID25108 was ended after path/creation verification;
no W mapping/file was changed. Capacity fixture statistical seed1024 (unit
only; no smaller production limit or1024 processes) proves quota rejection,
node0/count2, explicit truncated coverage and both targets still alive.
Both widths pass in quota-fixture.log/quota32.log.

Test-only denied/slow service variants alter only observation delegation in
generated main sources below build, never production source or Direct paths.
r036-deny passes native creation/child23/Direct37 with only one Direct node
and an explicit coverage gap. Initial scripts hit a parse typo and a separate
precondition failure; neither is counted. r037-slow then found a real candidate
defect: RPC_C_OPT_CALL_TIMEOUT did not bound this synchronous local RPC;
actual CREATEMS2031 matched the injected2000ms. Preserve that failed proof.

Candidate now adds event-driven async observation with abortive cancellation
and actual completion drain, rather than detaching an unsafe pending buffer.
The async service wrapper reuses the same authentication/fact handler and no
task completion/worker policy. Synchronous entry is temporarily retained for
comparison tests; remove unnecessary production duplication before closure.
All affected native stubs/Hook64 recompile. r040-async-slow proves the actual
delay fault no longer blocks the native child for2000ms, and actual child23 /
Direct37 remain. This is diagnostic native-only evidence, not a coherent mixed
or production-P acceptance; both widths/normal/negative/rundown and full package
must be revalidated for the final async shape.

The synchronous report wire/client was removed. There is now one async
ObserveNativeCreationAsync and one common bounded implementation; server
authentication/fact logic is private and shared with its manager wrapper.
The obsolete CALL_TIMEOUT option is removed. Mock issuance-failure assertions
remain in the common fixture (124 checks); real async replies/negative versions
and abort behavior are covered by actual RPC tests, not a fake completion.
r007-monitor/async-negative-rpc2.log passes the same unknown/false reporter and
old-protocol node0 invariants on the final wire. The first attempt hit an
external W service (1717), not a passing final-interface run.

r042 stages the final ten-image async candidate. r043-final64/r044-final32
pass normal creation, source parent and kernel exit23 with Direct37. Diagnostic
copies r045-final-deny-runtime/r045-final-slow-runtime contain only deliberately
modified service images, never publication inputs. r046-deny-x64/r047-deny32
pass source-loss/no-fake-node and actual child23/Direct37. r048-slow64 and
r051-slow32 pass injected2000ms delay with bounded publication, actual child23
and Direct37 (32-bit CREATEMS265). Remainder is not a coherent-P gate yet.

Sequential-test precondition investigation found a test-owned service started
through Z:/tests/OBS/../../system32; its raw module path did not match cleanup
paths. The host now canonicalizes launcher with GetFullPathName before startup.
The exact test residual was validated before ending it; mappings are not reused
or deleted outside this task's Z ownership. Existing-broker checks now pin actual
process liveness instead of trusting a potentially terminated WMI row, but still
reject every live service. This check is not a retry-to-success or fixture
assertion waiver. Actual later occupancy at W remains a separate external issue.

## Final coherent-package regression

r052-full/timings.json records Full14 passed in416919ms on the final r042
APP434/RPC43/I/O25 ten-image package. Console17 and Window17, independent WOW
frontiers, real RPC/version rejection, native GUI, strict DIR, modern EDIT
return, cooked return, repeated launches, session isolation, retirement wiring
and nested Window handoff all pass. This is the retained product gate, not
proof of every Observed hierarchy field or the later DOS collector.

Review confirms observation refs/waits are bounded, callback state mutation is
serialized with snapshot copying, and stop drains waits without holding the
callback lock. Observed facts do not enter Direct completion, IsEmpty or worker
retirement. The current250ms client acknowledgement is a candidate budget, not
an OpenNT requirement. Async issuance still waits before the existing child
resume; extending it would extend the creation transaction's fault latency.
The owner's ten-second question is discussed, not silently treated as an
instruction to change the tested package.

r053-mixed/results.json proves22/22 cases passed (summed case time162506ms),
including COMMAND/CMD32/CMD64, both display routes, search, native GUI and WOW.
r054-typeahead/results.json proves6/6 continuous-input mixed cases passed.
Both runners completed cleanup with Z removed. Their exact per-invocation
journals retain width, Hook installation, stdout/stderr, order and actual
results; they do not depend on optional terminal scrollback.
Final-wire r056-final-{x64,x86}-{ShortChild,SuspendedChild,Concurrent,CmdNested,
RootFirst,ForcedChild} all pass actual CREATE/EXIT, source-parent edges and
Direct37 independence. Each Concurrent case proves16 real short children.
An initial invocation was blocked before execution by W:/system32/ntsrv.exe
PID26988; only that pinned process was ended under standing authorization.
No W mapping or file was changed. The twelve actual cases then ran serially.

r055-publication installs the exact r042 ten-image manifest and Hook license
at O:/winnt/system32, with S2 recovery preserved under build. Guest/configuration
remain unchanged. Actual deployed r057 smoke reports live in
O:/winnt/logs2/t434-s3-r057-{dos,native32,native64}.txt: original DOS /C VER
prints MS-DOS Version5.00.500, both native CMDs print their unique marker,
all return0, and all ten installed hashes match. Test-owned product processes
are identity-checked and ended; no Z mapping remains.

Final review: no src/mvdm or src/opennt-host mirror changes. Common carries
one async copied observation RPC/client, Hook32/64 share the existing successful
creation boundary, and NTSRV owns refs/one-shot waits/sidecar projection.
Observed nodes never enter execution records, STACK, receipt, IsEmpty or
retirement. Main NTMON actions/summary remain unchanged. Unknown image stays
UNKNOWN; coverage is explicitly incomplete. Bounded controlled Hook chains
are proved, not universal API interception or complete parent-override history.
Capacity is statistical-unit evidence, not1024 live-process stress; replacement
identity/history is unit evidence, not a physical PID-reuse guarantee.
Detailed state/time/revision UI and DOS original callback observation remain
S5/S4 respectively. Physical/RDP use is not newly certified. Review and
documentation governance pass. Production1e0f66177 is committed/pushed;
bounded S3 closes and CURRENT admits S4. Other-session proposal/queue changes
remain outside this delivery. The full T434 goal remains open.

## Remaining acceptance

S3's bounded native-source implementation, actual both-width positive/fault
tests, identity/quota/history/rundown unit checks, retained Full14/Mixed22/
typeahead6 and publication/smoke gates are proved above and1e0f66177 is pushed.
DOS observation and full monitor hierarchy/fidelity
remain S4/S5, not waived and not a full-goal completion claim.

Native-query ABI checked against
[Microsoft NtQueryInformationProcess documentation](https://learn.microsoft.com/en-us/windows/win32/api/winternl/nf-winternl-ntqueryinformationprocess).
Callback semantics verified against
[Microsoft WaitOrTimerCallback](https://learn.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms687066(v=vs.85)).
