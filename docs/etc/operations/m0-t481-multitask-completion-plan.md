# T481 multitask payload completion

## Ownership and baseline

Owner admits the whole queue-first package into T481. S1 delivered integrated
T479/T480 source at ceea7f1f8, package25408bd6-e51b-489d-9255-0951c53ad9f3.
S1 Full16/16 and Win3.1 display/command/exit evidence does not prove complete
asynchronous VM I/O. Original guest binaries remain immutable.

The [S6 handoff](../evidence/m0-t479-s6-closure-handoff.md) and
[operation audit](m0-t479-s6-io-audit.md) preserve the full failure history.
The [original proposal](../../proposals/proposal-guest-multitask-io-completion-001.md)
retains the nine-row obligation inventory. Admission transfers all rows;
no missing capability is silently waived.

## Sequential scope and exit conditions

| S | Scope | Required outcome |
| --- | --- | --- |
| S2 | Input candidate, input return, normal/cancelled lifecycle | Ordered/repeated/extended input, greater-than15-word handback, two-VM isolation/reuse; blocked I/O cancellation and normal exit; historical fill002/119 disposition follows the owner-approved closure below. |
| S3 | Remaining Win3.1 input/text/geometry/fonts/codepages/display/metadata and nested chains | Reached production caller/provider matrix; actual guest application acknowledgements; two real VM chains in both completion orders, redirection and cancellation. |
| S4 | Win95 after Win3.1 | Wait/wake, identity, init rollback, input/presentation and lifetime verified separately; qualified .vxd artifact. |
| S5 | Combined qualification | All obligation rows have actual acceptance evidence or explicitly approved source-proven immutable-guest disposition; complete11 host images and addon manifests, applicable Full regressions, publication, review, commit/push. |

Every production delivery follows the existing complete-package gate. Necessary
foundation repairs stay in their owning stage. Protocol definitions without real
callers do not justify speculative implementation. Keep NTSRV lifecycle authority,
RPC control and existing I/O channels. Approved unread-input handback retains
BIOS/INT16 characters, function keys and order without IRQ1/TSR replay guarantees.

## Initial source audit

Read-only inspection on2026-11-07 finds current vm_text.c still rejects changed
grid dimensions and non-full-grid viewport changes. mvdm_vm_io.c rejects unsupported
input/output processing flags. These are explicit present limitations, not passes.
The archived input candidate is still unselected. git apply --check succeeds
against current source; the inspected executor/I/O/wake/notification and addon
paths have no changes since its pinned base. This proves mechanical applicability,
not correctness or acceptance. Review all10 files before selection.

The old proposal's separate windowed-DOS display debt is superseded by
[T479 S9](../evidence/m0-t479-s9-delivery.md); that does not waive VM payload
presentation, geometry, acknowledgement or input coverage. Historical S6 package
identities remain evidence only; the S1 package is the current delivery baseline.

## Evidence and remaining proof

S2 review must bind each test to its exact production entry and guest boundary.
Archived passing input189/190 results are partial and pre-integration. The failed
fill002 and119 outcomes remain open until attributed; a later passing run alone
is not closure. No current implementation or runtime pass is claimed by this plan.

## S2 candidate recovery and verification checkpoint

The10-file archived candidate was reviewed and applied without conflicts.
Input005 passes2959 notification unit assertions on each of x86/x64; conversion
uses a test substitute and is not original-keyboard or real-guest proof.
Worker006 builds the actual NTVDM and authored .386/.vxd outputs successfully;
the .vxd is not accepted or released. Product publication remains unchanged.

Fill007 fails before guest probe startup because the source installation already
loads NTVDM64.386 and the old runner added a duplicate. The runner now replaces
only that authored-device entry in its copied386Enh section; original source
installation hashes remain unchanged. Fill008 reaches the guest desktop but
exits with0xc0000005. It is NOT a qualified reproduction of historical fill002:
the supplied normal product lacks host-continuation.c and its test-only BOP254
interposition required by this diagnostic witness. The actual owned NTVDM error
record identifies offset0x1a66a0. Do not attribute this to input or historical
shutdown failure without further evidence.

Diagnostic009 restores the original separate-link recipe and finds stale test
calls to mvdm_vm_executor_key missing the explicit VM argument. These are fixed
using the calling BOP's existing vm identity. Diagnostic010 is the rebuild;
its result must be inspected before staging or running the diagnostic package.
All receipts are under build/t481-s2. No failed check is counted as passed;
fill002/119 and the real-input/nested acceptance requirements remain open.

## S2 real-guest input evidence update

