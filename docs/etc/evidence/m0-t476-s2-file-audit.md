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


### Ten-file selected-branch and line candidate

A second source batch contains the selected `globals.h` memory macros and line corrections in `wowmmcb.h`, `support/main.c`, `cfpu_def.h`, `nt_inthk.h`, `nt_unix.c`, `wcntl32.c`, `probe.c`, `ntexapi.h` and `ntpsapi.h`. The two native executive declarations were matched to the correct original declaration variants; `probe.c` physical lines were counted by LF, including the original form-feed. This batch is awaiting the x86/x64 cached build and compiler-line verification; none of these files is newly counted as complete.


### Second ten-file build and publication

The second ten-file batch is listed by pinned before/after source hash in [the input record](../../../artifacts/mvdm-audit/t476-s2-batch2-input.json). The first build linked the x86 targets but its owned wrapper never supplied an exit result after the build subprocesses exited; that run was not accepted. After verifying no product compiler or linker child remained, only the stalled wrapper was stopped. An interrupt-header follow-up revealed a second line shift and it was corrected before the repeated cached build. The revised x86 and x64 build and inner NTVDM/WOW32 links completed with exit code 0.

The prior temporary S1 worktree had been removed, so the first reconstruction attempt could not read its command plan. A later reconstruction from the committed command index initially reached 429 of 433 contexts before an obsolete x64 directory path caused an error; that run is not a pass. The corrected [reconstructed compiler check](../../../artifacts/mvdm-audit/t476-s2-batch2-preprocess.json) covers each observed branch and one file marker per candidate in 12 successful current-build-root contexts. For `nt_inthk.h`, a separate transformed-output run confirms all six original declaration lines. This reconstructed observation is distinct from the original S1 marker evidence.

The [ten-component comparison](../../../artifacts/mvdm-audit/t476-s2-batch2-pe-comparison.json) found eight byte-identical images and only COFF/debug timestamp changes in NTVDM and WOW32. As authorized, runtime tests were not repeated. The complete package and hook license were published and SHA-256 verified at `assets/release` and `O:/winnt/system32`; the previous package is backed up under `build/t476-s2/batch2-publish-backup`.

The [four completed header reviews](../../../artifacts/mvdm-audit/t476-s2-batch2-final-reviews.json) cover `wowmmcb.h`, `cfpu_def.h`, `nt_inthk.h` and `client/globals.h`, with original condition inventories, observed arm dispositions, candidate hashes and compiler observations. The other six candidates are built and published but remain open for full-file review. Total: **797 of 1,055 complete; 258 open**.


### Completed startup, host utility and WOW control reviews

The [three source reviews](../../../artifacts/mvdm-audit/t476-s2-batch2-source-reviews.json) complete `support/main.c`, `host/nt_unix.c` and `wow32/wcntl32.c`. Full original/current diffs, branch dispositions and registered adaptations were checked. In `main.c`, actual expanded compiler output also confirms the two SoftPC globals and both startup calls at their original lines; an earlier naive static scan had treated an inactive branch's `#line` as active. The second batch now has seven completed files and three still open. Total: **800 of 1,055 complete; 255 open**.


### Completed probe and native declaration subset reviews

The [three subset reviews](../../../artifacts/mvdm-audit/t476-s2-batch2-sdk-reviews.json) complete `probe.c`, `ntexapi.h` and `ntpsapi.h`. They record the original condition structures and S1 arm dispositions, exact selected declaration/compiler lines, registered selected subsets and the specific omitted NT4 function or declaration groups. Empty frames correspond to removed code outside those subsets; they are not claimed to reproduce the omitted functionality. The two original `NtGetTickCount` variants were distinguished by their actual branches. All ten files in the second source batch are now reviewed; total: **803 of 1,055 complete; 252 open**.


### Completed WOW communication header review

The [WOW communication header review](../../../artifacts/mvdm-audit/t476-s2-wowcomm-final-review.json) completes `wowcomm.h`. Apart from the S1 `#error` replacing the unhit `UNALIGNED` fallback on the same physical line, all original and current lines are identical after CRLF normalization. The enclosing guard was hit and actual compiler observations agree; no `#line` continuation adjustment is needed. Total: **804 of 1,055 complete; 251 open**.


### Completed DEM excluded-branch reviews

The [four DEM file reviews](../../../artifacts/mvdm-audit/t476-s2-dem-excluded-final-review.json) complete `dem.c`, `demfile.c`, `demhndl.c` and `demsrch.c`. Each binds the entire original/current excluded-branch tree and matching-code projection to both source hashes, checks the complete local diff and registered adaptations, and corroborates the result against selected compiler observations. This proves the stated conditional and line checks, not unrelated runtime behavior. Total: **808 of 1,055 complete; 247 open**.


### Additional excluded-branch reviews

The [four file reviews](../../../artifacts/mvdm-audit/t476-s2-excluded-four-final-review.json) complete `xmem.c`, `c_tsksw.c`, `environ.c` and `lboxrare.c` using hash-matched full original conditional-tree and selected-code line evidence, the complete local diffs and recorded compiler observations. `wuman.c` remains open because the older tree evidence does not match its current source hash. Total: **812 of 1,055 complete; 243 open**.


### WOW shared-domain header migration reconciliation

