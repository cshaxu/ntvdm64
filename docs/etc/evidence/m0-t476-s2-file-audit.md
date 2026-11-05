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
