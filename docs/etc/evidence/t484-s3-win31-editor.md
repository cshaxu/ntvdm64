# T484 S3 P37: native Win31 editor controller

The DIR-ordered B0023 editor runner previously constructed assembly by string
replacement and directly invoked NASM. Its three insertion sites are now
explicit optional NASM includes in the independently authored startup witness.
The existing actions/routines/events remain unchanged; editor data moves from
the PowerShell string into editor-data.inc. CMD invokes the native declarative
build with the same destination and five options, then copies its cached output
into the fresh test task. No original guest, mirror or product source changes.

All20 supported switch combinations produce byte-identical EXEs compared with
the old generator. Twelve combinations rejected by the original runner are
also rejected by the native build. The initial test incorrectly tried all32
as successful builds and reached the original assembly's invalid-combination
error; its receipt is retained. The final matrix applies the original runner
contract and asserts both successful bytes and rejected combinations.

The unchanged plain startup witness is byte-identical with all new hooks
disabled. No-op builds preserve the cached executable timestamp and hash;
spaced output works and an existing destination is rejected. The modified
PowerShell runtime runner parses successfully. No new guest/editor runtime
pass is claimed: byte-identical controllers reuse the existing runtime
behavior evidence under the owner's no-repeat authorization.

The final receipt records cached full host compilation,11 raw host hashes,
22 published hashes and documentation governance. Unchanged production images
need no recopy. Source/config generation and assembly now require only CMD,
CMake/Ninja and NASM, but B0023 remains pending until its transitive runtime
matrix and shared staging producers have also been reconciled.

[Matrix and adjacent receipts](../../../artifacts/build-migration/t484/s3-win31-editor/verification.json)
record the owned processes and actual results. The containing commit delivers
P37. S3 is active; S4 and S5 are not complete.