The [wuman.c review](../../../artifacts/mvdm-audit/t476-s2-wuman-final-review.json) binds the prior full-tree audit to its exact historical Git blob and establishes that the only subsequent source change is the shared-domain header path. The two declarations used by this file match in the previous and current headers; the current compiler observations match original line positions. The file is complete for the conditional and line audit. Total: **813 of 1,055 complete; 242 open**.


### EOI and sound source reviews

The [EOI and sound reviews](../../../artifacts/mvdm-audit/t476-s2-eoi-sound-final-review.json) complete `nt_eoi.h` and `nt_sound.c`: each full original/current diff, hash-matched original branch tree and line projection, and real compiler observations were checked. Original MONITOR-only and non-PROD diagnostic branches remain excluded; the original lock declarations remain available through the registered sound binding. Total: **815 of 1,055 complete; 240 open**.


### DEM dispatch review

The [DEM dispatch review](../../../artifacts/mvdm-audit/t476-s2-demdisp-final-review.json) completes `demdisp.c`, checking its complete original diff, four excluded DBG groups, hash-bound conditional and line projection, and selected compiler observations. The existing fast-I/O fallback remains registered under DIV-188. Total: **816 of 1,055 complete; 239 open**.


### C-video, host and CSR declaration reviews

The [three file reviews](../../../artifacts/mvdm-audit/t476-s2-host-cvidc-csr-review.json) complete `ev_glue.c`, `host.h` and `ntcsrmsg.h`: full original diffs, original conditions and S1 arm observations, hash-matched prior line projections and actual compiler samples were checked. Existing vector, native ABI and capture-metadata adaptations remain registered. Total: **819 of 1,055 complete; 236 open**.


### DPMI data and dispatch source reviews

The [two DPMI file reviews](../../../artifacts/mvdm-audit/t476-s2-dpmi-data-dispatch-review.json) complete `data.c` and `dpmi32.c`. Their full original/current differences, original and local branch dispositions, hash-matched recursive projection and selected compiler line observations were checked. This establishes the conditional and line audit, not independent proof of the CPU40 descriptor adaptation's runtime semantics. Total: **821 of 1,055 complete; 234 open**.


### DPMI header reviews

The [DPMI header reviews](../../../artifacts/mvdm-audit/t476-s2-dpmi-headers-review.json) complete `dpmi32p.h` and `dpmidata.h`. The full original/current diffs, registered CPU40 bindings, S1 observed arm dispositions, prior hash-bound detailed line evidence and selected compiler samples were checked. The full preprocessed-output evidence for `dpmi32p.h` is one x86 context; other context coverage is identified separately by markers. Total: **823 of 1,055 complete; 232 open**.


### DPMI memory and debug service reviews

The [DPMI memory and debug reviews](../../../artifacts/mvdm-audit/t476-s2-dpmi-memory-debug-review.json) complete `dpmimemr.c` and `i386/dpmi386.c`. Full original/current differences, recorded CPU40 branch observations, hash-bound recursive projection and selected compiler line samples were checked. Existing shared-XMS allocation and debug-register lease adaptations remain registered; this does not independently prove their runtime behavior. Total: **825 of 1,055 complete; 230 open**.


### DPMI selector source review

The [DPMI selector review](../../../artifacts/mvdm-audit/t476-s2-dpmiselr-review.json) completes `dpmiselr.c` for conditional structures and original diagnostic lines. Its full diff, S1 branch observations, hash-bound recursive projection and selected compiler samples were checked; existing CPU40 descriptor-domain adaptations are identified but their independent runtime equivalence is not claimed. Total: **826 of 1,055 complete; 229 open**.


### DPMI DOS interrupt translation review

The [INT 21 translation review](../../../artifacts/mvdm-audit/t476-s2-int21map-review.json) completes `int21map.c` for original conditional structures and diagnostic lines. The entire original/current diff, registered protected DOS adaptations, 14 excluded and one selected original arm, prior hash-bound 2,987-line projection and actual compiler samples were checked. This does not independently establish runtime equivalence of the local adaptations. Total: **827 of 1,055 complete; 228 open**.


### DPMI mode-switch source review

The [mode-switch review](../../../artifacts/mvdm-audit/t476-s2-modesw-review.json) completes `modesw.c` for conditional and diagnostic-line scope. The full original/current diff, S1 branch dispositions, registered CPU40 transition changes and current selected preprocessor output were checked. The actual output confirms original anchors for both transition functions, including relocated MSW/CS operations and closing braces; project-added statements that happen to share original logical positions are not counted as original matches. This review does not independently prove CPU40 runtime equivalence. Total: **828 of 1,055 complete; 227 open**.

## Reused strict file reviews

A further 44 files have hash-matched recursive original-structure and logical-line reviews corroborated by the S1 per-arm ledger and compiler line observations, with no static line leads. Their identities and evidence links are recorded in [the batch](../../../artifacts/mvdm-audit/t476-s2-strict-reused-batch.json). This brings the checklist to 872 reviewed and 183 pending. These records do not establish behavior in unobserved configurations or certify local runtime behavior. No source or product binary changed in this batch.

A further 36 files reuse hash-matched original-tree and local-difference reviews, checked against the per-arm ledger, static line scan and compiler line observations. The [batch record](../../../artifacts/mvdm-audit/t476-s2-reused-local-review-batch.json) preserves each prior review and its limitations. The checklist now records 908 reviewed and 147 pending. No source or product binary changed.

