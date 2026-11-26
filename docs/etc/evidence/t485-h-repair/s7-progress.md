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