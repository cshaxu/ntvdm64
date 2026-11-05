# M0 T476 S2 — per-file audit baseline

S2 inherits the published S1 source at `2f9e2aea7`. The active checklist is
[`t476-s2-file-checklist.csv`](../../../artifacts/mvdm-audit/t476-s2-file-checklist.csv); each row records both identities, S1 marker evidence and status. The original identities and current files were rehashed against the recorded original source mapping.

The 1,055 files are partitioned as follows: 180 byte-identical to their pinned
original, with their file marker and all non-excluded branch markers observed;
874 changed files needing complete-file review; and one generator input requiring
a separate audit. The latter, `c2cpusad.h`, is not directly compiled, but
`tools/build/GenerateCvidcCpuBinding.mjs` reads it to generate production binding
code, so no compiler marker is not grounds for removing it.

Fifteen changed files differ from the S1 marker-time hashes due to the 30 S1
exclusions. Each matches the S1 final worktree byte-for-byte; its recorded
1,018-context post-change preprocessing comparison rebinds marker evidence to
the final source. This rebinding does not itself complete per-file review.
A fresh original/current structural scan found 37 unmatched leads in seven
files; all 37 have S1 dispositions with matching current and original hashes.
The scan is a review aid, not proof that all code or line mappings are correct.

A source-line matching scan reported candidate line mapping discrepancies in
78 changed files; the counts are recorded per file as leads, including code
that may be unreachable or intentionally adapted. They are not compiler
diagnostics or confirmed defects. Real selected compiler line evidence and
full-file original comparison remain required. `modesw.c` and `rtl/wow.c`
remain in review: their earlier six and nineteen uniquely matched compiler
lines agree with the original, but that sample is not full-file proof.

At this checkpoint the remaining 875 files have not been certified complete.
No mirror or guest file was modified and no runtime artifact was republished.

## Initial compiler-line samples

For the 78 files with static leads, a greedy selection of 63 S1 compile
contexts covers every previously observed branch marker in those files. All
63 `/E` commands exited successfully with the current source root. The
[selected compiler-line samples](../../../artifacts/mvdm-audit/t476-s2-compiler-line-samples.json) retain selected commands, their plan identity and raw matched lines.
They contain uniquely occurring original-line matches for 62 files; 30 have
1,055 candidate mismatched statements. Duplicate expansions, local adapters,
conditional removal and incomplete matching can create false leads. The other
16 files have no matching unique statement in this sample; none is a pass on
that basis. The initial run had a source-prefix parsing defect and was repeated
with slash normalization; only the corrected run is recorded.

These samples prioritize complete-file review and do not establish that every
selected statement or all contexts have correct diagnostic lines. No file has
been promoted solely on the absence of a sampled mismatch.

## Expanded compiler-line index

The [compiler-line index](../../../artifacts/mvdm-audit/t476-s2-compiler-line-index.json) records 433 successful real preprocessing commands: 230 cover all recorded hit branches of the 874 differing files, and 203 additional contexts cover their remaining S1 file markers. All 874 have a recorded file-marker context; 604 yielded uniquely matching original text and 270 did not. Forty-eight files contain 1,152 possible line mismatches under this lexical sampling. Original text expanded more than once, local adaptations and macro transformations require review; these are leads, not confirmed defects. Source and original hashes, matching context IDs, commands and the plan hash are retained per record.

No source was changed by these preprocessing runs. A branch/file marker proves
that a context selected the indicated source region, not that each original
statement was preserved or that every logical line is correct. The 180
byte-identical files remain the only completed category at this checkpoint;
the 874 different files and generator input remain open.

## Completed condition-free files

The [condition-free review](../../../artifacts/mvdm-audit/t476-s2-condition-free-review.json) completes 130 differing files. Each has no original or current conditional group, identical non-comment code after removing line directives and indentation, matching original/current hashes in the earlier full matched-code line review with no mismatch, and at least one actual compiler line sample with no observed mismatch. The comparison retains string contents and does not equate changed code with comment-only differences. These results add 130 completed files to the 180 byte-identical files: 310 completed and 745 still open, including the generated-binding input. Source files were not modified in this batch.

## Source and logical-line equivalent files

The [source/line review](../../../artifacts/mvdm-audit/t476-s2-code-line-equivalence.json) completes four more files. Every uncommented, nonblank line matches the pinned original in sequence and logical line after interpreting line directives; the inventory has no missing conditions or parser errors, all conditional arms are observed, and actual compiler line samples agree. No source was edited. The running partition is 180 byte-identical, 130 condition-free code-equivalent and four source/line-equivalent complete; 741 files remain open.