A further 16 files reuse hash-matched original-tree and continuation/projection reviews, corroborated by per-arm hit records and compiler line observations. See [the batch record](../../../artifacts/mvdm-audit/t476-s2-reused-projection-review-batch.json). The checklist now records 924 reviewed and 131 pending. No source or product binary changed.

A further 10 files combine hash-bound full-tree evidence, S1 arm hits and compiler observations with lexical classification of static line leads as comment-only. The [per-file record](../../../artifacts/mvdm-audit/t476-s2-comment-lead-review-batch.json) retains each lead and prior evidence. The checklist now records 934 reviewed and 121 pending; no product source or binary changed.

Four macro-heavy headers now have a [complete review](../../../artifacts/mvdm-audit/t476-s2-macro-header-review.json): actual product preprocessor contexts cover every recorded hit arm, and expanded declarations were compared at their original line positions or verified to emit no code. The checklist now records 938 reviewed and 117 pending. Other candidate headers without matching context or current-hash evidence remain pending. No product source or binary changed.

Eighteen further files reuse hash-bound complete reviews: [five](../../../artifacts/mvdm-audit/t476-s2-duplicate-lead-review-batch.json) resolve static leads as comments or duplicated original lines, and [thirteen](../../../artifacts/mvdm-audit/t476-s2-multihit-registered-review-batch.json) cover multi-hit projections or registered retained differences. Both batches cross-check the S1 arm ledger and real compiler line observations. The checklist now records 956 reviewed and 99 pending; no product source or binary changed.

Three additional files reuse [file-specific reviews](../../../artifacts/mvdm-audit/t476-s2-special-case-review.json), corroborated by hash-bound S1 arm observations and compiler line samples. For the EOF continuation, no subsequent source line needs mapping. The checklist now records 959 reviewed and 96 pending. No product source or binary changed.

A [1055-file direct-error scan](../../../artifacts/mvdm-audit/t476-s2-duplicate-error-findings.json) found three pending files whose S1 outer rejection retained a superseded second `#error`. These are concrete corrections, not completed reviews. Complete each file and preserve required diagnostic anchors before the next source batch build. The scan does not certify other branches or line mappings.

A [ten-file source batch](../../../artifacts/mvdm-audit/t476-s2-batch3-line-repair.json) removes the three redundant inner errors and restores eight files’ original diagnostic line anchors. All nine selected preprocessing contexts and the 30-arm exclusion verifier passed; cached x86/x64 builds succeeded. A complete ten-image package was published and verified. Eight images are byte-identical to the prior package; NTVDM and WOW32 differ only in their PE COFF/debug timestamps. Per owner instruction no product regression was repeated for these non-substantive differences. The affected files remain pending complete file-level review; the overall count is unchanged.

Four batch-three files have now completed [full conditional and line review](../../../artifacts/mvdm-audit/t476-s2-batch3-complete-reviews.json): their prior original-tree proof is rebound across the exact single `#line` insertion and all recorded hit arms have actual preprocessing context evidence. The checklist now records 963 reviewed and 92 pending; no additional product source changed.

The [PIF header review](../../../artifacts/mvdm-audit/t476-s2-pif-rebound-review.json) additionally verifies that S1 changed only its recorded unhit outer arm: bytes outside that arm exactly match the hash-bound prior full-tree review. Its eight hit arms were rechecked in the product preprocessing context. The checklist now records 964 reviewed and 91 pending.

The [WOW shared-header review](../../../artifacts/mvdm-audit/t476-s2-wow-shared-header-review.json) compares all nonblank original/current source outside the recorded rejected arms, checks surviving hit contexts and mapped lines, and records the differing non-emitted closing `#endif` directive positions. No guest source or image was modified or rebuilt. The checklist now records 966 reviewed and 89 pending.

The [first exact-byte batch](../../../artifacts/mvdm-audit/t476-s2-batch4-original-bytes.json) restored 10 host mirror headers whose normalized content already matched the pinned original. They have no conditional groups; three previously pending comment-only files now have complete product include evidence, advancing the checklist to 969 reviewed and 86 pending. Cached x86/x64 builds passed, and the full ten-image package was published and hash verified. Eight images are byte-identical and two differ only in COFF/debug timestamps; per owner instruction no product regression was rerun. Of 302 normalized-identical byte differences, 292 remain to restore.

The [second exact-byte batch](../../../artifacts/mvdm-audit/t476-s2-batch5-original-bytes.json) restored 10 already reviewed, condition-free video C sources to pinned original bytes. Cached builds and complete package staging, publication and hash verification passed; only NTVDM COFF/debug timestamp fields changed, so product regression was not rerun per owner instruction. The file-level checklist remains 969 reviewed and 86 pending; 282 exact-byte restorations remain.

The [third exact-byte batch](../../../artifacts/mvdm-audit/t476-s2-batch6-original-bytes.json) restored 10 already reviewed, condition-free video C sources to pinned original bytes. Cached builds and complete package staging, publication and hash verification passed; only NTVDM COFF/debug timestamp fields changed, so product regression was not rerun per owner instruction. The file-level checklist remains 969 reviewed and 86 pending; 272 exact-byte restorations remain.

The [ICA header review](../../../artifacts/mvdm-audit/t476-s2-ica-header-review.json) resolves the apparent empty CPU30 guard against the original and registered CCPU enum ownership, checks the original-tree mapping and rejected-body line anchors, and retains the complete source diff. Compiler line observations and the limits are recorded. The checklist now records 970 reviewed and 85 pending.

