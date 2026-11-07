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
