# S13 mirror conditional reconciliation evidence

Scope: host-executed MVDM and non-MVDM mirror sources; guest source and
images are excluded. The pinned original identities and the branch bodies'
hashes are recorded in `s13-rejected-legacy-branches.json`. A previous
inventory paired 1,676 host/non-MVDM mirror files to pinned originals: 43
were byte-identical, 1,142 differed only in line endings and 491 differed
otherwise. This describes the admission baseline, not the final tree.

The diagnostic inventory used 1,050 compiler contexts from the current x86
and x64 Ninja trees. A branch not observed in these contexts is not thereby
proven impossible in every supported configuration. Original-directive
sequence comparison before this edit reported 3,666 unmatched lexical
candidates in 138 files; after these edits it reports 4,195 in 142 files. The
increase includes original inner directives removed with the newly excluded
YODA body. These are candidates, not a count of defects. For example, all 215 original
`CCPU` alternatives in `cpu4gen.h` already have their rejected outer arm;
645 original inner directives lie inside those rejected arms and must not be
reconstructed under the current rule. Entire functions omitted from a retained
source subset similarly produce lexical candidates without proving a missing
conditional boundary. The 150 `nt_cprgs.c` candidates include the original
CCPU and A2CPU implementations nested under the already rejected `SUN_VA`
arm; the `ega_read.c` candidates include descendants of its rejected
non-CPU40 arm. The `bios.c` `NOVELL_CFM` and `V4CLIENT` examples are nested
inside its rejected `NOVELL` arm; the retained `else` preserves the original
illegal-BOP entries. These examples are not a disposition of the remaining
files.

For `cpu4gen.h`, a directive-multiset comparison provides an additional
cross-check on the repetitive LCS result: the only original directive counts
in excess of the current file are exactly 215 each of `#ifdef PROD`, `#else`
and `#endif`, the nested three-directive structures removed inside the 215
rejected CCPU alternatives. This explains why its 1,286 LCS candidates are
not 1,286 missing outer guards; multiset equality alone is not proof of
correct placement of every retained guard.
An ordered block comparison then located all 215 original and current
`#ifdef CCPU`/`#else` pairs in the same order, with `#error` in every current
alternative. The selected bodies are byte-equivalent after newline decoding
in 202 pairs; the remaining 13 are the segment-load and flag-getter return
type adaptations registered as DIV-046. This check addresses these 215
repetitive pairs, not unrelated conditions in the header.

The original `yoda.c` has its large `#ifdef YODA` body at original lines
74–9518; the current file retains that outer condition and rejects it. A
stack-based scan finds 519 directive lines inside that original outer region
(including its closing directive). The lexical LCS count of 516 is a
different, comment-filtered metric; neither number requires reconstructing
inner directives beneath the rejected outer arm. The original `PROD` dummy
definitions outside that arm remain present and selected.

`s13-selected-ancestor-candidates.json` is a lexical aid generated after
DIV-564 and OPENNT-HOST-143. It
matches code lines and compares their surrounding conditional spellings;
equivalent expressions, repeated lines, intentionally cropped functions and
include guards can all create false positives. It is a worklist, not proof
that a listed branch is missing or that an unlisted branch is correct.
For example, an original-guard anchor search nominated a second
`BIT_ORDER2` block in `ega_prts.c`; review shows the existing selected
`BIT_ORDER2` block already contains those definitions, while its adjacent
`BIT_ORDER1` block is rejected. `BIT_ORDER2` is defined by `host_def.h`, so
inserting an extra `#error` at the ambiguous anchor would break live VGA code.

Additional checked lexical candidates: `dos/dem/dem.c`'s original nested
`i386` alternatives are inside the retained `#if DBG` arm, now rejected by
DIV-516; the nested structure must not be reconstructed. In `reset.c`, the
matcher pairs repeated `setAH(14)` statements across the DIV-531 rejected
`LICENSING` arm and the subsequent selected code. In `com.c`, it pairs the
same `host_com_char_read` call in the original `FIFO_ON` and `!FIFO_ON`
arms; DIV-525 retains that boundary and rejects only `FIFO_ON`. In
`c_stack.c`, it pairs the generic `GLOBAL VOID` line of rejected PIG-only
`spush_flags` with the distinct selected `spush16` function; DIV-498 already
retains and rejects the original outer PIG arm. These dispositions cover
those specific matches, not their entire files.