Diagnostic010/017 compile the separate test-only host link. Fill011 passes the
original diagnostic fill path, two VM identities,320 callback checks and normal
Windows exit. This passing repetition alone does not attribute old fill002.
Real012 passes all per-VM command/callback/exit checks but fails the old final
interval-overlap assertion. dos-identity.asm starts its .timing interval AFTER
commands and peer wake; this is not VM lifetime. The corrected assertion retains
distinct VM/generation and both successful pair checks, including actual peer
VMM blocked+driver-wait state0x401. Task overlap/exit ordering still needs the
ordered-command witness, not this identity proof.

Unread015/016 initially fail with actual BIOS word0. The diagnostic host
interposed BOP18 with empty success, predating production input handback.
Real-command mode now delegates BOP16--18 to production. This is a test routing
repair, not a change to the runtime input implementation. Unread018 then passes:
two actual VM parents, two native commands each,20 ordered returned characters
per command, double peek before read, exact scan/character checks and empty
queue after consumption. VM A receives a/b; VM B receives c/d. Both return
normally and original installation hashes remain unchanged. Native records and
guest results reside in build/t481-s2/unread018 and O:/tmp/m481u4/PATCH.

The native witness writes deliberate unread Console records; it is not physical
keyboard evidence. Input-matrix020 extends that exact route to20-repeat Z,
standard F1 and extended F11. Its PID/result must be observed before claiming
those modes. No candidate publication, production P or S closure has occurred.

## Extended-key failure attribution and candidate correction

Input020-repeat and input021-function pass actual guest handback and normal
exit. Input021-extended fails: both VM reports have unread stage1 (first peek
empty), continuationErrors2, then notificationErrors1. Native reports prove
F11 scan0057 was written. Thus the later notification assertion is not the
first failure and remains to be rechecked after input repair.

The candidate reverse-searches BiosKeyToInputRecord for zero-character keys.
Its original nt_keycd.c table maps BIOS85/86 to zero and87--8c to85/86 rather
than physical57/58: there is no usable inverse for native F11/F12. The host
adapter now maps those two physical keys using spckbd.asm lowercase133/134,
uppercase135/136, ctl_f_table137/138 and keybd_io.c Alt code+52; modifier
precedence follows original Alt, Ctrl, Shift. Original mirror and guest bytes
are unchanged. Input022 passes x86/x64 notification tests including both keys,
all four modifier modes and Ctrl/Alt precedence. These are unit receipts;
real guest extended-key and notification recovery are not yet proved. No new
publication or production commit is claimed.

Unread024 rebuilds the diagnostic link from Worker023 and passes the actual
Win3.1 extended F11 path: both VMs, both native rounds, repeated peek/read,
BIOS8500, zero continuation/notification errors and normal Windows exit.
Original installation hashes remain unchanged. This attributes input021's
first failure to the missing F11 conversion; it does not attribute historical
fill002/119. Product delivery remains pending the remaining S2 matrix.

Retirement025 uses the actual production NTVDM candidate (not diagnostic
interposition), driver023 and coherent package025. A native CMD starts before
forced VM retirement; late wake is rejected, Windows exits normally, and the
same VM handle2155450368 is reused from generation2 to4. Both old direct and
queued wakes are rejected; the new command returns37. The matrix's
386-dosonly1.native-retirement.json observes remaining=[] BEFORE scope cleanup.
This proves the marked native child retired, not all worker/service-loss paths.
Historical fill002/119 remain unclosed and are not replaced by this pass.

Ordered026 passes both A-first and B-first actual native process orders, each
with both54:08 forms and normal Windows exit. The child synchronizes both
live processes and the second waits on the first process handle, verifying
its exit code before exiting. Both guest VM reports have zero continuation,
callback and notification errors. This proves the native command overlap and
opposite completion orders missing from the clock-interval identity check;
it is not physical input, service-loss or full nested-chain evidence.

Service027 reuses verify-run16-service-loss with RequireCarrierCleanup against
production package025. It pins the real CUI CMD and ping descendant before
killing only the exact isolated NTSRV. Run16 returns ERROR_INVALID_HANDLE(6),
NTVWM exits6, and both pinned target/descendant terminate before containment;
observed elapsed100ms. This is native CUI service-death evidence, not guest VM
service-loss or attribution of old119. The remaining service-loss integration
must inject while an actual guest VM owns a running native chain.

Read-only lifecycle review identifies a possible Count/ReadConsoleInputW race
in ntvwm_presentation_end's return_unused_input: a prior pending count does
not itself make a later synchronous read nonblocking. Existing input-pump
join and NTSRV ownership must be checked against every possible reader before
calling this the old119 cause. No speculative production patch is applied.

