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

## S3 first reproducible geometry failure

Geometry001 runs the existing real ordered native-command path in two Win3.1
VMs, diagnostic host040 and addon038, adding a native80x50 buffer with80x25
viewport. Both native RESIZE receipts prove SetConsoleScreenBufferSize and
GetConsoleScreenBufferInfo succeeded. The actual guest continuation assertion
fails for A; the outer normal Windows exit passes. This is a current failing
integration case, not an old-risk investigation or a passing delivery.
Evidence currently resides at build/t481-s3/geometry001 and the test-owned
O:/tmp/m481g1/PATCH/RESIZE-A.txt and RESIZE-B.txt.

Source chain: presentation.c capture retains full native buffer cells, while
ntvwm_text_frame_pack already packs only srWindow into its display frame.
publish_copy sends BUFFER_SIZE before that frame whenever full buffer size
changes. vm_text.c rejects that unsupported operation, and also requires the
VIDEO frame to match the actual guest grid. The native buffer is80x50 but its
visible frame is80x25; conflating them rejects an otherwise unchanged visible
geometry. The next repair should distinguish full Console state consumers
from presentation-frame consumers using the existing I/O contract, preserve
NTCON full-buffer semantics, and avoid adding a second virtual Console engine
inside the VM provider. Actual visible-grid changes remain a separate S3
obligation; accepting scrollback alone must not be called complete geometry.

## S3 required two-editor interaction acceptance

Owner adds simultaneous Windows3.1 MS-DOS prompt windows running the exact
O:/winnt/edit.exe and O:/winnt/edit.com inputs. Verify image identity and actual
native/DOS route, typing, Alt menu activation, arrows/Enter/Esc, guest mouse
menu clicks and menu exit, focus switching without cross-VM input, survivor
interaction after the peer exits, return to both prompts and normal Windows
exit. Startup alone is not acceptance. Both files currently exist (266800 and
413 bytes); edit.exe is PE AMD64; edit.com is the DOS QBASIC /EDCOM launcher. Interaction remains unverified.

## S3 candidate geometry verification

Cached002 builds pass for x86/x64. Full004 passes all16 gates in315247ms;
this is the production r004-package, not diagnostic package003. Native006
passes presentation, input-return and execution-lifetime fixtures after the
new query is explicitly handled by their real I/O peer.

Geometry003 first passes the former80x50-buffer/80x25-display failure.
Geometry005 strengthens it: each real native child writes a distinct lowercase
VM marker with attribute0x2f at absolute row25, moves its viewport to rows25..49,
and the guest continuation reads physical screen cell0,0 through INT10 and
checks that exact marker/attribute. Both VMs and command rounds pass, as do
320 callback identity/stack/register/flags checks and normal Windows exit.
The original installed Windows files remain unchanged. These runs use the
existing diagnostic continuation observer and do not prove dual EDIT interaction.

Candidate uses the existing WINDOW_QUERY to distinguish a physical display
consumer from a full native Console consumer. Native buffer/absolute viewport
state remains with NTVWM; the guest receives the existing packed visible frame.
NTCON retains full buffer publication. I/O wire version31 rejects incompatible
peers. No mirror or original guest modification, new transport, lifecycle owner,
retry or product timeout is introduced. Actual visible-grid resizing and the
owner-required dual EDIT input/mouse acceptance remain open. Candidate has not
yet been published or committed; the published baseline remains S2 P1.
S3 P1 delivery: geometry007 also passes with B completing first. All16 Full004
gates, native006 and governance008 pass. Complete11 production package
c0c312c1-01ca-408f-8b98-58ab8d9e812e is published and hash-verified at both
assets/release and O:/winnt/system32; unchanged Win3.1 addon remains assets-only.
Raw failure and success receipts are preserved in artifacts/t481-s3/receipts.json.
This supersedes the candidate-not-published statement above. Review confirms
all actual protocol providers answer the capability, failed query/commit bubbles
through the existing error path, and full Console publication remains selected
for NTCON. No mirror diff. Dual EDIT interaction and remaining S3 obligations
are still open; this is a P baseline, not S3 closure.
## S3 dual EDIT input boundary audit

Against P1 0472ee23c, source tracing finds two distinct paths. The actual target is dosonly=0. Both commands first traverse cmdExec32/Run16;
Run16 routes edit.com into a DOS NTVDM worker (then QBASIC /EDCOM), and
edit.exe into NTVWM. Both require I/O back to their originating guest VM. bios_character in the owned
ntvdm64.asm forwards BIOS-translated keys plus real make/release/modifier
notifications through host_notify17; mvdm_vm_executor and mvdm_vm_io_keys
convert/enqueue KEY_EVENT only. The VM I/O public ingress has no mouse record
entry. Insignia per-VM mouse-state preservation alone therefore cannot prove
native EDIT mouse delivery. This is a required capability gap, not evidence
that the DOS editor or its original mouse driver is broken.

The existing interrupt-observer.asm stimulus also uses operation17, while
host-continuation.c forwards operations16..18 directly to production under
MVDM_REAL_COMMAND. Its A/B-only calibration stimulus cannot simply be reused
for real-command editor input: it would collide with production key ingress.
Keep test stimulus separate from production operations; do not bypass the
production recipient or claim injected native INPUT_RECORDs as guest input.
Original USER keybd_event289 and mouse_event299 are register-based guest APIs
(documented in wow16/user/winmisc2.asm); they are another candidate stimulus
boundary, not yet proof of VKD forwarding into a DOS VM.

Next bounded witness must launch the two exact editor images in separate
real guest DOS prompts, retain their VM identity and native process evidence,
then establish one actual guest keyboard/menu interaction before adding mouse
clicks. Preserve failures with the selected VM/window and visible state.
Only after a current click trace shows the missing native ingress should the
adapter be wired to an original mouse event owner with the same VM/token;
no second mouse controller or guest binary modification is admitted. Existing
Full gates and geometry receipts do not satisfy this editor contract.
Owner correction: the earlier DOS-editor-local-input inference was wrong for
dosonly=0. Original tcode.asm d16_run checks scs_dosonly; only DOSONLY_YES
returns to local DOS execution, otherwise AH=1 SVC_CMDEXEC carries the command.
The identity runner previously hardcoded386-dosonly1. It now exposes RuntimeCase
and defaults to386-dosonly0; explicit386-dosonly1 remains a control. Historical
P1 geometry evidence retains its actual mode and does not establish ordinary
COMMAND prompt routing or dual EDIT acceptance. Both editor chains must be
observed under dosonly=0, including the nested NTVDM for edit.com.

### Required DOSONLY editor routing matrix

Owner explicitly requires both modes; neither substitutes for the other:

| Mode | Command typed in the guest MS-DOS prompt | Required result |
| --- | --- | --- |
| dosonly=0 | O:\winnt\edit.exe | cmdExec32/Run16 to NTVWM; keyboard and guest mouse interaction in the originating VM frontend. |
| dosonly=0 | O:\winnt\edit.com | cmdExec32/Run16 to nested NTVDM; editor I/O returns to the originating VM frontend. |
| dosonly=1 | O:\winnt\edit.com | Executes within the original guest DOS VM, including QBASIC /EDCOM, with usable keyboard/menu/mouse and normal exit. |
| dosonly=1 | O:\winnt\edit.exe | Reports execution failure; no native host editor task is launched. Prompt remains responsive and exits normally. |

Record actual command routing and worker/task identities, not just screen
appearance. For dosonly=0, the two editors must coexist in separate guest
prompt windows with isolated input and independent completion. The dosonly=1
negative must assert both the visible error and absence of a launched native
editor; a hung prompt is a failure, not a successful rejection. Current direct
BOP geometry probes are separate boundary evidence and do not prove this matrix.
DOSONLY isolation acceptance (owner requirement): with dosonly=1, neither
editor command may create the project's detached host multitask payload. The
original Windows3.1 DOS VMs may still exist and run concurrently; that is not
a host payload. Assert no command-correlated nested Run16 launch, no new
NTSRV worker/task chain and no corresponding VM payload frontend registration.
Take before/during/after identity-bound observations so a short-lived unwanted
launch is not missed by an end-only process snapshot. Scope observations to
the test-owned session and distinguish the outer launcher/NTVDM from a new
command child. edit.com must remain usable in its original VM; edit.exe must
fail and leave a usable prompt. A blank monitor screen alone is not evidence.

Geometry012 passes on386-dosonly0 with both real BOP command continuations,
exact visible cells and normal Windows exit. This is additional mode coverage
of the projection repair only, not the prompt/editor/DOSONLY-isolation proof.
Prompt calibration013/014 (temporary experiment, no product change): isolated
copies m481p13/m481p14, production P1 package, two COMMAND.COM PIFs and a
Win16 USER-keybd_event controller. Both reach Program Manager with actual
DOSONLY0 witness, but neither produces typed echo receipts or normal exit.
The014 controller additionally waits for GetTickCount progress before recording
a guest window inventory; that inventory is absent. This does not yet localize
a keyboard or product fault: the controller may not reach its stimulus. Keep
these as failed calibration, not EDIT results. Prompt015 moves inventory before
that wait to distinguish target-window creation from probe pacing; runner PID
23684, output build/t481-s3/prompt015, isolated installation O:/tmp/m481p15.
No original guest bytes or published product changed. Scripts and assembled
controller sources are retained under build/t481-s3 for this active investigation.
Prompt calibration correction:013..021 used a nonexistent O:/winnt/COMMAND.COM
in their generated PIFs. The actual immutable COMMAND is under system32; mere
window titles were not successful guest process admission. No product timer
failure is established.022 (same timer/controller, no DOS windows) passes.
023 copies package COMMAND.COM into its owned PATCH directory, verifies SHA256
908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43 and
uses that exact path. Both real dosonly0 prompts produce lowercase a/b receipts
through typed echo/redirection. Normal exit still fails: actual key traces and
subsequent BOP admissions are retained, but final command text is not yet known.
Do not call that failure a product defect until the received command is checked.

Keyboard019 separately passes the established two-VM BIOS consumer, including
exact1e61/3062 key words and normal Windows exit. Test-only observer operations
now have distinct250/252/253 aliases, so real production16..18 ingress can remain
selected while physical stimuli run. The diagnostic stimulus accepts explicit
make/break to support actual Alt/Shift menu sequences; it still enters the
original controller, not BIOS memory or native child INPUT_RECORD injection.
Diagnostic024/Prompt025 add read-only command-tail tracing before admission
(no second capture or CR normalization) to identify the exit failure. Product
package remains P1; no new runtime repair or completed editor acceptance claimed.
Prompt025 reproduces023 with read-only pre-admission command traces. The two
expected echo tails arrive intact and create exact a/b text files. The later
expected exit commands arrive as IT and XIT. Physical make/break stimulus and
INT15 observations include the full E/X/I/T sequence in the correct VM. This
localizes the failed shutdown to command input before cmdExec admission, not
Windows ExitWindows itself. It does not yet establish whether input was lost
by project handback or legitimately consumed/flushed by the still-running native
CMD child. Preserve that distinction; do not replay input already consumed by
a child. Need a prompt-ready/command-completed comparison and ownership trace
before changing runtime. Evidence: build/t481-s3/prompt025/trace.txt and guest
copy O:/tmp/m481p25. All prompt013..025 failures remain non-passes; the successful
standalone timer control022 and physical keyboard019 are separate calibrations.

Prompt031 calibration passes (receipt: artifacts/t481-s3/prompt031.json).
The independently authored diagnostic observer chains INT21 unchanged and
records actual AH0A line-read entry. Both VM identities reach this entry before
the controller targets either prompt. Key make and release are separately
paced; the controller waits for production BOP12 completion before exit.
Both echo redirections produce exact a/b files, both COMMAND windows close,
and the unchanged Windows3.1 system exits normally under dosonly0. This is
real prompt/command evidence, not EDIT acceptance or DOSONLY1 isolation.
The comparison changed both readiness and key-edge pacing, so it does not
alone attribute the previous missing prefix to one cause or prove early
unread-input handback. No production behavior was changed or republished.
Prompt027/028 timed out before command admission.028 accidentally reused
027's routine include and therefore is not a separated-edge control.
Diagnostic029's first runner failed before compilation because its output
directory was absent; corrected runner029b built successfully. Prompt030
failed during controller assembly (missing separator before wait_readers),
never executed a guest, and was corrected in fresh probe031. Preserve these
as fixture failures, not product regressions. Build/test products remain
inside owned temporary roots; the immutable input installation was copied.

Prompt032 applies the same real-prompt calibration to386-dosonly1. Both
original guest COMMAND windows execute local echo redirection, create exact
a/b receipts and exit; Windows exits normally. The complete diagnostic trace
has four INT21/AH0A read entries and zero production VM command admissions
(operation11). Receipt: artifacts/t481-s3/prompt032.json. This establishes
basic DOSONLY command separation, not the requested EDIT-specific routing,
interaction or whole-lifetime worker/frontend isolation acceptance. Those
remain mandatory open S3 items; no original guest or product package changed.


Editor040 establishes actual simultaneous editor execution under dosonly0:
the trace contains exact O:/winnt/edit.exe and edit.com tails in distinct
VMs before either completion, and copied VIDEO text contains both editors.
The DOS editor's welcome dialog is dismissed with Esc, Alt+F renders Save As
and Exit, then its menu Exit returns to COMMAND. Modern EDIT exits by Ctrl+Q;
its Alt+F menu was NOT observed, so that requirement remains open. Both
COMMAND windows and Windows exit normally. Receipt: artifacts/t481-s3/editor040.json.
Text is sampled after production BOP13 transfer, before guest frees it; BOP14
separately reports successful guest application. It is not a pixel screenshot.
Before this valid run,034's diagnostic incorrectly read the buffer at BOP14
(after command_buffer_free) and terminated itself with487;036 read the older
BIOS snapshot instead of the authoritative VIDEO text;038 copied actual editor
frames but its readiness fwrite resolved to original fprt.obj's debug function
(which returns len without writing). The diagnostic uses checked WriteFile
for the readiness count now. These are corrected fixture defects, not product
fixes. Run038's owned native EDIT22136 survived timeout cleanup; verified exact
image/creation time and terminated that test-owned process before040. No user
process or published host package was replaced. Modern Alt, typing/save, guest
mouse, completion-order reverse and DOSONLY1 editor isolation remain mandatory.


VT Alt candidate: original nt_keycd.c BiosKeyToInputRecord preserves zero
UnicodeChar for BIOS Alt+letter words. Native hidden-console EDIT control041
shows zero-character Alt+F does not open New File, while otherwise identical
Alt+F with f does. NTVWM's VT boundary now derives a single printable character
from copied Shift/Caps/Num state and the current layout, using non-mutating
ToUnicodeEx; classic mode, key-up, existing characters and Ctrl+Alt are unchanged.
No mirror/addon/guest change. Native042 opens the menu for both input forms.
State045 passes the complete real-Console fixture, including classic zero-char
preservation and actual conhost VT ESC+f. The initial new assertion044 wrongly
expected an unencoded native record in VT mode; corrected to its observable
character sequence rather than weakening the required Alt behavior. Historical
standalone runners also needed the current process-observation/console_members.c
link dependency. Receipt: artifacts/t481-s3/alt-input045.json.
Cached043 PID24656 is the current x86/x64 build; editor046-run PID26216 waits
that exact process and checks its exit receipt before staging and testing.
The candidate is not yet fully qualified, published or committed. The prior
complete11 P1 package remains the installed verified baseline.

Editor046 now passes with the candidate NTVWM: modern New File menu and DOS
Save As/Exit menu are both present in their distinct VM text frames; both
commands complete, both prompts return/close and Windows exits normally.
Cached043 x86/x64 builds/shared tests and Console045 passed. Receipt:
artifacts/t481-s3/editor046.json. Full047 PID25152 is running the established
specialized/Full sequence on fresh production r047 staging, explicitly taking
VCDEX.DLL from the current x86 build. This is not yet a publication or P.
Mouse, typing/saved-content isolation, reverse completion order and DOSONLY1
editor acceptance remain open; these successful menus do not discharge them.

### S3 reusable editor probe preparation