## Completed strict conditional projections

The [strict conditional review](../../../artifacts/mvdm-audit/t476-s2-strict-conditional-review.json) completes 114 additional differing files. Each reuses a hash-matched earlier recursive original/current conditional projection that checked exclusion boundaries and matching-code logical lines. The fresh inventory reports no missing structure, nesting conflicts or parser errors; S1 observed every remaining non-excluded branch, and no static or collected compiler-line mismatch was found. The stored record links the specific prior review and current marker evidence per file. These facts establish the requested conditional and line audit, not unrelated runtime behavior.

The current partition is 180 byte-identical, 130 condition-free equivalent, four source/line-equivalent and 114 strict conditional files complete: 428 of 1,055. The other 627 remain open, including the generator input. No source was changed in this batch.

## Generator-only metadata

The [generator-input review](../../../artifacts/mvdm-audit/t476-s2-generator-input-review.json) completes `c2cpusad.h`: original and current bytes are identical after CRLF normalization, neither has conditional groups, and the production generator uses the metadata. The C-VID contract audit and actual NTVDM link map passed for 154 public and 55 private slots. Its old test had expected removed CPU30 code and ignored an allowed line directive; the test now checks CPU30 exclusion and accepts the line directive. This is a test-only fix. The running partition is 429 complete and 626 open.

## Directive-only files

The [directive-only review](../../../artifacts/mvdm-audit/t476-s2-directive-only-review.json) completes 22 more headers. They contain no conditional groups or non-directive source; their original and current directive sequences and logical positions match, their earlier per-file review is bound by both hashes, and the compiler file marker was observed. There were no unique executable source lines to compare in those files, so this is source/directive-line evidence plus actual inclusion evidence, not a claim of emitted-line sampling. The current partition is 451 complete and 604 open. No source was modified.

## Completed excluded-only projections

The [excluded-only projection review](../../../artifacts/mvdm-audit/t476-s2-excluded-projection-review.json) completes another 65 files. Each has a hash-matched prior complete original/current conditional-tree and outside-branch projection review, with original exclusion and continuation anchors; fresh structure checks find no unresolved original group, and the actual compiler samples have no line mismatch. This confirms the conditional and line contract, not unrelated runtime behavior. The running partition is 516 complete and 539 open. No source was modified.

## Compiler-transformed original-equivalent files

The [transformed-line review](../../../artifacts/mvdm-audit/t476-s2-transformed-lines-review.json) completes 157 further condition-free files. Their complete uncommented original/current source and logical line positions match, but macros caused the earlier exact-text sampling to find no unique source line. Twenty-one real compiler contexts cover all 157 file markers. Each file emitted preprocessed text, and every emitted line maps to a nonblank original source line or an identical original pragma. This checks the transformed compiler output without assuming macro expansion preserves its literal text. The running partition is 673 complete and 382 open; no mirror source was changed.

## Conditional files with transformed compiler output

The [strict transformed-line review](../../../artifacts/mvdm-audit/t476-s2-strict-transformed-review.json) completes 14 conditional files with hash-matched recursive original/current projections. Twenty real compiler contexts cover the candidate file and hit-branch markers; for these 14 files every emitted line is attributed to a nonblank original source line or identical original pragma. Fifteen candidate files emitted no source lines and one has an unexplained emitted line; those remain open. The running partition is 687 complete and 368 open. No mirror source was modified.

## First COMMAND file reviews and remaining lead

The [COMMAND file review](../../../artifacts/mvdm-audit/t476-s2-command-file-reviews.json) completes `cmd.c`, `cmdconf.c`, `cmddata.c` and `cmdenv.c`, recording every local adaptation, branch disposition and compiler-line result. Repeated `TerminateVDM()` text in `cmdconf.c` was checked against its actual original line rather than counted as a false mismatch. The running partition is 691 complete and 364 open.

The next file, `cmdexec.c`, is not complete: the original `RtlInitString` at line 363 appears at logical line 374 in the sampled product preprocessor output, and the static scan identifies additional possible offsets. Its surrounding added failure branch and duplicated `CreateProcess` paths require a complete file-level line audit before one grouped correction. No source change has yet been made.

