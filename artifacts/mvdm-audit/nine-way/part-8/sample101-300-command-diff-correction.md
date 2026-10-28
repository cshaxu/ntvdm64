# Correction: three COMMAND source mirror diffs remain partial

This correction supersedes the recommendations for exactly the following three paths in `sample101-300-partial-five-review.md` and `existing-classifications-sample101-300-corrections.csv`:

- `dos/v86/cmd/command/init.asm`
- `dos/v86/cmd/command/rdata.asm`
- `dos/v86/cmd/command/stub.asm`

The current mirror has actual source differences, and the owner requires **all mirror-diff source entries to remain partial**. Accordingly all three retain `partial`; they must not be changed to pending because source-to-binary or actual path evidence is missing. Their real remaining obligations are to attribute each diff and registered exception, bind actual source/body to the immutable COMMAND binary where possible, and validate the corresponding behavior. The existing generic COMMAND test does not itself prove every source hunk. This is a status-governance correction, not evidence that those runtime obligations passed.

The 101–300 frozen original sample and corrections CSV are deliberately not modified: the coordinator already used the latter as frozen input for an 83-row conservative rollback that omitted these three. Preserve exact original artifacts and refer to this correction when reading them. The other two partial guest binaries in the sample remain subject to their separately reviewed hash/load evidence. No shared ledger, source, guest or product changed.
# Historical snapshot: current S10 classifies host C/H files only. Guest and other artifact records below remain supporting history, not current per-file scope.