## Actual guest-VM service-loss failure028

The new service-loss witness starts two actual Win3.1 VM native ORDER.EXE
processes. Each atomically publishes its PID then remains alive. The matrix
pins both children and all exact-stage host processes, kills only that stage's
NTSRV, and checks departure before containment. Service028 fails: native tasks
and NTVWM disappear, NTVDM26160 remains. matrix-results.json records
Service-loss survivor:26160, normalExit=false and passed=false. Cleanup is not
a pass. The test's original guest installation hash check still passes.

Captured service028-ntvdm-snapshot.txt maps against the exact diagnostic link
map: event thread23604 is in DisplayErrorTerm -> host_error -> ErrorDialogBox,
its UI thread27396 is in ErrorDialogBoxThread; termination thread24132 waits
in mvdm_softpc_event_thread_alert_and_join. The CCPU thread continues running.
The dialog error argument is233 (pipe not connected). Original nt_event_loop
calls DisplayErrorTerm on failed Console read; the adapter exposes disconnect
as an ordinary read error before terminal cleanup can join the event thread.
The pending alert cannot release a thread already blocked in a modal dialog.
This is a current reproduced shutdown defect, not proof of historical119's
exact cause. Repair must distinguish authenticated terminal shutdown from
ordinary input failures, preserve original event-thread return/cleanup, and
not introduce a deadline, forced thread kill, fake input or dialog dismissal.
No production repair for this new finding has yet been applied.


Service030 repeats the exact two-VM service-death witness after the minimal
terminal input join repair. It passes: all pinned host and native task handles
signal before scope cleanup; normalExit remains false, serviceLoss true.
Worker029 cached production build and diagnostic relink succeed. Terminal031
builds/runs console-client-test --input-terminal: uninitialized/live-service
errors stay nonterminal, confirmed service death is terminal, GetLastError is
preserved, and lifetime end clears the predicate. The mirror delta is8 added
lines in nt_event.c, no deleted lines, registered under DIV-355; existing mixed
line endings and diagnostic logical lines390/409 remain intact. ConsoleEventThread
still performs its original handle close and identity reset after loop return.
Input032 checks ordinary sequence/extended handback and normal exit against
this repaired link; results must be read before claiming those regressions.


## Simplicity review and delivery preparation

Owner reiterates minimal unified ownership: prioritize working normal paths;
errors bubble to the existing lifecycle owner, with no added retry/recovery
machine. The S2 candidate adds no product deadlines or retry loops. Command
held-key state belongs to that command; unconsumed records transfer once after
callback join to the VM generation; the addon caches only the current BIOS
word/repeat while implementing peek. These are successive ownership stages,
not competing keyboard-state authorities. Guest BIOS remains modifier owner.
The terminal-input predicate observes existing lifecycle state rather than
creating a new shutdown state. Input032 passes sequence and extended-key normal
handback plus normal Windows exit after the shutdown fix. Terminal predicate
coverage is now part of verify-vm-frontends, not a scratch-only test.
Cached033 starts the existing complete x86/x64 component and shared-library
build/test recipe; do not claim its result before observing completion.

Cached033 completes successfully: selected11-component x86/x64 builds and
shared-library suites14/14 per architecture. Governance036 passes. Full034
starts with the established logon-environment wrapper; all9 VM frontend and
lifecycle cases, including input-terminal, pass. Complete Full result remains
pending and must not be inferred from these preliminary passes.


Final ownership review catches a predicate refinement before delivery: local
closing can be claimed by authorize_exit BEFORE NTSRV grants the request.
Input terminal status must instead read only the existing signalled server,
worker-close or Console-close handles. Natural-exit fixtures now assert false
after the request but before grant and true after grant. This adds no state.
Full034 is still exercising the previously built candidate and cannot qualify
this refinement; after its process finishes, rebuild and rerun applicable gates.
Selected failure/passing receipts are preserved with hashes in
[artifacts/t481-s2](../../../artifacts/t481-s2/receipts.json).

## Refined ownership validation039/040

Full034 passes all16 Full gates (313639ms), but precedes the final terminal
predicate refinement. Worker038 rebuilds that refinement successfully.
Terminal039 passes all9 registry/lifecycle cases, including natural-exit
permission: merely requesting exit remains nonterminal; the authenticated
service instruction makes it terminal. Service040 repeats the actual two-VM
native-child service-death case successfully. All10 pinned processes depart
before scope cleanup;8 project clients return6, both native children receive
CONTROL_C_EXIT. normalExit=false is intentional, serviceLoss=true.