The successful editor046 experiment is being promoted to
`test/integration/ntvdm64-386/verify-win31-editors.ps1` with checked-in
controller includes. `New-Win31ProbeCopy.ps1` now owns the same immutable-copy,
PIF/GRP rebasing and authored-driver installation previously embedded in the
identity runner. No original guest binary is changed. The editor runner no
longer extracts and executes another script's source text; it restores its
environment and checks the original installation hashes. Menu assertions require
modern and DOS menu contents in distinct VM frames and two command completions.
Unused inventory diagnostics were removed. Background source048 (PID15280)
passed PowerShell parsing; this is not runtime qualification. Full047 remains
live (PID25152, child13764), progressing through cooked-return cases. The new
formal runner must execute serially after that package run. DOSONLY1 editor,
mouse, save/isolation and reverse completion remain open.
Full047 subsequently exited 0: all 16 Full gates passed. Formal editor049
(PID19580) also exited 0 with distinct VM menu assertions, two completions and
normal Windows exit. Exact input/result hashes are retained in
`artifacts/t481-s3/editor049-full047.json`. This qualifies the bounded Alt
repair, not remaining mouse/save/DOSONLY1 acceptance.

Publication051 passed: complete11 package b3ea681a-18f4-490b-960c-b1acd249eeba
at assets/release and O:/winnt/system32, all hashes equal the tested manifest.
The authored addon and original guest binaries are unchanged. Governance050
passed. Product scope is the 30-line VT Alt character adaptation; the remainder
is diagnostic acceptance and shared test preparation. Mirror diff is zero.
No S3 closure or remaining input/display qualification is claimed.

### S3 editor completion order and saved-content isolation

The formal runner now selects `-FirstExit Native|Dos` and optional
`-SaveWitness`. Editor052 (PID5940, FirstExit Dos) passed: DOS EDIT completes
first, then modern EDIT still accepts Alt+F; both prompt windows and Windows
exit normally. Editor053 (PID16016, SaveWitness) passed: distinct files A.TXT
and B.TXT contain exactly aaa and bbb after trimming their editor-specific
line endings. The guest controller types through the original keyboard path;
no file content is injected by the host. Menus remain in distinct VM frames.
The tests use the published P2 product sources plus diagnostic host observation;
no new production source changed. Receipts: artifacts/t481-s3/editor052.json
and editor053.json. These do not qualify mouse input or DOSONLY1 editors.

### S3 DOSONLY1 editor qualification

Editor059 (PID10352) passes: exact O:/winnt/edit.exe emits "This program
cannot be run in DOS mode" and the COMMAND prompt reads again; O:/winnt/edit.com
opens the original DOS editor, Alt+F displays Save As/Exit, and both prompts
and Windows exit normally. The complete BOP trace contains zero operation11
admissions. Source closure: mvdm_vm_command_admit is the only executor submit
caller; execute registers the VM frontend and launches the host chain. The
startup payload/frontends context is heap/lock bookkeeping only;
ntvdm_vm_frontends_create does not register an NTSRV frontend or launch a task.
Thus no command-triggered host chain/frontend was created in this case.

Editor055 and058 completed guest shutdown but failed the menu-observation
assertion. The timer observer used EBX after BOP had returned its result there;
restoring the saved actual VM handle fixes that test-only observation error.
Editor056 exited110 while the diagnostic screen logger repeatedly reopened its
file; attribution beyond the diagnostic open-error path is not claimed. The
logger now retains its stream and deduplicates per VM. None of these failed
runs is counted as passing product acceptance. Editor059 retains current positive
receipts in artifacts/t481-s3/editor059.json. No product or original guest
change is involved. Editor060 is the separate guest USER mouse-menu calibration;
mouse delivery through the host payload still requires its own test and repair.

Editor060 (PID8888) is a failed mouse calibration, not a host payload failure:
DOSONLY1 guest USER SetCursorPos + register-based mouse_event down/up did not
open File; following x appeared in Untitled text and the 90-second runner timed
out. The saved final guest screen establishes that input continued but the
menu click was not accepted. Next investigate actual guest client coordinates
and the original VMD/INT33 delivery before choosing any production mouse seam.
No assertion was relaxed. Evidence: artifacts/t481-s3/editor060.json.

### S3 mouse stimulus route correction

Editor063/064 tested original nt_process_mouse absolute Console input, which
is not the active NTCON Window route. Editor064's guest-written MOUSE.BIN gives
client 600x301, desired desktop (69,51), actual USER cursor (320,240). Source:
nt_mouse.c host_os_mouse_pointer first consumes mvdm_softpc_mouse_apply;
mvdm_softpc_mouse_next returns a zero-motion sample even with an empty queue
while its Window route remains active, so the legacy absolute queue is bypassed.
These probes do not establish a production mouse delivery defect.

Editor066 switched to the selected relative route but sent duplicate ENTER:
trace255 reports ERROR_INVALID_STATE (0x139f), matching the existing submitted
NTCON route. The diagnostic now reports ERROR_ALREADY_EXISTS for that setup
observation, and its guest stimulus explicitly accepts that setup-only result;
MOVE/button events still require actual success from the production submitter.
No production semantics changed or failures were converted to success for
actual input. Editor068 (PID26920) is running the corrected calibration.
Evidence for the failed probes: artifacts/t481-s3/mouse-calibration063-066.json.

Editor068 accepted relative MOVE but Windows exited255. The source installation
has current published MOUSE31.DRV but no INSIGNIA.386 entry. Existing mouse31
acceptance explicitly depends on that NIDDB companion. The shared probe-copy
helper now accepts an explicit InstanceDriver; the editor runner selects the
published companion and records source/installed SHA256. Original installation
is untouched. This configuration correction is necessary, not yet proof of
complete DOS mouse delivery.

Editor069 with the companion accepts all movement/button samples and retains
the editor, but target (69,51) observes cursor (60,43); the failed click then
inserts x. Editor070 uses one measured relative-motion correction and observes
(69,47), inside the first text row, but still inserts x rather than opening
File. Both reach the runner timeout; neither passes. The selected input path is
now the existing Window relative queue through original mouse processing and
the guest driver, not direct USER mouse_event or the bypassed legacy absolute
queue. The remaining source investigation is the guest VMD/INT33 DOS-VM mouse
handoff, including whether original mouse_EM_callback (BOP BF) is reached.
Do not infer that adding host payload MOUSE_EVENT ingress alone solves this
pure-guest control case. Evidence: artifacts/t481-s3/mouse-calibration068-070.json.

Editor072 (PID26444, diagnostic071, DOSONLY1 with published NIDDB companion)
failed mouse-menu/normal-exit acceptance. Actual INT33 reset returns FFFF/2;
EDIT registers mask1F at 3DF8:0223. All four relative movement/button submissions
return success; the already-active ENTER is explicitly observed as183. Original
mouse_EM_callback/BOP BF has zero hits. No host command admissions occur.
Evidence: artifacts/t481-s3/mouse-boundary072.json. These observations narrow
the next audit to the original VMD/windowed-DOS mouse notification contract;
they do not alone prove that BF is required by this retail guest path. The
published companion currently implements NIDDB lifecycle only. Do not add a
second mouse engine or modify immutable guest binaries.
Microsoft Q74572 distinguishes Windows mouse input from DOS application mouse
support and assigns hardware ownership switching to VMD:
https://jeffpar.github.io/kbarchive/kb/074/Q74572/ . Original Microsoft DDK
Virtual Device Adaptation Guide describes VMD interrupt reflection/EOI:
https://www.pcjs.org/documents/books/mspl13/win/w3ddkvxd/ . These are interface
research references only, not imported code or runtime dependencies.

Editor073 confirms no observed INT2F1607/BX0C through Hook_V86_Int_Chain;
this hook alone is not proof about VMD Exec_Int nested execution. Read-only
WIN386 module extraction vmd074 (image SHA256 C67E667E25E91C65EFB5ACDDEC437DA8B586037DA7DE502228CA8FBD806CF3F8)
finds the real VMD callout: object2 00CD-0118 requests AX1607/BX0C/CX1,
requires AX0 and saves ES:DI. Both exported V86 and PM API entries are0A11.
The independent test-only mouse-vmd-callout.asm exposes the existing BOP BF
provider, with NIDDB remaining state owner; no external implementation is copied.
Editor075 fails assembly before runtime (fixed scalar paragraph count).
Editor076 still fails menu/exit; this is not a product fix. Editor077 adds
read-only INT2F1684/BX0C + GetVersion and receives a zero VMD API pointer,
although SYSTEM.INI has mouse=*vmd. Original VMD is present in the bundle;
its runtime loading/selection is now the concrete question. Editor078 explicitly
adds device=*vmd only to the isolated copy; PID4276 owns that pending contrast.
Evidence: artifacts/t481-s3/mouse-vmd073-077.json. Original installation and
published product remain unchanged. API cross-check (research only, no import):
https://git.javispedro.com/cgit/vbados.git/tree/int2fwin.h
blob d3bd969df1f6d6e85aab25d6803f8ecc3a800daa, author Javier S. Pedro, GPL2+;
retail VMD machine code and original mouse_EM_callback establish the selected
ABI independently. This reference does not authorize importing its code.

Editor078 is terminal exit1. Explicit device=*vmd changes the observed VMD
API from0000:0000 to003B:0370, GetVersion0300, and makes the original VMD
CX1/CX0 broadcasts visible. This is direct evidence that the prior copied
configuration did not activate that interface; merely finding mouse=*vmd in
SYSTEM.INI was insufficient. Mouse-menu/normal-exit still fail and BOP BF
still has zero hits, so loading is necessary to investigate, not a complete
fix. Next check must inspect the actual real-mode CX1 reply (AX and ES:DI)
and the retained DOS bridge installation before changing callback semantics.
No candidate was published. Evidence: artifacts/t481-s3/mouse-vmd078.json.

ABI correction: the earlier notes saying VMD saves ES:DI were incorrect.
Retail object2 00EE reads Client_ES (offset3C), 00F5 reads Client_SI (offset4).
Original VMM Client_Reg_Struc independently confirms these offsets. Editor079
DPMI0300 query proves the diagnostic TSR is resident: AX0, ES064A, DI012A,
SI0. Therefore the first bridge returned the wrong offset register. The test
now returns ES:SI; no product was published with the incorrect bridge.
Editor080 runs that corrected bridge but fails before two DOS prompt reads;
only system VM creation is observed and no BF callbacks occur. Windows probe
startup/enhanced mode is observed, so this is not evidence that Windows never
started. Need inspect why child VM creation did not complete before claiming
the corrected adapter works. Evidence: artifacts/t481-s3/mouse-vmd079-080.json.

Superseding ABI correction: mechanical parsing of the original MS-DOS6 VMM
Client_Reg_Struc gives Client_ES=56, Client_DS=60, Client_ESI=4. Thus retail
VMD object2 00EE/00F5 requests DS:SI, NOT ES:DI or ES:SI. Both previous manual
interpretations were wrong. artifacts/t481-s3/vmm-client-offsets.json records
the computed layout. The independent diagnostic bridge now returns DS:SI.
Editor081/082 window enumeration and the read-only private-desktop capture
show actual Prompt probe A/B minimized windows, not missing Windows startup.
The runner now opens both via its existing OpenIcon before waiting for DOS
readiness. Editor083 with the wrong ES:SI bridge still failed; editor084 without
the bridge reaches both DOS reads and the mouse stimulus, isolating the bridge
as the new startup interference. Editor085 added the original documented
Win386 instance descriptor to the diagnostic TSR but still used wrong ES:SI;
its failure does not establish an instancing defect. Editor086 (PID27592)
is the first test with the mechanically verified DS:SI reply. Its result is
pending. No prototype/diagnostic code has been published as a product fix.
Instance descriptor ABI reference, read-only: MS-DOS6 dev/ega/ega.inc
Win386_Startup_Info_Struc/Instance_Item_Struc and dev/ega/int10rtn.inc
Do_Startup_Instance_Stuff; no external implementation imported.

Editor086 PASSES: actual relative mouse left-down/up reaches original BOP BF
with masks3/5, opens the DOS EDIT File menu, then keyboard X exits EDIT;
both COMMAND prompts and Win3.1 exit normally. DOSONLY1 native executable
rejection remains observed and host command admission count is zero.
Evidence: artifacts/t481-s3/mouse-vmd086.json. This proves the bounded callback
contract: load original VMD, return the DOS callback as DS:SI, reuse original
mouse_EM_callback/NIDDB. The diagnostic TSR is not a product deliverable;
production integration must preserve callback memory/VM lifetime through the
admitted addon, avoid a second mouse engine, and retest normal/reused VMs.
Mouse selection of Exit and native payload mouse delivery remain unqualified.

### S3 production callback candidate / editor087

2026-11-07: replaced the test TSR with an independently authored 4-byte shared
V86 stub in insignia-386. Original VMM allocation A8 owns lifetime; interrupt
chain service41 answers VMD's original 1607/000C queries with DS:SI. Original
BOP BF/NIDDB remain the mouse state/algorithm owner. The original vmm.inc
service table and Client_Reg_Struc were checked, including DS60/SI4. No original
guest binary or host mirror changed. Source provenance is in the addon README.

Background editor087 PID5348 returned0: DOSONLY1 guest EDIT File menu by actual
mouse, native DOS rejection, zero host command admissions and normal Windows
exit. The test does not install MCALL.COM. Input/source/binary hashes and limits:
artifacts/t481-s3/mouse-vmd087.json. This supersedes the diagnostic-TSR dependency
of086, not its historical result. Explicit `device=*vmd` remains a test profile
prerequisite; general installation wiring has not yet been changed. Remaining:
mouse Exit, native payload mouse ingress, direct zero-host-chain evidence,
full existing S3 matrix and coherent delivery. No publication/closure claimed.

### S3 mouse Exit / editor088

Background PID25420 exit0. Reused insignia087 unchanged. The controller opens
DOS EDIT File with real mouse down/up, then clicks row9 Exit with real down/up;
no X keystroke substitutes for Exit in MOUSE_MENU mode. DOSONLY1 native rejection,
zero host admissions, both prompts and normal Windows exit pass. Artifact:
artifacts/t481-s3/mouse-vmd088.json. Removed the unused recursive window-dump
helper and its data; it was diagnostic-only and had no remaining caller.

Host payload mouse gap is concrete: mvdm_vm_executor_bios_key_bop accepts only
translated keyboard descriptors; mvdm_vm_io_keys produces KEY_EVENT only and
prepend_keys rejects other types. Original mouse_EM_callback gets VMD events
but owns guest INT33 state, not a native payload queue. A correct extension
must retain current VM/token admission and retirement, route MOUSE_EVENT through
the existing I/O channel, preserve original BOP BF for no-payload guest handling,
and derive coordinates from the actual guest geometry. No foreground-global
routing or duplicate mouse controller is authorized. This is the next source
integration boundary, not evidence of native payload mouse completion.

### S3 host mouse queue / input089

Background PID25100 exit0; cached x86/x64 mvdm-vm-notification target and CTest
both pass. mvdm_vm_io_mouse accepts absolute Console mouse records through the
same enqueue lock, capacity, invalidation and readiness transition as keyboard.
No new queue or lifecycle owner. Existing arbitrary valid keyboard batch size
is preserved. Added tests prove mouse fields, queue isolation and invalid
coordinate rejection; artifacts/t481-s3/mouse-io089.json records exact hashes.
This entry has no production caller yet and is not end-to-end acceptance.

Source audit found the smaller guest integration seam: original mouse_io.c
mouse_get_and_set_subroutine (INT33 AX14) already exchanges CX mask and ES:DX
callback while preserving old values. jump_to_user_subroutine supplies AX event,
BX buttons, CX/DX position and an original mouse_int2 return frame. Allocate a
VMM V86 callback in ntvdm64-386, exchange the actual VM's callback while its
command owns the payload token, then restore the saved original callback on all
command exits. Existing insignia VMD/BOP BF binding and original NIDDB remain
unchanged. This avoids a second VMD query owner or a foreground-global mapping.
The guest callback must validate its executable VM/current token, copy a bounded
mouse descriptor through the existing BOP notification, and far-return through
the original mouse completion path. Coordinate conversion, teardown, unread
mouse disposition and real dual-editor behavior still need implementation and
proof; these are not granted by the passing queue tests.