The same [COMMAND file review](../../../artifacts/mvdm-audit/t476-s2-command-file-reviews.json) additionally completes `cmdkeyb.c`, `cmdpif.c` and `cmdredir.c`. The keyboard directory adaptation was missing from the existing `MVDM-HOST-DIV-464` registry entry and has been added; the other local changes were already registered. The partition is 694 complete and 361 open. `cmdexec.c` and `cmdmisc.c` have grouped line-anchor candidate corrections in progress; no production build or publication is claimed for them yet.

## Excluded-only files with transformed compiler output

The [excluded transformed-line review](../../../artifacts/mvdm-audit/t476-s2-excluded-transformed-review.json) completes nine additional files with hash-matched complete excluded-branch and outside-projection evidence. Twenty-eight actual compiler contexts cover 34 candidates; nine emit lines and all those emitted lines map to nonblank original code or identical original pragmas. The other 25 emit no lines and remain open. The running partition is 703 complete and 352 open.

## Excluded-only files without emitted lines

The [no-emission exclusion review](../../../artifacts/mvdm-audit/t476-s2-excluded-no-emission-review.json) completes the remaining 25 excluded-only candidates. Their hash-matched prior review checked the complete original conditional tree and outside projection, all conditional arms are explicitly excluded, the file marker was observed in a selected product compile, and the selected compiler context emitted no source lines. The record does not claim emitted-line sampling. They remain build-selected inputs; no file was deleted. The running partition is 728 complete and 327 open. The two COMMAND line-only source corrections remain unbuilt as part of the owner-directed ten-file batch.

## Conditional projections without emitted lines

The [strict no-emission review](../../../artifacts/mvdm-audit/t476-s2-strict-no-emission-review.json) completes 15 files from the previous 30-file transformed-output group. Their full conditional projections and matching-code positions were already reviewed against hash-identical originals and current source. The 20 actual contexts cover each file marker and every hit arm; these 15 emit no source text, so no emitted-line claim is made. The remaining unexplained file from that group stays open. Current progress is 743 complete, 312 open. The two COMMAND line corrections remain an unbuilt source batch.


### Ten-file diagnostic-line batch

Ten host mirror files received only `#line` corrections, listed with original and before/after source hashes in [the batch record](../../../artifacts/mvdm-audit/t476-s2-line-batch1.json). The original file and S1 marker evidence remain the references; the changed files are **not** counted as complete per-file audits yet. The checklist current hashes were updated; completion remains 743 of 1,055.

The x86 and x64 cached product builds succeeded, including the inner NTVDM and WOW32 Ninja targets. The first build attempt stopped before those targets because it invoked CMake on an inner Ninja tree; the corrected build exited zero. Candidate staging succeeded. The [ten-image PE comparison](../../../artifacts/mvdm-audit/t476-s2-batch1-pe-comparison.json) found eight identical images and two images differing exclusively in COFF and debug-directory timestamps; no executable bytes differed. The owner explicitly allows skipping repeat runtime tests for line-only changes with no substantive executable differences, so no runtime test was repeated for this batch.

The coherent ten-image package and hook license were synchronized and SHA-256 checked at `assets/release` and `O:/winnt/system32`; the prior package is retained under `build/t476-s2/batch1-publish-backup`. Build logs and stage results are under `build/t476-s2`. No guest source or binary was rebuilt or changed.


### Post-publication compiler-line follow-up

The ten changed files were reprocessed in 39 recorded product contexts; all commands succeeded. The [follow-up index](../../../artifacts/mvdm-audit/t476-s2-batch1-preprocess.json) binds each current source hash to matching original-line observations and unresolved lexical leads. Several apparent mismatches are repeated statements or project-added failure handling; these cannot be accepted or corrected automatically. This is sampled compiler evidence, not complete per-file certification, so the progress count remains unchanged.


### Completed COMMAND line-batch files

The [final COMMAND file review](../../../artifacts/mvdm-audit/t476-s2-command-final-review.json) completes `cmdexec.c` and `cmdmisc.c`. It binds both source versions to their original hashes, records every matched original source line and its logical location, checks the entire original conditional inventory and the single excluded `CHECK_IT_LATER` branch, and references the successful actual compiler observations. Both entire original/current diffs were reviewed; added guest-location, process, stream and environment adaptations remain under existing mirror divergence records. Macro and local-added duplicate statements were distinguished from original statements. The earlier ten-file production build and publication remain applicable because no source was modified in this review. Total: **745 of 1,055 complete; 310 open**. The other eight files from that earlier batch remain open.


### Completed WOW DDE line-batch file

