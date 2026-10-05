# T431 S2 nthook32 implementation

Owner admits32-only implementation on2026-10-05, with CUI/GUI propagation
and exact component name nthook32-dll. [S1 conclusion](m0-t431-s1-hook-contract-conclusion.md)
retains source/API constraints and the deferred cross-width blocker.

Baseline: b33a4cece source; accepted T430 eight-file system32 package.
Other-session lifecycle proposal/TODO edits are preserved. Build working
root: build/M0-T431/S2/r001; validated formal cache remains
build/M0-T427/S2/r001. No new guest media or mirror edits.

## Source and recovery admission

Original OpenNT vdm.c classifier cohort remains selected unchanged through
its existing finite adapter. The historical full Kernel32/CSR process shell
cannot supply modern per-process interception without the excluded NT4
subsystem. The smallest selected installer is MIT Detours4.0.1 commit
e4bfd6b03e50de46b47abfbd1e46b384f0c5f833: detours.cpp, modules.cpp, disasm.cpp,
image.cpp, creatwth.cpp, uimports.cpp and their finite headers/notices.
Import exact bytes; no cross-width helper caller is selected by our code.
Record each imported file hash in the component manifest before linking.

The new project boundary is the specialist interception/context transaction:
actual suspended child, copied paths/recipient capability references and
existing run16/NTSRV authorization. It owns no worker/task registry, GUI
wait, presentation, observation or process-tree cleanup. Explicit caller
suspension stays intact. Native DLLs and unsupported/debug/security/width
cases are not silently redirected.

## Verification progress

The formal MSVC14.43/SDK22621 x86 /MT graph selects C++14 for this specialist
and the byte-exact Detours slice. NTVWM's common suspended creation path calls
the installer before existing BindNativeTarget/ResumeThread. run16 links only
the copied-context consumer; NTCON, NTVDM and NTMON do not acquire Hook logic.
APP0.0.427/RPC38/I/O25 are unchanged. The private copied header has version1.