Driver038 is byte-identical to driver029, SHA256
A2A8690A5667BF402A491163BE8E77662C2A9A408ECD642AF0F1FDF8577C6D64.
Thus earlier addon normal-input checks do not cover a changed addon; the host
predicate has changed and is undergoing a new Full041 gate against a newly
staged production package. Full034 cannot stand in for that result.

Final source review confirms one-way input ownership: producer admission stops
before frontend retirement joins callbacks; only then are unread records moved
to the VM-generation FIFO. Retirement/reset frees that FIFO; the addon owns
only its currently returned BIOS word/repeat. Input terminal detection adds no
state, retry, deadline or alternative shutdown coordinator. Unexpected live
input failures still retain the original error path. These are source findings,
not additional physical keyboard or Win95 acceptance.

## S2 P1 verified input/lifecycle baseline

Full041 passes16/16 on the refined production package (328673ms).
Live042 passes production VM forced retirement/reuse and normal Windows exit;
the final diagnostic host also passes two VMs x two native rounds x20 returned
characters, repeated peek/read,320 callback checks and normal Windows exit.
Original Windows installation hashes are unchanged. Diagnostic-only executables
are excluded from the release. Full and live receipts are archived above.

Publication044 verifies all11 images, hook license and common manifest at both
assets/release and O:/winnt/system32, package
8b216ec7-75d1-4979-a21e-c588ca4d02d6. The qualified Win3.1 addon is published
only under assets/release with its updated manifest; no Win95 alias is published.
The complete previous package is preserved in build/t481-s2/publication-backup044.

This P is a working baseline, not S2 closure. fill002/119 historical attribution,
remaining cancellation/rollback obligations and encoding boundaries remain S2
work; the previously assigned S3/S4/S5 obligations are unchanged. Neither old
failure is declared repaired by a newer passing run. No new semantic concession,
retry, deadline, reconnect policy or second lifecycle authority is introduced.
Mirror runtime delta against the previous baseline: nt_event.c +8/-0, registered
DIV-355; guest bytes unchanged. Addon +252/-4 and NTVDM-owned runtime +369/-11
implement the retained input ownership/conversion, not mirror algorithms.

P1 is committed/pushed at a7024b666. A post-commit blob audit finds34 of36
raw evidence files were newline-normalized by inherited core.autocrlf behavior;
their retained build-source bytes still match every recorded hash. A follow-up
evidence-only P pins artifacts/t481-s2 to -text and restores those exact bytes,
then verifies each staged blob against the manifest. No source or product image
changes and no runtime-test repetition are involved.

## S2 OEM boundary and historical evidence audit

OEM048 is a test-only addition using the unchanged published runtime closure
and separately linked diagnostic host040, addon038. Both actual native children
report codepage437, write U+00E9/scan12 for A and U+00E4/scan1e for B, twice.
Both guest parents observe exact BIOS words1282/1e84 after two peeks and one
read, then an empty queue.320 callback checks and normal Windows exit pass.
Native reports, matrix and pair receipts are preserved. The existing unit
negative leaves unrepresentable U+4E00 unconsumed with
ERROR_NO_UNICODE_TRANSLATION. This closes the CP437 non-ASCII proof gap only;
DBCS/code-page changes remain the assigned S3 operation boundary.

Historical fill002 review confirms its trace reached both report markers and
both VM-retire notifications, but retained evidence does not identify the later
shutdown failure. Its original receipt names NTVDM hash525223C8860140E654DBCEB30B4A563B978B3C078A531AB56870E485872583EF.
The still-existing disposable fill002-package now contains hash
10306EB6FB18E156E6C4A873CB7BDBAA607F94E7CE6D749B1AC1C80F0A1B513F
and a different package manifest. The current continuation-worker image/map
also predates/differs from that observation. Do not symbolize the old snapshot
against those mutable paths or treat them as the original failing package.
This is an evidence limitation, not proof that the product failure is resolved.

Four dispatch cancellation inputs have source owners: channel stop and worker
handles come from frontend-base; VM retirement and broker handles come from
vm_frontend. Existing application_tests selects each independently and a
combined cancellation; actual service040 and retirement042 cover broker death
and VM retirement. Full cancellation/rollback acceptance still needs explicit
mapping of remaining real paths; no new recovery mechanism is warranted by
this audit. Historical119 still lacks a captured blocked carrier stack.

## S2 P4 actual cancellation coverage

Worker-close051, Console-close054 and service-loss055 pass with two real
Win3.1 VM native chains held alive before the selected lifecycle action.
Each receipt proves all10 pinned service/client/native processes exited before
scope cleanup. Management cases authenticate the monitor against the staged
package and require the service alive at acknowledgement. Service death uses
the exact pinned isolated NTSRV. These intentional exits are not normalExit.
The existing production retirement042 covers VM retirement independently.

