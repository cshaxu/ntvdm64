# S7 ordered repair progress

Baseline: f0df10cdbf7cd5842f4ee1a944915689f394af38. This is local review/progress,
historical entry notes followed by the final P1 receipt. S7 owns 20 files and 1,055 frozen audit points; P4 made
later semantic changes, so each file is remeasured at entry. Do not add snapshot
counts to current counts or equate reviewed files with delivered files.

## 1. dpmi32.c

Non-error differences: 5 points / 14 lines -> 4 / 12. Local reduction: 1 / 2.
R00113: removed two unused LDTR declarations; neither symbol is referenced in
this file. R00114: retain the early descriptor binder, with S6 actual FSTI
startup evidence. R00115: retain use of original DpmiGetFastBopEntry; its software
branch returns zero BX/DX/ES, while the original direct call requires the
hardware fast-BOP entry. R00116: replace i386 spelling with explicit _X86_ and
!CCPU selection; preserve error-only body, restore original logical lines
304/305/310. R00117: retain software CPU handler registration; P4/S6 real DOSX,
WOW and guest ownership tests establish the required boundary.

The preserved alternatives are necessary adapters, not permanent approval of
unrelated code. Current source review is complete; final candidate compile and
publication are pending with the next substantive whole-file batch. Changes are
unused declarations and an unselected kernel arm, not runtime algorithm changes.

## 2. dpmi32.h

Non-error differences unchanged: 7 points / 12 lines. No source change.
R00118 selects software state declarations matching data.c and stack.c's
BeginUseLockedPMStack/EndUseLockedPMStack, rather than native kernel VDM TIB
macros. R00119/R00120 avoid redefining the x86 SDK LDT_ENTRY tag while retaining
the original fallback for other architectures. R00121 prevents buffer.c and
selector queries from executing host LSL against guest selectors.
R00122 retains FlushSelectorCache: beyond the excluded JIT loop it reloads
CS/DS/ES/FS/GS/SS after descriptor changes, called by int31.c and dpmiselr.c.
R00123 retains GetDescriptorMapping for software DIB mappings; R00124 declares
the actual FlushSelectorCache provider instead of the unavailable kernel LDT
setter. Native hardware macro bodies are unchanged original source, not newly
implemented alternatives. These seven residual adapters remain registered for
owner review; no permanent approval is inferred from this audit.

## 3. dpmiint.c — entered, incomplete

Current entry: 44 points / 317 lines (261 added, 56 deleted), not the frozen
40-point snapshot. Preserve P4's three-entry DOSX ownership and failure-state
contract. BuildStackFrameWithWidth duplicates stack.c's descriptor/limit logic;
its explicit frame width is necessary for interrupt gates independent of the
application width, but a second algorithm is not. Review the complete file and
coupled original stack owner before editing or building. No changes to this
file have been made at this checkpoint.

## Totals

2/19 files reviewed; 0/19 delivered in S7. One point restored locally, unverified
in this batch; residual decisions retain exact IDs. Current local non-error
reduction is 1 point / 2 lines. The previous S6 delivery separately reduced
1 point / 1 line and remains committed. Migration remains 39/159.
## S7 P1 whole-file candidate

The ordered review now covers dpmi32.c, dpmi32.h, dpmiint.c and required stack.c.
stack.c transfers from S8 (zero frozen points) to S7: S7 has 20 files, still
1,055 frozen points; S8 has 851 files. No file is counted twice. Four files are
locally reviewed; no S7 file is delivered at this checkpoint.

The shared original stack.c calculator receives explicit frame width. The
original BuildStackFrame ABI delegates with CurrentAppFlags; interrupt gates
call BuildStackFrameWithWidth directly. CurrentAppFlags is never temporarily
mutated. Failed bounds or null VdmMapFlat aliases return FALSE before writing
outputs; callers retain P4's CPU-state rollback. Test187 covers eight frame,
stack and application-width combinations, original wrapper, underflow and a
real inaccessible memory alias. Original guest binaries remain unchanged.

### dpmiint.c point dispositions

- R00125: retain one original TIB owner and narrow printer view; the provider
  exposes existing fields and operation, not a second state store. Its native
  TIB/thread/printer regression is in ccpu_monitor_boundary_fixture case184.
- R00126/R00130/R00141/R00142/R00143: explicit software-backend selection;
  native kernel registration/return bodies remain excluded. Original branch
  pairing now supplies #line at each of eight guard/end boundaries.
- R00127/R00129/R00138/R00140/R00147/R00149/R00150/R00151: restore original
  matching whitespace/end comments; no runtime policy added.