### S3 INT33 ingress candidate / 091-095

The candidate exchanges the original INT33 AX14 callback before admission and
restores it on failed admission and completed/error return. Per-VM saved state
is in the existing VMM control block; destroyed VMs retain original NIDDB
retirement. VMM callback forwards seven DWORDs through existing signed BOP19:
exact token, original event/buttons, text-cell coordinates and BIOS417 modifiers.
Original text_grids uses16x8 in modes0/1,8x8 in2/3/7; command_screen only admits
these text modes. No host mirror changed. ABI ordinal audit corrected the local
candidate FAR-return service to4A before any runtime execution.

Diagnostic090 failed compilation (missing SHRT_MAX declaration); corrected to
INT16_MAX, diagnostic091/addon build passed. Cached unit092 x86/x64 passes,
including BOP19 actual dispatch, wrong VM/token rejection and copied descriptor.
Real dual-editor095 is NOT yet run: diagnostic094 PID18056 is building added
observation first. Preceding editor093 PID8464 terminal exit1: both commands
start, native command completes, but no File menu and Windows does not exit.
Mouse requested69,159 yet actual guest pointer320,240; callback/VM/status tracing
is needed to distinguish stimulus position from provider failure. Do not
reinterpret this failed test as success or publish the candidate. Evidence:
artifacts/t481-s3/mouse-ingress093.json. Unread mouse disposition and full teardown
review remain open in addition to required integration acceptance.

### S3 coordinate boundary / 095-099

095 PID5508 and097 PID5996 are terminal exit1, no normal Windows exit. Both
provide stronger evidence than093: BOP19 receives correct VM/token with status0,
but cell coordinates36-39/16-18 miss the menu.097 observes real relative deltas
(-251,-81) accepted into the root mouse queue, whose virtual extent is640x200.
The guest GetCursorPos remains320,240. Pure-guest087/088 actual positions69,50
and69,158 matched their requested menu cells. This narrows the issue before
host task input conversion; it does not prove whether the physical coordinate
cache, NIDDB selection or a callback changed it. No speculative runtime patch.
Artifact: artifacts/t481-s3/mouse-coordinate095-097.json.

Diagnostic098 PID21336 adds pass-through physical mouse_int1 observations:
actual BIOS04AC instance, queued delta, and resulting guest callback CS:IP.
editor099 is prepared but not yet launched. Existing original handlers execute
unchanged, and diagnostic images are never published. This will distinguish
incorrect physical IRQ instance/callback from scaling or stimulus issues.

### S3 instance/coordinate observation103

Editor103 PID10852 exited1: Windows normal exit passed, but both editor File
menu assertion failed. Actual relative movement reaches mouse_int1; BIOS449
is1b while the video cache and selected mouse instance mode are03 and position
is0,0. Negative movement therefore remains clamped. This is not proof that
mode decoding alone is the root cause. Evidence: artifacts/t481-s3/mouse-state103.json.
Diagnostic104 built successfully; editor105 adds pass-through observations of
the original video-to-mouse mode notification to identify the state writer.
No original guest change or new release.

Observation qualification: the diagnostic trace path is inherited by nested
workers. Before attributing a mode update to the root worker, diagnostic106
adds the actual process ID to coordinate/mode observation records (upper bits
of the instance-mode value). Unqualified earlier mode-transition records must
not be interpreted as a proved root-worker write. Diagnostic106 built/staged
successfully; editor105 is still observed live and must finish before107.

Editor107 exited1; PID-qualified records prove root PID20724 itself has mode03
and position0,0 during BIOS1b physical motion. Nested PID22604 is separately
identified. Normal exit failed. See artifacts/t481-s3/mouse-owner107.json.
Diagnostic110 built; editor111 observes NIDDB lifecycle plus INT33 and INT10
entrypoints in one run. No speculative production state correction applied.

### S3 companion initialization ordering candidate112

Editor111 traces root19480 setting each child BIOS mode03 while04AC is still0,
before the companion's VM_Init notification obtains identity1/2. This proves
an initialization ordering gap: child BIOS work can change the system mouse
instance. Candidate112 calls the unchanged original NIDDB operation9 from
Create_VM7 instead, before Init_VM/BIOS execution; later VM_Init is ignored.
Existing creation flag and VM_Not_Executeable retirement remain the owners.
The addon compiled successfully; editor113 is the same-package runtime contrast.
See artifacts/t481-s3/mouse-instance-order111.json. Acceptance remains open.

Editor113 exited1 on both-menu assertion. The ordering contrast is positive:
NIDDB operation9 now precedes each text-mode notification, which reads04AC1/2.
At physical input, root mouse instance mode remains67 instead of baseline03.
The host-global video cache still reads03 and submitted scaling uses640x200;
position remains0,0. Do not claim complete mouse repair. Editor114 checks the
existing DOSONLY1 mouse/exit control with candidate112 and diagnostic110.

### S3 CPU-side mouse geometry candidate115

DOSONLY1 editor114 passes guest menu/mouse Exit, native rejection, zero command
admissions and normal Windows exit with companion112. Candidate115 preserves
source dimensions in the existing mouse queue and scales its head once on the
CPU owner after original instance selection. Existing short-counter splitting
then runs unchanged. Coalescing also requires equal source dimensions. The
original instance virtual_screen/current_video_mode replace asynchronous
VirtualX/Y and the host video cache as geometry authority. Arithmetic failure
bubbles through the existing fatal adapter boundary; no retry or timer added.
Bridge115 and original-coordinate fixture116 passed (mocked ICA/IRQ/memory).
Editor117 is the real dual-editor contrast; no publication/acceptance yet.

Geometry/fragment118 passed: a sample queued under stale host geometry uses
the selected CPU extent, and a partially consumed scaled sample is not scaled
again after an extent change. Editor117 fails; it confirms correct mode67 and
640x480 displacement, but original coordinate origin0,0 differs from USER320,240.
Editor119 adds real button-free positive movement before GetCursorPos delta
calculation, without guest state writes, synthetic success or weaker assertions.
Evidence: artifacts/t481-s3/mouse-geometry115-118.json. No release qualification.

Editor119 terminated with C0000005 before command/mouse receipts; trace queue
countf7f7f7f7 is not a mouse calibration result. Candidate queue sample layout
changed, but diagnostic115 rebuilt softpc-bindings only. Diagnostic120 now
requests the entire cached ntvdm.exe dependency closure before diagnostic link
to rebuild every holder of the changed structure. Do not qualify115/117 runtime
as ABI-coherent until this rebuild and comparison complete.

Coherent cached rebuild120 rebuilt console_client (holder of the enlarged
mouse queue) and passed bridge tests. Editor121 reaches native EDIT File menu
by actual mouse and normal Windows exit, but DOS EDIT menu remains absent.
BOP19 carries cell4,0 to the right VM; nested DOS absolute-input handling is
the remaining mouse ingress seam. No dual-menu pass is claimed.

Mouse return review found PREPEND_KEYS still rejected MOUSE_EVENT records that
the same VM queue now supplies. Candidate122 preserves those exact valid
records, validates negative/out-of-range cells and rejects unrelated input
types/receipts; unit122 x86/x64 is running. BIOS return semantics unchanged.

Mouse return unit122 passed x86/x64. Editor124 still fails dual-menu acceptance;
actual nested DOS callback observed only initial position0,0, not the later
File click. Diagnostic125 did not compile because its nt_event observation
wrapper omitted the existing per-file thread_start_compat forced include.
No runtime126 occurred. Diagnostic127 uses the exact nt_event edge flags;
editor128 will observe decoded ordinary MOUSE_EVENT records and clipping size
at the original input loop. Existing errors/LastError are preserved.

Editor128 exited with error32: diagnostic trace opened the shared file with
FILE_SHARE_READ only, so concurrent NTVDM append could terminate the process.
Observer129 permits shared append. Editor130 then passes both mouse File menus,
DOS mouse Exit, native keyboard Exit and normal Windows exit. Evidence:
artifacts/t481-s3/editor130.json. This supersedes128 as an input-chain result;
it does not retrospectively diagnose every older failure. Editor131-133 now
exercise native mouse Exit, reversed completion order and DOSONLY1 serially.
No candidate publication or S3 closure is claimed.

Editor131 passes both actual mouse File/Exit paths and Windows normal exit.
Editor132 reverse order does not pass: the measured USER target/actual point
are both69,159, but VM callback delivers cell9,9 instead of the forward-case
cell4,9; input-loop and original DOS callback both receive that wrong column.
Its cleanup also reported access denied on isolated ned0/nthook64.dll; no
remaining processes in ned0 or m481e132 were found afterwards. Do not treat
the incomplete matrix result as a pass. Editor133 DOSONLY control runs separately.

Editor133 completed exit0: guest EDIT mouse menu/Exit, native rejection, zero
host command admissions and Windows normal exit. Artifacts131-133 seal source
controller and result hashes. Separate host registry assertions remain open;
reverse-order coordinate mismatch remains a required S3 correction.

Editor134 reverse-order contrast passes with read-only GetScrollPos receipts;
no product change since129. Final receipt has zero scroll position, but is for
the final native window, not the earlier DOS window. Thus132 is still open,
not a deterministic order failure and not proved to be product scaling.
Editor135 records each click separately instead of overwriting MOUSE.BIN.

Editor135 reverse order passed again. Four separate receipts prove exact
USER target/actual coordinates and zero scroll for each click; original VM
callbacks deliver DOS4,0/4,9 and native4,0/4,7. No product edit since129.
Evidence: artifacts/t481-s3/editor134.json and editor135.json. Preserve132
as unresolved failure, not a proved product regression or waived gate.
Test reporting now derives mouseExitVerified from the executed mouse branch.

Editor136 registry query failed1722 because the selected monitor fixture was
built2026-10-26 before current scope code. Registry137 rebuilds the fixture
from current x86 cached graph. Editor138 passes: actual NTSRV snapshot while
guest EDIT is ready contains exactly one NTCON and one DOS worker carrying
WIN386.PIF, with no VM frontend/native task/extra worker. Full-run BOP11
admissions remain zero; guest mouse EDIT, native rejection and normal exit
pass. artifacts/t481-s3/editor138.json seals registry and runtime evidence.
Cached139 full product/shared-unit build is running; not yet qualification.

Cached139 completed exit0: complete x86/x64 product build and shared tests
14/14 for each width. Full140 started as hidden PID6368 using the existing
logon-environment runner and current observer graph. It must finish before
any new default publication. The current mirror/guest source diff is empty.

Input review142 removes duplicate partial mouse modifier decoding: reuse
bios_controls and pass BIOS417/418 plus496 in the existing seventh dword.
Tests prove lock toggles and right-side Ctrl/Alt on x86/x64. Evidence:
artifacts/t481-s3/mouse-modifiers142.json. No new transport or state. Driver
and product rebuild/real guest qualification remain required; Full140 is
still testing the pre142 frozen package and cannot qualify these new bytes.

Full140 completed exit0,16/16 gates on its pre142 frozen product package.
Candidate143 rebuild completed exit0 including current .386. Serial runner
qualification144-147 (PID3556) now executes reverse/forward mouse editors,
DOSONLY registry and new Full147; it stops on any failure. No publication yet.

### S3 candidate Full147 completed

Current production package 42a32c73-c220-4be6-98b6-de443efdc6cc passed Full147
16/16, exit0. Evidence: artifacts/t481-s3/full147.json. Together with actual
editor144/145 mouse exits and editor146 DOSONLY no-payload witness, this
qualifies the bounded P3 candidate. Publication and reviewed commit/push remain
pending; remaining S3 and S4/S5 obligations are unchanged.

Publication150 completed with exit0 after governance149. All eleven host
images and the manifest match at assets/release and O:/winnt/system32.
NTVDM64.386, INSIGNIA.386 and addon-manifest.json publish only to assets/release.
Original guest source/media and O:/w31 configuration are unchanged. Exact
publication evidence is artifacts/t481-s3/publication150.json; reviewed
commit/push are still pending. This is not whole-S3 closure.

### S3 production I/O call witness153

P3 is committed/pushed as 4895e93c4. Diagnostic152 reuses the successful
143 link recipe and wraps only VM I/O dispatch, preserving the implementation,
return value and last error. Editor153 passes two mouse exits and normal
Windows exit. Its trace confirms BUFFER_SIZE, GET_TITLE_A, SET_TITLE_A and
SET_POINTER_CLIP and KEYBOARD_LAYOUT return ERROR_NOT_SUPPORTED on reached calls. READ_TEXT_CONFIGURATION
initially returns ERROR_NOT_FOUND before a frame exists; this is not conflated
with missing implementation. No font query is observed; do not infer universal
unreachability. Counts and raw hashes: artifacts/t481-s3/io-operations153.json.
Next work follows these actual consumers and their state owners. No production
source or default release has changed since P3.

### S3 request parameters155 and identity-size candidate156

Read-only diagnostic154/editor155 passes both mouse editor exits and normal
Windows exit. Exact parameters are sealed in artifacts/t481-s3/io-parameters155.json.
The reached BUFFER_SIZE requests80x25, identical to the current guest grid.
Candidate156 shares PREPARE_TEXT_REGION's existing idle/positive/exact-extent
validation. It adds no state or resize engine; different extents remain an
explicit unsupported boundary pending guest geometry work. Positive, invalid
and different-size unit cases are added. Cached x86/x64 text156 is running
as PID23836; no passing claim or publication yet.

GET_TITLE_A requests260 bytes; setters carry18/31 bytes. Clip is NULL release.
Original cmdkeyb.c cmdGetKbdLayout deliberately takes NoInstallkb16 after a
failed Console layout query; do not fabricate a host HKL as the guest layout.
Title/caption ownership and guest clip ownership still need completion.

Identity-size156 completed: x86/x64 each169 assertions. Diagnostic157 and
actual editor158 pass; operation16 now returns0 instead of50, both editors
exit by mouse and Windows exits normally. Evidence: identity-size158.json.
No whole-S3 completion or new default publication is claimed.

Title source review: original config.c temporarily sets a unique title to
find ConsoleWindowClass, then restores the initial title. VM io.title currently
means caption/handoff, not a complete Console-title or guest USER-window
implementation. Do not conflate these roles or return a fabricated HWND.
The inspected original MS-DOS6 dev/smartdrv/sdvxd/shell.inc (Microsoft notice,
SHA256 F02962370C12489BA283F947A30DD932721E98C68B89A5CF7829F77089CC4C6B)
contains five services, no direct title setter. This is source research only,
not proof that no guest title interface exists; no code was imported.

### S3 Console-title candidate159--163

The protocol already separates full SET/GET_TITLE_A from128-byte presentation
PUBLISH_TITLE_A/GET_HANDOFF_TITLE_A. Existing console_client_test.c explicitly
expects SET followed by supplemental caption publication. Candidate VM I/O
therefore owns one dynamic full API title per command, independently of the
existing caption. Initial text is copied; replacement allocates before freeing
the old value; teardown and failed event creation free it. No native/guest
window lookup or guest USER title mutation is claimed. Publishing a shortened
caption cannot truncate the API title. Same-VM channel ownership remains the
serialization boundary; NTSRV lifecycle and transport are unchanged.

Unit159 passed but its expected truncation was disproved by independent hidden
Console witness160. The selected native GetConsoleTitleA returns0 and one NUL
for a nonempty title with insufficient capacity; zero capacity and an empty
title leave storage untouched. Candidate161 and tests are corrected accordingly;
both architectures pass. Review then fixed event-creation failure cleanup.
Qualification162-163 PID23900 reruns both widths, rebuilds the complete selected
NTVDM dependency closure and executes actual mouse editors. It is running;
no default publication or full-S3 claim. Raw native controls: title-native160.json.

Original Windows3.1 DDK INT2F documentation was consulted at
https://dos-help.soulsphere.org/ddag31qh.hlp/winint2fabout.html (historical
Microsoft documentation mirror). Its listed services are not proof that every
possible VM title interface is absent; no external implementation was imported.

Qualification162-163 completes exit0. Actual requests16/24/25 all return0,
contrasted with155 status50; both editors exit by mouse and Windows exits
normally. artifacts/t481-s3/title163.json seals the positive/negative comparison
and native title control. Candidate remains unpublished pending remaining
review/qualification; layout and clip remain explicit unresolved operations.

