# T433 S7 — COMMAND and mixed-width CMD batches

## Inputs and scope

Owner admits S7: add mixed-width BAT tests executed separately by COMMAND and
CMD. Baseline is S6 production35a38780a / closure2ec54eeea, sealed
build/M0-T433/S6/r011-final-runtime and the identical published system32 set.
APP433/RPC41/I/O25; six AMD64/four I386 images. Tests copy that unchanged
package to build/M0-T433/S7/r002-runtime. Original guest bytes remain immutable.
Actual DOS execution selects the existing I386 CCPU40 worker; native fixtures
are independently authored /MT MSVC14.43/SDK22621 x86/x64 CUI/GUI images.

Build-MixedBatchProbes.ps1 emits only build-owned fixture images, manifest and
compiler logs. verify-mixed-batch.ps1 uses existing owned Console/Window input
observers, actual SysWOW64/System32 CMD file-machine checks, a serialized global
BaseSrv endpoint and only Z:, removed in finally. Each BAT receives an eight-
character unique run directory; exact journals prove this invocation's sequence
and real native width/Hook, rather than an old prompt, scroll history or input
delivery. Native exits and DOS7 are tested by bounded ERRORLEVEL branches.

## Coverage and measured results

| Contract | Entrypoint and evidence | Result |
| --- | --- | --- |
| Mixed BAT and nested CALL from COMMAND/CMD32/CMD64 in Console/Window | verify-mixed-batch -Cases Mixed; r011-mixed-native | Six pass:16→32→64→16 and64→16→32→64, quoted two-word argument, native37/23/19 and DOS7, stdout/stderr, CALL parent continuation. |
| Native stdin, GUI waits/startup and missing target from both CMD widths in both modes | -Cases Native; same r011 | Four pass: actual stdin consumption, START /WAIT both GUI widths with37, START and default run16 return before held GUI exits, explicit run16 --wait37, missing-target failure. Test-owned events replace guessed delays. |
| Separate typeahead | -Cases Mixed -Typeahead; r013-typeahead | Six pass. Zero inter-command delay, milestone pacing disabled, identical exact journals and root completion checks. Not a claim that every per-character diagnostic delay is removed. |
| Journal oracle | test-batch-journal-witness.ps1 | Exact sequence passes; missing, duplicate, reordered, stale-run, wrong-width, missing-Hook and extra-line data are rejected (unit evidence only). |
| Shared checker integration | r017-witness-integration | COMMAND and AMD64 CMD Console mixed cases pass with the same exact predicate in batch_journal_witness.ps1. |
| CWD/PATH COM/EXE/BAT/PIF search | r008/r012 retain old failures; r031 and r044 test the repair | Six final cases pass: COM priority, CWD64, explicit launcher64, bare CALL/BAT, PIF7, PATH32 and exact shell missing-command1, followed by parent exit. |

The root completion contract is intentionally shell-specific: COMMAND's bare
EXIT after mixed/search retains the established1; its WOW startup-only branch
returns0, also proved on unchanged S6 in r045. Native CMD returns the final
native19. Those root statuses do not replace the independently checked
individual DOS/native results. No production assertion or guest is changed.

## New PIF recovery gap — not an original guest exemption

The authored standard PIF uses the existing original pif.h layout, names the
unchanged authored G7.COM and sets CloseOnExit. No installed/default PIF or
CONFIG/AUTOEXEC is modified. Direct run16/PIF returns7 and prints the actual
guest marker (pif-direct.txt). Nested COMMAND→run16→PIF reports1067; nested
COMMAND→native CMD→run16/PIF records PIFRC=1067 (r010-pif-native). Both native
CMD search cases mark FAILED after the PIF, then time out waiting to exit.

r014/r015 are explicitly diagnostic-only: the existing launcher_receipt_probe
composition links the actual production main/providers and adds outcome logging,
never a fabricated receipt or replacement PIF provider. It is never published.
The normal-package failures above precede this diagnostic. r016 records:

```text
pid=26024 stage=dos-result error=0 code=7 completed=1
pid=26024 stage=resume error=1067 code=0 completed=0
pid=24484 stage=receipt error=0 code=1067 completed=1
```

Thus DOS completion is already correctly7. The native-parent resume fails1067,
and run16's existing handoff-error path replaces its return with1067. This is
not an invalid guest exit or batch parser failure. The exact association/lifetime
cause still requires investigation. The existing NTSRV service_prepare_parent_resume
returns1067 for a failed/dead/missing recorded parent generation; its original
execution-origin authority must be examined, not bypassed. Do not suppress the
resume failure, insert sleeps or invent another parent registry.