The [fourth exact-byte batch](../../../artifacts/mvdm-audit/t476-s2-batch7-original-bytes.json) restored 10 previously reviewed condition-free C sources. Cached builds and complete package staging, publication and hash verification passed; NTVDM and WOW32 differ only in COFF/debug timestamps, so product regression was not repeated per owner instruction. There are 262 exact-byte restorations remaining.

The [fifth exact-byte batch](../../../artifacts/mvdm-audit/t476-s2-batch8-original-bytes.json) restored 10 previously reviewed condition-free WOW C sources. Cached builds and complete package staging, publication and hash verification passed; NTVDM and WOW32 differ only in COFF/debug timestamps, so product regression was not repeated per owner instruction. There are 252 exact-byte restorations remaining; the file audit remains 970 reviewed and 85 pending.

The [sixth source batch](../../../artifacts/mvdm-audit/t476-s2-batch9-original-bytes.json) restored nine condition-free C/H files to original bytes and corrected the retained class-removal return line. The [class review](../../../artifacts/mvdm-audit/t476-s2-batch9-class-review.json) resolves its empty FE_SB shell in its original function context and verifies all recorded hit branches and mapped retained lines. Cached x86/x64 builds and complete package staging, publication and hash verification passed; only NTVDM and WOW32 COFF/debug timestamps changed, so product regression was not repeated per owner instruction. The checklist now records 971 reviewed and 84 pending; 243 exact-byte restorations remain.

The [exit-window source review](../../../artifacts/mvdm-audit/t476-s2-exitwin-review.json) identifies the retained WOW registration function and distinguishes a structural `LATER` shell from the omitted original shutdown function; the shell hit is not claimed as a hit of that omitted function. Its original conditional structure, retained-function difference and diagnostic line samples are recorded. The checklist now records 972 reviewed and 83 pending.

The [seventh source batch](../../../artifacts/mvdm-audit/t476-s2-batch10-original-bytes.json) restored 10 previously reviewed condition-free headers to original bytes. Cached builds and complete package staging, publication and hash verification passed; NTVDM and WOW32 differ only in COFF/debug timestamps, so product regression was not repeated per owner instruction. There are 233 exact-byte restorations remaining.

The [message source review](../../../artifacts/mvdm-audit/t476-s2-clmsg-review.json) and [input source review](../../../artifacts/mvdm-audit/t476-s2-input-review.json) check original condition groups and line anchors while explicitly identifying shells belonging to omitted original functions; a shell hit does not prove those functions are connected. The checklist now records 974 reviewed and 81 pending.

The [eighth source batch](../../../artifacts/mvdm-audit/t476-s2-batch11-original-bytes.json) restored 10 previously reviewed condition-free headers to original bytes. Cached builds and complete package staging, publication and hash verification passed; NTVDM and WOW32 differ only in COFF/debug timestamps, so product regression was not repeated per owner instruction. There are 223 exact-byte restorations remaining.

The [include-path rebindings](../../../artifacts/mvdm-audit/t476-s2-include-rebound-review.json) and [WOW header rebinding](../../../artifacts/mvdm-audit/t476-s2-wow-header-rebound-review.json) verify exact intervening source changes against the earlier hash-bound original-tree and line reviews. S1 hit records and actual compiler line observations were cross-checked. The checklist now records 977 reviewed and 78 pending.

The [ninth source batch](../../../artifacts/mvdm-audit/t476-s2-batch12-original-bytes.json) restored 10 previously reviewed condition-free headers to original bytes. Cached builds and complete package staging, publication and hash verification passed; only NTVDM COFF/debug timestamps changed, so product regression was not repeated per owner instruction. There are 213 exact-byte restorations remaining.

The [XT source review](../../../artifacts/mvdm-audit/t476-s2-xt-review.json) binds its prior complete compiler-line evidence to the S1 branch ledger and resolves the original MAC_LIKE group inside an already excluded non-CPU40 body. The [C-video CPU header review](../../../artifacts/mvdm-audit/t476-s2-cvidc-cpu4gen-review.json) checks all 842 conditional arms, the 209 excluded anchors and the exact 2,110-line original projection outside excluded bodies. Compiler-observation limits remain explicit. The checklist now records 979 reviewed and 76 pending.

The [tenth source batch](../../../artifacts/mvdm-audit/t476-s2-batch13-original-bytes.json) restored 10 previously reviewed condition-free headers to original bytes. Cached builds and complete package staging, publication and hash verification passed; only NTVDM COFF/debug timestamps changed, so product regression was not repeated per owner instruction. There are 203 exact-byte restorations remaining.

The [IOS source review](../../../artifacts/mvdm-audit/t476-s2-ios-review.json) rebinds its prior complete original-condition review through a single verified `#line` insertion, cross-checking S1 hits, excluded anchors and compiler observations. The checklist now records 980 reviewed and 75 pending. The [x64 fixture investigation](../../../artifacts/mvdm-audit/t476-s2-suballoc-x64-finding.json) reproduced an S1 #error build failure and an x86-specific count assertion; the subsequent batch below resolves both.

The [eleventh source batch](../../../artifacts/mvdm-audit/t476-s2-batch14-original-bytes.json) restored 10 previously reviewed condition-free headers to original bytes. Cached builds and complete package staging, publication and hash verification passed; only NTVDM COFF/debug timestamps changed, so product regression was not repeated per owner instruction. There are 193 exact-byte restorations remaining.