The 203 unmatched original directives reported for
`windows/core/ntuser/kernel/ntstubs.c` need a different treatment from an
excluded arm of a retained function: OPENNT-HOST-076 retains only the complete
`NtUserSetWindowFNID` function from original lines 6170–6188. The unmatched
directives belong elsewhere in the much larger original translation unit.
They must not be inserted as unconditional failures around this retained
function based solely on their count. Whether the owner's structural rule
requires additional sentinels for omitted whole functions still requires
per-boundary classification; this observation does not close that file.

Other matched-code ancestors are not automatically missing shells:
`egawrtm0.c` and `egwrtm12.c` deliberately extend the original `A_VID`
condition to select the CCPU40 writer declarations (DIV-079);
`emm_mngr.c` extends `!NTVDM` for the registered CCPU LIM writeback profile;
and `mouse_io.c` selects the registered non-MONITOR software cursor route
(DIV-318). These existing conditions cannot be replaced by the historical
expression without separately proving those selected semantics.
The `host_cpu.h` matched-line warning similarly crosses original CPU30 and
byte-order arms: DIV-320 documents the selected CCPU40 port-I/O union layout.
`cpu4.h` includes this header in the live executor, so those declarations
cannot be removed as hardware V86 code.
`base/support/ios.c` is another necessary exception: DIV-477 explicitly
selects the original port-I/O entries for CCPU as well as their historical
monitor profile. Their old `MONITOR` ancestry does not make the currently
used CCPU entry points a hardware-monitor implementation.
The BIOS BOP table is compiled with `MONITOR` for its original FD entry;
that entry resolves to the registered CPU40 DOSX continuation in
`dpmi32/modesw.c` (DIV-229). It is a selected protected/real-mode transition,
not a reason to restore the retired hardware V86 monitor.

For example, `windows/core/ntuser/client/wow.c` retains only the original
`_GetDlgItem` function (OPENNT-HOST-108), while `kernel/ntstubs.c` retains
only `NtUserSetWindowFNID` (OPENNT-HOST-076). Directives belonging to the
omitted functions are not automatically missing branches of the retained
functions; their omission and separate owners remain covered by those existing
mirror registrations.

The recorded context inventory also shows why labels cannot be rejected by
name alone: `X86GFX`, `#ifndef PROD`, and `#ifndef CCPU` each have selected
contexts, and a DPMI observation deliberately compiles with `/UCCPU` as a
negative control. Examples in that snapshot include `cvidc/accessfn.c:6`
(`!PROD`), `base/inc/cpu4.h:113` (`!CCPU`), and `video/video.c:2283`
(`X86GFX`). The inventory predates the later branch edits and is used
as a warning for classification, not as proof that every other context is
unreachable.

The initial original `UNICODE` selection in `inc/ntcftxt.h` is restored under
OPENNT-HOST-139. Its selected ANSI arm owns `IS_ANSI` and clears `_UNICODE`,
as in the original; the duplicate definition is removed from
`wow_class_client_bindings.h`. The Unicode arm is explicitly rejected. The
product's separately compiled class-client configuration is ANSI; the
class-registration fixture also consumes the ANSI subset. Both the guard and
the full retained class-client closure require renewed verification after this
change. Prior build, preprocessing, binary comparison and runtime results
below precede this change and do not certify it.

DIV-558/559 restore the original `MONITOR` condition boundaries in the
quick-event, EOI and heartbeat-timer source owners. Their excluded monitor alternatives are
errors, not a hardware-V86 implementation. Both current qevnt compilation
contexts omit `MONITOR`; the selected CCPU quick-event, timer and PIC bodies
are unchanged in the selected profile. Exact original ranges and source hashes are recorded in
`s13-event-condition-boundaries.json`. DIV-558 was applied after the
preceding complete runtime regression. The two affected compilation contexts
have equal normalized preprocessor output against the earlier snapshot; the
cached x86/x64 build passed. PIC reset/EOI recorded 24 passing assertions,
keyboard/PIC 86, and the quick-event object contract 37 checks with eight
callbacks. These object-boundary tests are not full DOS/WOW/Win3.x acceptance;
the full product regression including the external DOS4GW fixture passed on
that intermediate staged package. DIV-559 and DIV-560 were applied later and
were then covered by a new cached build, the 24 PIC, 86 keyboard and 37
quick-event checks, and the full product suite including the external DOS4GW
startup witness. DIV-561 and OPENNT-HOST-142 were applied after that suite;
their cached product build and 1,050-context preprocessor comparison passed.
DIV-562 and DIV-563 were applied afterward. Their five affected compilation
contexts match the earlier normalized preprocessing snapshot, the cached x86
and x64 builds pass, and the 24 PIC, 86 keyboard and 37 quick-event checks
pass again. All ten staged `.text` sections match the previous fully tested
candidate; whole-file hashes differ. Full product and four-mode Win3.1
verification of this latest candidate remains in progress.

