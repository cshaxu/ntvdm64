# T434 S4 — Original DOS callback observation

## Admission and baseline

CURRENT admits S4 after S3 production1e0f66177 and closuree3c08d467. Published
APP434/RPC43/I/O25 ten-image package r042/r055 remains unchanged. This record
starts with source audit; it does not claim DOS observation implemented.
Implementation and tests use build/M0-T434/S4/r001 and subsequent runs.

## Original source and selected boundary

| Existing source | Confirmed behavior | S4 adaptation |
| --- | --- | --- |
| mvdm/softpc.new/host/src/nt_msscs.c:1186–1325 | VDDInstallUserHook stores local callbacks, VDDCreateUserHook/Terminate call them synchronously, DeInstall removes the registration. No helper/DLL is required by this API. | Register one NTVDM-local collector before guest entry; unregister on its execution thread before disposing guest/session storage. Original callback list is not rewritten. |
| mvdm/dos/dem/demmisc.c:299 | demEntryDosApp uses DX PSP and suppresses the callback while IsFirstCall. | Observe only actual callbacks. Prove first requested target coverage; do not guess a missing boot COMMAND entry. |
| mvdm/dos/command/cmdmisc.c:267–271 | Successful first GetNext clears IsFirstCall before the requested program proceeds; environment-size retry has its own ordering. | Root correlation must follow real successful delivery, not startup time or image equality. Its coverage remains an integration proof obligation. |
| mvdm/dos/dem/demsrch.c:697–708 | Termination callback receives BX PSP before original HostTerminatePDB and file/search cleanup. | Copy EXIT fact only; original completion and cleanup remain untouched. |
| mvdm/dos/v86/inc/pdb.inc | Parent PSP is the word at0x16, environment segment at0x2c; numeric guest locations are not host pointers. | Use existing checked synchronous guest-location leases and release before enqueue; copy only bounded values/strings. |
| mvdm/dos/v86/doskrnl/dos/msctrlc.asm | Ordinary termination calls SVC_PDBTERMINATE; stay-resident takes a distinct path without the ordinary terminal fact. | Missing terminal event is uncertain, never fabricated EXIT or a lifecycle count. |
| ntsrv-exe/opennt/source/base_service.c:Get | Project transport wraps original dispatch under the service lock and only later authorizes I/O. | Capture read-only Direct display identity at actual successful command delivery. Preserve original GetNext reply, waits, streams and execution ordering. |

Recovery ladder: the original VDD callback implementation is already composed
and selected directly. The unavailable boundary is cross-process copied
observation delivery, not guest execution. The smallest additional mechanism
is a worker-owned bounded copied outbox and authenticated observation RPC;
neither recreates the original scheduler or completion. No mirror hook is
currently selected. Any missing first-target boundary must be proved before
considering and registering a minimal intrusion.

## Implementation contract

NTVDM owns PSP extraction and occurrence/parent correlation; worker-base owns
the bounded outbox/wake/publisher mechanics; common owns copied event types and
RPC client; NTSRV validates worker identity and stores only read-only facts.
Callbacks cannot call RPC, wait for the broker, or retain guest aliases. Queue
loss is explicit coverage loss, not permission to block the guest or retry
execution. Thread stop must cancel/drain transport before client disconnect.

The command delivery must identify its existing Direct display node, so a
delayed callback batch cannot be assigned to whatever happens to be current
when it arrives. Prefer a separate copied output of the project RPC binding;
do not change original BASE_GET_NEXT_VDM_COMMAND_MSG, DOSRECORD or guest BOPs.
The first entry for that delivery correlates to Direct only when proved; known
internal EXEC children are Observed even with DOSONLY0. PSP reuse gets a new
occurrence, not a replacement of historical identity. Unknown parent/image and
TSR terminal uncertainty remain visible. WOW callbacks are not DOS task facts.

## First implementation evidence

common/protocol/dos_observation.h declares copied occurrence/parent/Direct
identity and PSP/image facts only. worker-base/task_observation owns an explicit
128-entry FIFO, event-driven consumer, nonwaiting producer offer, sticky loss
notification, finite normal drain and cancellation/join. It neither selects
tasks nor extracts guest pointers. The existing latest-frame publisher cannot
be reused because replacing a pending CREATE with EXIT would lose history.