The [twelfth source batch](../../../artifacts/mvdm-audit/t476-s2-batch15-suballoc-repair.json) restores the previously witnessed x64 `suballoc.h` alternative and updates the fixture to assert actual granularity. Both x86 and x64 real fixtures build and run successfully; the S1 exclusion verifier checks 29 remaining exclusions and the restored branch. Nine condition-free headers were restored to original bytes. Cached builds and complete package staging, publication and hash verification passed; only NTVDM COFF/debug timestamps changed, so product regression was not repeated per owner instruction. The checklist now records 981 reviewed and 74 pending; 184 exact-byte restorations remain.

The [thirteenth source batch](../../../artifacts/mvdm-audit/t476-s2-batch16-context-repair.json) restores eight arms used by the actual PIF addon build, WOW get/set fixture and CCPU diagnostic build, which were omitted from S1's product-only contexts. The PIF build and WOW fixture failed before restoration and passed afterwards; the diagnostic compile and symbol check passed. Four condition-free headers were restored to original bytes. Cached product builds and complete publication passed; NTVDM and WOW32 changed only in timestamp fields, and the other eight images were byte-identical. There are 180 byte restorations remaining. Prior per-file completion counts must be cross-checked against the omitted supported contexts before being considered conclusive.

The [hash reconciliation](../../../artifacts/mvdm-audit/t476-s2-hash-reconciliation.json) found 13 stale checklist hashes, including the ten batch-16 sources and three previously pending files. Four byte-identical headers retain their prior review category; PIF is reopened for full-file review; the other modified files remain pending. The current checklist thus records 980 in prior review categories and 75 pending. Those 980 remain subject to the omitted-context qualification above.

The [CCPU header review](../../../artifacts/mvdm-audit/t476-s2-ccpu-header-review.json) compares every original line in `c_bsic.h` and `c_debug.h`, verifies their sole registered local declaration insertions and logical positions, and checks actual MSVC x86 preprocessing line witnesses for 13 declarations. Neither file has conditional arms. The checklist now has 982 in review categories and 73 pending; the omitted-context qualification continues to apply to prior records.

The [allocator and WOW header review](../../../artifacts/mvdm-audit/t476-s2-allocator-wow-header-review.json) compares every original line in `ckmalloc.h` and `wumsg.h`, their registered changes and the real product preprocessor line witnesses. Both lack conditional arms; `ckmalloc.h` contributes only macros and hence emits no text, but its restored line directive is observed. The checklist now has 984 in review categories and 71 pending, subject to the same omitted-context qualification.

The [`c_xtrn.c` review](../../../artifacts/mvdm-audit/t476-s2-cxtrn-review.json) compares the complete original file, its sole literal-zero excluded branch, the registered exception-finally adaptation and MSVC preprocessing witnesses for all three functions. It does not independently revalidate exception behavior. The checklist now has 985 in review categories and 70 pending, with the same omitted-context qualification.

The [`winconp.h` review](../../../artifacts/mvdm-audit/t476-s2-winconp-review.json) checks the original condition groups and all unchanged original line mappings, together with real x86 ANSI `cmdkeyb.c` preprocessor witnesses. The three C++/Unicode excluded alternatives remain subject to the global check of omitted fixture and addon contexts. The checklist now has 986 in review categories and 69 pending.

The [merged S1 and historical hit ledger](../../../artifacts/mvdm-audit/t476-s2-merged-branch-evidence.json) records 7,604 snapshot arms across 1,055 files, including separately verified PIF, WOW fixture, CCPU diagnostic and XMS fixture hits. Exact source hashes identify entries requiring rebinding; a missing hit is not an exclusion proof. The [69-file triage](../../../artifacts/mvdm-audit/t476-s2-remaining-groups.json) groups changed/unhit evidence, structural leads, line leads and files without automatic leads; it does not certify any of them. Historical hit matching uses exact snapshot path, line, directive and expression. The complete set of supported addon/test contexts is still to be reconciled.

The [`wow32/precomp.h` review](../../../artifacts/mvdm-audit/t476-s2-precomp-review.json) checks the full original projection and the three local branches observed in actual product builds; no original condition groups occur in that file. MSVC preprocessing observed 93 original include resumptions; three others have static line mapping only. The checklist now has 987 in review categories and 68 pending, subject to the omitted-context qualification.

The [`vdmapi.h` review](../../../artifacts/mvdm-audit/t476-s2-vdmapi-review.json) checks the complete original projection, the observed local inclusion guard and actual x86 preprocessor line positions. Only original preamble comments have shifted positions. The [ten-file batch](../../../artifacts/mvdm-audit/t476-s2-batch18-line-repair.json) completed nine line fixes and one exact-original byte restoration; cached builds and ten-image publication passed, with only two timestamp-only PE differences. The [`nturtl.h` candidate](../../../artifacts/mvdm-audit/t476-s2-nturtl-line-candidate.json) was withdrawn because its anchors were correct when counted by LF. A [39-file line-count reconciliation](../../../artifacts/mvdm-audit/t476-s2-line-count-reconciliation.json) tracks prior evidence potentially affected by non-LF control characters; its raw matches are leads rather than proven defects. The checklist now has 997 in review categories and 58 pending, with 179 exact-byte restorations not yet started.