The original `UNICODE` selection in `inc/ntsend.h` is distinct: the header is
also included by separately compiled Unicode control clients, so rejecting
that profile would be wrong. OPENNT-HOST-140 restores both original choices
for the three retained aliases and the original Unicode capture/cleanup macro
dependencies. Current references to these aliases are confined to the ANSI
class-client subset. Both widths' preprocessor controls must verify the ANSI
and Unicode expansions, followed by the actual product build; no Unicode
control client is rejected merely for including this shared header.

Original whole-header guard mismatches also require per-owner review. For
example, `kernel/userk.h` is a registered partial carrier with a separate
`WOW_USER_OBJECT_FLAGS_ONLY` inclusion mode; `public/sdk/inc/ntrtl.h` has a
project-specific subset guard. Blindly reinstating their original whole-file
guards could suppress declarations on a later include or collide with SDK
headers. These are unresolved structural candidates, not demonstrated runtime
defects, and have not been modified by this reconciliation.

`base/inc/cpu.h` has an original `NTVDM && MONITOR` outer condition. The
project's `CpuH` resolves to `cpu4.h` in both admitted selections; a source
search found only the historical CCPU callback fixture directly including
`cpu.h`, and that fixture inherits flags without `MONITOR` from
`New-T313CcpuLifecycleNinja.ps1`. DIV-561 rejects only that unused header
profile, retaining the original outer condition. This decision does not
classify every removed declaration as hardware V86 and does not remove the
selected CCPU or software-V86 implementation. Its fixture and product
boundaries require renewed verification after the change.

OPENNT-HOST-142 restores the original `!MSDWP` selection around the two
retained `userk.h` WOW critical-section functions. Pseudo-event definitions
were taken from a different original region and remain outside that guard;
the current subset interleaves them between the two functions. The original
whole-header include-guard discrepancy remains a separate, non-branch-body
adaptation.

OPENNT-HOST-143 restores the original `!MIDL_PASS` condition around the
retained `RtlConvertLongToLuid` inline, without gating the rest of `ntrtl.h`.
DIV-564 restores the original `!LDBIOS && !CPU_30_STYLE` BOP-FE selection
and rejects its unadmitted alternative; the selected CCPU continuation and
the distinct BOP-FD mode-transition owner remain unchanged.

DIV-039's virtual-interrupt initialization keeps the selected CPU40 software
path and now rejects its unsupported non-CPU40 hardware-monitor alternative.
The original host-ISA selection cannot be used unchanged because this build
uses the software CPU on x86. The selected and rejected profiles have separate
preprocessor controls; this does not remove software V86 execution.

The latest selected x86 and x64 cached builds passed. The x86 and x64
guard probes passed, including the DIV-039 positive and negative profiles.
The current 1,050 observed compiler contexts all matched the earlier
post-first-26-edits snapshot after normalization; this is not a comparison
with pristine OpenNT. The PIC reset, keyboard controller and event queue
fixtures reported 24, 86 and 37 assertions respectively, and the staged
ten-component package passed the Full selected product gates including the
DOS4GW/NESticle startup witness. That witness checks a live graphics window,
not gameplay or pixels.

The first Win3.1 matrix attempt failed to find its output because the reused
test witnesses contained paths from a previous isolated installation. The
test-only copies were retargeted to their reserved result-path fields, without
changing or rebuilding guest system images. The rerun passed standard and
enhanced mode with both DOSONLY selections, verified the DOS and Windows
witnesses and observed normal exit. Six original guest executable/driver
images were hash-checked against the isolated copy before the run. This
corrects the failed test setup; the first run is not product-failure evidence.