tests/app/worker_task_observation_test.c exercises the actual shared module:
hold the consumer outside the queue lock, fill128 accepted facts, reject the
next with a gap, release and prove129 accepted occurrences in exact order on
a different thread. Drain cannot finish during pending publication; after all
accepted facts it completes. Cancellation releases a pending send before join;
repeated stop is harmless. MSVC /MT /W4 x86 and AMD64 compile/run pass with no
warnings in r001-outbox/{x86,x64}-{build,test}.log. The temporary build wrapper
is build/M0-T434/S4/build-outbox.ps1; the assertion source is tracked.

That first result is mechanism/unit evidence only. The subsequent candidate
connects collector registration to NTVDM startup/cleanup, original VDD callbacks
to copied PSP occurrence/image capture, the outbox to async observation RPC,
and NTSRV to scoped sidecar facts. Original mirror files remain unchanged.

The project copied Get prefix adds a read-only Direct identity instead of
altering the original BASE message. It is captured under existing locks at
actual command delivery and parent RETURN_ON_NO_COMMAND resume. The receiving
execution thread preserves it in each fact, so delayed publication cannot be
reparented to a later request. Common async cancellation/completion mechanics
are shared with native reports; native250ms is unchanged, DOS background
publication uses10000ms plus the worker's stop event. Producers never wait.

r002-build deliberately reuses the declared unsealed S3 compiler caches,
preserving their earlier graphs/manifests before regeneration. Sealed r042
runtime and installed S3 images are untouched. Initial MIDL44/native service/
CCPU40 NTVDM compilation passes; original VdmTib storage validation passes.
r003-service-fixtures passes all29 retained cases (concurrency2,19698ms),
including a new check that the exact Get reply carries a nonzero Direct ID
and wrong-request/truncated replies cannot supply one. Native observation,
ordered outbox and common management124-check fixtures pass separately.

The initial default fixture failed at line1944: its old Direct-only row-count
assertion did not allow the now explicit read-only delivery history. Preserve
service-fixture.log and the failed result. The replacement asserts exactly one
current fresh Direct identity plus the previous HISTORY-marked unknown row;
it does not count history as active or weaken record-reuse identity checks.
The failed parent's suspended child13852 retained redirected output. Its actual
kernel image/creation identity and terminal parent19272 were checked before
ending only that orphan; rerun uses the existing bounded private-Console runner.

PSP source identity is subsequently added to the copied trace row so actual
guest witnesses can verify Direct/root association, not just image labels.
Final layout is576 bytes/image offset56 at both widths; common/IDL evolve44
together. The new DOS sidecar fixture covers occurrence replay/reuse, true
termination versus missing-terminal uncertainty, forged scope and quota.
This final layout/fixture needs recompilation and real guest validation; the
earlier compile/unit results do not certify the later ABI.

r004-build then compiles the final576-byte/offset56 layout with MIDL44 and
the production native service and x86 CCPU40 NTVDM. dos-fixture-fixed.log and
dos32-fixture.log both pass occurrence/parent/replay/reuse, actual callback
EXIT versus entry-only uncertainty, wrong scope, quota and non-retention.
Native observation fixture still passes; client32-fixture.log records124
checks/zero failures/zero allocations. VdmTib storage remains4208 bytes with
the original owner. The first DOS fixture reached teardown but asserted a
DWORD ERROR_SUCCESS as BOOL; preserve dos-fixture.log, correct the test to
compare ERROR_SUCCESS, and rerun rather than claiming the failure as pass.

No real DOS callback/RPC/COMMAND root correlation has yet been proved.
All ten native consumers still need coherent44 rebuilding/staging before
that integration. Installed S3 stays43. No publication, source delivery or
S4 closure is claimed.

## Actual guest integration so far