- R00128: retain 32-bit exception frame flags at +8: IP/CS/FLAGS occupy 0/4/8;
  original +4 overwrites CS. This is a concrete frame-layout correction.
- R00131-R00137/R00152: remove local copied calculator and its forward
  declaration, use the original stack owner with explicit gate width. Retain
  current stack-pointer commit ordering; application width does not select
  interrupt-gate frame width.
- R00139/R00144-R00146/R00148 and later P4 deltas: retain unified DOSX ownership,
  debugger/real-mode handling, prevalidation and failure rollback. S5 P4 and
  S6 final-package guest/WOW tests establish these boundaries; current candidate
  must rerun them because the shared stack calculation changed.
- R00153-R00164: retain bounded nonfaulting CS/page fetch, prefixes, CLI/STI,
  scalar I/O and existing CR0/segment compatibility. Real NESticle establishes
  the privileged-instruction requirement; scalar/decode/IRQ/TF tests establish
  widths, rejected side effects and completion ordering. No kernel monitor
  decoder or unrelated instruction family is restored.

New residual differences: dpmi32.h's shared-calculator declaration and stack.c's
width parameter/wrapper/failed-output contract remain registered, pending
owner review. They replace the 83-line copy rather than move it unchanged.

Non-error candidate metrics: dpmi32.c 5/14 -> 4/12; dpmi32.h 7/12 -> 8/15;
dpmiint.c 44/317 -> 35/222; stack.c 0/0 -> 4/18. Batch total 56/343 -> 51/267,
net -5 points/-76 lines. #line metadata is tracked separately. Cached build
is running; none of these local results is a delivery claim.

[Current candidate mirror measurements](s7-p1-mirror-diff.json) and [remaining decision register](pending-decisions.csv) track the final local code; runtime delivery is still pending. The latter preserves evidence-backed residuals, not blanket retention approval.

## Final P1 verification receipt

The preceding candidate checkpoints are historical. All four reviewed files
now passed the final published-package checks. S7 coverage is 4/20 reviewed,
4/20 final-verified pending commit/push; sixteen remain, next dpmiselr.c.
Frozen snapshot coverage stays 1,055 points; migration stays 39/159.

Cached full build and 61 CCPU tests passed (build/t485-s7/build3.log,
unit-final3.log, build-unit-result.txt). The final immutable package is
build/t485-s7/runtime1, with 13 images. Dual publication to assets/release and
O:/WINNT/SYSTEM32 was hash verified before runtime tests. Owned modern-pwsh
PID19644 completed with final/targeted-result.txt PASS.

Real WINMINE, Win3.1 standard/enhanced desktops, Win95 desktop, enhanced/Win95
strict Alt+Enter round trips and real NESticle menu/Escape all passed.
Win95 retains its admitted exit1 boundary; this does not claim normal exit0.
Original guest binaries were not modified or rebuilt. This is targeted
verification; S5 P4 Full23/23 remains historical rather than a new Full claim.

The unsupported kernel preprocessing actually failed at logical line305 with
MVDM-HOST-DIV-619. Its initial result checker had a Select-String argument error;
the corrected checker reads the existing compiler rejection rather than
repeating the build.

Final non-error mirror measurements:
dpmi32.c 5/14 -> 4/12; dpmi32.h 7/12 -> 8/15;
dpmiint.c 44/317 -> 35/222; stack.c 0/0 -> 4/18.
Batch 56/343 -> 51/267: reduction five points/76 lines.
Together with S6, reduction six points/77 lines. These are upstream-relative
diff measurements, not source LOC. Diagnostic #line changes are separate.

Self-review: one original stack algorithm replaces the copied implementation;
original caller ABI and width selection remain. Failure publishes no outputs.
No queue, second state machine, outer dependency or guest change was added.
Residual adaptations remain registered for owner review; verified minimization
does not grant blanket approval to retain them.
## S7 P2: dpmiselr.c complete local review, verification running

Entry 12 points/61 non-error lines; candidate 7 points/56 lines (-5/-5), including three exact provider prototypes.
P1 remains four delivered files; this fifth file is local and not delivered.
R00173/R00174/R00177/R00183 retain original emulator bodies under CCPU:
host x86 ABI does not provide the native NT kernel VDM descriptor service.
R00175/R00184 retain the descriptor projections and DOSX binding. Original
Ldt is a DOSX free-list/construction image, not an architectural process LDT;
the latter starts empty so free links cannot masquerade as present descriptors.
GDT0040 is independent of LDT0040 (S6 case186 and real WINMINE evidence).
The paired shadow allocation is one ExtMemSA allocation; failure returns before
publishing either address and clears LDTR. No new allocator or owner is added.