`client/usercli.h` similarly has a registered subset-specific include guard
(`OPENNT-HOST-072/074`); a matched-line heuristic flags hundreds of lines
because the original outer include-guard spelling differs. Include guards are
not deleted conditional bodies under the adopted rule, so this count is not
evidence of hundreds of missing `#error` branches.

| Item | Original responsibility and decision | Boundary and evidence |
| --- | --- | --- |
| Legacy platform, CPU, hardware and debug alternatives | Retain the original outer selection and replace proven unsupported bodies with `#error`; nested rejected bodies need not retain their own structure. | DIV-553, DIV-556, DIV-557, DIV-560, DIV-561, DIV-562, DIV-564, OPENNT-HOST-141 and the machine-readable ledger record 227 reviewed replacements, including eight subsequently subsumed by an excluded outer branch. Selected command lines, local macro definitions, source references and original context were reviewed; observation alone was not treated as proof. DIV-556's 106 Mac MPW `SEGMENTATION` bodies consist of `.seg` includes or nested selections of those includes. DIV-557 and OPENNT-HOST-141 cover 25 original literal `#if 0` bodies checked against their original counterparts. |
| YODA debug implementation | The original large YODA-only body is excluded; its outer condition remains. | The selected worker compiles the file with `PROD`; the original host header enables YODA only without `MONITOR` and `PROD`. The body hash is retained in the ledger. No hardware monitor is introduced. |
| DPMI INT 21 registration | Restore the original `i386` outer boundary without calling the unsupported NT kernel VDM service. | DIV-554 rejects the non-CPU40 kernel VDM path. `i386` and CPU40 may coexist, so that combination remains accepted. The selected CCPU route is not changed. |
| CPU30 boundaries | Restore original outer selection around existing retained declarations and tape/reset logic; preserve DIV-265's selected reset behavior. | DIV-555; selected behavior remains CPU40. Line controls preserve locations where insertion changes source line numbers. |
| Original i386 and portability selectors | Keep intentional modern-compiler adaptations and the CPU40 exception, rather than replacing them mechanically with historical spelling. | Original `i386` alone does not describe every supported modern compiler profile. Existing registered divergences retain their owner and selection rules. |

`tests/build/test_mirror_rejected_conditions.py` extracts the retained source
guards and runs positive and negative preprocessor controls. These are guard
tests, not proof that the whole translation unit or every lifecycle path is
correct. Full context preprocessing and product builds provide separate
evidence. The remaining lexical candidates need source-context classification;
this record does not claim they are all resolved or that historical profiles
outside the admitted product are supported.

The x86 and x64 positive/negative guard probes both passed for 239 retained
guards, the separate i386/CPU40 DPMI boundary, ANSI/Unicode `ntsend.h`
capture expansion, and the `MSDWP` and `MIDL_PASS` selections. For literal `#if 0`, the negative test temporarily
selects the extracted branch in the test input; the source remains `#if 0`.
An earlier cached product build completed for the selected x86 and x64 targets
before OPENNT-HOST-139. Comparing that earlier staged package
with the prior `assets/release` showed identical `.text` section hashes for
all ten product images, but not identical whole-file hashes. In particular,
NTVDM and WOW32 resource-item comparison found the package identity resource
changed; their `.rdata` sections also contain different link-time bytes, and
WOW32's resource-section layout has a different size. These observations do
not by themselves establish complete binary equivalence or replace runtime
verification. The fixed-source comparison completed with all 1,050 observed
compiler contexts producing equal normalized preprocessor output against an
earlier snapshot taken after the first 26 guard edits. It does not establish
equivalence of those first edits to the pre-edit tree, validate diagnostic line
mappings or unsupported profiles. The selected `Full` product regression
passed on the staged ten-component package, but its external DOS4GW/NESticle
case was skipped because the fixture argument was not supplied. That case
was subsequently run as a separate existing probe: the pinned original
DOS4GW/NESticle fixtures remained active with a visible graphics window. The
probe used the sanitized standard runner environment and does not assert
gameplay, pixels, or the reported inherited-environment failure. It is not
included in the `Full` pass claim. The class-registration original-source
fixture comparison now passes with the restored ANSI guard, the previously
retained `GetClassInfo`, and the existing class-source outer boundary checked
explicitly; this is not a runtime test.
