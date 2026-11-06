# T479 S5 cmdexec reuse correction

Owner asks to modify/reuse existing cmdexec instead of maintaining the new
parallel execution semantics. Baseline54996029a; preserve the pending nested
probe changes. This record is an implementation-boundary audit, not acceptance.

## Confirmed dependencies

| Original entry | Existing responsibility | Required disposition |
| --- | --- | --- |
| cmdExec, cmdexec.c:710 | Guest command/handles/environment capture, AH branch, COMSPEC validation/composition, default drive | One shared preparation implementation in cmdexec; ordinary and VM callers supply explicit input views. Remove duplicated service/COMSPEC decisions in mvdm_vm_command_capture. |
| cmdExecComspec32, cmdexec.c:659 | Interactive COMSPEC selection and service identity | Share that selection with the VM path rather than independently interpreting service10. |
| cmdCreateProcess, cmdexec.c:390 | Reentry count, encoding/environment conversion, handle binding, suspended creation, resume/wait/result and cleanup | Parameterize request-owned inputs/results; reuse the conversion/create ordering. Keep NTSRV-authenticated launch policy in the existing compatibility binding. |
| cmdExec32, cmdexec.c:571 | Shell-count/event-thread coordination, command-worker launch, GetNextVDMCommand, CF/AL and repeat state | Separate original worker-global coordination from per-VM scheduling; do not execute CPU register writes on a native completion thread. |
| cmdReturnExitCode, cmdexec.c:797 | Return nested DOS result, directory and redirection updates, next-command/reentry decision | Part of the same semantic family. Reusing only cmdExec32 cannot prove reentry. |
| cmdGetNextCmd, cmdmisc.c | Consumes IsRepeatCall/Exe32ActiveCount and the next VDM command | Must be reconciled with any real CF=1 producer before claiming original reentry. |

cmddata.c owns pCommand32,pEnv32,dwExitCode32,chDefaultDrive,
fSoftpcRedirectionOnShellOut and Exe32ActiveCount. The current capture object
belongs to a host session, not a VMM VM. Thread-local aliases alone would not
solve ownership across CPU admission, native execution and callback return.

## Approved refactoring direction

Owner allows one extracted implementation rather than requiring all logic to
remain physically inside cmdexec.c. Clear ownership and one implementation are
mandatory. Following the design report and original COMSPEC source check, the
owner approved implementation and semantic correction. The physical placement
preference below is superseded by that permission.

Use an explicit request context for copied inputs, resolved streams, execution
identity and result. Keep original wrappers for ordinary COMMAND entry. Extract
their shared command preparation inside the original cmdexec module, with
registered mirror edits, so VM entry calls the same implementation. A copied
VMM Client_Reg_Struc supplies the VM input view; never temporarily impersonate
its registers in the live ring-zero CCPU state.

Retain the minimal BOP/CCPU CALLF/VMM bridge for scheduling and identity only.
Retain continuation tokens, generation checks, provider lifetime and counted
wait/wake. They are new multitask infrastructure, not replacement command policy.
Keep the original cmdXformEnvironment and the existing authenticated Run16
creation binding. Do not add a generic callback framework or another launcher.

The execution split must preserve prepare-before-resume, handle ownership,
last-error capture and cancellation. The normal worker may still use its
original event-thread/GetNextVDMCommand coordination; the VM CPU cannot wait
there. Guest-visible completion must have one result mapping, not separate
invented CF/AL rules in the driver and a second command engine.

## Deletion and size accounting

Current candidate files are69 lines in mvdm_command_launch.c,49 in
mvdm_vm_execute.c,143 in mvdm_vm_command.c and180 in mvdm_vm_executor.c.
The first two contain118 lines eligible for consolidation into shared original
preparation/execution, but this is an upper bound, not a promised net reduction.
The143-line capture file includes indispensable VMM frame/mapping validation;
only its duplicate command-policy portion should disappear. The180-line job
owner cannot be deleted merely to reduce line count: lifetime/cancellation must
still have one owner. Actual mirror/autonomous added/deleted totals will be
measured from the implementation diff; no exact net estimate is proven yet.

