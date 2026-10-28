# T473 S10 tools retention review

The owner explicitly retains tools/opennt, win31-path, win31-setup,
win95-launch, win95-path and win101-setup. This is a directory/ownership
review; it does not certify every historical script for current execution.

| Directory | Decision | Evidence and boundary |
| --- | --- | --- |
| opennt | Keep | Owner-designated original-tool closure; retain provenance and licenses. |
| win31-path | Keep | Active APPLY/UNAPPLY, discovery through the released path31.exe. |
| win31-setup | Keep | Active media-copy preparation and profile generation. |
| win95-launch | Keep | Offline launch preparation and exact journaled recovery. |
| win95-path | Keep | Offline CREG/INI/PIF/GRP/LNK relocation and recovery. |
| win101-setup | Keep | Canonical scripts; older copies under src/addon removed after comparison. |
| build | Keep active engineering owners | CMakeLists.txt directly calls New-T310OriginalSoftpcNinja.ps1. Stage-AddonUtilityRelease, Stage-MouseDriverRelease and Stage-Compat95Release are actual release owners. A dated name is not sufficient to archive a script. |
| audit | Keep current observers and acceptance | Invoke-ProductVerification.ps1 owns the selected Full product suite; source/dispatch observers support the active MVDM reconciliation. Separate obsolete experiments by actual consumers. |
| governance | Keep current gates | EXECUTION requires Verify-DocumentationGovernance and the relative-link check. Older task-specific checks need individual consumer review, not wholesale directory deletion. |
| probe | Keep reusable probes; archive dated experiments with their fixtures | Inspect-SoftPcCompositionCandidate, Invoke-CliNativeControlledStopObservation and Read-StartupSnapshotEvidence are general diagnostic entrypoints. The four T95/T148/T198/T199 entrypoints are historical experiment candidates; T95 still has an explicit companion fixture caller, which must travel with it. |
| historical | Archive whole obsolete closures with old consumers | 247 files, 9,166,213 bytes at initial inventory. Original DOS/OpenNT toolchains and old project snapshots live here. Build-T225S8PrimaryCommand/Ntio/Ntdos/Share and Build-T235S5GuestFallbacks still refer to the old paths. They are not current permission to rebuild guests; preserve historical context and move them together. |
| import | Archive the one-time importer with its closure | Import-MsDos6QB45Tools.ps1 defaults to an old D: source path and populates historical/msdos6-qb45. No current CMake/product/authority call was found. Preserve import provenance; it is not a runtime dependency. |
| observation | Archive the old startup pair with matching evidence, or merge a genuinely current replacement into build/probe | Two old entrypoints. The cmd wrapper writes build/tools directly; current selected acceptance uses the formal observer/runtime pipeline. No current caller was found in CMake, active tools/tests or current design/execution authorities. |
| research | Archive the T335 generator and its ledger provenance | Generate-WowPointerPlaneLedger.mjs explicitly targets T335/S3, the removed src/mvdm-host/wow32 tree and an old ledger output. It cannot be presented as a current general-purpose source-audit entrypoint. |
| pif | Removed duplicate directory | No active code consumer found. PIF.EXE is built from src/addon/pif and consumed from assets/release. The old tool-copy SHA differed from the release; usage documentation moved to its source component. |
| hash | Removed duplicate directory | Source under src/addon/hash, released HASH.EXE, no active tool-directory consumer. Usage documentation moved with the source. |

The six requested directories remain. The current build/audit/governance
owners remain executable engineering source. Historical/import/observation/
research relocation is a dependency-aware archive recommendation, not a claim
that those directories have already moved. No historical toolchain is deleted
by this directory review. The archive needs its byte/hash manifest and a
no-default-build/no-runtime label; dead commands are recoverable from matching
Git history rather than being repaired into a second current build system.

Search scope for current callers: root CMake/presets, tools, tests, current
design/execution authorities. Historical report text alone does not make an
executable current. No static reference is not proof that a documented manual
engineering tool has no users.

The unreachable old Win3.1 bodies following exit /b in both Win95 path CMD
wrappers are removed; their live seven-line entry paths are retained. This
corrects the initial mistaken inference that Win95 actively called the
Win3.1 discovery helper. No public Win95 behavior is removed by that cleanup.