R00176 restores the original kernel-publication condition boundary and
error-only unsupported body; logical original lines226/231 are preserved.
R00178/R00179/R00181/R00182 remove the extra CCPU guards on the original
declaration and VdmFlushCache loop. sim32.c's existing provider already
selects its CCPU/non-CCPU implementation; disabling the call at this caller
has no established correctness justification.
R00180 restores original VdmTraceEvent. dbg.c supplies it and gates recording
on the registered debugger provider and VDM_TRACE_HISTORY. Suppressing it with
the release DBGTRACE macro was not evidence of necessity.

The local measurements are build/t485-s7/dpmiselr-mirror-diff.json, using
pinned XP blob e8641305844f909e98746e223cd577272c6437b6. Newly restored
error-only kernel body is masked against its original paired body, never
counted as removed product functionality. Build/unit runner PID20512 owns
build/t485-s7/p2; no runtime or delivery pass is claimed yet.
The first build and 61 cases passed, but C4013 exposed implicit declarations
for VdmFlushCache and the two existing LDTR calls. Their actual definitions
are sim32.c and cvidc/accessfn.c (the latter delegates to c_reg.c); exact
BOOL/VOID/int and argument prototypes are now declared at this caller.
No algorithm, wrapper or runtime path is added. Final-code rerun PID20296
supersedes the first build for acceptance; earlier logs are retained as
build-before-prototypes.log and unit-before-prototypes.log.
Final local measurement is 7 points/56 lines, reduction five points/five lines.
All three implicit-declaration warnings disappeared in the final semantic
build; 61 cases passed and runtime2 was dual-published. Its real guest tests
are running. Final line-mapping audit additionally restores original logical
lines36,482,484 around the local declarations and descriptor binder.
This changes no selected tokens or behavior. After runtime2 tests complete,
rebuild and republish that metadata-only correction; owner authorization
permits retaining the runtime2 behavioral results without another runtime
cycle for line-only changes. Record this distinction explicitly.

[Current P2 measurements](s7-p2-mirror-diff.json) exclude diagnostic metadata.
## S7 P2 final verification receipt

Runtime2 passed all 61 CPU cases, real WINMINE, Win3.1 standard/enhanced,
Win95 strict desktop/Alt+Enter round trips and DOS4GW NESticle menu/Escape.
Win95 exit1 remains its approved boundary, not exit0 acceptance.
Only diagnostic #line directives changed afterward; final cached build and
dual publication produced runtime3. Its 13 images were hash checked at both
destinations. Runtime2 behavior results are reused only for this line-only
change under the owner's explicit authorization. No guest binary changed.

Unsupported native-kernel preprocessing rejects at original logical line227.
The initial copied response file still named dpmi32.c; the corrected response
names dpmiselr.c and produced the required rejection. No failed experiment is
counted as product verification.

P2 reduces 12/61 to 7/56, five points/five lines.
S7 now has five whole-file reviews and final-verified P deliveries out of20;
fifteen remain, next dpmi32/i386/dpmi386.c. Cumulative S7 reduction10/81;
including S6 reduction11/82. This record's P commit/push completes delivery.
The frozen audit population remains1055 and migration remains39/159.
## S7 next file: i386/dpmi386.c

Entry: two non-error points/219 lines. Original XP blob
2d55fb7da5272e46bc20f8e53580b244e95b7eea has no conditional framework around
the four removed functions. R00185's old blanket instruction to restore an
excluded framework is incorrect; no invented guard/error branch is added.

A complete original-function restoration was compiled in the main worktree
with the actual CCPU product response file (owned PID19728).
build/t485-s7/p3/restore.log and restore-exit.txt record exit2:
PPROCESS_LDT_INFORMATION, VDMSET_LDT_ENTRIES_DATA and
VDMSET_PROCESS_LDT_INFO_DATA are absent from the current admitted header set.
This is a concrete current restoration obstacle, not proof that these original
types or algorithms are wrong. Restoring that unused native-kernel function
would require a separately reviewed original header closure.

The three removed pointer providers have actual duplicate owners in the
published ntvdm.exe map: Sim32pGetVDMPointer, ExpSim32GetVDMPointer and
VdmMapFlat are provided by original-softpc-host-roots:sim32.obj.
Its selected software mappings handle IntelBase and page translation;
the hardware implementations assume native identity-addressed VDM memory.
They cannot simply be restored alongside that owner.