The [`nturtl.h` review](../../../artifacts/mvdm-audit/t476-s2-nturtl-review.json) verifies the original guard and two reached declaration line positions with LF counts and actual compiler witnesses, while documenting its pre-existing declaration-subset adaptation. The [RTL `getset.c` review](../../../artifacts/mvdm-audit/t476-s2-rtl-getset-review.json) checks its original condition group, retained subset and restored fixture-hit arms against actual preprocessing. The [`ntsend.h` candidate](../../../artifacts/mvdm-audit/t476-s2-ntsend-line-candidate.json) keeps both witnessed Unicode alternatives and corrects two directive anchors; it remains pending until the next grouped build. The checklist now has 999 in review categories and 56 pending, subject to the global reconciliation qualifications.

The [client `winmgrc.c` review](../../../artifacts/mvdm-audit/t476-s2-client-winmgr-review.json) matches its only retained original function and compiler line position. The [next ten-file batch](../../../artifacts/mvdm-audit/t476-s2-batch19-line-repair.json) builds and publishes eight line corrections and two exact-original byte restorations; product differences are limited to two PE timestamps. RTL `draw.c`, client `wow.c` and client `ntstubs.c` remain open because their selected empty shells do not prove omitted original functions are implemented. The checklist now has 1,005 in review categories and 50 pending; 177 byte restorations remain.

The [empty-shell ownership leads](../../../artifacts/mvdm-audit/t476-s2-empty-shell-ownership-leads.json) identify actual WOW32 link owners and the USER32 `DrawFrameControl` import; they do not establish semantic equivalence. A [SIM32 structure lead](../../../artifacts/mvdm-audit/t476-s2-sim32-structure-lead.json) resolves one false missing-condition match nested inside the excluded non-CPU40 body, while leaving that whole file open. The [ten-source batch](../../../artifacts/mvdm-audit/t476-s2-batch20-line-repair.json) builds and publishes `host_cpu.h` and `userrtl.h` line corrections plus eight exact-original byte restorations, with only two timestamp-only PE differences. Both line-corrected files remain open for selected original empty-shell ownership. The checklist retains 50 pending files and 169 byte restorations not yet started.

The [host CPU accessor inventory](../../../artifacts/mvdm-audit/t476-s2-host-cpu-accessor-inventory.json) compares all 29 original CCPU macro names with current CPU40 definitions and static C/H references. Fifteen names have current direct macros; fourteen have neither a same-named direct macro nor a static reference in the scanned roots. Source references are not proof of product reachability, and same-named macros are not proof of semantic equivalence. The [next ten exact-byte restorations](../../../artifacts/mvdm-audit/t476-s2-batch21-original-bytes.json) built and published with nine byte-identical images and one timestamp-only image. There are 159 restorations not yet started; 50 file reviews remain open.

The [packstr review](../../../artifacts/mvdm-audit/t476-s2-packstr-review.json) uses LF counts, checks the original has no condition groups and matches the complete retained function and product preprocessor line. It does not claim that omitted functions are linked. Two source fixes are pending a ten-file build: [`inctlpan.c`](../../../artifacts/mvdm-audit/t476-s2-inctlpan-line-candidate.json) and [`profile.c`](../../../artifacts/mvdm-audit/t476-s2-profile-line-candidate.json). The former retains an unresolved selected empty shell. The checklist now has 1,006 in review categories and 49 pending.

The [Winsock data review](../../../artifacts/mvdm-audit/t476-s2-wsdata-review.json) verifies that both original and current source have no conditional arms, checks retained LF-based source lines, and identifies the registered async lifetime adaptation. The [clipboard review](../../../artifacts/mvdm-audit/t476-s2-wuclip-review.json) retains the original excluded DEBUG framework and corrects two logical-line anchors; the final grouped build and full supported-context reconciliation remain pending. The checklist now has 1,008 files in review categories and 47 pending.

The [taskman per-function review](../../../artifacts/mvdm-audit/t476-s2-taskman-line-repair.json) corrects reordered function line mappings and removes five anchors matched to the wrong original function. Both current and original have no condition arms; final build and context reconciliation remain pending. The checklist records 1,009 files in review categories and 46 pending.

The [CCPU operand header review](../../../artifacts/mvdm-audit/t476-s2-c-oprnd-review.json) binds all three recorded active PIG alternatives and three existing rejected arms to unchanged source hashes, reuses the earlier line/token witness, and identifies the x87 line-matching false positive. The x87 exception entries retain their existing gate qualifications. The checklist records 1,010 files in review categories and 45 pending; global context reconciliation and the final grouped build remain open.

The [GDP guard review](../../../artifacts/mvdm-audit/t476-s2-gdpvar-line-candidate.json) retains this file because the observed product compilation opens it via a quoted include after the generated GDP header has already defined its guard. The old body remains rejected and the closing original line has been restored; final build and cross-context reconciliation remain open. The checklist now has 1,011 in review categories and 44 pending.

The [profile review](../../../artifacts/mvdm-audit/t476-s2-profile-line-candidate.json) additionally records the actual product preprocessor witness of both retained original function entry lines after the correction. Final grouped compilation and full context reconciliation remain pending. The checklist records 1,012 in review categories and 43 pending.

The [control-panel table review](../../../artifacts/mvdm-audit/t476-s2-inctlpan-line-candidate.json) records the preprocessor witness and distinguishes a hit on the empty KOREA shell from execution of its omitted original parent function. Final compilation and cross-context reconciliation remain pending. The checklist records 1,013 in review categories and 42 pending.

