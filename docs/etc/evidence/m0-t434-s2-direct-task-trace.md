# T434 S2 — Direct task trace implementation

## Question and inputs

Expose existing Direct task records to NTMON without creating another execution
authority. Baseline: T433 closure47d78555e and T434 S1 designf2b8af5f9;
delivered APP433/RPC41/I/O25. Owner continues with the full Direct/Observed
implementation goal. S2 implements Direct only; S3/S4 still own Observed sources.

## Candidate changes

common/protocol/task_trace.h and service.idl describe bounded copied facts;
application434/protocol42/IDL42 agree and I/O25 is unchanged. The connectionless
WorkerTaskTrace RPC authenticates the real caller using existing transport and
resolves an existing full management key under the service lock. It projects
only existing DOS/WOW/native Direct records; no receipts, close actions or
worker state are mutated. DOS admission labels gain a stable display identity;
WOW tasks remain peers instead of an invented call stack. Snapshot allocations
are bounded to256 rows and truncation is explicit. Missing observation is
explicit, never evidence that a worker has no descendants.

NTMON Enter on a worker opens a separate read-only detail; Escape returns.
Delete cannot dispatch close while in detail. Ordinary labels, Direct STACK/
TASK and existing main keys are retained. common/rpc owns the copied finite
client, including partial-allocation cleanup. No mirror file changed.

## Procedure and current evidence

All generated/compiled/run artifacts are under build/M0-T434/S2. Generate
New-NativeWorkerNinja -Component Service/Monitor against the delivered formal
build/M0-T433/S6/r010-formal/build.ninja. r001-service/r002-monitor builds
require the MSVC-capable execution environment. Initial sandbox Ninja instances
had no compiler children/output and were replaced after exact process checks;
this was not a product failure or passing build.

| Evidence | Actual result | Scope |
| --- | --- | --- |
| r001-service/build.log | NTSRV and basesrv-service-reservation-test compile/link pass after syncing explicit ifspec references to42. | AMD64 production/service fixture. |
| r001-service/service-fixture.log | Exit0, no failed assertions; new DOS Direct identity/image, stale generation/epoch/nonworker rejection and child liveness checks pass alongside retained lifecycle checks. | Local production service archive, not real RPC. |
| r002-monitor/build.log | NTMON, monitor-layout-test and common-management-test compile/link pass. | AMD64 production/client. |
| common-management-test |105 checks,0 failures,0 retained allocations. | Client success, rejection/exception and partial result release. |
| r002-monitor/layout2.txt and .console.txt | result=exited, exit0, PASS marker. | Private desktop actual Console API renderer; detail DIRECT/gap text, Enter/ESC and Delete isolation. |
| r002-monitor/rpc-build.log | monitor-rpc-test compile/link pass. | Adds real trace JSON querying and absent/old-protocol negatives; runtime not yet executed. |
| r002-monitor/rpc-empty.log | Exit0 on actual candidate NTSRV; absent-key and old-protocol trace calls reject with empty outputs. | Real authenticated connectionless RPC. |
| r004-formal common-management/service fixture |105 client checks pass with no retained allocation; service fixture exit0. | I386 client/local service archive, same assertions as AMD64. |
| r010-trace-integration/direct-trace.trace-*.json and observer reports | Both live CMDs identify their own Direct PID/image/root, ordinary tree identical before/after queries; first worker closes, independent CMD consumes input and returns23. | Real two-session native Direct projection/isolation and existing close lifecycle. |

r003-common/r005-frontend/r006-launcher rebuilt NTVWM/NTCON/run16 AMD64
successfully. r004-formal rebuilt NTVDM/Hook32 and linked Redirector against the
new actual import library; r007-hook64 and r008-wow32 compile/link pass.
r009-runtime is a coherently staged ten-image candidate with baseline guest
media/configuration retained, not a deployed or accepted package. A formal
fixture graph regeneration must explicitly select these AMD64 images; a first
attempt without those selections hit the existing no-x86-fallback guard and
was not counted as passing control verification.

The direct default-desktop layout invocation failed its fixture buffer resize;
the established private-desktop observer passed. This does not weaken the
fixture assertion or certify a physical desktop layout. Source/fixture evidence
does not yet prove coherent publication or full real-package query behavior.

## Further review and failed runs