### S3 nested-chain witness164

The identity runner now accepts InstanceDriver and ExplicitVmd, matching the
current editor profile without modifying O:/w31. Current package162 with
143/112 addons was exercised with two concurrent real command chains:
command /c cmd /c mem, followed by exit. Run164 failed the90s contract.
B.TXT contains actual MEM output and exit; A.TXT and both final result records
were absent. The runtime was not qualified. Cleanup additionally failed to
remove the test-stage nthook64.dll; this does not explain the preceding timeout.
Original installation hashes were verified unchanged. Exact hashes and limits:
artifacts/t481-s3/nested164.json. Control165 removes only the reentrant command
layer to separate basic nested MEM from deeper chain failure. No default
publication changed; neither failure has an established root cause yet.
Control165 completed exit0: both concurrent VM chains execute MEM, receive its
actual output, finish all320 callback checks and exit Windows normally.
artifacts/t481-s3/nested165.json seals the results. This narrows investigation
to the deeper164 chain; it does not establish causality or waive its failure.
The164 timeout snapshot retained one native CMD and nested NTVDM worker.

Repeat166 of the exact deeper chain passed both actual MEM screen assertions,
320 callback checks and normal Windows exit. Evidence: nested166.json.
The164 nested-worker stack resolves through the matching diagnostic162 map
to cmdGetNextCmd/GetNextVDMCommand waiting; it is not proof that MEM was still
running. The root CPU sample is lin2phy, insufficient to locate a stall.
Do not erase164 or claim it repaired. A late registry capture attempt found
the already cleaned166 stage and supplied no registry evidence.

Cached167 uses the established x86/x64 Ninja/ccache recipes. Full168 waits
for that exact build process and a successful receipt before constructing
the current complete11 package and running existing component/Full gates.
Owned process records and stdout/stderr are under build/t481-s3. Pending,
not publication or acceptance evidence. No production source changed during
these runs; isolated test-runner profile wiring is the only new source edit.

### S3 remaining operation ownership audit during Full168

Cached167 completed exit0; shared-win32 tests14/14 pass in each architecture.
Full168 is live and owns the current frozen candidate. No production mutation
or competing runtime test is permitted while that qualification is in flight.

The153/155 unsupported-call observations classify protocol coverage, not each
caller's functional outcome. Source checks narrow the remaining work:

- cmdkeyb.c:cmdGetKbdLayout explicitly takes NoInstallkb16 on layout-query
  failure, sets DX=0 and calls cmdInitConsole. Do not manufacture the modern
  host thread's HKL as the guest Windows/VKD layout. Existing editor passes
  do not prove non-US layouts or layout changes; that input boundary stays open.
- nt_mouse.c:MouseDetachMenuItem calls ClipCursor(NULL) without checking its
  result. The155 request is release-only. MvdmClipCursor routes this to the VM
  I/O owner; no code should clip the unrelated modern desktop. Acquisition,
  guest clipping ownership and release behavior remain to be classified from
  actual required callers, not resolved by an unconditional success stub.
- ntvdm64.asm:command_video_apply uses BIOS1100 (load font only), deliberately
  preserving text geometry. Different VIDEO glyph height alone therefore does
  not justify overwriting the snapshot's BIOS cell-height field. Prove actual
  geometry changes through the guest owner before changing that projection.
- ntvdm64.asm captures the current DOS code page with INT21/6601 in the target
  nested VM. That is the existing authority, not host OEM defaults. SBCS input
  receipts do not qualify DBCS or an active code-page transition.
- Existing geometry005/007 verifies80x50 native buffer with80x25 viewport and
  shifted viewport output. It does not prove a changed guest visible grid.

These findings neither waive the original operation obligations nor introduce
new behavior. Full168, publication and remaining actual-operation acceptance
must be reported separately.
Full168 completed exit0, all16 gates passed on the current complete11 package.
Evidence: artifacts/t481-s3/full168.json. Governance169 passed. These results
do not erase nested164, qualify unfinished S3 operations or constitute a new
default publication. Cached167 evidence is artifacts/t481-s3/cached167.json.

Identity runner now forwards the existing optional registry-monitor/marker
arguments to the matrix; it does not add product or lifecycle behavior.
Run170 reached normal Windows exit and generated both text outputs, but
registry capture failed RPC1722, so its overall result is failed. It selected
the x64 monitor fixture dated2026-10-26. Repeat171 uses the proven146 x86
fixture (rebuilt2026-11-07). This comparison is pending; do not attribute1722
to the product or declare its cause proven. Evidence: nested170.json.

Repeat171 reproduced the90s timeout and cleanup DLL occupation, without B.TXT.
Thus the earlier B-completion snapshot trigger cannot diagnose this case.
Evidence: artifacts/t481-s3/nested171.json. The matrix now optionally captures
the live service tree on failure before scope teardown, preserving the primary
error even if capture fails. This is test-only, no product wait/retry change.
Repeat172 PID19760 runs this diagnostic. Production remains frozen since Full168.

### S3 nested-chain diagnostic172--177

172 and174 passed with the current x86 registry fixture;173 repeated the timeout.
The successful live task tree shows separate VM frontend ownership and real
native/DOS workers, not ancestor NTCON takeover. It does not prove I/O handback.
Evidence: artifacts/t481-s3/nested172-174.json.175 also passed unchanged.
The matrix previously classified observer timeouts only after cleanup; it now
recognizes the existing timeout receipt before cleanup so optional failure
registry capture can observe the live service. This changes no product timeout.

The failed nested NTVDM stack is idle in cmdGetNextCmd/GetNextVDMCommand.
Input tracing is therefore the next specific diagnostic: the witness queues
command plus exit before the nested child runs. Original nt_event.c calls
ReturnUnusedKeyEvents then ReturnBiosBufferKeys before relinquishing I/O.
Diagnostic176 logs successful VM READ_INPUT/PREPEND_KEYS records by queue
identity, character and make/break, without consuming or modifying records.
The selected product source remains unchanged from Full168. Diagnostic176
built exit0; actual run177 PID3656 is active on its separate package. Input loss
is a hypothesis, not a root-cause conclusion. No new default publication.
Run177 reproduced the timeout and captured the failure tree before cleanup.
The remaining VM has an active native CMD, with the nested DOS worker EMPTY.
READ_INPUT/PREPEND_KEYS observations show one queue's original exit sequence
subsequently read as texit, with t/CR and exi fragments among returns. This is
stronger than a timeout alone, but not yet attribution to a particular return
stage. Repeated record delivery across ownership changes is not by itself
duplicate consumption. Evidence: artifacts/t481-s3/nested177.json.
Next inspect original ReturnUnusedKeyEvents/ReturnBiosBufferKeys and the
project history/zero-receipt VM boundary; do not assume physical-key receipts
can simply be copied from NTCON or introduce a second keyboard engine.

### S3 keyboard handoff correction189--193

The failed deep-chain witnesses repeatedly returned exit as texit; native
Console observation185 saw the corrupt sequence already on ingress, while
queue enqueue observation187 contained only the original stimulus. Child
keyboard observation182 ruled out the separate read-before-EOI history defect
as the cause of that run.

The selected reset chain is Reset6805and8042 ->
mvdm_keyboard_reset_pending_irq -> ica_hw_interrupt_cancel ->
host_EOI_hook(-1) -> KbdEOIHook -> continue_output. Previously the cancellation
ran before clearing the device queue. Its callback queued the next scan and
left a new delayed IRQ after the subsequent reset discarded its provenance.
The actual controller/PIC fixture188 proves this scheduling error; candidate189
captures the existing user-origin eligibility, clears the device first, then
cancels. The same fixture changes from89 assertions/1 failure to89/0.
Original keyboard/EOI algorithms remain selected; DIV-321 records the ordering
adaptation. No original guest is changed.

Separately DIV-317 now excludes an already-read output slot from unused-input
selection without retiring the slot before EOI. Origin178 fails that contract;
origin193 passes it and the pre-existing origin/reset cases. This is not claimed
as the cause of texit.

Nested191 and192 pass both guest VM chains, MEM output,320 callback assertions
and normal Win3.1 exit. These runs precede the separate DIV-317 predicate fix;
the combined candidate still requires current-package acceptance. Evidence:
artifacts/t481-s3/reset189.json, nested191.json and repeat193.json. Cached194
and dependent Full196 are the combined candidate delivery qualification.
This is a bounded P repair, not completion of the remaining S3 operation,
geometry/font/codepage, redirection/cancellation or later Win95 obligations.

### S3 combined qualification and publication201--203

Full196 passed16/16; final197 passed real editor198, DOSONLY1 editor199 and
nested200 after restoring original diagnostic line1163. DOSONLY1 had zero
host admissions. The final production NTVDM differs from Full196 only in
COFF/debug timestamps after package-ID normalization; ten other build input
hashes are identical. See artifacts/t481-s3/final201.json.

Publication203 delivered complete11 package c7f7cdca-4214-4452-a01d-d2f1c3858d25
to both release destinations, verifying every image and manifest. Addons
remain assets-only and unchanged. See artifacts/t481-s3/publication203.json.
Governance202 passed. P4 final review/commit/push are not yet complete.
Remaining S3 geometry/fonts/codepages, operation and nested cancellation
requirements are unchanged; publication does not close S3 or qualify Win95.

### S3 remaining-owner audit205

P4 is committed and pushed at6f0c60bfa; publication203 is the current complete11
package. Post-delivery source audit separates these obligations:

- KEYBOARD_LAYOUT: original dos/command/cmdkeyb.c explicitly sends a failed
  Console-layout query to NoInstallkb16, returns DX=0 and calls cmdInitConsole.
  ADAPTER-WIN32-034 forbids substituting a thread HKL. The VM provider currently
  returns unsupported; guest OEM codepage is not a keyboard-layout identifier.
  This is a proved fallback path, not proof of international layout support.
- SET_POINTER_CLIP(NULL): original nt_mouse.c MouseDetachMenuItem releases the
  host Console menu clip. The VM provider cannot release the outer desktop's
  clip or claim arbitrary guest clipping is implemented. Non-null clipping and
  actual per-VM ownership still require a guest-owner design; no success stub.
- Geometry: command_screen captures INT10 mode, rows, columns, font height and
  cursor; command application validates current VM geometry before writing.
  vm_text's host snapshot alone cannot implement a mode/viewport change. Any
  resize work must include guest application and acknowledgement, not only a
  larger host allocation. Current same-size BUFFER_SIZE support is bounded.
- Codepage: the guest vm_codepage is carried in the screen header and validated
  by executor_return_key_bop. It is already the conversion input; it must not
  be replaced by the Windows thread locale. Additional codepage cases remain.

Worker-close205 reuses the passed S2 worker-close051 runner, with final
package197/driver143 and explicit VMD/insignia112. It exercises real ordered
native commands and NTSRV worker shutdown after both HOLD markers, not normal
completion. Owned PID8696; logs and exit receipt under build/t481-s3. No product
source change or rebuild is made for this audit. Inspect the terminal result
before counting this case; broader nested cancellation remains separate.

### S3 lifecycle and nested redirection206--209

Service-loss206 and console-close207 passed real held native tasks in two
Win3.1 VMs using the S2 proven runner on final package197. Evidence
artifacts/t481-s3/lifecycle206.json distinguishes forced retirement from normal
exit. Neither modifies the original installation or production sources.

The test-only continuation adds a NestedRedirect mode requiring ReentrantCommand.
Each VM runs command /c cmd /c mem with output to result.txt, appends its ROLE,
and uses TYPE piped to FIND before EXIT. Native key records use VkKeyScanA for
punctuation/modifiers; this remains deterministic provider stimulus, not actual
physical guest input. Diagnostic208 builds successfully. Redirect209 passes
actual file MEM content, separate A/B append markers, pipe screen content,320
callback checks and normal Win3.1 exit. Evidence artifacts/t481-s3/redirect209.json.
Geometry/font/codepage and the full cancellation-owner matrix remain open.

### S3 guest-selected grid213

The new GuestGridControl probe invokes BIOS mode3 in both VMs and1112 in A,
then requires A=80x50/font8 and B=80x25/font16 after real ordered commands.
Grid213 fails with actual PUBLICATION_END50 and runtime timeout. It is not
proof that BIOS mode switching itself failed. Initial VM snapshots contain
height but no published font configuration; NTVWM falls back to its built-in
16pixel font. Diagnostic214/grid215 will observe the transmitted BIOSFONT and
VIDEOFONT values before application. Product implementation is unchanged.
Original ega_char_gen in base/video/ega_vide.c distinguishes1100 load-only
from1110 load+recalc_text(height); use this original mechanism if row-changing
font application proves necessary, rather than authoring duplicate VGA logic.
Evidence artifacts/t481-s3/grid213.json; PID22968 owns the diagnostic sequence.

### S3 guest grid correction and qualification224--228

Grid217 proves initial BIOS8/VIDEO16 disagreement, not a guest failure. The
NTVWM initial snapshot has no font configuration; its native fallback font
must not overwrite the physical VM's glyph owner. Candidate presentation.c
uses the existing Unicode cell transaction for a physical VM without a copied
font configuration, then the existing text barrier. Font-bearing video and
NTCON retain their serial-bound presentation barrier. Errors abort the staged
transaction and propagate through the existing lifecycle; no new transport,
font cache, retry or deadline is introduced. This changes project-owned code,
not mirrored source or original guest binaries.

Grid219 failed all three A continuations; correcting the cell-only final
barrier yielded two passes in grid221. Grid223 captured54320165 in round1:
the native fixture returned101 because test BIOS1130 overwrote ES, the next
command's environment segment. Preserving ES fixes the test. Grid224 and
reverse-order grid226 both pass the actual A80x60/font8 and B80x25/font16
preservation, native08/AH0/AH1 and0A returns,320 callback checks, normal Windows
exit and unchanged original installation. This supersedes the initial fixed
50-row assumption. Evidence: artifacts/t481-s3/grid224.json and run logs.

Qualification225 (PID19480) runs cached225, Full227 and native228 serially.
No new release is claimed until their terminal receipts are inspected.
Source review identified an additional required candidate check: a valid
native cursor outside the viewport must not be misclassified as an invalid
frame by the new cell-only route. The current guard rejects it; preserve this
open item until corrected and tested. Arbitrary runtime geometry changes,
font bitmap preservation, palette/codepage and remaining S3 operations are
not inferred from the passed grid-height witness.

### S3 viewport cursor correction229--230

Qualification225 completed: Full22716/16, both grid completion orders,
x86/x64 shared checks14/14 and keyboard/PIC89/89. Native228 passed the existing
presentation/input-return/lifetime fixtures. See qualification228.json.

Source review found the new cell path rejected a valid cursor outside the
native viewport. The physical-peer fixture229 reproduces five failures in
flush, re-entry and final handoff. Production now keeps absolute cursor state
in the hidden Console, omits guest position updates while outside the viewport,
and projects visibility as native-visible AND inside. Re-entry sends the real
relative position. No new cache or guest state is introduced. Fixture230
passes presentation564, input-return605 and lifetime1084 assertions. Its peer
rejects VIDEO_BEGIN/PRESENT_BARRIER on the fontless physical route, proving
that text-only handoff does not invent a video serial. Existing full Console
and known-font cases remain exercised. See artifacts/t481-s3/cursor230.json.

Final-source qualification231 (PID26100) runs cached231, Full232 and diagnostic
package233 with grid234/235. Additional real editor and nested chain checks
must use that same candidate NTVWM before publication. Prior Full227 does not
qualify the cursor correction by itself. S3's broader requirements remain.

### S3 real editor regression236 and mapping correction240

Final Full23216/16 and grid234/235 passed, but editor236 failed the native
File-menu witness. Trace identifies op15 (WRITE_CELLS_W) error1113; the native
editor's initial frame appeared but its menu never committed. Thus the new
cell path must not substitute strict guest Unicode encoding for the established
text_frame VGA-glyph mapping. It now sends the already packed glyph/attribute
bytes through existing WRITE_CELLS_A. This removes the duplicate conversion
and preserves VIDEO's fallback/reverse mapping. Underline remains explicitly
unsupported by the guest application; it is not silently discarded.