## Reentry and failure evidence constraints

Current executor completion.reentry is always zero; the reentry=1 unit case
only proves transport. A real original reentry cycle also reaches service0B
and cmdGetNextCmd. Do not close S5 by testing only08/0A terminal exit codes.
The existing NTSRV original DOS command/reentry records must be traced to the
specific VM launch context before selecting how that cycle is reused. No new
semantic waiver or unconditional CF result is approved by this audit.

Run177 readback contains COMMAND's Cannot execute message. Source tracing finds
Exec_Err/Default_Message in guest command1.asm: this is the generic EXEC error
message, not proof of environment exhaustion or missing CF=1. Run178 returns
normally but the stronger MEM text assertion fails; A's saved text is blank
apart from its seed. These observations do not identify a single root cause.
Preserve both; do not replace them with the earlier exit0-only success claim.

Implementation must re-run ordinary DOS/native nesting, both real VM orders,
failure/retirement/reuse, exact CF/AL/client state, and actual nested output.
No original guest edit/rebuild, CPU-mode impersonation or singleton overlap is
an acceptable shortcut. Win95 stays deferred.

## Ownership proposal

- Keep cmdExec/cmdExecComspec32 as original guest ABI wrappers. Ordinary entry
  reads live CCPU inputs; VM entry reads the validated saved VMM client frame.
  Both supply explicit request data to one NTVDM-private command implementation.
- Share AH/service selection, COMSPEC validation/composition, original
  cmdXformEnvironment, encoding, stream resolution and launch preparation.
  Prefer directly composable cmdexec logic; where extraction is necessary,
  register provenance and remove the superseded body rather than retain two
  implementations. Use the existing command component, not a new library.
- Keep scheduling separate. Ordinary cmdExec32 retains original event-thread
  coordination and GetNextVDMCommand/reentry. The VM scheduler retains
  identity/generation, continuation and VMM wait/wake. Waiting for one VM must
  not set worker-global fBlock or stop the common event thread. Native
  completion threads never write CCPU registers.
- NTSRV owns task admission, associations and lifecycle. Reuse existing Run16
  binding and authenticated frontend/execution capabilities. The command core
  does not select workers, retry requests or independently terminate chains.
- Each request owns its copied inputs and duplicated launch resources. The
  scheduler owns its continuation; the existing frontend owns I/O. Required
  final I/O and retirement precede completion. VM invalidation rejects stale
  completion even if the numeric VM handle has already been reused.

Do not replace original guest reentry with a native process exit result.
cmdReturnExitCode/cmdGetNextCmd retain ownership of that cycle. CF=1 requires
an actual pending guest command for the correct execution context, not an
invented flag or a synthetic transport test. This proposal does not authorize
routing a required same-guest reentry to a different DOS instance.

## Additional source findings

OpenNtBaseServiceReenter and OpenNtBaseServiceGet in
src/ntsrv-exe/opennt/source/base_service.c select connection->console. VM
launch instead obtains its own frontend/execution pair. Sharing command code
does not make that original worker-global queue per-VM. Keep these contexts
explicit and prove actual routing before claiming nested acceptance.

The interactive binding in
src/ntvdm-exe/win32/command_process_compat.c formats Run16 command for
MVDM_COMMAND_SERVICE_INTERACTIVE. Service0A therefore does not itself prove
host cmd.exe consumed the test input; it can launch DOS COMMAND.COM. Do not
change that policy just to make the MEM assertion pass. Trace the EXEC failure
and verify the intended chain. This is source evidence, not runtime attribution
of runs177/178.

## Delivery plan

First consolidate preparation/request ownership while preserving ordinary
execution; then consolidate creation/result mechanics and slim the VM adapter;
then verify both ordinary and two-VM paths, real nested output, original
reentry, failure, retirement and reuse. Register mirror changes and report
actual added/deleted lines; do not promise an unmeasured net reduction.
Original guest source and binaries remain unchanged and are not rebuilt.
The original design update had no runtime change, build, test or publication.