R00186 reads the actual emulated debug registers through the existing CPU
provider; the original ThreadGetDebugContext does not describe that CPU.
int31.c consumes this result for hardware-breakpoint operations; the existing
setter also binds the CPU owner, without introducing another register store.

The failed restoration was precisely withdrawn. Only the original nt_vdd.h
include was restored; precomp.h already includes the same guarded header, so
this is nonbehavioral and accumulates until the next substantive whole-file
batch. Local result remains two points, now218 lines (-0/-1).
S7 remains five delivered files; this sixth file has source findings and a
local include-only change, not a delivery claim. Native header restoration
is explicitly pending review with the concrete compilation evidence above.
## S7 P3: modesw.c whole-file review and dpmi386.c include restoration

modesw.c entry18 points/53 non-error lines -> candidate16/44 (-2/-9).
Together with dpmi386.c's include restoration the candidate reduces2/10.
Five files remain delivered; this batch has two locally reviewed files.

R00204/R00205/R00207 retain the existing CPU declarations, WOW page-domain
exit and real-mode segment-cache reset. The original single SS limit write
cannot reset CCPU CPL and DS/ES/FS/GS caches after the PE transition.
The selected providers are c_reg/c_seg and the existing page-domain owner,
not a second mode implementation or copied session state.

R00206/R00208/R00210/R00211 restore XP's software-first branch ordering.
The unsupported kernel bodies remain error-only at original lines110/172;
the caller CCPU predicate stays because host ABI does not supply kernel VDM.
The object-contracts generator previously enforced reversed order; it now
extracts the actual software-first branches. Positive CCPU and negative native
kernel preprocessing assertions are retained rather than waived.

R00209/R00212 retain the proved RF/NT-clear-before-PE ordering, covered by
pm_entry_contract_fixture's131074 boundary cases (not a CPU execution claim).
R00213 preserves DOSX GDT/IDT ownership and final descriptor selection, with
the existing shared DpmiCpu40GetDosxIdtDescriptor provider and S6/P1/P2 real
guest evidence. No fixed WOW selector is used to guess interrupt ownership.
R00214-R00220 preserve PE-before-selector and SS-before-ESP ordering plus
the existing WOW reentry boundary. Original blank separation is restored;
logical line anchors return unchanged source to its original positions.
R00221 retains selection of the original emulator callback IOPL preservation.
The remaining callback/reflection functions are otherwise original;
no unrelated failure policy or instruction support is introduced.

Build runner PID21360 owns build/t485-s7/p3. Planned final gates are cached
full build, selected/rejected guard profiles, PM entry contract and existing
CCPU cases, followed by final-package targeted verification. No pass or
publication is claimed at this local checkpoint.
P3 first build stopped in object-contracts configuration, before product
compilation: MATCHALL stored real C bodies containing semicolons in a CMake
list and falsely rejected the branch count. The generator now matches each
original else location109/171 separately; it neither changes selected code
nor removes a negative assertion. build-generator-failure.log is retained.
Corrected build PID9408 and dependent publish/runtime PID23924 supersede
the failed runners. No product or runtime failure is inferred from the
generator error.
## S7 P3 final-package receipt

The corrected cached full build, two selected/two rejected mode guards,
PM-entry contract and61 CPU cases passed. The final runtime4 package was
published and13 image hashes verified at assets/release and O:/WINNT/SYSTEM32.
Real WINMINE, Win3.1 standard/enhanced, Win95 and DOS4GW NESticle passed.
Both enhanced/Win95 round-trip witnesses assert window input, fullscreen text,
return input, second fullscreen and same window. Win95 exit1 remains the
approved boundary, not a normal-exit0 claim.

P3 contains dpmi386.c's redundant original header restoration and modesw.c's
original software-first conditions/diagnostic mappings. Selected runtime
operations are unchanged. The generator parses each original else boundary
individually, avoiding CMake list splitting; no assertion was removed.
Original guest binaries are untouched.

[Final measurements](s7-p3-mirror-diff.json):20points/272lines ->18/262,
net reduction2points/10lines. S7 cumulative reduction12/91; including S6,
13/92. Seven files have source conclusions and final verified deliveries
with registered residuals, out of20;13 remain. dpmi386.c's native-header
restoration obstacle remains explicitly in pending decisions, not silently
declared upstream-invalid. Commit/push of this P completes delivery.
Next ordered file is inc/wowusr.h. Migration remains39/159.
## S7 P4 final-package receipt