Native240 passes the same2253 assertions, and the physical-peer fixture now
rejects Unicode-cell publication as well as invented video barriers. Real
editor requalification is required: native fixtures alone cannot close this
regression. Qualification241 (PID22816) rebuilds only affected ntvwm.exe in the
existing Ninja cache, then stages package241, runs editor242, nested243,
grid244/246, and Full245 serially. Earlier gates do not qualify this last fix.

The prior r232 and diagnostic233 NTVWM byte difference was separately proven:
20 differing bytes, exclusively parsed COFF/debug timestamps and package GUID;
normalized complete images match. Evidence ntvwm-package-identity233.json is
not a claim about later packages. Publish only the final tested production
package, never the diagnostic NTVDM image.

Final qualification241 completed0: editor242, nested243, grid244/246 and
Full24516/16 passed on the mapping-corrected candidate. Native240 passed2253
assertions. Evidence artifacts/t481-s3/final247.json. Complete11 production
package r245 is manifest-verified and shares the NTVWM input hash with the
guest-tested diagnostic241 package. Governance/publication/P6 remain pending.

P6 publication248 completed: package14d987d1-7fe7-4a7d-bc6e-af50e08c0bd6,
all11 images and manifest verified at both release destinations, addons
assets-only. Governance247 passed. Publication248 and final247 retain exact
hashes and gate results. P6 delivers this bounded repair; S3 stays active.

### S3 actual glyph preservation 251--253

The existing grid probe now snapshots both actual VGA font banks before native
command admission and compares every active glyph row after each of three
continuations. It reuses the existing plane-reader and restores all touched VGA
registers/latches; it does not reload fonts after the command. The snapshots
are guest-local test scratch, not new product state.

251 passed initial comparison. Strengthened252(B first) and253(A first) both
pass nonempty-byte and three-comparison assertions: A1540 and B2320 nonzero
bytes,320 callback checks per run, normal Windows exit and unchanged original
guest installation. Evidence: artifacts/t481-s3/font253.json. Run250 failed
test assembly due to unsupported NASM defined() syntax; corrected before251.
No production code, mirror or published package changed. This proves retention
of the tested BIOS-selected glyph banks, not arbitrary custom font application,
palette changes or whole S3 closure. Test delivery remains pending.

### S3 P7 glyph/palette preservation qualification

254(A first) and255(B first) add guest BIOS DAC7 red/blue controls, then
compare all16 text DAC entries and attribute mappings after each real native
continuation. Both pass three font and three palette comparisons per VM,
320 callbacks and normal Windows exit, preserving original installation bytes.
The existing plane-reader is shared; each read restores registers and latches.
Evidence: artifacts/t481-s3/palette255.json supersedes the narrower font253
result without erasing it. No product/mirror/addon source changes.

This is a test-only P7: production cached231, Full24516/16 and publication248
remain the unchanged-input baseline. Delivery256 rechecks all11 actual deployed
images and manifests at both destinations before committing. No rebuild or
package replacement is necessary for unchanged production inputs. Custom font
loading, native-requested palette changes and other S3 rows remain unqualified.

### S3 P8 candidate: guest-codepage physical cell publication

257 passes216 VM text assertions including independent CP437/CP850 Unicode
round trips and unrepresentable-character failure/abort. DOS INT21/6601 owns
guest codepage. VM raw input mode is separate from NTVWM's actual hidden
Console cooked/VT processing; notification fixtures cover raw mode get/set
and rejection of unimplemented cooked/output flags.

258 deterministically fails: actual NTVWM publication of U+00F8 to a CP850
physical peer yields003f instead of009b. Sender reused its CP437 packed frame.
259 queries existing CODE_PAGE at physical-cell publication, validates SBCS
and reuses the existing glyph converter with that page. NTCON's owned font
remains CP437; trailing-cell blanks and cursor/publication rules are retained.
This corrects project-owned native presentation, not an OpenNT algorithm. No
mirror/guest change, new ABI, cache, timeout or replacement state owner.
259 passes604 presentation,605 input-return and1084 lifecycle assertions.
Evidence: artifacts/t481-s3/codepage259.json. Frozen qualification267 owns
cached260, candidate261, editors262, nested263, glyph/palette264/265 and Full266.
These remain pending until receipts are inspected. No new default publication.
Guest codepage switching and DBCS are not proved by this native peer fixture.

### S3 P8 qualification and publication

Cached260 and candidate261 passed; actual editor262, nested263 and glyph/
palette264/265 passed. Full266 failed only final nested-window input delivery:
exit23 and parent-return text were present. It is retained as a failed run.
An unchanged assertion set with diagnostic-only observer fields passed all
three focused272 runs, Full27316/16, and ten per-milestone focused274 runs.
The157 accepted messages,6 Enter downs and29 waits match between success and
failure; the extra20000ms points to a milestone observation wait, not a
proven key-send failure. No speculative runtime shutdown fix or test waiver
was made. The single failure is unreproduced and is not claimed repaired.

Final275 verifies source review268, all11 candidate hashes and equality of
NTVWM build input with the actual Win3.1-tested package261. Governance276
passed. Publication277 deploys the entire tested r273 package to both release
destinations with matching hashes; unchanged authored addons remain assets-only.
Evidence: artifacts/t481-s3/final275.json, qualification273.json,
handoff272.json, handoff274.json and publication277.json. This is bounded
CP850 presentation delivery, not proof of original DOS codepage switching,
DBCS or whole-S3 completion. Remaining scope stays in the sequential plan.

### S3 P9 guest-owned grid transitions

The actual producer audit distinguishes native scrollback from guest-visible
geometry: NTVWM's presentation-only route skips BUFFER_SIZE/WINDOW_RECT for
its hidden Console; VM BIOS remains the grid owner. Test-only GuestGridSwitch
now changes that owner between three real native continuations. A alternates
8/16/8-pixel BIOS fonts and B16/8/16, recapturing nonempty glyphs and palette
before each handoff and verifying them on return. Shared setup saves registers
and ES; ordinary GuestGridControl retains its initial-only setup.

279(A-first)/280(B-first) pass three configurations and three glyph/palette
comparisons per VM,320 callbacks per run, normal Windows exit and unchanged
original guest installation. Evidence: artifacts/t481-s3/grid-switch281.json.
No product or mirror changes. Existing Full273/publication277 remains the
unchanged-input release baseline. Native-requested resize during a command,
arbitrary grids and graphics remain separate obligations, not inferred passes.

### S3 P10 candidate: virtual BIOS font geometry

The400-scanline control fails at stage31 on the unchanged baseline: standalone
DOS gives50 rows, but Win3.1 VM gives60. Diagnostic288 records BIOS1112 changing
BDA rows25->60 using physical height481 before native handoff. Diagnostic290
shows mode3 changes physical height in the system VM but leaves it unchanged
in child VMs. Diagnostic292 reads the virtual CRTC through existing inb/outb:
VDE399 before1112 becomes479 afterwards. The CRTC index is restored after
observation. All controls exit Windows normally and leave original guest bytes
unchanged; these are retained failures, not passing geometry tests.

The actual original chain is low_set_mode/do_outb -> ios.c outb ->
ccpusas4.c c_IOVirtualised -> biosDoInst in software V86. That correctly gives
Win3.1 VDD the port operations, while recalc_text directly reads physical
PCDisplay. The mismatch is an original host BIOS input-boundary defect, not
NTVWM handoff, guest code corruption or a reason to import hardware V86 logic.
SoftPC has the same physical calculation; no existing correction was copied.

DIV-890 adds16 mirror lines: only VGA in software V86 reads VDE/overflow and
CRTC17 retrace scaling via existing virtual ports, restores the index, then
uses the original formula and remaining algorithm. No cache, state owner,
new ABI, guest modification or unsupported-dimension success is introduced.
Source review preserves the V7 horizontal-retrace multiplier (CRTC17 bit2),
not the unrelated scan-doubling bit in CRTC9.

Candidate293 builds. Actual294(A-first) and295(B-first) pass the strict50-row
assertion, three font/palette comparisons in each VM,320 callback checks,
normal Windows exit and original guest identity. Virtual VDE remains399.
Evidence: artifacts/t481-s3/scanline285.json, scanline288.json,
scanline290.json, scanline292.json and scanline294.json. Cached296 production
qualification is ongoing. No candidate default publication or S3 closure is
claimed. Native-requested resize remains distinct from these guest-owned
BIOS transitions, as retained in resize-audit283.json.

P10 qualification: cached296 passes both widths/shared14/14 each and keyboard
regression; Full297 passes16/16. Production DOS298 preserves50 rows/font8.
Editor299 was an invalid production-package/diagnostic-fixture pairing, whose
controller waits for diagnostic-only EDITORS.BIN; retained as non-pass in
qualification297.json. Correct diagnostic editor300 passes menu/mouse Exit.
Uninstrumented production301 passes all four Win3.1 mode/DOSONLY combinations
and normal exits. Fixture302 compiles default/TEST_VIDEO_GEOMETRY variants;
default has no geometry-observer import. Review303 records exact source hashes
and unchanged scope limits. Complete package publication and commit pending.

P10 publication304 completes the coherent11-image release at both destinations
with manifest/hash equality, package d75521ce-8b98-4319-9ec7-31b445a12af3.
Authored addon hashes remain unchanged/assets-only. Evidence: review303.json
and publication304.json in artifacts/t481-s3. This delivers DIV-890 only;
the remaining S3 obligations and later Win95/combined qualification are open.
### S3 P11 candidate: native visible grid publication

The retained308 failure is at WRITE_CELLS_A after25 successful rows: native
visible80x50 was never prepared in the VM provider. The older buffer80x50 /
viewport80x25 witness did not cover this case. Evidence: resize308.json.

NTVWM now sends PREPARE_TEXT_REGION inside its existing publication before
writing visible cells/VIDEO; it still does not send hidden Console scrollback
to a VM. NTVDM allocates a private candidate and derives its byte extent from
the validated header, removing the duplicate size field. Refusal/disconnect
keeps committed state. Acknowledged text-only geometry invalidates old VIDEO
configuration. Native BUFFER_SIZE semantics are unchanged.

For changed rows at unchanged columns, the host proposes a fitting font height;
this is not hardware authority. Authored ntvdm64.386 independently validates
current BIOS dimensions and the requested8/14/16-pixel font-row relation, invokes
the original BIOS1111/1112/1114 service, then queries BIOS1130. Only matching
rows/height permit cells and success acknowledgement. No original guest binary,
insignia.386, mirror source or protocol layout changes. No new wait/transport.

313 builds.314 exposed an ES preservation defect in the new BIOS-query probe,
not a passing run. After correcting that probe,315/316 pass native80x50 in both
completion orders;317 passes25->50->25 with actual row/font and cell assertions,
320 callback checks and normal Windows exit. Original installation unchanged.
Evidence: artifacts/t481-s3/resize316.json and resize317.json. Region311 unit
and Native312 protocol fixtures are separate evidence, not guest substitutes.

Mouse source trace uses the existing original INT33 text8x8/16x8 coordinate
contract, converted to absolute cells by mouse_event, then MOUSE_EVENT through
mvdm_vm_executor/io. It is not the NTCON frame-pixel-delta path; no speculative
font-height cache or second input conversion was added.

Remaining boundaries: custom-font and other mode/column transitions, unexpected
BIOS failure after a service call, and the rest of S3 operations are not closed
by these cases. Unsupported preconditions still return errors; no arbitrary
VGA layout support or universal rollback is claimed. Cached319 completes both
widths, shared14/14 each and keyboard/PIC. Qualification320 serializes Full320,
dual editor save/mouse321/322 and production four-combination323. Publication,
review and P11 commit/push remain pending.

Qualification320 completes exit0: Full32016/16, editor321/322 save and guest
mouse Exit in both orders, production323 all four mode/DOSONLY startup and
normal-exit cases pass. Evidence: artifacts/t481-s3/full320.json, editor322.json
and review324.json. Complete publication and P11 commit/push remain pending.

P11 publication326 completed: all11 tested host images and manifest match at
assets/release and O:/winnt/system32, package b6c8ac10-d9d0-46d8-81d4-e7f86ee8991a.
Updated authored ntvdm64.386 and unchanged insignia.386 remain assets-only.
Evidence: artifacts/t481-s3/publication326.json. Governance324 passed.
P11 commit/push pending; S3 and later S4/S5 remain open.

P12 test-only custom329/330 pass both completion orders: original BIOS1110 loads
distinct glyph65 bytes in each VM, actual VGA reads prove installation and
three complete glyph/palette comparisons per VM preserve them across commands
and guest grid switching;320 callbacks and normal Windows exit per run.
Custom328's1100 stale-geometry test assumption is retained as a failed control.
Evidence: artifacts/t481-s3/custom328.json and custom330.json. Production
source/addons unchanged; publication326 all11 hashes still match both targets.
Full320 is reused by unchanged-input identity. Native font replacement/DBCS
and remaining S3/S4/S5 are not qualified by this test.

### S3 P13 codepage owner validation

Original cmdconf.c removes user COUNTRY entries and synthesizes country and
GetOEMCP; authored command_directory reads resulting DOS6601. The isolated
CP850 CONFIG attempt332/333 therefore sees437 before native command and fails.
Retain artifacts/t481-s3/codepage333.json; no product defect or CP850 acceptance
is inferred. Native fixture259's CP850 proof remains a separate peer case.

Actual334 validates DOS437 and Unicode cent cell2f9b in both VMs,320 callbacks
and normal Windows exit.335 additionally changes native output page to850;
guest must still report437 and render9b, not850'sbd. Opposite command order
remains required. No source/locale/guest patch is used to manufacture CP850.

P13 test-only334/335/336 pass: actual DOS437 before/after commands; native
Unicode cent becomes guest2f9b.335/336 additionally set native outputCP850
and pass both completion orders,320 callbacks and normal Windows exit.
Evidence: artifacts/t481-s3/codepage336.json. Original COUNTRY filtering and
failed850 prerequisite are retained in codepage333.json. No runtime source
changes; all11 publication326 hashes match both targets, Full320 reused.
Guest850, active guest codepage transitions and DBCS remain unverified.

## S3 complete VIDEO diagnostic prerequisite (338-345)

The new short-font VIDEO witness338 and ordinary-font339 both exit13.
Diagnostic343 identifies the capture assertion at host-continuation.c:323:
first A/B applications acknowledge zero, but the second continuation reports
PATCH instead of its initial PATCH/VA or PATCH/VB directory. Direct DOS AH47
and the captured directory agree. Deferred344 reproduces; no-VIDEO345 also
reproduces, excluding font application as the cause of this specific failure.
Evidence: artifacts/t481-s3/video345.json. Keep the strict assertion; source
attribution remains open. These diagnostic failures neither qualify full VIDEO
nor justify changing BIOS1100 to1110. Diagnostic stderr was not retained by
the observer, so failure locations now use its existing trace channel.
All changes are test-only; no new production repair, publication or P delivery.
Original guest installation hashes remain unchanged. S3 remains open.

### S3 VIDEO font-height baseline and candidate352/354

Diagnostic349 reports only failure bit256 (directory).350 yields in guest
before the first continuation and fails before any application acknowledgement;
this excludes host frame completion as a necessary cause. Original AH47 reads
CDS directly for a fixed drive (doskrnl/dos/path.asm:CurrentValidate/noquery).
The strict VA/VB test remains available and unresolved. Explicit optional
DirectorySnapshotControl compares capture with the independent current guest
AH47 result; it does not qualify cross-VM directory isolation.

With that control,352 reaches six successful application acknowledgements and
normal Windows exit, but actual BIOS height is16 instead of requested8.
Authored ntvdm64.386 candidate353 uses original BIOS1110 and checks1130 height
and rows before acknowledging.354 proves height8 and normal exit, but font
bank1 reads0 rather than3c (bank0 is3c), so overall acceptance still fails.
The original BIOS trace reaches1110/BX0800 and1110/BX0801. Do not attribute
the remaining bank failure without following its virtual VGA write/read path.
Evidence: artifacts/t481-s3/video354.json. No mirror, original guest or
insignia.386 change. Candidate is not published, committed or qualified.