The owner subsequently approves repair within S7. r019 diagnostic NTSRV proves
the recorded native parent is live (wait258), not failed, BUSY and bound to the
same root. Parent authentication succeeds; downstream native worker selection
rejects the launcher's completed independent-DOS Console identity. Thus the
earlier suspected dead/missing parent is ruled out.

The final fix is one project-owned assignment in service_prepare_parent_resume:
after DOS completion and exact live parent/root/BUSY validation, restore the
launcher's current console association from that native parent. The original
DOS worker/ConsoleRecord retain their separate identity. This restores coherent
input to existing retention, execution-context creation, queue delivery and I/O
confirmation paths, without relaxing their checks or adding state/protocol.
Original PIF new-Console and CloseOnExit behavior are untouched; no mirror diff.

r021's targeted old-provider fixture fails before the fix. A first incomplete
retention-only exception passes that fixture but leaves later queue delivery
inconsistent and fails the real r026 search case; it is removed, not retained.
r029 native service29 and r032 x86 service29 pass the final single-boundary fix,
including incomplete/foreign/stale/failed parent denials. r031 all16 strict
Mixed/Search/Native cases and r033 typeahead6 pass, including all formerly
failing PIF cases and parent input/exit continuation. Final package is
r030-final-runtime, changing only NTSRV from the S6 manifest.

Final r034 Full14 passes409388ms: Console17, Window17, three independent WOW
frontiers, RPC/GUI/version negatives, strict DIR, modern EDIT/cooked parent
return, rapid/typeahead relaunch, session isolation, retirement and nested
handoff. r043 adds three actual COMMAND/CMD32/CMD64→BAT→WINMINE startup-only
receipts and visible Mines UI frontiers while the owned desktop is alive.
r044 Search/Native10 retains strict journals and exact missing-command1;
run16's existing unclassified-command COMSPEC fallback, not file API error2,
owns this outcome. No CLI/search semantics are changed to satisfy the test.

r038 preserves coherent S6 recovery, publishes the final ten images/notice at
O:/winnt/system32 and checks all hashes; guest/configuration are unchanged.
r039 actual deployed DOS/32/64 smoke passes with matching ten-image hashes and
Z: removed. Final NTSRV hash:
C7B1CECC398FEA5F207D468DF319AF23554CFA061C848F520377D8286D021049.
r036 native import and r037 map/source review prove Windows AMD64 and the same35
selected service units. The nine other images retain S6 hashes. No diagnostic
image is published. Governance, relative links and diff/source review precede
commit/push. T433 stays open; S8 is not admitted.

Production P1 d1aa8336c3940e84879d6bc3c625a2ea5175ced1 is committed and pushed
after the measured gates and publication. Documentation-only P2 records S7
closure without another production change. Work remains stopped before S8;
T433 stays open for owner direction/acceptance.

r022's frontend-authority fixture also exposes an existing asynchronous
cleanup race: it calls IsEmpty/Stop before the process-exit callback has drained
the worker watch. That case never calls the changed retention function. The
unchanged S6 suite passes its comparison r023; source review identifies the
missing acknowledgement, not a proven production regression. The test now
waits on the existing configured worker-cleanup event in both affected branches,
retaining the exact IsEmpty/Stop assertions and eliminating its Sleep(1) retry.

## Retained harness attempts and limits

r003/r004 prove the initial shorter three-Console mixed chain. r006 uses a
template updated while the draft runner was active; the journal correctly has
the new16 entries while its loaded expectation has the old14. That attempt is
invalidated harness evidence, not a product failure. The final runner snapshots
all templates once and records them in inputs.json; r011/r013 use the frozen
source. An attempted simultaneous typeahead start is rejected by the existing-
broker ownership guard before launching any process; only the later serial r013
is product evidence. The first diagnostic compile lacks the native NT include
ingress; corrected include ordering and preserved last-error compile at r014.

The held GUI fixtures have GUI PE subsystem and real Windows process waits,
not gameplay/UI acceptance. r040 first assumes COMMAND/WOW bare EXIT is the
mixed-case1, but the observed0 and unchanged S6 r045 prove the correct distinct
contract. r042 observes after the private observer desktop is closed; that
test-owned observation failure is corrected by observing live in r043, not
by modifying WOW or accepting an absent window. r041's assumed missing-file2
fails against the established COMSPEC fallback; source inspection and final
exact1 assertions in r044 preserve that original product policy. None of these
failed harness assumptions is classified as a production repair.
Physical desktop/RDP acceptance or gameplay is not claimed. All temporary tests,
reports and diagnostic products are below build/, not the installed package.
