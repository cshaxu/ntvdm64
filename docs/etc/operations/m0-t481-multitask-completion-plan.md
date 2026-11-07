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