### S3 BIOS scalar privilege investigation363/364

Real guest MOV font control362 passes both banks and normal Win3.1 exit;
BIOS-mediated361 writes bank0 but immediately reads zero after bank1 writes.
The read-only observer records CPL3/VM1 and raw nonpresent PTE values; those
raw entries are not evidence of a current valid TLB mapping. DirectorySnapshotControl
now deliberately uses the same PATCH directory in both VMs, avoiding the
independent AH47 race; strict distinct-directory acceptance remains open.

The actual CCPU/SAS permission fixture363e fails24/42 with the prior six
inverted CPL tests; candidate364 passes42/42 after changing !=3 to ==3.
The selected original386 permission table and PDE/PTE OR rule define the
expectations. Both tests link real CPU, SAS and physical memory with existing
asserted-unreached host lifecycle seams. First363c failed linking because the
host seams were omitted; that was not runtime evidence. DIV-891 records the
six same-line mirror substitutions, without new APIs or guest modifications.
Evidence: artifacts/t481-s3/access364.json. This PM matrix does not prove
V86 fallback, cross-page/reentrant behavior or the Win3.1 bank-loss root cause.
Isolated diagnostic365 builds; real Win3.1 VIDEO366 is running.
No candidate publication, commit or S3 closure is claimed.
Candidate366 still fails the second font bank (A samples60/0, height8),
despite six successful applications and normal Win3.1 exit. Therefore DIV-891
does not resolve the observed bank loss and remains independently unqualified
for publication. Diagnostic368 reads the registered PM data selector as zero
at every failed second-bank write, excluding selector contamination on this
path. Original CCPU intentionally bypasses segment-limit checks in V86; do not
invent a segment workaround. The next bounded observation compares virtual
sequencer/graphics-controller registers at each bank write.
Further completed controls: 368 confirms the PM selector is zero; 370 records
SEQ2=04, SEQ4=06, GC4=00, GC5=00, GC6=04 at both bank writes. 371 uses the
BIOS-style DS=0/32-bit operand in the real guest and passes both banks,
320 callback assertions and normal exit. 373's complete trace shows both
banks enter the actual ROM MOV/BOP helper (bytes c4028867 at F3080); an early
partial-log inference that bank1 skipped fallback was withdrawn. 375 selects
GC read plane2 only and still fails; 377 changes SEQ4 from06 to07 only and
still fails. These explicit diagnostic interventions are not production fixes.
All failures retain the font assertions and report normal guest exit separately.
379's font-bank selection control also fails bank1; do not retain that
intervention as a repair. Completed control matrix and log/trace identities:
artifacts/t481-s3/video379.json. The finite unresolved split is now original
BIOS full font-loading mode/restore versus the working direct guest VGA write
path, plus the separately reproduced strict VA/VB directory identity failure.
Do not claim a confirmed font root cause from the CPL matrix, selector,
operand-size, read-plane, sequencer-mode or bank-selection hypotheses; all
relevant negative controls are retained. No new package is published.
### S3 ordinary guest BIOS font control380

The existing VIDEO_FONT_CONTROL independently calls DOS VM INT10/1100 for
both banks after the native continuation, then reads actual VGA plane2.
It fails bank1 (A: 60/0 at offset0, height16) while Windows exits normally.
This demonstrates that the failure also occurs with ordinary guest BIOS
calls in the same post-continuation VM state; it does not establish behavior
in a fresh VM before any continuation, nor identify the responsible owner.
Original installation bytes remain unchanged. Log and trace hashes are
recorded in artifacts/t481-s3/video380.json. No new publication or closure.
Early control381 calls original DOS VM INT10/1100 twice and reads both VGA
font banks before its first diagnostic continuation BOP. A.BIN records
stage2/bank0=60/bank1=0/height16/offset0; Windows exits normally. The harness
correctly fails its later identity assertion because this early failure exits
before that API check. This establishes pre-continuation reproduction, not
the responsible BIOS/VDD owner or B-task acceptance. Evidence and hashes:
artifacts/t481-s3/video381.json. No original guest changed or candidate
published.
Early direct control382 explicitly writes and reads both banks before the
first continuation as well as after each VIDEO application. Both VMs pass
(A60/60, B102/102), 320 callback checks and normal Windows exit, with original
installation unchanged. Together with early BIOS control381 this localizes the
difference to the BIOS loading/virtual mapping path, while their differing
register setup prevents attributing the fault to a specific component.
Evidence: artifacts/t481-s3/video382.json. Neither control qualifies the
addon BIOS font application or the outstanding distinct-directory test.
### S3 directory ownership source check

The strict VA/VB test assigns distinct existing absolute paths via DOS AH=3Bh,
selects the corresponding drive and reads AH=47h before the driver callback.
command_directory independently executes AH=47h in the current nested VM and
copies the result before ending nested execution. Original DOS path.asm
$CURRENT_DIR reads ThisCDS directly for CURDIR_NT_FIX drives; original host
demSetDefaultDrive maps the selected DOS path onto the process current
directory with SetCurrentDirectoryOem. Original cmdmisc.c also reports current
directories on command completion. Those are different state owners and
neither fact establishes that host process CWD is the authoritative per-VM
CDS. Diagnostic350 yields before the first continuation and reproduces
failure before application acknowledgements, excluding completion as a
necessary trigger. The strict test and original guest remain unchanged; a
VM scheduling/CDS writer has not yet been identified, and adding a second
independently maintained directory table would be speculative.
Directory observer387 resolves original CDS guest logical043d0 using each VM
linear base. Captures B/A/B report linear844043d0/848043d0/844043d0 and same
CR3 001de000; xtrn2phy resolves only A to000043d0, not B. Therefore equal
DOS logical CDS addresses do not prove shared physical storage, and this
observer cannot determine B physical identity. Strict directory assertion
still fails and normal Windows exit is not achieved. Diagnostic385 omitted
the VM base and is invalid for mapping comparison. Evidence and hashes:
artifacts/t481-s3/directory387.json. Diagnostic383 first failed to compile
for missing prototype; corrected candidate386 built successfully.
### S3 guest-memory lease privilege consistency388-390

The project-owned mvdm_softpc_guest_memory_range_valid copied the original
inverted CPL comparison. With candidate DIV-891, that would make lease
preflight and actual CCPU SAS accesses disagree. It now requests PG_U for
CPL3, matching the corrected original CCPU helpers and the selected 386
access table. The expanded real-CPU/session fixture390 passes90/90 checks
covering scalar read/write and lease read/commit across CPL0/3 and PTE
combinations. Initial388 reported27 failures using the old preflight, but
three write-acquire expectations were invalid: the bounce lease checks
write permission on commit. Updated fixture390 tests actual commit.
Evidence: artifacts/t481-s3/access390.json. This is no proof of the font
bank root cause, V86 fallback, cross-page/reentrancy or production readiness;
the candidate remains unpublished.
Access391 repeats the corrected 90-check fixture against identical CCPU
candidate and test inputs: only project lease CPL selection changes. The old
lease check fails30/90; the candidate passes90/90. The script restores the
candidate source in a finally block before rebuilding. This supersedes the
inexact388 baseline count; it does not qualify V86 or full runtime behavior.
Evidence: artifacts/t481-s3/access391.json.
WOW shared-owner sweep: mvdm_softpc_wow_page_domain.c also copies the original
BIOS write access request for its bounded allocated-page bulk-store fast path.
Its CPL test now matches DIV-891 and the scalar fallback (CPL3 selects PG_U).
This is a project-owned one-line change, not an additional mirror divergence.
Existing WOW page-domain fixture is linked by the lifecycle verifier, so its
actual coverage must be examined and executed before qualifying the change.
Cached392 is running with owner PID22044; do not infer success from startup.
The font-bank and distinct-directory failures remain open.
Cached392 completes exit0: x86 and x64 builds and shared tests14/14 each,
keyboard/PIC89/89. WOW window-borrow fixture393 failed to link because its
library list omitted existing lib-process-launch; fixture list corrected.
WOW394 passes actual page-domain fixture, including immediate stores and
trace fallback. This is not a comprehensive CPL/PTE matrix nor real Win16
acceptance. Evidence artifacts/t481-s3/wow394.json. No package published;
font bank and directory tests remain failing.
Directory source audit395 corrects observer387's target: test drive O is
index14, while observed DOS CDSCount is5. Original GetCDSFromDrv selects
DOSDATA:NetCDS for index >= CDSCount, not the ordinary table at043d0.
It conditionally calls CMDGETCURDIR on sync; original cmdGetCurrentDir
reads host process environment =O:, and original demSetCurrentDir sets
that environment entry. Therefore observer387's ordinary-table mappings do
not identify the O: directory's backing. video350 records B seeing VA before
any application acknowledgement. Host process environment is shared, but
source alone does not establish whether shared NetCDS, sync, another writer
or VM switching produced that observation. Evidence directory395.json.
No guest changes or second directory authority introduced.
Directory399 adds a diagnostic-only observer around original CmdDispatch
service4, retaining the real service and logging its returned guest bytes.
Both VA and VB appear in successful DEM directory-set traces. B's second
capture reads VA and fails; the last DEM directory set before that capture
is VA, with no O-drive CMDGETCURDIR call in that interval. This rules out an
O: CMDGETCURDIR invocation in that specific interval, not all process-wide
environment effects or NetCDS/VM restoration causes. DEM trace lacks VM
identity, and the diagnostic abort prevents normal Windows exit.
Evidence: artifacts/t481-s3/directory399.json. Diagnostic396 failed because
the script renamed a component path; 398 corrected the script and built.
Directory400 source/identity audit: the pinned original NTDOS.SYS and installed
O:/winnt/system32/ntdos.sys have identical SHA256 95766232...F93BC84.
NTDOS.MAP places SYSINITVAR at0026, CurDrv0338 and NetCDS13f3; video399's
AH52 also returned offset0026. Original dostab.asm Win386 instance table
does not list CurDrv or NetCDS in its seven described regions. This is not
proof that DOSMGR or another VMM mechanism never instances them, nor proof
that the map matches every loaded variable. Existing insignia.386 calls
_AddInstanceItem during Device_Init; blindly adding NetCDS would leave the
separately shared host =drive environment unaddressed. No binary identity
assumption or guest modification has been added to production. Evidence:
artifacts/t481-s3/directory400.json.
Directory402 identity-bound read-only guest witness: package NTDOS.SYS
SHA256 95766232...F93BC84 is checked before execution, and AH52 SYSINITVAR
offset0026 is verified before reading DOSDATA:13f3 (NetCDS, original map).
B first captures VB in AH47, NetCDS and copied command; A captures VA in all
three; B second captures VA in all three and strict assertion fails.
This excludes command-capture copying as the cause of that occurrence:
B's observed original DOS directory already changed before the copy.
It does not identify the writer or prove page sharing/instancing.
Diagnostic terminates before normal Windows exit; no original guest modified.
Evidence: artifacts/t481-s3/directory402.json.
Directory405 observes actual CPU translation immediately after each guest
NetCDS read, before callback. B/A/B all report V86 CPL3, CR3=001de000 and
linear=physical=00001e63 for NetCDS. Directory402 established B's VB value
becoming VA across the A run. This establishes same physical location at
the observations, but does not exclude VMM copy/swap instancing. The
identity-bound test uses the original map and verifies NTDOS binary hash;
no production offset or guest patch is introduced. Strict failure aborts
the diagnostic and does not establish normal exit. Evidence directory405.json.
Diagnostic403 failed on duplicate test prototype; 404 built after correction.
Directory406 corrects the inference from the initial seven DOS instance
entries: original mscode.asm win386_inst explicitly returns not-previously-
instanced for CDS/SFT/device chain so Windows DOSMGR handles them. Original
msinit.asm fills the seven initial entry segments; absence from that list
alone does not establish no other instancing. The original DOSMGR service
declaration includes DOSMGR_Copy_VM_Drive_State and _DOSMGR_Set_Exec_VM_Data,
but names alone do not establish their calling convention or treatment of
NTDOS's NetCDS / shared host =drive environment. Evidence directory406.json.
Do not register fixed NetCDS offsets in the addon or claim a fix without
tracing those boundaries. Original guest remains unmodified.
Directory407-409: independently authored insignia.386 was copied into a
disposable diagnostic build only, adding a second VMM _AddInstanceItem
descriptor for observed NetCDS linear00001e63, length71, ALWAYS_Field.
Original installed guest and production addon are unchanged. Relative to
strict failing405, 408 passes two distinct VM directories, 320 callback
checks and normal Windows exit;409 additionally yields18 guest ticks before
each callback and also passes. NetCDS page instancing is therefore a
causally supported repair direction for this test, not an accepted final
design: the fixed diagnostic address is not production-safe, and first-use
host =drive synchronization, other drives, variable DOS layouts, VM
creation/retirement, setup failure and Windows modes remain unproved.
Evidence artifacts/t481-s3/directory409.json; candidate is not published.
Directory410 is a disposable test-only insignia.386 derived from407. During
Device_Init it tries original DOS AH52 through VMM nested V86 execution,
computes the diagnostic NetCDS address from returned ES plus map offset13f3,
and rejects unexpected SYSINITVAR BX. The fixed absolute address is removed
from this experiment; the offset remains identity-bound and is not a product
design. Binary built successfully. Actual Windows test411 is running as
owner PID20108; its outcome has not been observed. Do not restart it based
on an observation timeout. No original guest or released addon changed.
Directory411 failed: isolated insignia410 nested DOS AH52 during Device_Init
did not reach the Win3.1 desktop and observer timed out. Original guest
installation stayed unchanged. The failing VMM call is not identified, so
do not state that every initialization-stage AH52 is unsupported; do not
reuse this initialization technique as a product fix. Evidence directory411.json.

Source alternative: original msinit.asm calls CMDSETINFO from DOSDATA DS,
passing DS:DX = SCS_COMSPEC, before normal command operation. The existing
session-owned mvdm_command_guest_state records this source location; its
scs_field accessor canonicalizes the linear address. Original NTDOS.MAP
places SCS_COMSPEC12fc and NetCDS13f3, difference00f7, but map-derived
offset and input identity are not a universal DOS ABI. Neither execution
timing of that registration relative to Device_Init nor a generic way to
identify NetCDS has yet been established. No new BOP or production offset
has been added. DOSMGR service declarations alone do not supply the missing
NetCDS offset.
Directory415 attempted an isolated BOP54 marker from test-only insignia413
Device_Init to query existing session SCS location without a DOS call.
Win3.1 failed to reach the desktop and the expected marker was absent;
original CMDSETINFO service marker261 did occur earlier. The missing marker
does not prove Device_Init was reached or registration was unavailable.
Do not use this ring0 BOP54 instrumentation in production.
Evidence directory415.json. Diagnostic412 first failed due to an unavailable
test include; the corrected test-only declaration built in414.
Directory418: test-only insignia417 uses the existing BOP B1 mechanism to
observe session SCS registration during Device_Init. It succeeds with
canonical SCS01d6:000c (linear1d6c), and the pinned map's 0xf7 delta gives
observed NetCDS1e63. Strict distinct-directory test,320 callback checks and
normal Windows exit pass. This tests availability, not a product ABI or a
generic DOS layout. Shared =drive environment and lifecycle remain open.
Evidence directory418.json. BOP54 experiment415 was an invalid approach.
Directory421/423 tests a diagnostic BOP B1 returning the existing session's
registered SCS linear address plus pinned NTDOS map delta0xf7; addon derives
its instance address at runtime. First421 failed after the addon allowed an
intervening VMM service to overwrite EDX before saving the result. The
corrected test-only addon422 saves it immediately;423 passes strict dual-VM
directory assertions, 18-tick guest yields,320 callback checks and normal
Windows exit. Original guest installation unchanged. This is not yet a
production ABI: the delta is identity-specific, and shared host drive state,
initial synchronization, other DOS layouts and VM lifecycle remain open.
Evidence directory423.json. No candidate published.
Directory425-429 eliminates the prior fixed0xf7 delta in the diagnostic
provider. Original NTDOS lmstub.asm DOSWOWDATA declares lpCDSBuffer as
OFFSET DOSDATA:NetCDS, and msinit.asm passes DS:SI to DEMSETDTALOCATION.
Original demSetDTALocation already reads this same structure for lpSftAddr.
Diagnostic426 observes actual DS:SI=00a7:1482, lpCDSBuffer=000013f3,
and derives linear1e63 from DS and the original offset. This address remains
available at Device_Init. Test-only provider428 returns that value directly;
429 passes strict dual-VM distinct directories, 18-tick guest yields,320
callback checks and normal Windows exit. Original guest unchanged. The test
observer retains a process-global numeric value; production must instead
honor session and guest-memory epoch lifetime, avoid copying private guest
data, and propagate absent/invalid registration without fabricating success.
Shared =drive environment, first-use/other-drive state, VM lifecycle and
font-bank failure remain unverified. Evidence directory429.json.
### Owner-frozen remaining scope

