# T484 S3 P212: retire obsolete single-image publishers

This retrospective tool cleanup removes StageProductExecutable.mjs and
StageWow32Provider.mjs. A current tools/test/CMake reference scan finds no
executable callers. Historical T404/T406/T412 evidence and the old proposal
remain as provenance; EXECUTION.md's still-imperative single-image invocation
is corrected to the already adopted coherent-package publication rules.

The first script publishes ntvdm32.exe into build/output and O:/ntvdm64, then
removes ntvdm.exe and other names. The second publishes WOW32 alone without the
current package manifest. Neither represents the current eleven-image package,
selected x86/x64 mix or dual publication. Migrating them would perpetuate an
obsolete second publication route. No replacement producer is introduced.
The current Stage-System32ProductPackage.ps1 and addon staging owners are not
removed: their CMD conversion remains S4, not silently treated as complete.

Owned PID26824 started2026-11-24T14:15:28.2400946Z runs
build/t484-s3-obsolete-publishers/run.ps1. It verifies cached full host build,
eleven raw input hashes and22 dual-published file hashes. Original guest,
product sources and published bytes remain unchanged; no fresh runtime claim.
result.json records its actual exit; closure-result.json records independent
tracker/documentation/diff checks before commit and push. No runtime test is
required for removing two uncalled scripts without product implementation changes.

The frozen355-file inventory moves from316 disposed/39pending to318/37.
The23 supplementary preparation chains remain pending; these counters are
not additive and are not completion percentages. S3 remains active.