The [DDE file review](../../../artifacts/mvdm-audit/t476-s2-wdde-final-review.json) completes `wdde.c`. Neither source has a conditional group; the full original/current diff and retained matching line positions were reviewed, with 143 actual compiler observations. The apparent mismatch arose from two original comparisons that become text-identical after a trailing comment is stripped. Existing local DDE lifecycle changes remain registered. The prior ten-file product build and publication are unchanged. Total: **746 of 1,055 complete; 309 open**.


### Selected globals header branch under repair

The original `client/globals.h` `#ifndef RC_INVOKED` arm is selected in 39 recorded S1 product contexts, but its original `LHND` and `LMHtoP` definitions were absent from the retained frame. The current supported SDK independently defines `LHND` as the same flags; selected EDIT sources use it. An in-progress restoration puts the original definitions back inside that reached arm. It has not yet been compiled or published; it remains an open file and must join the next ten-file source batch. No conclusion about build or runtime equivalence is claimed yet.


### Completed INT 15 host source review

The [tape I/O file review](../../../artifacts/mvdm-audit/t476-s2-tape-io-final-review.json) completes `tape_io.c` against its pinned original. The complete diff, all recorded condition dispositions and selected PM/CPU40/NTVDM code were reviewed; unsupported paths retain `#error`. All matching original source lines retain their logical positions, and selected compiler contexts corroborate them. Existing XMS and register-only observation adaptations remain registered. Total: **747 of 1,055 complete; 308 open**.


### Completed WOW message source review

The [WOW message file review](../../../artifacts/mvdm-audit/t476-s2-wumsg-final-review.json) completes `wumsg.c`. Its complete original/current diff and conditional-arm dispositions were checked, including the selected non-API16 implementation and excluded DEBUG and API16 branches. Matching source-line positions and 101 preprocessed observations agree; the existing message and DDE ownership adaptations remain registered. Total: **748 of 1,055 complete; 307 open**.


### Completed original dialog creator review

The [dialog creator review](../../../artifacts/mvdm-audit/t476-s2-dlgbegin-final-review.json) completes `dlgbegin.c`. Its complete diff, original DEBUG branch and local dialog-procedure ownership conditions were checked against hit records; the existing ownership and lifecycle adaptations remain registered. Matching source positions and 140 actual compiler observations agree. Total: **749 of 1,055 complete; 306 open**.


### Completed DOS data and FCB source reviews

The [DEM file reviews](../../../artifacts/mvdm-audit/t476-s2-dem-final-review.json) complete `demdasd.h` and `demfcb.c`. Neither original nor current file has conditional groups. The full source diffs, fixed guest DPB address and scoped DTA lease adaptations, matching original source lines, and actual compiler samples were checked against the pinned hashes. Total: **751 of 1,055 complete; 304 open**.


### Completed DOS state source review

The [DEM state file review](../../../artifacts/mvdm-audit/t476-s2-demdata-final-review.json) completes `demdata.c`. Neither source has conditional groups. Its only retained original directive is at its original line; the four original data declarations are replaced by registered guest-location owners, and continuation anchors were checked. The product included the file in recorded compiler contexts, but no matching original executable statement exists to sample; this evidence does not claim one. Total: **752 of 1,055 complete; 303 open**.


### Completed DOS state access source review

The [DEM state access review](../../../artifacts/mvdm-audit/t476-s2-demgset-final-review.json) completes `demgset.c`. Neither source has conditional groups. The full original/current diff, registered numeric guest-state ownership, DPB representation and filesystem-field repair were checked along with matching source positions and 197 unique compiler-observed original lines. Total: **753 of 1,055 complete; 302 open**.


### Completed OEM file helper review

The [OEM file helper review](../../../artifacts/mvdm-audit/t476-s2-oem-file-final-review.json) completes `oemuni/file.c`: there are no original or current conditional groups, the full diff consists of registered output initialization and OEM encoded-byte/capacity fixes, and matching source line positions and 161 unique compiler-observed original lines agree. Total: **754 of 1,055 complete; 301 open**.


### Completed RAP structure-size review

The [RAP structure-size review](../../../artifacts/mvdm-audit/t476-s2-rap-strucsiz-final-review.json) completes `strucsiz.c`. Neither source has conditional groups. The complete diff preserves the original RAP sizing code, removing prose and comments with continuation anchors; matching source positions and actual compiler observations agree. Total: **755 of 1,055 complete; 300 open**.


### Completed DPMI, export and CPU source reviews

