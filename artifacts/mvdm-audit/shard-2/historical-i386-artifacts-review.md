# Shard 2 retained historical i386 build artifacts

Frozen source ledger SHA-256 `F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196`. This batch selects 59 pending current-only paths below `obj/i386` with obj/lib/res/exp/pdb/dll extensions; exact paths and frozen/live hashes are in `historical-i386-artifacts.csv`. Every live file matched its frozen hash. No file was executed or deleted.

Read-only `ninja -t inputs` transitive closures, paths resolved against each Ninja directory and compared case-insensitively to the exact repository artifact paths:

- `ntvdm.exe`: `build/.ninja/msvc-x86-release/worker`, 1014 entries, SHA-256 of raw Ninja stdout `D95E467D9E75C95737DEFE3ADEC70014681D371D76AEF134A828AFC3947DEC4C`.
- `VDMREDIR.dll`: `build/.ninja/msvc-x86-release/worker`, 1105 entries, SHA-256 of raw Ninja stdout `C653E688DFE76D508ABECC63B72CCA20B52E2E66901433FD91B970F59A337BD2`.
- `wow32.dll`: `build/.ninja/msvc-x86-release/wow32`, 454 entries, SHA-256 of raw Ninja stdout `A1C4813C11EBD0640571CA4C388A91675C1E2E800440E8943FE6A2E4CAFBA276`.

None of the 59 exact paths appears in any of these graphs. `build/package-uuid/package-c` contained 83 files when inspected; `historical-i386-package-hashes.csv` records each relative path and actual SHA-256. None of the 59 artifact hashes matches a package file. A linked executable necessarily has a different hash from its constituent objects; the build-closure comparison, not the package hash check alone, supports this input disposition. No statement is made about other build graphs or later packages.

All 59 retained artifacts are excluded **only as selected input to these product graphs and as verbatim package files**. Their original/source-derived functionality and guest capabilities remain separate pending or partial decisions. No capability is deemed removed or restored by this classification.
