# Project Status

## Current Work

**Active: M0 T483 S11** (Ordinary Mode). Owner admitted cleanup of A-layer build dependencies after [S10 closure](#s10-closure-record). Baseline 49d0ce087.

## Active Packet

| Field | Record |
| --- | --- |
| Original owner request | 合并B层和C层好了，先不管BC之间的相互依赖。接下来在本T中 准入一个S11，清理A->B的违规，把该挪到A的东西从B挪到A |
| Objective | Both A libraries (`lib-win32`, `lib-nxvm`) build independently of the merged BC layer (`compat`, `mvdm-host`, `non-mvdm`). BC internal edges are outside S11. |
| Files And ABI Surface | Audit production compilation and link inputs for A-owned code across x86 and x64. Remove inherited historical build inputs. Move genuinely generic declarations or implementations from compat to their owning A library when a real A caller requires them. Preserve existing calling conventions, exported names, ordinals and caller behavior. |
| Reference Baseline | The [WOW32 generator](../../tools/build/New-T404S5Wow32ProviderNinja.ps1) compiles [the A-owned CRT unit](../../src/lib-win32/crt-compat/crt_compat.c) with a forced compat nt.h. Its explicit includes are standard headers. Also inspect native service flags in [the native generator](../../tools/build/New-NativeWorkerNinja.ps1) and the [x86 generator](../../tools/build/New-T310OriginalSoftpcNinja.ps1). |
| Applicable Rules | Apply the architecture, coding, source-layout and execution authorities indexed in [the guide](../README.md). Keep the two A libraries independent. Preserve historical declarations with their owner unless the source and actual caller justify relocation. Other layer violations are deferred. |
| Verification | Inventory A compilation units for each product target, inspect effective commands, compiler-recorded header closures and resolved object references. Repeat after cached production rebuilds. Require zero A-to-BC dependencies, no new A sibling edge, and passing affected ABI and consumer checks. Record each moved item, former and new owner, caller and verification, or the evidence that removing flags suffices. |
| Asset Needs | Use the existing release inputs and build trees. Follow [execution and publication rules](../rules/EXECUTION.md) for implementation delivery. Stop for owner decision if resolution requires changing a historical ABI or extending the packet to another layer. |
| Candidate Proposal | [T483 parent proposal](../proposals/proposal-unconnected-mirror-diff-cleanup-001.md) |
| Identifier Mode | M0 T483 S11, Ordinary Mode. |
| Admission And Approval | Owner reopened T483 for S11 following accepted S10. |
| Non-goals | BC internal dependencies and other layer violations. |
| Expected Markers | No BC header, forced include or resolved BC provider in A-owned production units. |
| Similar-Issue Sweep | Check both A libraries across native, worker, WOW32 and Hook builds. |
| Reporting Requirements | Record affected objects, flags, dependency paths and ownership decisions. |
| Stop Conditions | Escalate any required ABI change or expansion beyond A dependencies. |
| Exit Criteria | Evidence covers every selected A unit, relevant tests and verified product delivery. |

## Current Technical Baseline

S11 P2 isolates the WOW32 production CRT object from the BC headers. Its native flags retain the x86 CRT, calling convention and optimization settings. No declaration or implementation moved. The cached x86 and x64 release builds passed. The [CRT regression](../../test/lib-win32/crt-compat/verify-crt-compat.ps1) checked the production command, compiler dependency record, both link maps, formatting with the actual WOW32 object, and missing/duplicate provider failures. All eleven images of package 1941b6a0-3dc9-4549-a420-64c4a06c13b2 were hash-verified at both release destinations. Build, test and publication receipts are under build/t483-s11-p2. S11 remains active for the other A-library compilation contexts.

S10 delivered the frontend-base and nthook-dll layout at 14969ed80. S11 admission starts from the accepted closure at 49d0ce087.

## S10 Closure Record

Owner accepted and closed T483 S10. P1 shipped at 14969ed80 with the targeted verification recorded in the [S10 delivery and closure record](../etc/evidence/m0-t483-s10-components.md).

## S9 Closure Record

[40-file compat audit and verification](../../artifacts/t483-s9-non-mirror-audit/sequential.md), delivered at 55643b6c5. [Successor packet delivery](../etc/evidence/m0-t483-s10-components.md) records this revision as its baseline.

## S1 Closure Record

[T483 S1 closure](../etc/evidence/m0-t483-s1-closure.md).

## S2 Closure Record

[T483 S2 closure](../etc/evidence/m0-t483-s2-closure.md).

## S3 Closure Record

[T483 S3 closure](../etc/evidence/m0-t483-s3-closure.md).

## S4 Closure Record

[T483 S4 closure](../etc/evidence/m0-t483-s4-closure.md).

## S5 Closure Record

[T483 S5 closure](../etc/evidence/m0-t483-s5-closure.md).

## S6 Closure Record

[T483 S6 closure](../etc/evidence/m0-t483-s6-closure.md).

## S7 Closure Record

[T483 S7 closure](../etc/evidence/m0-t483-s7-closure.md).

## S8 Closure Record

[T483 S8 closure](../etc/evidence/m0-t483-s8-closure.md).

## Recent M0 Closures

- [T483 S7 closure](../etc/evidence/m0-t483-s7-closure.md)
- [T483 S6 closure](../etc/evidence/m0-t483-s6-closure.md)

## Recent Governance

Each sequential-file entry and exit reports both file and whole-audit totals.
The unrelated project [TODO](TODO.md) remains separate.