Owner directs the remaining work in this order: (1) determine and correct
the actual font-bank cause; (2) implement directory isolation without fixed
addresses, preserving the original DOSWOWDATA contract and VM lifecycle;
(3) qualify all candidates together with Win3.1, Win95 and full regression.
Coverage expansion without evidence of a current functional defect is
recorded as a boundary instead of recursively expanding this work. Actual
known defects and failed acceptance remain open; passing a narrow case does
not replace the combined qualification or authorize publication.
Owner freezes remaining work to actual font cause, address-independent
directory isolation and combined Win3.1/Win95/full qualification. Font431
changes both previously observed VGA differences together (GC read plane2 and
sequencer mode07); second bank still reads0, Windows exits normally.
This excludes their combination in this control, not unmeasured register or
VMM mode effects. Direct guest MOV passes but original load_font calls
low_set_mode(FONT_LOAD_MODE) then restores a full BIOS mode; compare that
whole transition before proposing a fix. Evidence font431.json.
Font433 substitutes only diagnostic font-plane setup for the original initial mode switch while retaining original mode restore; bank1 still reads zero and Windows exits. Font435 also omits the restore, but Windows exits 0xc0000374 before the desktop: this is an invalid font-bank comparison, not a proposed fix. Evidence font435.json. Production mirror remains unchanged by these controls.

Font437 captured paging before/after BIOS writes: bank0 PTE 003d0e00->003d1e67 reads back 3c; bank1 000c3e00->000c3267 reads back 00 after writing 3c. The mappings differ, but no evidence yet proves why or compares guest direct writes in the same VM context. Evidence font437.json; this is not a runtime fix.

Font439 guest linear MOV control passes and samples both bank PTEs to RAM (0032ce67 and 0032fe67 in one observed VM); font437 BIOS bank1 PTE is 000c3267 and SAS type 2 is ROM. Original phy_w8 discards ROM writes. This establishes why that observed BIOS write does not persist but not why VMM chooses the mapping. ios.c defines VIRTUALISATION and outb invokes IOVirtualised: do not claim BIOS port operations simply bypass the VMM. Evidence font437.json and font439.json.

Font441 observes unchanged CR3 001de000 and CS:DS f000:0c56 across a bank1 BIOS store in one VM, so those particular before/after PTEs are from the same page-table root. Windows exits normally but the overall probe fails detached continuation state; do not treat it as a passing functional case or exclude changes to PTE entries. Evidence font441.json.

Font443 changes the five diagnostic font-plane index/data accesses to the same 16-bit OUT width used by the successful guest MOV control; bank1 still reads 00 after writing 3c. This rules out this width substitution as a sufficient fix, not all VMM state differences. Windows exits normally; the overall witness also fails detached continuation state. Evidence font443.json. No production video-source change.

Font444 completes the reverse width control: ordinary guest uses the same five byte OUT pairs yet reads both banks correctly (A 3c/3c, B 66/66), 320 checks and normal Windows exit. Together with font443, differing instruction width is not sufficient to explain the BIOS/guest result. Evidence font443.json and font444.json; original guest and production runtime unchanged.

Font447 test-only relocation of VGA byte helpers into witness code registered segment 0c56 and showed an early bank1 write/read 66/00, but runtime timed out without normal exit. This intervention is invalid as a conclusive location test; no claim that helper segment is irrelevant or safe. Evidence font447.json. No production or original guest change.

Font448 preloads opposite role values by real guest MOV, disables further direct stores, then invokes BIOS control loads: A expects 3c but final guest reads 66/00; B expects 66 but reads 3c/00. Both exit Windows normally, witness fails before identity as designed. Pre-touch return itself was not independently asserted (role restore changes flags); no claim beyond recorded bytes and BIOS-stage trace. Evidence font448.json. This shows immediate BIOS bank0 readback does not establish persistence into subsequent guest-visible mapping; both mappings need investigation.

Font451 observes mode restore invalidating both font-aperture PTEs: one sample bank0 0032ce07->0032ce00 and bank1 000c3267->000c3e00. This agrees with font448: data visible during the BIOS mode is not necessarily visible when the DOS VM remaps the aperture. The early witness includes an unmatched diagnostic marker and fails before identity validation; mode instrumentation and original guest remain unchanged. Why VMM selects ROM remains unproved. Evidence font451.json.

Font452 source comparison: pinned non-installed VDD reference explicitly distinguishes INT10 font mode from direct-programmed font mode and may map a null page during nested mode set or illegal page mappings. This is consistent with observed ROM/zero mapping and PTE invalidation, but is not evidence that installed V7VDD takes that branch or that C3000 is its null page. No reference source imported. Evidence font452.json.

Font454 queried _GetNulPageHandle then _PageGetSizeAddr in an isolated VxD; returned size ffffffff, EDX 80604000 translates to 0015f000. Because the special null handle may not be accepted by PageGetSizeAddr, this is inconclusive and cannot identify or exclude C3000. Normal Windows exit; early font witness still fails. Evidence font454.json.

Font458 maps the actual original VMM _GetNulPageHandle into an isolated allocated V86 page: linear 7000 translates to 7000 before and 15f000 after. Thus this VMM null handle's page is not the observed BIOS bank1 C3000 mapping. Return convention ffffffff remains unverified but the mapping change is directly observed; no original guest change. Early font witness still fails before identity. Evidence font458.json. Reference VDD null-page branch is not the cause established by this test.

CORRECTION font463: in the same Win3.1 instance _GetNulPageHandle returned 8000ce64, and the installed V7VDD immediately mapped exactly 8000ce64 into A4 before each failed bank1 store, with resulting PTE 000c3267. Therefore font458 cannot exclude a null mapping; its different observed physical page may reflect parameters/context or stale translation and the earlier exclusion is withdrawn. Hook observes original service calls only; offset/flags and physical discrepancy remain open. Windows exited normally; full witness fails early. Evidence font463.json.

Font465 same-instance observation: null handle 8000ce64 maps isolated page 7000 to 15f000; installed V7VDD requests handle 8000ce64 into page A4 with count1, offset0, flags0 in each failing DOS VM, yet PTE after the nested write is c3267 and readback 00. Hook observes request entry only: service result and timing are not established, and VM/context differ. Windows exits normally; early font witness fails before identity. Evidence font465.json.

Font469 observes raw PTE at _MapIntoV86 service return: same null handle/offset0/flags0 returns ffffffff with scratch system-VM page 7000 PTE 0015f207 but DOS font page A4 PTE 000c3207 immediately on return. The C3000 mapping is already present at service return, not a later PTE rewrite. Return hook forwards through existing chain using copied args; verify with a nonwrapped control before attributing implementation owner. Windows exits normally; full witness fails early. Evidence font469.json.

Font471 same-DOS-VM scratch mapping attempt returned 0 and no translation for linear7000 (PTE c3c26 not present); therefore it cannot compare the null backing page within that DOS VM. This is an inconclusive isolated control, not a product failure or proof of a mapping cause. Windows exits normally; early font witness fails. Evidence font471.json.

Directory472 failed before compilation because cmd.exe interpreted a forward-slash script path; directory473 uses the corrected path, completes cached x86 build, shared tests 14/14, mvdm-vm-notification 1/1 including the new ring0/result checks, and builds the authored insignia.386 candidate. New production candidate retains DOSWOWDATA by session/memory epochs, resolves current lpCDSBuffer with a short lease, and passes the address/sizeof(CDS) over existing BOP to VMM _AddInstanceItem. This is not yet guest qualification or publication. Diagnostic474 is being built for the strict dual-VM directory witness. Evidence and hashes pending.

Directory480 passes strict distinct-directory dual DOS-VM test with production dynamic DOSWOWDATA query, 18 guest ticks, 320 callback checks and normal Windows exit; original installation unchanged. Updated insignia candidate also checks zero-failure returns from both _AddInstanceItem calls. Scalar478 real session fixture passes registration-before-SCS, live far-pointer segment, session isolation and expired memory epoch; unrelated PIF branch is an aborting test-only substitute. Notification BOP unit test covers ring0 and missing registration. This proves only the pinned NTDOS diagnostic continuation case, not all layouts, native process behavior or Win95. Evidence directory480.json. Font mapping failure remains open; no publication.

Production-shaped isolated Win3.1 matrix481 passes std/386 x DOSONLY0/1, all four reach correct mode and exit normally with candidate host and insignia479; source installation hash checks pass. This is startup/exit evidence only, not editor interaction, full font fidelity, Win95 or Full. Evidence production481.json. No default publication.

Font483 pins installed V7VDD.386 SHA256 909a5e6c313daf4de4559171e211ef9ae7c5f15d10194f091a91be8a352f0275 and extracts its LE objects read-only. The original VMM mapping hook's immediate return address 80017052 is shared across different map handles, so it is a common dispatch boundary, not a proven V7VDD decision site. Evidence font483.json. No installed guest changes.

Editor484 fails before W31CHECK/desktop witness with runtime observer timeout; no menu assertions run. Installation root O:/tmp/m481editor484 is longer than previous passing O:/tmp/m481e321; this is an unproved difference, not a diagnosis. Original source untouched; previous processes ended. Evidence editor484.json. Editor487 repeats with prior 8.3-style short path.

Editor487 with short install path gets both editors, DOS File/Exit menu, but native Edit rather than File and times out; no normal exit. Cleanup found test EDIT PID36640 holding the isolated nthook64; its identity was verified and process closed. Long-path editor484 had previously timed out before guest witness; short path alone is not a fix. Old complete candidate editor490 is running as same-environment comparison. Evidence editor487.json.

Editor490 same-environment prior host/addons baseline passes both editor menus, both saved files, guest mouse Exit and normal Windows exit; evidence editor490.json. Editor491 was an invalid new Insignia/old host combination because old host lacks BOP operation6; it failed normal exit and cannot isolate regression. Editor492 now compares current host against prior ntvdm64.386 and Insignia.386. No candidate accepted.

CORRECTION: diagnostic474/package474 used by directory475/480 and editor487/492 accidentally interposes build-only ega_mode_probe432 font-mode experiment, not the original video implementation. The distinct-directory observations remain observations under that altered link, not production-video qualification; editor failures under package474 cannot be attributed to the production host candidate. Production481 used actual production NTVDM 9cf31b... and remains valid for four-mode startup/exit. Rebuild diagnostic with original video observer before relying on the directory or editor guest checks. User reproduction runs deployed NTVDM 4a0678... (same as r320-package) and installed O:/w31/PATCH/NTVDM64.386 d5f6b1..., different from published addon f9804f..., with no explicit Insignia entry in SYSTEM.INI. These are configuration facts, not yet a root-cause finding.

Directory498 supersedes directory480 for the original video implementation: dynamic DOSWOWDATA NetCDS location, strict two-VM directory/yield witness, 320 callback checks and normal Windows exit pass. This remains a diagnostic continuation with one NTDOS identity. Evidence artifacts/t481-s3/directory498.json.

Editors499-501 with original video observer pass real Win3.1 EDIT.EXE and EDIT.COM keyboard, menu, guest mouse Exit and saved-content checks in both exit orders; DOSONLY=1 guest EDIT and native rejection pass with normal exit. These are diagnostic-host observations, not Full or Win95 qualification. Evidence artifacts/t481-s3/editor499.json.

Owner requested immediate manual-test candidate publication. Package481 was staged with 11 host images and synchronized to assets/release and O:/winnt/system32 with manifest/hash verification. Authored ntvdm64.386 and insignia.386 were published to assets/release and installed in O:/w31/PATCH, with Insignia enabled in SYSTEM.INI; prior files/configuration were backed up under build/t481-s3/w31-install-backup502. Publication502 is a candidate: the owner's illegal-instruction dialog has not been reproduced or diagnosed, and Win95 and Full are not qualified. It does not close S3.

Owner reports actual MS-DOS Prompt immediately raises a Win3.1 invalid-instruction integrity dialog. Before candidate publication the installed host NTVDM was 4a0678... and NTVDM64.386 was d5f6b1..., without configured Insignia. Two isolated probes using those bytes saw the prompt but did not capture a #32770 dialog; startup-PIF probe exited normally, while post-desktop WinExec-PIF probe failed normal exit after its own WM_CLOSE. Dialog class matching and the latter close path are not validated against the user report. The report remains unresolved, not refuted by either probe. Evidence artifacts/t481-s3/prompt-regression502.json.

Owner reports manual validation on the newly published candidate: both EDIT.EXE and EDIT.COM work in the Win3.1 MS-DOS prompt. This is an owner observation, not a claim about untested fonts or Win95. Full503 runs against a fresh-identity copy of the published package; all PE package-identity resources necessarily differ from published binaries. The test must be evaluated as behavior of the identity-adjusted copy, not exact byte-identical package qualification.

Full503 on a fresh-identity candidate copy passed seven gates and failed version-negatives before execution: the direct test caller omitted OPENNT_BROKER_PRODUCT_BUILD, so fixture compilation searched the staged package for service.h instead of the actual service build tree. This is a caller-configuration failure, not a product pass; remaining gates did not run. Full504 reruns the entire suite with the existing successful service build directory. Evidence artifacts/t481-s3/full503.json.

Owner disposition: the earlier MS-DOS Prompt invalid-instruction dialog is attributed to the incomplete prior driver configuration. The current candidate installs and enables both authored drivers, and the owner manually verified EDIT.EXE and EDIT.COM in the prompt. Do not continue a separate investigation of that historical dialog; this disposition is the owner's determination, not a traced exception-level root-cause proof.

Full504 passes 16/16 gates on the complete candidate package with a fresh PE package identity to isolate the owner's active session. It is behavioral evidence for the identity-adjusted copy, not byte-identity proof for the published images. Previous full503 failed only due to test caller configuration; full504 re-ran the full suite with the correct build tree. Evidence artifacts/t481-s3/full504.json. Font bank cause and Win95 remain open.

Font508/509 read-only caller attribution: sampled runtime instruction bytes match the installed V7VDD object 1 at offsets 0x0f72 and 0x11f0 (run509 base 8001615c). At the null mapping, the observed V7VDD instance flags are 00400090, its ordinary page handle is 804023a8 and the requested handle is the VMM null handle 8000ce64. The actual instructions at offsets 0x0f9f-0x0fc4 select the null handle on a sign-marked page with instance bit 0x10; this narrows the observed path but does not establish why that state arose or equate it to a different VDD version's bit definitions. Windows exited normally; the early intentional font witness still fails. Test-only diagnostic BOP numbering conflict was corrected; original guest and production code were not changed. Evidence artifacts/t481-s3/font509.json.

Font509 follow-up disassembly of the actual installed V7VDD finds the observed 0x10 state set at object1:2a47 and cleared at 2adf; this path inspects client AH=11h at 2a7f, consistent with the video/font mode operation. The null-map routine checks a negative page entry and this bit before selecting the null handle. This links the symptom to the VDD mode-transition interval, but does not yet prove the upstream state cause or authorize bypassing the VM mapping. Evidence artifacts/t481-s3/font509.json.

Font510 attempts a narrow test-only direct guest comparison: bank0 scalar MOV, bank1 REP STOSB per glyph. The enhanced Windows desktop appeared, but the runtime observer timed out without normal exit; therefore no font or REP correctness conclusion can be drawn. It also is not the prior helper's REP MOVSB instruction. Original installation bytes remained unchanged. Evidence artifacts/t481-s3/font510.json. No product CCPU or guest-image modification followed.