## Original COMSPEC proof and first implementation

Pinned OpenNT base/mvdm/dos/v86/cmd/command/tcode.asm:1135 distinguishes
Do16BitPrompt from do32BitPrompt. Only the latter issues SVC_EXECCOMSPEC32 at
line1189. The service is10 in inc/cmdsvc.h. Original dos/command/cmdexec.c:500
reads the native process COMSPEC and passes it to cmdExec32. Original
dos/command/cmdenv.c:126 explicitly discards guest COMSPEC after retaining the
native process COMSPEC. Frontend placement cannot change this service contract.

The project command_process_compat.c interactive branch had instead formatted
Run16 command. It now quotes and forwards the captured host executable through
the same Run16/NTSRV binding. No guest changes, direct child bypass, new control
transport or worker-selection policy is introduced. Ordinary and VM callers
share this correction already; the remaining preparation refactor is pending.

Run179 cached Ninja rebuilt the launch fixture and binding. All65 per-request
launch assertions pass, including exact preservation of a selected executable
path containing spaces, independent requests, suspended start, handle lifetime
and failure rollback. The former fixture expected the wrong literal command;
it now checks the supplied executable instead. This unit substitutes the Run16
image with its own argument-checking child; it is not full Run16/guest evidence.
Run180 owns the production/diagnostic build and real dual-VM MEM/EXIT probe.
No new package has been published at this point.

Run180 finished: normal Win3.1 exit, unchanged installation and320 callback
checks pass, but actual MEM output fails. Both captured guest screens now
contain the native Windows CMD banner and per-VM prompt, followed by mem and
exit. A additionally reports Cannot execute followed by the staged MEM.EXE
path. This proves the interactive-target correction reached the real frontend;
it does not prove DOS nesting. Run181 stopped before product execution because
the package staging tool requires a repository-build root. Run182 repeats the
same candidate under the allowed shorter build/p182 root to distinguish path
sensitivity; do not treat the earlier generic EXEC message as a diagnosis.
build/p182 is a disposable S5-owned package, not a reusable build tree/cache;
it shares the active packet's evidence-retention and cleanup budget.

Run182 also fails the MEM output assertion. A's actual screen contains the
host CMD banner, its per-VM current directory, and Cannot execute
build/p182/system32/MEM.EXE. Win3.1 exits normally,
320 callback checks pass and original installation hashes remain unchanged.
Shortening the package path did not fix the failure; do not attribute it solely
to the earlier long staging path. The DOS EXEC error and command-chain context
remain to be traced. No full delivery or nested-execution acceptance is claimed.

## Shared preparation batch183

Ordinary cmdExec/cmdExecComspec32 and saved-VM capture now call the same
mvdm_command_prepare_comspec implementation in the existing command owner.
Its source is the pinned original cmdexec COMSPEC lookup, validation and /c
composition; the ordinary AH branch, capture order and scheduler remain with
their original owners. VM capture retains only its frame/address validation
and explicit service selection, not a separate COMSPEC validation algorithm.
This is the first preparation consolidation, not the complete cmdExec32
execution/request refactor.

Mirror exception MVDM-HOST-DIV-196 is extended for this bounded extraction:
cmdexec.c invokes the common helper rather than duplicating native COMSPEC
lookup and composition. Direct reuse of the whole original CPU-entry function
cannot accept a saved VMM frame without changing live CCPU state; the helper
is the finite shared input operation. Original mirror encoding/CRLF are kept.
The local mirror delta for this batch is13 additions/22 deletions (net-9).
Provenance remains OpenNT base/mvdm/dos/command/cmdexec.c; no original guest is
changed. The helper also rejects the exact buffer-full case, which the original
greater-than check would allow before writing a terminating NUL out of bounds.
It retains ERROR_BAD_ENVIRONMENT for rejected COMSPEC/command capacity.