r005-consumers rebuilds all native protocol consumers and both Hooks44;
client64 still passes124 checks. r007-runtime stages one ten-image candidate,
never publication. Independently authored dos_observation_probe.asm produces
real-mode COM witnesses in r006-probes. Each process obtains its own PSP with
INT21h/51h, copies its parent from PSP16h, appends a10-byte journal record,
and uses ordinary INT21h/4B EXEC/4D result/4C exit. Test-only filesystem gates
hold actual programs while real NTSRV snapshots are queried; no production
protocol/scheduler or guest media is modified.

r008-normal passes: actual rootPSP1828/childPSP2201 and parent1828 match one
Direct and one sourceDOS Observed. Child7 termination is recorded, with no
invented observed exit code, and Direct remains pending until actual root37.
r009-Reuse passes: the same PSP2200 is reused by C.COM and F.COM, producing two
distinct nodes with preserved terminal history and correct root parent.

r009-Tsr first exposed the test's live ReadAllBytes sharing error. Correct the
reader to share read/write and only treat transient sharing32/33 as journal
readiness inside the existing finite case; no assertion or whole-case retry
is waived. r010-tsr then proves parent return type3 and child uncertainty but
fails its assumed automatic root37 completion. Identical sealed S3 inputs in
r012-baseline-tsr also wait beyond5000ms and reach COMMAND's Z prompt rather
than automatically returning. This proves the behavior is not introduced by
S4, not that a generic guest/host defect has been diagnosed or waived.
r017-tsr consequently verifies the actual contract: TSR node stays uncertain,
the root's genuine termination callback records EXIT, and the launcher remains
waiting as in S3. Its final process cleanup is explicit; no natural TSR carrier
retirement or actual Direct completion is claimed by this case.

r013-command failed before execution because bare command.com was not in the
test working-directory/PATH. Use the existing explicit system32 target, not a
production search/parser change. r014-command then proves the3-node PSP chain
but fails the test's assumed37 receipt: actual COMMAND /C returns0. The sealed
S3 control r015-baseline-command returns the same0. r018-command passes exact
COMMAND Direct PSP1829, Observed P.COM2629 and C.COM3002 with both sourced
parent edges, unchanged Direct depth1 and baseline0 completion.
The attempted S3 scripted-EXIT control r016 has only an initial running report,
not a valid completion result; it is not a pass or receipt-code oracle.

dos_observation_pif.c uses original packed STDPIF/NT31 declarations to request
SUBSYS_DEFAULT0 or SUBSYS_DOS1 for COMMAND /C. r019-dosonly0 exposes missing
startup paths in the authored NT extension, which replaces CONFIG/AUTOEXEC
even when empty. The fixture now references the existing immutable system32
files. r020-dosonly0 and r020-dosonly1 both pass real3-node PSP/parent/normal-exit
and baseline0 checks. These prove the EXEC witness under both explicit PIF
configurations; no private runtime flag-memory inspection is claimed.
All cases run serially, remove only their own Z mapping and owned processes,
and retain their exact logs rather than merging failed attempts into passes.

Remaining gates: failed/load-only/overlay/builtin exclusions, authenticated
transport negatives, parent-return/reentry integration, full retained native/
product/mixed regressions and coherent publication. Installed S3 remains43;
S4 is not closed or committed.

## Verification still required

r022-exclusions uses the expanded authored COM witness: missing EXEC returns2,
AL1 load-only succeeds without entering F.COM, its current-PSP/memory effects
are handled using original AH50/49 APIs, and AL3 overlay loads the expected
first byte into a separately allocated block without executing it. After all
three operations, ordinary EXEC still produces the exact two-node root/child
trace, real child7 and Direct37. No fake running node is introduced.

r005-consumers/negative-rpc-final.log proves actual DOS observation rejects a
non-worker authenticated connection, wrong generation, a different process
attachment and old protocol, with no tasks created. Native negatives still
pass. The initial negative-rpc.log failed1717 because the monitor fixture's
sibling service was an older43 cache image; replace only that fixture-owned
service with the verified44 candidate, retaining the failed run.

