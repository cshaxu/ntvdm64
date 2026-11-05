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