Cached Ninja batch183 builds ntvdm.exe and the two command fixtures successfully.
107 capture assertions and65 launch assertions pass. New checks exercise bare
COMSPEC, /c composition, the final valid byte and the rejected next byte with
an adjacent guard. These are focused source/boundary tests, not actual guest
reentry or complete integration proof. The shared request/execution extraction,
actual nested DOS failure, full gates and publication remain unfinished.

## Nested MEM path controls184--192

Run184 uses the same candidate package without guest Windows: Run16 mem in a
short physical runtime prints actual MEM output. Run185 removes PATH and cannot
resolve the bare token; it is not an EXEC reproduction. Run186 uses the explicit
repository-package MEM path with a reduced environment and reproduces Cannot
execute. Controls187--190 selectively retain environment groups and all still
fail. Crucially, control191 retains the original environment and uses the same
explicit package MEM path: it also fails. Thus the environment hypothesis is
not supported by these controls. The helper runners' zero exits only indicate
observation completed; screen content determines the command outcome.

The guest witness environment embeds PATH=$PackageRoot/system32 before the
runtime matrix creates its short physical copy. This points DOS children back
at the repository path (including repos.hobby), not that short runtime. Original
BaseCheckVDM calls GetShortPathNameW and expects a DOS-addressable alias; merely
shortening the final package leaf did not remove the problematic path component.
This is a concrete fixture-path defect; these controls alone do not establish
the exact guest EXEC error number or the filesystem's short-name configuration.

Run192 uses an unchanged MEM.EXE copy in the short guest fixture PATCH directory
and puts that location first for the nested witness. It verifies copy hashes,
retains the real Run16/NTSRV/worker path and actual MEM-output assertion, and
does not change original installed media or add any product command special
case. It also rebuilds the diagnostic candidate from batch183's shared code.
Run192 completes successfully: actual MEM output returns to BOTH guest VMs
before EXIT, both08 AH forms and0A complete,320 callback checks pass and the
original installation hashes are unchanged. This is translated provider input,
not physical keyboard acceptance. DOS descendants follow the existing host
worker chain; this is not proof of CF=1 same-guest reentry.

The native short-path expansion of the failed repository MEM path returns
the identical build/p182/system32/MEM.EXE path, so
shortening the package leaf never supplied an alias for repos.hobby. The passed
short-fixture control, failed same-image repository-path controls and original
BaseCheckVDM alias dependency attribute the observed nested MEM fixture failure
to its DOS-incompatible PATH, not VM scheduling or the reduced environment.
No product workaround for arbitrary unsupported DOS paths is introduced.

Batch193 starts cached complete x86/x64 builds and shared-library tests for a
coherent checkpoint. Full product gates/publication and the remaining command
execution consolidation remain pending; S5 is not closed.


Checkpoint194 completes Full16/16 selected product gates and coherent ten-image
publication at both configured destinations. The external DOS4GW fixture is
explicitly skipped, not passed. Package2915fea5-c2ec-4242-8f6a-fbe152c8c4f2.
See the [checkpoint evidence](../evidence/m0-t479-s5-comspec-checkpoint.md).
S5 remains active for the shared execution/request and reentry audit.

## Shared execution text preparation198

The next bounded extraction moves OEM command conversion and the call to
original cmdXformEnvironment into ntvdm_command_prepare_text in the existing
command implementation. Ordinary cmdCreateProcess and VM launch preparation
both call it. It does not create, wait, select a worker, change guest state or
own a scheduler. NULL environment retains the original inherited-environment
case. Conversion errors retain RTL's DOS status; environment failure remains
ERROR_NOT_ENOUGH_MEMORY. Failed preparation clears both owned string outputs.

Original cmdCreateProcess no longer modifies its captured OEM command in place.
It passes an owned ANSI copy to the same CreateProcess binding; service/tail
identity still comes from the existing snapshot. The copy remains valid until
child completion and is freed alongside Env_A. Source inspection of original
cmdenv.c confirms failure paths destroy their private environment, while the
returned ANSI block is caller-owned. No environment algorithm is replaced.