inc/wowusr.h, HXP-R-00272: removed two unconsumed added parameter-frame
types. Actual wuman.c and wutbl2.h consumers use the distinct original
WOWWORDBREAKPROC16. Retained code now matches pinned XP SP1 blob
0396ec944eeb468252b419449d1fae36bf685edd; all47 conditional arms retain
original order, and41 prior excluded bodies remain error-only. Restored
upstream newline bytes separately, without counting them as semantic reduction.

Every exclusion's error has its original logical line; its restoring #line
is after #endif, so the selected path executes it. All47 guard logical lines
and41 restoring-anchor positions passed the whole-file comparison in
build/t485-s7/p4/review.py. The first candidate put restoring anchors inside
excluded arms; final review corrected this before commit. Its runtime5
acceptance is superseded by the rebuilt final runtime6 package.

Final evidence: build/t485-s7/p4b/build-exit.txt=0, final/publish-result.txt,
final/wow-result.txt and verification-result.txt=PASS. Cached full build,
dual publication and all13 image hashes at assets/release and
O:/WINNT/SYSTEM32 passed. The checked-in observe-wow-frontiers runner
proved a live visible real WINMINE window through a helper-owned runtime.
No execution algorithm or original guest changed. P3 DPMI/desktop/DOS4GW
sources remain unchanged, so those unrelated tests were not repeated.
This is targeted verification, not Full.

[Final measurement](s7-p4-mirror-diff.json):1point/15lines ->0/0.
Remaining differences are only existing exclusions and diagnostic anchors.
No pending decision existed for R00272; its restoration is in the main ledger.
S7:8/20 files final-verified for delivery,12remaining; reduction13points/106lines.
Including S6:14points/107lines. Commit/push completes P4. Migration39/159.
Next ordered file:softpc.new/base/bios/reset.c.

## S7 P5 reset.c local whole-file checkpoint

HXP-R-00408 through R00498 comprise91 frozen points. Current paired
non-error metric is91points/802lines (400added/402deleted). Restored
original whitespace/comments and both original IP adjustment statements.
The only residuals are original #ifndef CPU_30_STYLE narrowed to
#if !defined(CPU_30_STYLE) && !defined(CPU_40_STYLE), at XP lines782/846.
Result2points/4lines, reduction89points/798lines. No new condition nesting.
47 existing excluded bodies are retained by reviewed whole-body pairing;
nested directives inside those bodies are not mistakenly paired twice.
Original line anchors are restored across retained conditional boundaries.

Necessity: guest dxutil.asm enrm20 pushes offset enrm50 and enrm31 stores
offset enrm45, not an address before those labels. c_main.c's C4 and D6 BOP
paths UPDATE_INTEL_IP before calling bop, then CANCEL_HOST_IP rather than
adding another instruction length. Its next fetch therefore uses the callback's
exact setIP. The global HOST_BOP_IP_FUDGE=-2 cannot be applied to these two
CPU40 direct continuations. Original source algorithms and guest stay intact.
Residuals R00447/R00451 are registered, not approved-away. The second was
inside a larger formatting hunk; its partial restoration is not a wholly
restored point. Source contract evidence is not a claim of real runtime hits.

Candidate and reproducible original-relative measure are under
build/t485-s7/p5/review.py, reset-before.json and reset-after.json. The
baseline is pinned to e50cd23d4, source blob50bc67377477675d2670fc3387ac8863b2e01f43.
Background build/publish/targeted startup runner PID6736 is recorded in
build/t485-s7/p5/run.json. No P5 delivery is claimed yet. S7 delivered8/20,
reduction13/106; this local candidate adds89/798, making prospective102/904.

### S7 P5 final-package delivery receipt

The same final runtime7 passed cached full build, dual publication and
13-image hash verification. Real WINMINE, Win3.1 standard and enhanced,
Win95 and DOS4GW/NESticle passed. Both enhanced/Win95 test invocations
include the strict Alt+Enter round trip; NESticle has actual menu pixels
and Escape hide/restore witnesses. Win95 normal-exit=False is the previously
approved exit boundary, not a claim of exit0. These startup gates do not
prove actual execution of both rare CMOS-reset continuations; their retained
CPU40 predicate is justified by the explicit caller/provider source contract.
Evidence is build/t485-s7/p5/final, verification-result.txt and run.json.

[Final measure](s7-p5-mirror-diff.json):91points/802lines ->2points/4lines;
reduction89points/798lines. Frozen91points reconcile as89 complete restores
and2 minimized residuals. No guest source or binary was changed. S7 totals:
9/20 files final-verified for delivery,11remaining; cumulative reduction
102points/904lines. Including S6:103points/905lines. Migration39/159.
Commit/push completes P5; next ordered file is base/keymouse/mouse_io.c.