r011-full is a failed attempt, not Full14 acceptance: WOW frontiers,
Console17, Window17, RPC and native GUI passed, then the first generated
old-app service exited1740 before readiness. Its timings retain Passed=false
for version-negatives. A later read-only process check found independent
W:/system32 product instances from
build/performance-research-20261006/runtime repeatedly occupying the global
endpoint. The first failing instant did not save the process owner, so the
later instance is not claimed as proof of that exact earlier owner. Coordinate
serial product tests; do not tolerate duplicate-endpoint startup or retry it
into a pass. No W mapping is created/removed by this task.

The original r012 NestedDOS assertion expected two broker Direct records for
COMMAND /C system32/COMMAND.COM. r014 diagnostic preserves the unmet tree
(one actual DOS worker, one Direct record) and guest witness/VER/EXIT0 output.
Original msproc.asm's known-DOS INT21/EXEC route bypasses the unknown-binary
CMDCHECKBINARY path; this is not proof of missing broker admission. Correct
the test contract to one Direct plus an internal guest child requiring S4
observation, never manufacture the second Direct. The diagnostic stays failed;
revised tests require their own fresh evidence and actual witness/exit.

Review also fixes copied DOS display identity at successful new Check admission:
record address and numeric wait handle may both be reused. Assign a fresh opaque
identity for that actual boundary, never from the snapshot sampling. Original
execution is untouched, and identity exhaustion only marks missing trace
metadata, not failed guest delivery. r001-service/fixture-admission2.log and
r004-formal/admission-fixture.log pass the first/new task identity distinction
and retained lifecycle assertions. x64 copied client ABI assertions (568 bytes,
image offset48 and MIDL layout agreement) and105 client checks pass at both
widths (r002-monitor/abi-fixture.log, r004-formal/abi-fixture.log). These source changes invalidate affected earlier package
runtime acceptance; r016-runtime stages the updated coherent candidate for
fresh verification. r009 remains an unmodified older attempt, not delivery.

r018-direct-DOS/NestedDOS/Native32/Native run the actual updated r016 package
through verify-direct-task-trace.ps1. All four pass the exact current Direct
count, valid unique IDs/source/kind/image, unmodified main stack/state, actual
input witness and target completion (DOS0, native37). Native32 and Native
separately prove kindWIN32/2 and WIN64/3. NestedDOS remains one Direct because
the known guest EXEC child is not broker-admitted; no Observed success is
claimed before S4. Every invocation releases its exact test-owned processes,
restores input-test environment and the caller removes its own Z mapping.
r019-full is the fresh updated-package full gate: all14 groups pass in661987ms,
including independent WOW frontiers, Console17/Window17, RPC/GUI, all five
version negatives, strict DIR, modern EDIT, cooked/rapid/interactive return,
isolation, retirement and nested Window handoff. Version-peer generation takes
218213ms. This is a new successful run, never a merge of the earlier failed
version-negative result. r020 mixed batches and r021 typeahead are running on
the identical candidate before publication.

## Open gates

The bounded S2 delivery now passes both-width ABI/client/service fixtures,
actual Direct queries/isolation and private Console modal dispatch, updated
Full14, r020 Mixed/Search/Native/WOW22 and r021 typeahead6 (all Passed=true).
r022-publication installs the identical r016 ten images and license at
O:/winnt/system32, preserving guest/configuration and a complete old-package
recovery copy. r024 actual deployed DOS/CMD32/CMD64 output/exit0 smoke and ten
manifest hashes pass; test-owned product processes were identity-checked and
ended. Z is removed. r023 had a wrong oracle: /C VER produced the original
MS-DOS Version5.00.500, not the interactive host-version string; retain that
failed test attempt, corrected in r024 without any production/media change.

Review: no mvdm/opennt-host mirror, guest, frontend I/O format, execution wait,
receipt or retirement logic changed. Common owns copied declarations and
finite client; NTSRV owns projection/identities; NTMON owns read-only UI.
Callbacks are not yet introduced. No Job/member scan/helper/observed admission.
Main STACK/TASK/actions remain existing Direct semantics. Physical/RDP use is
not newly certified; retained WOW frontiers are not gameplay acceptance.
Production commit/push and closure-state registration are the final S2 steps.
The full goal remains open: Hook descendants, DOSONLY callback/TSR facts and
complete detail hierarchy/fidelity are following admitted stages, not waived.