MVDM-HOST-DIV-196 extends to this shared helper. Local cmdexec delta relative
to P3 is31 additions/70 deletions; both duplicate CreateProcess branches merge
without changing arguments or the surrounding standard-handle, count,
resume/wait/exit sequence. New command_text.h declares only this internal
preparation boundary. VM directory conversion and strict captured-block
validation remain VM admission responsibilities.

Cached batch198 builds the real worker and passes the guest-launch composition
unit with real RTL conversion, unchanged-source checks, NULL-environment
inheritance and failed-transform cleanup. Existing original cmdexec compiler
warnings also appear in batch183; they are not newly attributed failures.
Real two-VM witness199 is running. This is not yet a new publication or S5
closure. The original queue/reentry route still requires the explicit-VM audit.

Run199 fails the stronger output assertion: A contains MEM results but B only
contains its command and subsequent prompt. Callback/continuation state and
normal Windows exit pass, which is insufficient. The dependent release201
runner stops before fullbuild200 or publication. The P3 package remains deployed.

Control202 compiles cmdexec.c and mvdm_command_launch.c byte-for-byte from P3
5b7019e6a into explicit diagnostic-link objects without reverting the worktree.
Its first staging attempt fails because the control directory lacks VDMREDIR;
this is not a product execution result. After adding the unchanged library,
control203 executes the same real two-VM probe and fails MEM output in A.
Thus the symptom is not exclusive to the preparation extraction. This does not
prove the extraction harmless in every path or identify the output-loss cause.
No acceptance is granted by a successful rerun. Diagnostic204 records candidate
text at the actual commit boundary to distinguish missing child publication
from a later parent overwrite. It is isolated and not a publication input.

## Source-proven missing stream capability and startup correction

Trace205 reproduces B's missing output while recording every committed text
candidate: no B candidate ever contains MEM output. A normal candidate does.
This excludes a later committed-parent-frame overwrite for that observation.
Diagnostic206 shows B's host CMD receives MEMRC=0 despite absent output;
A's extended diagnostic input is corrupted, so that input extension is not
an accepted keyboard contract and is not retained as a production test.

Trace207's wrapper fails to compile because it named a nonexistent header;
208 corrects only the test declaration includes and executes. It reproduces B
failure and records worker33668, CONSOLE_IO_WRITE(operation1), ERROR_NOT_SUPPORTED
(50), twice. VM text still has no MEM-bearing candidate. Original
nt_graph.c::host_stream_io_update calls WriteConsoleA but ignores its return;
base/video/gfx_updt.c clears the dirty stream count after that call. The VM
frontend advertises CONSOLE_WINDOW_TEXT_FRAME_REQUIRED but implements cells
and frames, not native Console stream parsing. Original nt_reset.c enables
stream I/O at startup; the existing route adapter consults the frontend only
at a later software tick. Short tasks can write and finish in that gap.
This is a host capability-selection defect, not a guest defect or a missing
final render acknowledgement.

The correction belongs to mvdm_standalone_worker.c::begin_character_io. After
successful authenticated frontend setup, query the existing frame requirement
before original startup can enable stream I/O. When required, set the original
StreamIoSwitchOn switch false (the same switch consumed by original startup
and DOS reentry). Do not independently re-enable an option disabled by another
owner. Query failure follows existing startup cleanup. Native Console-mode
frontends retain their original stream optimization. No new renderer,
command-name test, worker-base policy, transport or guest modification is used.
The standalone adapter executes before the original runtime starts and after
session binding; the query does not invoke the readiness-deferring tick helper.
The added source seam is autonomous, with no new mirror edit for this fix.

Trace209 and a separate run210 without the extra I/O/text recording are the
required positive controls. Fullbuild211/release212 are sequenced after them;
failure prevents subsequent gates/publication. The P3 release remains deployed
until the new complete package passes. S5 remains active.

