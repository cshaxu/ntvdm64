# Project Status

## Current Work

**Active: M0 T484 S2** (Ordinary Mode). Owner requested continuous execution
through T484 closure. S1 inventory/design is complete as bounded research;
implementation and fresh build acceptance remain outstanding.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T484 S2, Ordinary Mode. |
| Admission And Approval | Owner: 准入新t任务！要求构建脚步去除python powershell javascript 仅使用cmd. |
| Original Owner Request | Admit unified build/release; CMD-only build scripts, removing Python, PowerShell and JavaScript dependencies. |
| Objective | Replace host configuration, source generation and compile/link orchestration with CMD and native declarative build tools; preserve all eleven image closures and caches. |
| Candidate Proposal | [Unified build/release](../proposals/proposal-unified-build-cmd-release-001.md) |
| Reference Baseline | T483 closure e74afba09; product P85 9e7eb19c0, package100b16d9-992d-4e2c-8d99-66a4e891dd26. |
| Files And ABI Surface | Build entrypoints, CMake/Ninja generators, generated sources, addon/test compilation and publication tools; no product ABI changes at admission. |
| Applicable Rules | CMD-only project build scripting, existing layers, source-owner inventories and caches, immutable original guest, truthful failures. |
| Verification | Source/flag/export inventory comparison, real x86/x64 compilation, targeted regression, coherent dual publication, documentation/link/diff gates; final interpreter-free clean acceptance remains S5. |
| Expected Markers | Canonical declarative source groups, CMD entry, generated source identity, native toolchain and cache identity, no forbidden interpreter in the migrated host path. |
| Asset Needs | Existing source/toolchains and approved assets only. |
| Non-goals | No runtime redesign, mirror audit, original guest rebuild or wholesale standalone test/audit language conversion. |
| Similar-Issue Sweep | Nested interpreters, configure hooks, generated commands, post-link steps, manifest/hash tools, addon/test producers and clean bootstrap. |
| Reporting Requirements | Separate admission, inventory, implementation and delivery; count remaining forbidden-interpreter build edges and output coverage. |
| Stop Conditions | No forbidden interpreter behind CMD or another script mode; no parallel build graph or stale output as clean-build proof. |
| Exit Criteria | Eleven host images built through the migrated host path, preserved source/ABI boundaries, applicable tests and coherent published/committed/pushed delivery; no cached-only clean-build claim. |

## Current Progress

S1 [inventory/design](../etc/evidence/t484-s1-build-inventory.md) records817
script/metadata inputs,355 ordered migration candidates,1116 literal references
and9 existing graphs containing21 interpreter command rules. Candidate counts
are not proved live dependencies. All11 host and14 addon outputs are accounted
for. S2 P1 migrates two native data generators; the remaining candidate
dispositions are not marked complete.

The [execution plan](../etc/operations/t484-cmd-build-release-plan.md) defines
the whole task. S2 now owns implementation; addon/test producer migration and
release consolidation remain sequential S3/S4 obligations. Existing release
images and previous-task staging directories must not become bootstrap inputs.

[S2 P1](../etc/evidence/t484-s2-native-generators.md): removed the interpreted
font/C-VID generators; all5 generated files are byte-identical to production.
Final cached full build passed. Eleven images published to both destinations,
22 hashes verified; package9459f9ca-4f2f-4643-bac7-e4c2b92149bb. Raw input
comparison:5 identical,6 PE-timestamp-only. Generator tests, linked C-VID
contract and9-graph strict boundary checks passed. Documentation, links and
diff checks passed. The containing S2 P1 commit is this batch's delivery
identity. Root CMake still requires interpreters; S2 is not closed.

[S2 P2](../etc/evidence/t484-s2-native-tib.md) removes the JavaScript post-link
TIB check. Ten actual-product/rejection cases and cached full compilation
passed. Both publication locations carry package
d4f65024-538c-4365-a9df-2240cd637f0b;22 hashes and post-publication targeted
checks passed. Five raw inputs are identical and six differ only by PE
timestamps. Mirror code delta is zero. S2 remains active; graph generators,
addon/test producers and publishing scripts are still migration work.

[S2 P3](../etc/evidence/t484-s2-hook-cmake.md): Hook64 now uses native CMake
targets; its old PowerShell generator is removed. All25 production sources
remain, cached full build and dual publication passed. Current package:
a56f5975-e552-4ccb-96a3-55fd0e3622d0;22 hashes verified. Post-publication
Hook lifecycle118 assertions, machine tests and strict boundaries passed.
Four raw images are identical/seven timestamp-only. Full macro-profile audit
still reports five unchanged WOW units, reproduced with the prior test;
resolve during WOW migration before S2 closure. Three host graph generators
remain. S3-S5 are not yet admitted.

## S1 Closure Record

[S1 inventory/design](../etc/evidence/t484-s1-build-inventory.md) completes
bounded research and fixes the migration worklist. No production build,
publication or runtime pass is claimed. S1 P2 carries this evidence and S2
admission; its containing commit is the delivery identity.

## Current Technical Baseline

[T483 closure](../history/m0-t483-closure.md) indexes all S1-S21 outcomes,
retained mirror decisions and known boundaries. Final production revision is
9e7eb19c0 (S20 P85), package100b16d9-992d-4e2c-8d99-66a4e891dd26.
The current publication is T484 S2 P3, recorded above; P85 is its behavior
comparison baseline. P85 Full20/20 runtime evidence is reused for this
byte-equivalent/timestamp-only build change, not reported as a new Full run.

S19 completed38/38 B-to-C2 files; S20 completed94/94 B-to-C1 files.
Private compiled headers/search roots and linked implementation dependencies
from B to C1/C2 are zero. Legal synchronous callbacks and original named-image
APIs remain explicit; C1/C2 preserve original OpenNT connections.

P85 non-error mirror delta+4points/+244lines; S20 cumulative+15/+815;
S19 cumulative+12/+574. Retained decisions are not blanket-approved by closure.
The inherited environment failure and other boundaries remain in
[TODO](TODO.md) and the closure record. No product or guest change in S21.

## Recent M0 Closures

- [T483](../history/m0-t483-closure.md).
- [T482](../history/m0-t482-closure.md).
- [T481](../history/m0-t481-closure.md).

## Recent Governance

T484 S1 P2 records inventory/design and admits S2 under the owner's full-task
authorization. Documentation, relative-link and diff checks passed. The containing
commit is its delivery identity; this does not claim a new product build, test
or publication. Queue order is unchanged.