Font511 repeats font510 with the same current package, addon, flags, isolated source and test path, but uses the original scalar MOV stores. It passes both banks to offset 8175 in two VMs, 320 callback checks and normal Windows exit; font510 REP STOSB variant timed out. This is a controlled difference, not proof of a CCPU defect or the prior REP MOVSB helper failure. Evidence artifacts/t481-s3/font511.json.

Correction font510/511: manual inspection of the original 86-byte A.BIN and B.BIN with the same offsets used by verify-win31-identity.ps1 proves font510 reached and passed the full font checks in both VMs (A 60/60, B 102/102, height16, final offset8175), each with 160 callbacks and three continuations, zero errors. Its overall run still fails because Windows did not exit before the observer deadline. Thus font510 does not show REP STOSB font writes fail; the exit timeout remains unclassified. Artifacts font510.json and font511.json corrected; raw output is retained.

Font512 uses the same current diagnostic host, authored addons, guest installation and strict witness as font511, but changes bank1 writes to REP MOVSB with a role-specific 16-byte source. Both VMs read both banks through offset8175, 320 callback checks and normal Windows exit pass. Therefore REP MOVSB itself is not sufficient to reproduce the previous nested helper's failure; the guest-issued versus nested-execution/mapping context remains distinct. Original installation unchanged; no production font-transfer acceptance. Evidence artifacts/t481-s3/font512.json.

Font514/515 compare A5000 mapping events using the same observer and host: the historical nested helper fails bank1 at offset4096; guest-issued REP MOVSB passes both banks in both VMs. Both observations contain null and ordinary handle mappings (nested 2/16, guest 14/8), so the mere presence of a null mapping is not sufficient to explain the failed font transfer. Events are not yet tied to the exact store, and addon implementations differ intentionally. Normal Windows exit and original installation hashes pass in both runs; font514 integration remains failed. Evidence artifacts/t481-s3/font515.json.

Font516/517 target the historical virtual writer at bank1 offset4096: immediately after REP MOVSB, the mapped A5000 physical page contains expected glyph data; later guest readback in the same CR3 resolves A5000 to a different zero page. Font518 guest direct write/read uses the same physical page and succeeds. Font519 shows omitting BIOS 1103 after writes avoids loss, and font520 preserves 1103 by moving it before virtual writes; same-valued banks pass both VMs with normal exit. BIOS-only reorder521 still fails. Candidate522 merges the virtual writer and ordering with current geometry checks and passes same-valued two-bank readback, but differing-bank control524/525 aborts before guest result (error995); 526 reproduces with original witness, so the candidate remains unqualified. Neither the product source nor published artifacts contain candidate522. Evidence artifacts/t481-s3/font526.json.

Font527 shows that changing both banks to the same alternate byte pattern also aborts before guest witness output; bank inequality alone is therefore not established as the cause. Font528/529/531/532 test-only instrumentation sees bank0 helper enter and copy completely, bank1 data copy and VMM Simulate_Far_Call return, and identical helper code bytes at both calls; the second helper entry is not seen before VM retirement. The exact VMM client frame and effects of instrumentation on flags/scheduling remain unproved. Candidate522 remains unqualified. Evidence artifacts/t481-s3/font532.json.

Font534/539 resolve the tested bank defect with the original VMM CX:EDX Simulate_Far_Call contract (historical helper had incorrectly supplied the target in EAX). Guest client EIP 0010 had confirmed the wrong target; correct offset 2000 enters the helper irrespective of glyph data. Apply BIOS 1103 font selection before virtual writes so subsequent bank mappings retain them; keep geometry validation. Formal VIDEO_DISTINCT_FONTS regression uses different bank data in each VM: A=3c/5a, B=66/a5, all glyph rows through offset8175, 320 callbacks and normal Windows exit. Original guest untouched. Source is integrated in NTVDM64.386, build538 is byte-identical to passing candidate534. EDIT.EXE/EDIT.COM both orders and DOSONLY1 pass editors535-537. Evidence artifacts/t481-s3/font539.json; geometry and Win95 still pending.

Resize540/543/547: the 80x50 visible-console witness fails before handing the requested geometry to the guest. A test-only diagnostic of the native fixture identifies SetConsoleWindowInfo returning ERROR_INVALID_PARAMETER (87); GetLargestConsoleWindowSize reports 106x31, with an 80x50 backing buffer. The previous driver reproduces the same failure, so this does not establish a regression in the new font driver. The Windows 3.1 desktop reached normal exit. Actual visible 80x50 guest handoff remains unverified on this desktop. The test harness now retains the process-scoped PowerShell execution policy when sanitizing its environment, so it can run its nested integration script under the documented per-process policy.
Resize548 uses the same current driver and isolated package with an 80x50 backing buffer and a 25-row visible window supported by the actual host. Both DOS VMs complete native 54:08/54:0A continuation, 320 identity/stack/register/flags checks and Win3.1 normal exit; original installation bytes remain unchanged. This verifies the supported geometry, not the unavailable 50-row-visible path.
Qualification549 stages the current 11 host images and driver538; Win3.1 std/386 x DOSONLY0/1 passes with normal exit. Full549 passes 16/16 gates using a fresh package identity (behavioral evidence for the staged package, not byte-identity proof for published files). Resize548 passes 80x50 backing with 25 visible rows; visible 50 rows is unverified because the host maximum is 31. Win95550 loads the current VxD but its current default DOS witness produces no VM output; Win95551 and 552 reproduce the same issue using a historical VxD and then both historical host and VxD. Win95553/554 substitute the identical previously successful DOS witness binary: both historical and current host/VxD combinations pass two concurrent VM identity and 320 callback checks. Win95554 observer exits with code 1, so normal Win95 exit and real input/output remain unproved. Original Win95 installation is unchanged. Evidence artifacts/t481-s3/qualification549.json.
Win95555/556 correct the old Win95 harness for the current DOS identity witness: fill the A/B role and PAIR.KEY placeholders, then validate its 72-byte result. Both VMs now write results: counted waits pass (3/0 in each), notification counters and IRQ checks pass, A reports successful cross-VM wait, but B reports one cross-VM assertion failure. Instrumented Win95556 records CF clear and ECX=1: the driver reports its own wait flag, while the probe's required VMM CB_VM_Status bit 0x400 is not set on this Win95 run. This does not demonstrate a lost wake: A resumed and passed; it also does not prove the VM was actually blocked at signal time. Win3.1 checked the 0x401 combination. The Win95 status-bit interpretation and actual blocked-wait coverage remain unproved; no product behavior or assertion was changed. Historical 44-byte witness passes but does not check this newer contract. Original Win95 installation hashes remain unchanged.
Win95557 observes complete VMM status 0x2 (background) while the driver waiting flag is set; no 0x400 bit was observed. Win95558 tests a separate observable-order contract: B verifies A has not produced its completion before the signal; A then completes, both counted waits, cross-VM order and PIC notification checks pass. It does not assert the kernel blocked bit or replace the Win3.1 0x401 test. The same test-only fixture is now in dos-identity.asm under WIN95_ORDERED_PROOF; its compiled bytes match the passing witness SHA-256. Win95 observer exits with code 1, so a normal guest shutdown and real Win95 command I/O remain unproved.
Win95559/561 attempt the existing two-VM REAL_COMMAND witness under Win95 using the Win3.1 diagnostic host. They time out before a command-submission event or result files. Win95562 removes diagnostic-only guest BOP markers and still times out; Win95563 uses guest DOS file stage markers and neither VM writes its entry marker. Win95564 repeats the same fixture under the historical host and VxD and has the same missing markers; original installation remains unchanged. This demonstrates the present fixture/setup has no validated Win95 command coverage and does not establish a regression in the candidate. No normal Win95 exit or real command I/O is claimed; the failure is not reclassified as a product success.
Win95566/568/569 padding controls show a 3033-byte DOS witness was blocked by Win95 conventional-memory configuration; 2048- and 2049-byte controls pass, so no 2048-byte threshold is asserted. The Win95 guest window displayed an insufficient conventional memory dialog. Win95573 sets 128 KiB minimum and 640 KiB maximum in both standard and Win386 sections of the test PIF, checks the PIF checksum, and both guest batches reach their DONE markers; their native stdout files remain empty. A temporary PowerShell assertion mistakenly treated null content as passing; that PASS is invalid. Win95574 fixes the assertion and fails. Win95575 shows the exact guest message "Incorrect DOS version" when invoking the native command. The OpenNT COMMAND.COM binary contains that message and its original init.asm requires an exact INT 21h GET_VERSION match, but the actual emitting image has not been identified; do not assert this as the cause of the empty output yet. The native command path has not been proved on Win95, and original installation files remain unchanged.
Candidate579 publishes the eleven-image package549 and its exact manifest to assets/release and O:/winnt/system32, installs driver538 NTVDM64.386 and insignia479 into O:/w31/PATCH, and records the Win95 VXD separately in addon-manifest.json. All destination hashes were checked after copy; O:/w31/SYSTEM.INI retained its pre-existing hash and addon configuration. Prior files are recoverable under build/t481-s3/publication-backup579. This publication is a user-test candidate, not S3 closure: Win95 real host-command output remains failing/unresolved and the full 50-visible-row path was unavailable on the current host. Evidence artifacts/t481-s3/publication579.json and qualification549.json.
Win95581/583 use two real interactive COMMAND.COM prompt VMs, not batch files. With the Win95 test PIF conventional-memory range set to 128-640 KiB, the existing guest input observer opens O:/WINNT/EDIT.EXE and EDIT.COM in distinct VMs, observes both File menus, closes both editors and receives two production command admissions and terminal completions. Formal test/integration/ntvdm64-386/verify-win95-editors.ps1 reproduces this on an isolated copy and checks distinct VM identities and trace; the host is the diagnostic real-command link. The observer process reports result=exited and exit=1, also seen in the earlier historical Win95 baseline; this does not prove normal shutdown. Win95 mouse menu, saved content, 0x400 blocked-state interpretation and exact production-host runtime remain outside this run. Earlier batch-test failure exercises a different COMMAND path and is not evidence that the interactive prompt fails. Artifact artifacts/t481-s3/win95-editors583.json.
Win95584 records the Win16 controller reaching its ExitWindows request after both editor completions, with no controller fatal or post-return marker; the host observer exits with code 1. Win95585/586 run an independent simple Win95 startup/ExitWindows controller without any project VxD: both the historical and current host packages also exit 1. The code is therefore not introduced by this addon or by the current host relative to that baseline. The historical meaning of exit 1 is still not established, so this is baseline-equivalence evidence rather than a claim that every Win95 shutdown is normal. Original installation hashes remain unchanged.
Win95587/589 mouse-menu attempts timed out: the Win95 DOS prompt contains a ttyGrab text child and a separate ToolbarWindow child, so the earlier Win3.1 parent-client calculation pointed at the toolbar; the original Win95 Welcome window also covered the target. Win95590 confirms child classes and rectangles. The formal Win95 test now selects the actual ttyGrab child and can close only an explicitly specified welcome title in the isolated guest copy. Win95592 and the formal repeat Win95593 pass two distinct prompt VMs, native and DOS editors, five guest mouse target receipts, both mouse-menu exits, saved contents aaa/bbb and two terminal completions. Win95593 uses driver538 and the diagnostic real-command host; original Win95 installation is byte-identical. Observer exit 1 matches both no-addon baseline controls; its original meaning remains unproved. Evidence artifacts/t481-s3/win95-editors593.json.
Win95594 repeats the 128-640 KiB Win95 DOS batch invocation with the historical host package and old VxD: both batches reach DONE, while both native CMD output files are empty, matching candidate Win95574. This is an existing observable batch-path limitation, not an S3 candidate regression. The interactive prompt path is independently verified by Win95593. The batch-path program classification and whether it should reach cmdExec remain an explicit S4 investigation; no conclusion or repair is asserted.

## S3 closure and S4 admission

S3 closes with the [fixed-scope evidence](../evidence/m0-t481-s3-closure.md),
complete candidate579 publication, four Win3.1 startup/exit combinations,
Full549 16/16 and separately scoped Win95 interactive editor qualification.
Candidate579 is available for owner hand testing; no T closure is claimed.

S4 now owns the original Win95 obligations in the stage table above: independently
verify VM lifetime/reuse and initialization rollback, input/presentation and
wait/wake; preserve original guest bytes. Also trace the reproduced native-CMD
batch failure against the historical baseline and the real guest entry path;
determine whether the admitted cmdExec contract reaches it, then either repair
the reached project-owned gap or explicitly retain the original/unsupported
boundary without inventing a guest modification. Preserve Win95593 interactive
prompt behavior, mouse menu and saved-content checks. Compare the observed Win95
exit code 1 with the original semantics before calling it normal; both historical
and current no-addon controls return 1. The diagnostic-host editor evidence
must not be described as exact production-host interactive proof. The absent
VMM 0x400 bit is a coverage boundary until its Win95 semantics is established;
ordered completion was independently verified. No broad new functionality is
admitted by these investigations.

S4 reuse001 uses the production candidate with a fresh isolated package identity: two concurrent Win95 VMs complete 320 callbacks; a third actually reuses an old VM handle with a new generation, rejects both stale wakes and consumes the new completion. Init002 injects a failure after IRQ reservation using a diagnostic host; the real guest closes its bridge and the host reacquires/releases the same IRQ and EOI hook. Editor003 reruns the shared fixture with keyboard, mouse menus and both saved contents. Original Win95 installation hashes remain unchanged. The Win95 observer exits with code 1; none of these runs proves normal shutdown or the VMM blocked-status bit. Evidence: artifacts/t481-s4/reuse001.json, init002.json and editor003.json.

S4 batch-entry source/trace classification: candidate Win95574 has zero diagnostic CmdDispatch service 7 (cmdCheckBinary), service 8 (cmdExec), and zero VM command admission/completion events 11/12; the DOS batches nonetheless reach DONE with empty native stdout. Win95577 records the isolated COMSPEC and its internal echo control succeeds; historical Win95594 also has empty native output. Thus this observed direct Win95 batch path does not exercise the interactive NT COMMAND prompt cmdExec32 entry covered by editor003. It remains an existing observable limitation, with the emitting image and precise root cause unproved; no guest mutation or new dispatch hook is justified by these observations. See artifacts/t481-s4/batch-entry.json.

S4 publication-check rehashes all eleven candidate579 images in both release and O:/winnt/system32, plus both authored W31 drivers in assets/release and O:/w31/PATCH; all match the recorded published hashes. No S4 production sources changed. The Win95 0x400 CB_VM_Status observation is returned to the diagnostic API and is not used to decide semaphore signaling or command wake; actual scheduling uses VMM Wait_Semaphore/Signal_Semaphore and the ordered cross-VM witness, but the Win95 meaning of that bit remains unproved. Evidence: artifacts/t481-s4/publication-check.json and reuse001.json.

S4 native001 and formal native002 use candidate549 without the diagnostic host interposition, reidentified solely for the isolated test. Two Win95 VMs each execute two native CMD commands through BOP54:08 and verify return values, registers, flags and stack; identity and 320 callbacks pass. This proves the production-host native command admission/completion path, not physical input or the BOP54:0A interactive shell. The original Win95 install is unchanged. Process exit 1 remains baseline-equivalent but not independently classified as normal. The formal test witness is byte-identical to native001 and the ordinary identity witness is unchanged by its marker macro. Evidence: artifacts/t481-s4/native002.json.

Owner-directed continuation: S7 is bounded closed as recorded in the
[closure](../evidence/m0-t481-s7-closure.md); its installer candidate and
delivery remain outstanding. S8 investigates and repairs the invisible
full-screen Win3.1 editor mouse pointer. Owner subsequently inserts S9 to
audit recent SoftPC software-emulation fixes against this project's actual
selected CCPU/device paths and import applicable corrections with provenance,
minimal mirror differences, and focused verification. Do not assume every
recent SoftPC commit is applicable or import unrelated changes. S10 researches
Win95 and completes the remaining applicable multitask and manual-acceptance
evidence after S9. S8 remains active; this is a sequence correction, not S9
admission or a waiver of outstanding delivery. The existing T481 integrated
delivery obligations remain in force.