Reproduction (from repository root):

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools/build/New-T310OriginalSoftpcNinja.ps1 -Architecture x86 -BuildRoot build/M0-T427/S2/r001 -NodeExecutable <pinned-node>
# Under the retained MSVC compiler.cmd environment, in the build cache:
ninja run16.exe ntsrv.exe ntcon.exe ntvdm.exe ntvwm.exe ntmon.exe VDMREDIR.dll nthook-install-test.exe nthook-gui-test.exe frontend-scope-lifetime-test.exe ntvwm-execution-lifetime-test.exe
build/M0-T427/S2/r001/nthook-install-test.exe
build/M0-T427/S2/r001/ntvwm-execution-lifetime-test.exe <fresh-build-report>
tests/observation/verify-native-hook-chain.ps1 -RuntimeRoot build/M0-T431/S2/r004-runtime -Observer build/M0-T427/S4/r049/console-startup-observer.exe -WindowObserver build/M0-T427/S4/r049/worker-window-snapshot.exe -LogRoot <fresh-build-log>
```

Run identities and actual results:

- r001/installer-negatives: **93 assertions PASS**. Context-only without DLL,
  paired non-inheritable recipient event references, CUI/GUI chains, actual
  SysWOW64 CMD, A API, explicit HANDLE_LIST with excluded inheritable event,
  Unicode environment, CWD and redirected output, caller suspension. Eight
  corrupt payload cases reject as INVALID_DATA rather than absence; invalid
  capability and missing DLL abort only a newly created unpublished child.
- r001/ntvwm-lifetime-final: **1077 checks PASS**, completed48/cancelled16,
  target-survival=yes and remaining-handles=0. No long-lived target pairing.
- r001/frontend-lifetime: all five retained production-scope assertions pass.
- r005-chain and r009-chain-wow: absolute/bare COMMAND, parent-output return,
  actual pinned run16 through Z: file identity all pass with real exit0 and
  current Console markers. r009 also passes Win16 startup-only receipt plus
  the actual retained localized WINMINE main-window identity.
- r006-product on sealed r004-runtime: Console17 and Window17 pass; independent
  WOW frontiers do not regress. Total199919ms, WOW66800ms, Console63476ms,
  Window64336ms (cleanup recorded separately). SOL/WRITE retain their known
  frontiers, not functional/gameplay acceptance.
- Final graph regeneration relinked generated base bindings and dependents.
  Immutable r010-runtime is the delivered coherent nine-file set. r011-full
  passes all required Product groups on that exact set: WOW67114ms,
  Console17 64161ms, Window17 68302ms; cleanup874/876/761ms. The **Full runner
  as a whole fails** its extra RPC group; do not label this Full PASS.
- r017-final-hook-chain repeats all five production chains on r010 and passes.
  Final93 installer, five frontend-scope assertions and1077 NTVWM lifecycle
  checks also pass after final graph regeneration.
- r014-controls passes client/bootstrap/startup-rejections/startup-timeout/
  native-worker-failure/native-completed-worker-loss and all six GUI routing
  cases: startup-only, explicit wait37, GUI child text, residency/survival,
  GUI-only management close, text→GUI→text result19/current marker.

Additional control limitations, kept as failures:

- r011 RPC bootstrap returned3 because the runner launched the service from
  the flat build cache, not its required system32 package layout. r012 is a
  fixture-only clone with the exact nine product hashes and test EXEs in
  system32. No production/package input or assertion changed.
- The startup-rejection fixture itself still built a flat provider substitute.
  The four-line fixture repair creates its system32 directory. r013 records
  the original failure; r014 proves the unchanged rejection/timeout/handle
  assertions at the correct layout. Broader Full-runner cache-layout repair
  is not claimed in this delivery.
- Monitor's zero-byte native-resume check expects NOT_READY21 but receives
  INVALID_STATE5023 before a completed DOS handoff. r014 preserves this failure;
  r015-control-baseline repeats the **same actual/expected/exit1** against
  unchanged T430/r007. No production regression or newly passing monitor gate
  is claimed. Keep that existing test-contract mismatch for subsequent review.
- One final NTVWM fixture invocation omitted its mandatory fresh report-path
  argument and returned usage2. The correctly parameterized invocation passes
 1077 checks; no test assertion or product source changed.

## Publication and delivery

Published r010-runtime to **O:/winnt/system32**, preserving the preexisting
coherent T430 eight files in build/M0-T431/S2/r016-publication/recovery.json
and recovery copies. No guest/config/NTVDM.REG content is replaced.
All nine deployed hashes match product-system32-manifest.json and
r016-publication/published.json. Hook SHA256:
`D345B52AEBCF417BF9DC8023C803EAFE2B42127DADB3AE8C0CE129B5C4FDA289`.
The byte-exact MIT notice is deployed as nthook32-LICENSE.txt.

Published ordinary COMMAND /c ver and real SysWOW64 CMD→COMMAND /c ver→parent
marker both pass with actual exit0. Original observation logs are in
O:/winnt/Logs2/t431-s2-{dos,hook}-published.txt; copies are in r016-publication.
Z: is removed; fixture-owned package resources are explicitly cleaned, not
counted as proof of ordinary retirement. No helper/host installation appears.

Final source/provenance, changed-file and ownership review is complete;
documentation governance, relative links and git diff --check pass. Preserve
the other-session proposal/TODO incoming hashes unchanged and exclude them
from this P. S2's32-bit implementation is delivered for owner verification;
T431 remains open and no next S is automatically admitted.

Earlier failed attempts are not passing evidence:

- The fixture's installer-like name triggered native Windows elevation
  heuristics before execution. Its test-only embedded asInvoker manifest
  corrected that; no host policy or production elevation was changed.
- The first actual-CMD descendant case returned exit10: blanket exclusion of
  extended startup attributes prevented payload delivery. Keeping the original
  attributes unchanged passed the same case, then the explicit HANDLE_LIST
  positive/negative checks. No caller attribute list is rebuilt.
- r007 Win16 observation sampled too early; r008 then wrongly expected the
  English Mines caption. Both fail the new test, despite actual startup0.
  r009 observes the actual UI frontier and reuses the existing UTF-16
  localized main-window identity; the product is unchanged between these runs.

## Review and bounded limitations

Source review: no MVDM/opennt-host mirror changes, new process/helper, Job,
service wire, frontend ownership or completion policy. The selected link map
contains no DetourCreateProcessWithDll/DetourProcessViaHelper creation caller.
Detours' local restoration metadata may retain helper-named state; that is
not helper process creation. Imported source hashes are pinned in
src/nthook32-dll/README.md and the generated nativeHookComposition manifest.

Bootstrap data is not authority. run16 still performs existing object/process
authentication at NTSRV. GUI carries no text capability; the actual launcher
receives context-only payload. Only creator-owned suspended/unreturned child
rollback is allowed; installer failure never resumes a partially edited image.
Original A/W creation and real child process/thread handles remain selected.

Not proved/accepted by these results: cross-width interception, alternate
token/logon/direct-system-call coverage, arbitrary parent/mitigation attributes,
different legacy argv[0], ambiguous null/bare API selection, Unicode package
paths not representable for Detours' ANSI import path, every concurrent/fault
scenario, or Win16 gameplay. Actual CMD bare COMMAND is supported by its
observed explicit application call, not a replacement Windows search engine.
Physical foreground/RDP/pointer observation is owner-waived, not tested.
T431 remains open; cross-width and broader S4 boundaries are not auto-admitted.

## Owner correction: shared application discovery (P2)

Question: why does actual x86 CMD reject `mem` while run16 already has the
required discovery/classification mechanism? Owner requires reuse, not a
second Hook-specific resolver or extension workaround.

Inputs: main cc3517e7d, sealed S2/r010-runtime, current x86 formal cache,
immutable baseline guest/configuration and SysWOW64 CMD. The original Hook
trace in r018-mem-diagnostic proves CMD supplies application
`Z:/system32/MEM.EXE` but command `mem  `. Original classification succeeds
as DOS; the Hook's subsequent argv[0]/basename equality rejects redirection.
The initial local extension workaround is discarded, not delivered.

Source recovery: reuse the complete existing T427 application-search body.
Move application_search.[ch] from run16-exe to common, rename only its exported
symbol, and link the same object in run16 and nthook32. A normalized comparison
against HEAD passes with only that symbol rename. CWD/PATH directory order,
COM/EXE/BAT/PIF precedence, explicit/drive-relative paths, Unicode, capacity
and failure behavior are unchanged. Both callers already select the same
original OpenNtBaseGetBinaryTypeW implementation; no new classifier/parser
or original mirror change is introduced. The Hook removes its separate
argv[0]/quoted-absolute eligibility policy, pins the resolved application and
preserves the original parameter tail. Native A/W creation, flags, environment,
startup attributes, suspended installation and rollback retain their owners.
Run16 CLI and shell fallback are unchanged.

Verification so far:

- x86 /MT CCPU40 graph regeneration and all affected product targets pass;
  sealed coherent candidate is S2/r027-runtime, with unchanged WOW32 reused.
- `application-search-test.exe .../S2/r026-search-fixture`: PASS, unchanged
  directory/suffix/explicit/drive/empty/Unicode/capacity assertions.
- `nthook-install-test.exe`: PASS, 93 assertions, including ANSI, inheritance,
  suspension, nested propagation, GUI and rollback negatives.
- `verify-native-hook-chain.ps1` against r027-runtime with the existing
  `MVDM_OBSERVER_SHORT_HISTORY=1` fixture: r028-hook-short-geometry passes all
  nine real CMD chains. Includes bare COMMAND, `mem`, `MEM.EXE`, absolute MEM,
  and ordered MEM output before `MEM-PARENT-RETURN`; actual exit0 and guest
  output are required, never just process creation. Z: and owned processes
  are cleaned in finally. This fixture geometry is also already selected by
  the standard Product runner, not a new relaxation of its assertions.

Retained failures/limits: default private-desktop geometry currently reaches
NTVDM but fails the frontend prepare-text boundary with error87, including
direct run16 MEM without Hook interception and the old package's COMMAND
comparison. Default observed host geometry is53x15 (buffer53x9001), unlike
the prior passing120x30 host. A temporary project-owned callback trace in
r024/r025-diagnostic locates prepare-text error87; the short-history fixture
gives command_ready error0 and real MEM output/exit0. All temporary diagnostic
source is removed before r027. No frontend or original DOS repair is bundled.
The complete default-geometry cause and arbitrary null-application API forms
are not proved by the focused CMD cases.

r029-product observes three WOW programs but cannot compare them because the
invocation supplied nonexistent baseline directory r011-product. This is a
failed prerequisite, not a WOW pass or product regression. r030-product uses
the actual retained r011-full baseline with unchanged assertions.

Final gate: r030-product PASS, same nine-file candidate throughout. WOW
frontiers68137ms, Console17 61528ms, Window17 65229ms; overall200764ms.
WINMINE remains an actual visible main window; SOL/WRITE retain their prior
known error-dialog frontiers, not functional passes. No reduced assertion or
timeout change. r032-final-hook-chains passes ten CMD chains and Win16
startup/UI, including an actual interactive CMD with observed input milestones:
fresh MEM output, MEM-INTERACTIVE-RETURN and exit0. Actual guest bytes and
parent output order are checked, not merely successful input submission.

Publication: r031-publication/publish.ps1 verifies the old P1 hashes before
replacement and preserves all nine files plus MIT notice in recovery/.
The nine deployed hashes match published.json at O:/winnt/system32;
guest/configuration/NTVDM.REG are untouched. Published CMD -> bare MEM ->
parent-return passes with actual exit0 and conventional-memory output. Exact
logs are retained in O:/winnt/Logs2 and copied to r031. No temporary trace is
selected by the final package. Source review retains the unchanged resolver
and original classifier, with no mirror, wire, frontend or worker lifecycle
changes. Documentation governance, links and diff checks pass. Other-session
proposal/TODO hashes remain unchanged and excluded. P2 is a repair delivery
awaiting owner verification at that delivery, not then an S2 or T431 closure.

## Owner-directed S2 closure

On2026-10-05 the owner explicitly requests S2 closure before personal
acceptance and admits S3 for64-bit Hook discussion. P2 commit12160c657 is
pushed; r027-runtime/r031-publication and tests above remain the unchanged
engineering baseline. S2 closes within its32-bit scope; personal verification
is not claimed. Default-geometry error87, unsupported API forms, cross-width
and remaining S4 coverage retain their limitations. T431 remains open. This
documentation transition changes no source or runtime package; no repeated
product testing or redeployment is claimed.