The [dialog-manager review](../../../artifacts/mvdm-audit/t476-s2-dlgmgr-line-repair.json) maps 11 retained functions separately to the original, repairs source positions without changing tokens, and retains five fixture-observed local branches. Grouped build and cross-context reconciliation remain pending. The checklist records 1,014 in review categories and 41 pending.

The [WOW message-dispatch review](../../../artifacts/mvdm-audit/t476-s2-wmdisp32-line-repair.json) restores seven original excluded closing lines while retaining the observed XTALKHACK branch. The [USER client review](../../../artifacts/mvdm-audit/t476-s2-client-line-repair.json) restores the logical positions of four exact original retained functions and two excluded conditional endings; its empty DEBUG shell is not treated as evidence that an omitted function executes. Grouped build and cross-context reconciliation remain pending. The checklist records 1,016 in review categories and 39 pending.

The [dialog keyboard review](../../../artifacts/mvdm-audit/t476-s2-dlgmgr2-review.json) preserves fixture-observed keyboard branches, corrects original condition/call lines, and explicitly rejects two text-matching false positives. The checklist records 1,017 in review categories and 38 pending; grouped build and cross-context reconciliation remain pending.

The [snapshot rebind batch](../../../artifacts/mvdm-audit/t476-s2-snapshot-rebind-batch.json) verifies nine historical marker versions differ only by line directives. The [four-file branch review and repair](../../../artifacts/mvdm-audit/t476-s2-rebound-four-line-repair.json) reuses those hits for timer, VGA ports, WOW32 and character translation, preserves selected text, checks each rejected arm and restores original branch anchors. The [drawing review](../../../artifacts/mvdm-audit/t476-s2-draw-review.json) separately proves its retained functions exact and identifies the empty original DrawFrameControl shell. The checklist records 1,022 in review categories and 33 pending; grouped build and global context reconciliation remain pending.

The [four-file conditional review](../../../artifacts/mvdm-audit/t476-s2-four-more-line-repair.json) checks VDM redirector mailslot/network, WOW kernel manager and USER kernel stubs against hash-bound branch evidence, retained original lines and empty rejected bodies, correcting original conditional anchors only. The checklist records 1,026 in review categories and 29 pending; grouped build and global context reconciliation remain pending.

The [host event review](../../../artifacts/mvdm-audit/t476-s2-nt-event-review.json) retains hash-bound reached branches, confirms excluded bodies contain only directives, and restores original conditional closing positions. The checklist records 1,027 in review categories and 28 pending; grouped build and global context reconciliation remain pending.

The [keyboard controller](../../../artifacts/mvdm-audit/t476-s2-keyba-review.json), [USER queue](../../../artifacts/mvdm-audit/t476-s2-queue-review.json) and [PIF header](../../../artifacts/mvdm-audit/t476-s2-pif-review.json) reviews retain their observed branches and correct original diagnostic positions. The PIF review explicitly includes the formal addon context that the old product-only record missed. The checklist records 1,030 in review categories and 25 pending; grouped build and global context reconciliation remain pending.

The [USER resource review](../../../artifacts/mvdm-audit/t476-s2-clres-review.json) aligns 13 retained functions independently, checks ten excluded arms and restores original conditional and statement positions without changing selected tokens. The checklist records 1,031 in review categories and 24 pending; grouped build and global context reconciliation remain pending.

The [USER handle-table review](../../../artifacts/mvdm-audit/t476-s2-handtabl-review.json) verifies the retained original ownership table and its recorded conditions, and restores the table and conditional diagnostic positions. The checklist records 1,032 in review categories and 23 pending; grouped build and global context reconciliation remain pending.

The [WOW client subset review](../../../artifacts/mvdm-audit/t476-s2-wow-client-review.json) proves the retained _GetDlgItem body and source lines, rebinds the original marker evidence, and explicitly does not equate selected empty shells with execution of omitted functions. The checklist records 1,033 in review categories and 22 pending; grouped build and global context reconciliation remain pending.

The [printer review](../../../artifacts/mvdm-audit/t476-s2-printer-review.json) checks recorded branch hits and rejected arms, corrects original conditional and selected setup-table line positions, and preserves non-line bytes. The checklist records 1,034 in review categories and 21 pending; grouped build and global context reconciliation remain pending.

The [EGA mode-1/2 writer review](../../../artifacts/mvdm-audit/t476-s2-egwrtm12-review.json) maps twelve missing nested original conditions to legacy bodies rejected under the registered CPU40 carrier, rather than restoring conditional structures inside an excluded body. It retains actual hit arms and restores original diagnostic lines. The checklist records 1,035 in review categories and 20 pending; grouped build and global context reconciliation remain pending.

The [EGA mode-0 writer review](../../../artifacts/mvdm-audit/t476-s2-egawrtm0-review.json) resolves missing nested original conditions against two rejected CPU40 legacy mover bodies and a rejected SEGMENTATION parent, restores retained function and condition positions, and verifies excluded bodies. The checklist records 1,036 in review categories and 19 pending; grouped build and global context reconciliation remain pending.

The [USER client-stub review](../../../artifacts/mvdm-audit/t476-s2-ntstubs-client-review.json) matches five retained original functions by identity, validates their source positions and preserves a fixture-only hit missed by the original product-only evidence. The checklist records 1,037 in review categories and 18 pending; grouped build and global context reconciliation remain pending.

