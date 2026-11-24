# T484 S3 P153 obsolete build closure retirement

## Per-entry decisions

| Entry | Disposition and evidence |
| --- | --- |
| B0242 VerifyMvdmFrozenGraphSelection.py | Completed frozen-inventory comparison utility, no current callers. Remove executable recomputation; retain frozen CSV and existing discrepancy/identity evidence. Other retention-analysis consumers of the frozen CSV are unchanged. |
| B0262 BuildCcpuExceptionProfile.cmd | Orphan compiler of historical generated exception-profile/prod-environment/before sources; no current producer or caller. Retire. |
| B0263 BuildCcpuIpProfile.cmd | Orphan compiler of historical generated ip-profile.c; no current producer or caller. Retire. |
| B0264 BuildConsoleRegression.cmd | Duplicate private compilation of terminal/startup observers; current fixtures/CMakeLists.txt already owns both. Retire unused wrapper. |
| B0265 BuildConsoleResize.cmd | Orphan historical compile wrapper using retired session archive/layout, no current caller. Retire. |
| B0266 BuildTree.cmake | Retain native shared tree/cache configuration used by root and eight child CMake owners. No forbidden interpreter. Current cached builds already exercise it. Its script-mode path remains used by Test-BuildHygiene. |
| B0278 GenerateCvidcVectorProviderStubs.mjs | Inert-symbol generator for historical focused binding experiment, no current caller. Retire; current actual-vector audit was verified in P149. |
| B0281 Initialize-NinjaTree.ps1 | Unused interpreted wrapper around native BuildTree. Current CMake owners include BuildTree directly. Retire wrapper, retain native implementation. |
| B0287 New-T184S3V5EvidenceRoot.ps1 | Frozen T182/T184 custom-profile staging tool for retired runner contract, no current caller. Retire without modifying guest assets or historical evidence. |
| B0336 New-T95S6LinkArgumentVector.ps1 | Frozen exact link-option vector utility with only its historical wrapper and two self-tests. Retire that whole four-file closure; no product/build caller remains. |

## Verification and limits

Read every removed entrypoint and the T95 transitive consumers before deletion.
Post-deletion searches of tools, test and root CMake find no references to the
removed tool names. The historical documentation remains indexed evidence.
T95 self-tests only exercised the deleted argument-vector generator; they did
not build/link a current product or validate a current runtime contract.

Twelve files,441 lines removed. No production graph/source, native tool,
guest or release image changed. Reuse P151 cached full/11 raw and22 published
identity evidence for unchanged build inputs; do not claim a new runtime,
full build or publication. Documentation/tracker and whitespace gates run for
this batch. Ten original inventory dispositions are complete:280 of355,75
remain. Earlier incomplete cases and S4/S5 remain open.