dos-observation-resume.bat exercises VER then real Win32 CMD output to a unique
file before the held DOS witness. r023 failed because the copied LF template
was interpreted as one DOS line; generated BAT now uses CRLF. r024/r025 exposed
test identity/oracle errors, not wrong recorded parents: a terminated internal
COMMAND and the new P.COM reused one PSP, so selecting by PSP alone returned
two occurrences; filter the held, nonterminated occurrence and preserve its
history. The same S3 BAT control r026 returns37, unlike the earlier direct
COMMAND /C control0. r027 passes actual native output, sourced current DOS
parents, three nonterminated DOS facts and that exact baseline37 receipt.
The VER route has a genuine observed internal COMMAND entry/termination in
this configuration. Do not suppress an actual original execution to force an
assumed builtin count; the negative guarantee applies when no entry occurred.

r028-full passes independent WOW and Console17/Window17, then fails RPC
bootstrap. Its console report explicitly says local protocol43 versus the44
broker: the product images are coherent, but a copied formal bootstrap test
client was not rebuilt. Preserve the failed timing/report, not a merged pass.
An initial dependency rebuild also correctly refuses implicit x86 frontend/
service fallbacks because the formal graph lacked selected native suppliers.
Regenerate with the existing verified native inputs and rebuild bootstrap,
request-client and service fixtures. No assertion or product wait is changed.

All ten r007 core hashes remain identical. r032-full now reruns the complete
serial gate with the corrected fixture closure; session15409 is its exact
running process handle at that checkpoint. It subsequently completes with
all14 Passed=true in436898ms: both17 routes, independent WOW, real RPC/GUI,
version rejection, DIR, modern EDIT, cooked/repeated return, isolation,
retirement and nested Window handoff. Full reports identical10 product files.
The earlier failed r028 remains separate; no assertion or case was removed.
Remaining: final native observation regression, complete product/mixed gates,
source/ownership review, coherent publication and deployed smoke. Positive
source/guest facts above do not prove those remaining gates or full S5 UI.

Review checkpoint: producer callbacks only copy checked guest data and offer
ordered facts; they never perform RPC or task completion. The outbox releases
its lock before transport, uses finite normal drain and cancellation/join before
the client/session is released. VDD registration is bound to the one execution
thread and explicit session; WOW skips it. NTSRV facts remain outside IsEmpty,
receipt and retirement state. No selected mvdm/opennt-host mirror file changed.

build-dos-observation-probes.ps1 is the reproducible tracked construction
entrypoint. r033-probes compiles authored COMs and the PIF builder, records
assembler/source/output identity, and reproduces all six r021 COM hashes.
PIF PE image differs after rebuilding; no byte-identical claim is made for that
native test builder. Source/tool/CRT and assertions, not an unproved binary
identity, define its equivalence. This adds no production process or asset.

r029-mixed/results.json now proves22/22 cases pass on the same r007 package,
including all three outer shells and both display routes, search, GUI and WOW.
r030-typeahead completes6/6. The same serial runner completes all12 final
native cases under r031-native-{x64,x86}-{ShortChild,SuspendedChild,Concurrent,
CmdNested,RootFirst,ForcedChild}, including16 concurrent short children at each
width and independent actual child23/Direct37. r038-{deny,slow}-{x64,x86} also
passes actual rejection/loss and injected2000ms delay after the shared async
helper change; slow32 CREATEMS281. These diagnostic services never publish.

r034-publication installs the exact r007 APP434/RPC44/I/O25 ten-image manifest
and Hook license at O:/winnt/system32; full S3 recovery is under build.
Guest/configuration remain unchanged. Actual deployed r035 DOS/native32/native64
reports are O:/winnt/logs2/t434-s4-r035-{dos,native32,native64}.txt: expected
output, exit0 and all10 installed hashes pass. Owned test processes are
identity-checked/ended and Z is removed. Review confirms no original mirror
or execution/receipt/lifecycle ownership change; only copied observation facts,
metadata, transport and projection are added. Source delivery commit/push is
the remaining bounded S4 step; full NTMON hierarchy/time/revision fidelity is
S5, not yet a full T goal completion. Other-session queue/proposal edits remain
outside this reviewed delivery.