Failed050 was test package authentication: converting the raw16-byte identity
as GUID text reversed GUID fields. The runner now constructs GUID from bytes.
Failed052 already proved all10 process exits, but the outer CMD test batch
waited for its legitimate Console-close confirmation.054 answers only the
observed (Y/N)? prompt through the existing observer and then verifies exit.
Neither failure justified a product recovery mechanism. Failed and passing
receipts are retained in the receipt index above. Failure containment only
kills already identity-checked pinned test processes, after recording failure;
it cannot establish the beforeScopeCleanup success assertion.

This delivery changes two integration runners only (+49/-12); runtime, addon,
mirror and release image bytes are unchanged from P1. No product build or
repeat Full gate is needed for this test-only change; the three actual affected
integration paths were run. No new product timeout, retry, state or lifecycle
authority is introduced. fill002/119 attribution and initialization rollback
remain open; this does not close S2 or advance to S3.

## S2 P5 initialization error propagation

Input057 cached builds and notification tests pass on x86 and x64. The added
executor_registration_failure_tests uses the real executor, threadpool,
continuation, I/O provider and completion consumer, with existing broker/launcher
substitutes. Payload creation denial leaves output NULL. Access-denied and
broken-pipe frontend registration failures reach the consumer unchanged, with
zero exit code, exactly one launch release per submission and no retirement of
an unregistered frontend. Destroy joins completion; the next initialization and
submission succeeds. No product logic or mirror bytes changed.

Source audit distinguishes pre-registration rollback from post-registration
ownership: vm_frontend.c publishes identity after broker registration, so later
service startup failure retains an owned entry for retirement. The existing
vm_frontend_test.c fail_state case asserts this identity survives failure,
rejects launch-context access and destroys the registry. Terminal039 previously
ran that registry fixture. These are unit boundary proofs, not new real RPC
fault-injection evidence. Historical fill002/119 still need attribution.

P4 was pushed as4aeae35de. Its evidence preserves failed050/052 and passing
051/054/055. Historical fill002's C: temporary directory is the deliberate
LocalDirectoryControl probe input, separate from its O: result files; this
path difference is not established as a failure cause. Do not patch it on
that hypothesis. Product remains P1, with no additional recovery machinery.

## S2 closure and S3 admission

Owner approves closing S2 with the two unreproduced historical failures retained
as history, then clarifies: investigate only a reproducible current failure.
fill002's fill/exit rerun passed; current normal/cancelled two-VM paths pass.
119's exact old stimulus timing was not preserved, so exact replay is not claimed.
Neither old failure is declared fixed. Neither is an active defect, TODO item or
S3 obligation. Reopen only on a concrete current reproducer. The broad historical
binary-hash search was cancelled after this instruction, without a match claim.
This supersedes earlier requirements in this plan/proposal to attribute them
before closure, not the obligation to fix newly reproducible failures.

S2 completion evidence is retained in artifacts/t481-s2/receipts.json:

| Requirement | Evidence and boundary |
| --- | --- |
| Ordered input beyond15 words, peeks, repeats, F1/F11 | unread042 and input020/021 real guest receipts; original conversion and focused units; no raw IRQ/TSR replay claim. |
| Two-VM input identity, reuse and retirement | unread042 and production retirement042, different generations and native child departure before containment. |
| CP437 non-ASCII return | OEM048 two VM/two round exact BIOS words1282/1e84; DBCS/codepage-change behavior belongs to S3. |
| Normal exit and both completion orders | fill011, ordered command controls and final live042; old failed runs remain non-passes. |
| Cancellation | application_tests all four cancellation handles; service040/055, worker-close051, Console-close054 and retirement042 actual chains. |
| Initialization and failure ownership | terminal039 registry rollback; input0573126 assertions per width, real executor/provider with broker boundary substitutes. No blanket real fault-injection claim. |
| Delivery | P1 complete11 package8b216ec7-75d1-4979-a21e-c588ca4d02d6, Full04116/16, published .386; P2-P5 preserve evidence and improve tests without product changes. |

S2 is closed by owner acceptance of this bounded evidence and historical-failure
disposition. P5 is f25220d98. S3 now owns the already approved remaining Win3.1
input/text/geometry/font/codepage/display/metadata and nested-chain requirements;
S4 Win95 and S5 integrated qualification remain unchanged. S3 starts by tracing
actual production producers to providers; no speculative implementation of unused
wire operations, new recovery engine or second lifecycle authority is admitted.