Run209 passes both actual MEM outputs and320 callback-state checks; its I/O
record contains no rejected stream-write operation. Run210 removes the extra
text/I/O wrappers and also passes both outputs and320 checks, normal Windows
exit and immutable-installation hashes. These are positive witnesses against
the retained199/203/205/208 failures, not exhaustive full-I/O acceptance.
[Hash evidence](../evidence/m0-t479-s5-text-preparation.json) records the inputs.
The sequential release212 runner remains live, first completing cached full
build211 and then the complete product gates. No new package is published yet.

Build211 and all16 selected Full212 gates pass. The verified normal ten-image
package6c79f45b-88bd-4359-a9c8-15e3a753457e is published at both targets.
[P4 delivery](../evidence/m0-t479-s5-text-preparation.md) retains the limits.

## Reentry ownership audit and stronger witness214

At P4, the original cmdExec32 sets CF=1 only when GetNextVDMCommand returns
CmdSize>0; cmdReturnExitCode repeats the same queue decision. BaseService Get
and Reenter select connection->console. This is execution-queue ownership,
not an attribute of where the text is drawn.

frontend_registry.c::OpenNtBaseServiceAcquireConsoleContext explicitly gives
an embedded frontend's owner a new logical console with worker_generation=0;
it does not inherit or mutate the enclosing payload worker's upstream console.
vm_host.c::ntvdm_vm_host_launch_context passes this capability to the host
launcher. BindConsoleContext fixes the authenticated child's console to that
context. CheckVDM reuses a DOS worker only if its original wait is pending;
otherwise it creates the original separate-session path. Consequently, a MEM
child alone cannot prove that the containing Win3.1 VM received CF=1.

The approved proposal requires independent downstream task chains and preserved
reentrancy. Before changing either queue authority or completion flags, witness214
extends the existing two-VM probe with `command /c cmd /c mem`, followed by EXIT.
Both actual MEM outputs and callback state remain mandatory. COMMAND and MEM
are same-hash copies in the short fixture path, not modified guest images.
This exercises DOS/native/DOS nesting beneath each independent VM frontend.
It still requires attribution of the actual reentry worker before resolving the
remaining CF=1 obligation; a passing screen or child exit alone is insufficient.
No runtime behavior is changed in this investigation. Earlier same-guest
reentry wording remains unresolved, not waived by this ownership observation.

Witness214 passes both actual command lines and MEM output, callback state and
normal Win3.1 exit. Diagnostic215 adds only append-only observation immediately
after the two original GetNextVDMCommand calls in a build-owned cmdexec copy.
It does not alter the queue, CmdSize or CF decision. The two downstream DOS
workers report:

| Worker PID | cmdExec32 CmdSize | cmdReturnExitCode CmdSize |
| --- | --- | --- |
| 29888 | 3 | 0 |
| 9924 | 3 | 0 |

Both actual VM outputs and all320 callback checks pass. This is a real original
CF=1 then CF=0 cycle in each downstream DOS worker, not the synthetic transport
unit. Run216 removes this extra decision observer and adds an exact nested
command-line assertion to the formal probe; its result is pending.

The ownership conclusion follows the approved independent-chain architecture:
the VMM caller has no pending original DOS queue in its new frontend context;
its native chain contains the DOS worker that actually consumes such a queue.
Returning CF=1 to the containing Win3.1 VM without a command for that VM would
fabricate a guest reentry. The earlier blanket requirement for a production
CF=1 producer in the VMM completion executor was an overbroad audit inference.
It is superseded by actual queue-owner attribution, not a waiver of reentrancy.
The original DOS worker still runs cmdExec32/cmdReturnExitCode unchanged around
the shared preparation. No extra per-VM command engine or upstream queue alias
is required by this route. Direct guest DOS execution remains guest-owned;
this does not add host-to-guest launch routing for arbitrary commands.

Run216 passes the exact nested command assertion, both MEM outputs, all320
callback checks and normal Windows exit without the extra decision observer.
[S5 closure](../evidence/m0-t479-s5-closure.md) records the resolved ownership
contract and mandatory S6 handoff; no production image changes after P4.