Ten unconditional original/current pairs were checked by complete diff, registered adaptation, line-position comparison and hash-bound compiler samples: `dpmimscr.c`, `dbgexp.h`, `memapi.h`, `c_debug.c`, `lmsw.c`, `localfm.c`, `ntthread.c`, `ntthread.h`, `popf.c` and `sascdef.c`. Their individual `t476-s2-final-*` JSON records in `artifacts/mvdm-audit` retain exact source hashes, compiler contexts and matched line positions. Their runtime code was not altered during review. Total: **765 of 1,055 complete; 290 open**.


### Completed C-video, host disk/PIF and WOW header reviews

Ten more unconditional original/current pairs were reviewed as complete files: `evidfunc.h`, `j_c_lang.c`, `nt_fdisk.h`, `nt_fdisk.c`, `nt_pif.c`, `isvwow.h`, `wdde.h`, `winsockp.h`, `wkman.h` and `wowhooks.h`. The individual `t476-s2-final-*` records retain hashes, compiler contexts, matching line positions and the registered local adaptation. DIV-463's affected-file list was corrected to identify `winsockp.h` rather than a nonexistent `.c` file. No runtime code was changed by these reviews. Total: **775 of 1,055 complete; 280 open**.


### Completed WOW, BaseSrv and XactSrv source reviews

Eight further unconditional pairs were fully reviewed: `wreldc.c`, `wreldc.h`, `wsext.c`, `wutmr.c`, `srvvdm.h`, `apiuse.c`, `apiwksta.c` and `xssubs.c`. The per-file `t476-s2-final-*` records bind original/current hashes to existing compiler contexts, matched line positions and each local adaptation. No runtime source was changed in this review. Total: **783 of 1,055 complete; 272 open**.


### Completed USER drawing, help and text reviews

The unconditional `client/draw.c`, `rtl/help.c` and `rtl/text.c` pairs were reviewed against their originals. The full diffs preserve the registered PaintRect subset, help property binding and text buffer ownership adaptations. Individual `t476-s2-final-*` records bind the current and original hashes, compiler observations and matching line positions. Total: **786 of 1,055 complete; 269 open**.


### LF-count correction and additional selected subsets

A review helper had used a generic line splitter that treated form-feed as a line boundary. This affected the recorded physical indexes of four individually reviewed files (`wsext.c`, `apiuse.c`, `apiwksta.c`, `xssubs.c`); their individual records were regenerated using LF-only physical lines, their original/current hashes and matching-position checks passed, and the per-file completion decisions remain unchanged. A separate earlier LF-based scan reported no leads in any of the 21 completed files containing form-feed; this observation does not add compiler coverage.

Four more original/current unconditional files were fully checked: `apibuff.c`, `drawtext.c`, `hdata.c` and `csrutil.c`. Their `t476-s2-final-*` records retain the selected subset, registered adaptations, hashes, matching positions and actual compiler observations. Total: **790 of 1,055 complete; 265 open**.


### Additional USER subset reviews

Complete original/current comparisons additionally finish `miscutil.c`, `rtl/random.c` and `rtl/rect.c`. Each has no conditional group in either version; the original selected function bodies and registered subset boundaries were checked with LF-based source positions and existing compiler samples. Their individual `t476-s2-final-*` records retain the evidence. Total: **793 of 1,055 complete; 262 open**.


### Interrupt hook declaration line under repair

The `nt_inthk.h` full original/current comparison has two `CCPU` branches and both were hit in the recorded product context. The emitted declaration for `VdmInstallSoftwareIntHandler` was attributed to line 30, while the pinned original declaration is line 31; the previously transformed-output index retained this as an unresolved line. A candidate `#line 31` is now present. It is not built or published and the file remains open until the next owner-directed source batch and actual compiler-line verification.


### FPU layout declaration lines under repair

The recorded compiler observations identify four original `cfpu_def.h` fields whose selected little-endian reordering changed their diagnostic attribution. Full comparison also identified related reordered fields in the same six declarations; candidate `#line` anchors now point each retained field to its original line while leaving the approved layout and branch conditions intact. This is an unbuilt candidate, not a completed file or an assertion of compiler equivalence.


### Additional candidate line anchors

The existing selected-context compiler index identifies `support/main.c` startup calls displaced by the registered vector-binding insertion, `nt_unix.c`'s retained error formatting moved by a registered CRT adaptation, and `wowmmcb.h` declarations displaced by an added ABI notice. Their candidate original-line anchors are unbuilt and unpublished; these files remain open until the next source batch and post-build checks.