The [mnemonic drawing review](../../../artifacts/mvdm-audit/t476-s2-drawtext-review.json) aligns three retained function bodies and condition positions while preserving the exception-safe local scratch lifetime. The checklist records 1,038 in review categories and 17 pending; grouped build and global context reconciliation remain pending.

The [USER server review](../../../artifacts/mvdm-audit/t476-s2-server-review.json) restores original default-message table, initializer and conditional positions while preserving hit branches and selected bytes. The checklist records 1,039 in review categories and 16 pending; grouped build and global context reconciliation remain pending.

The [BIOS reset review](../../../artifacts/mvdm-audit/t476-s2-reset-review.json) and [multi-context class header review](../../../artifacts/mvdm-audit/t476-s2-ntcftxt-review.json) preserve recorded hit branches, check rejected bodies and restore original statement/conditional positions without changing selected code. The checklist records 1,041 in review categories and 14 pending; grouped build and global context reconciliation remain pending.

The [EMS manager review](../../../artifacts/mvdm-audit/t476-s2-emm-review.json) restores the original non-NTVDM conditional framework while retaining the registered CPU40 LIM writeback through its existing explicit selector. Source inventory now finds no missing original branch for that file; selected original body bytes and original line positions are checked. This conditional change requires the final grouped build and appropriate EMS verification if the output changes. The checklist records 1,042 in review categories and 13 pending; global context reconciliation also remains pending.

The [SIM32 structural review](../../../artifacts/mvdm-audit/t476-s2-sim32-review.json) verifies recorded hit arms and retained original lines, and identifies a nested original debug condition already contained in the rejected non-CPU40 body. It does not claim complete equivalence of the existing CPU40 descriptor/exception adapter. The checklist records 1,043 in review categories and 12 pending; grouped build and global context reconciliation remain pending.

The [CPU40 main-loop review](../../../artifacts/mvdm-audit/t476-s2-cmain-review.json) checks the actual product and explicit diagnostic contexts, retains all selected execution tokens and restores original branch and statement line positions; it does not claim complete CPU exception equivalence. The checklist records 1,044 in review categories and 11 pending; grouped build and global context reconciliation remain pending.

The [RTL SDK header review](../../../artifacts/mvdm-audit/t476-s2-ntrtl-review.json) restores original inline and conditional positions, and records that its S1 _NTSYSTEM_ exclusion still needs global actual-context reconciliation. The checklist records 1,045 in review categories and 10 pending; grouped build and global context reconciliation remain pending.

The [host graphics review](../../../artifacts/mvdm-audit/t476-s2-ntgraph-review.json) preserves recorded hits and excluded bodies, maps retained functions to their own original identities, and restores original conditional and statement positions. Its static line projection does not replace final preprocessing or runtime V7VGA checks. The checklist records 1,046 in review categories and 9 pending; grouped build and global context reconciliation remain pending.

The [video source review](../../../artifacts/mvdm-audit/t476-s2-video-review.json) restores the original NTVDM/MONITOR-or-GISP condition and nested original else structure, preserving the selected standalone SAS behavior. A 16-combination source-level condition comparison preserves prior outcomes; the identical SAS bodies in mutually exclusive branches are documented explicitly. Original condition inventory has no unmatched arms or ancestry conflicts. The checklist records 1,047 in review categories and 8 pending; actual compilation, video behavior and global context reconciliation remain pending.

The [mouse source review](../../../artifacts/mvdm-audit/t476-s2-mouse-review.json) preserves the source-bound branch hits, independently aligns selected functions and records that thirteen apparent missing nested groups belong inside already rejected outer bodies. Its line projection is static; actual mouse regression and context verification remain pending. The checklist records 1,048 in review categories and 7 pending.

The [USER RTL header review](../../../artifacts/mvdm-audit/t476-s2-userrtl-review.json) moves the unchanged same-address REBASE definitions into the observed original non-_USERK_, non-WOW branch and binds 37 recorded product contexts. A separate header defines _USERK_; any unobserved supported combined include remains a global reconciliation question. The checklist records 1,049 in review categories and 6 pending; grouped build and global context reconciliation remain pending.

The [host CPU header review](../../../artifacts/mvdm-audit/t476-s2-hostcpu-review.json) distinguishes the rejected original CPU30 nested condition from the selected CPU40 little-endian adapter, and checks 101 obsolete pointer-vector names against current source, tests and tools. The original/current reg union size difference remains a registered behavior caveat, not a claim of complete CPU layout equivalence. The checklist records 1,050 in review categories and 5 pending; grouped build and global context reconciliation remain pending.

The five-file parallel pass reviewed [DPMI](../../../artifacts/mvdm-audit/t476-s2-dpmi-worker.json), [VDM client](../../../artifacts/mvdm-audit/t476-s2-vdm-worker.json), [USER client header](../../../artifacts/mvdm-audit/t476-s2-usercli-worker.json), [USER header](../../../artifacts/mvdm-audit/t476-s2-user-review.json), and [USER kernel header](../../../artifacts/mvdm-audit/t476-s2-userk-review.json). Four have candidate line-position changes; compiler and supported-context verification remain open. Existing product-only exclusions are not automatically proven by absence of hits. The grouped x86 build was stopped on owner direction at 299/470; it is incomplete and provides no build acceptance. No publication or tests followed.
