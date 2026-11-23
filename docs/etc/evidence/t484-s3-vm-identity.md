# T484 S3 P38: shared VM identity assembly

B0024 and B0026 use the same four authored sources: DOS identity, retirement,
reuse and Win16 controller. Their assembly calls now use one CMD/native graph
and a shared background-process delegation function. All switches remain at
the existing scenario owner. Reuse output is built under the fresh build task
before copying to the test-owned installation. Original guest and production
sources are unchanged.

Twenty-five source/define combinations match direct original NASM output byte
for byte. Cases cover all four sources, normal/native/COMSPEC continuation,
keyboard, blocked keyboard, reuse, initialization failure, shutdown, Win95
ordering, guest input, both codepages and all five unread-input modes. The
initial macro-name validation incorrectly rejected the digit in WIN95; the
final validation accepts normal identifier digits and the entire matrix passes.
Initial failure evidence is retained rather than counted as a pass.

The shared function and both callers parse; the matrix executes the actual
shared function and CMD entry. Cache checks cover unchanged output/hash/time,
a spaced output filename, invalid macro value rejection and refusal to replace
an existing output. Native command lines preserve all actual NASM defines.
No new guest runtime acceptance is claimed: identical probe bytes reuse prior
runtime evidence under the owner's no-repeat authorization.

Final receipts record cached full host compilation,11 raw host hashes,
22 published hashes and documentation governance. No changed product images
require recopy. Both inventory rows remain pending because per-installation
binary-field filling and the shared runtime/staging producers are still under
reconciliation; assembly migration is not complete-chain migration.

[Matrix and adjacent receipts](../../../artifacts/build-migration/t484/s3-vm-identity/verification.json)
record actual process/result evidence. The containing commit delivers P38;
S3 and the subsequent S4/S5 acceptance remain incomplete.
