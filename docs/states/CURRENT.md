# Project Status

## Current Work

**Active: M0 T484 S3** (Ordinary Mode). Owner requested continuous execution
through T484 closure. S2 host migration is complete; S3 addon/test producers
are now admitted. Final interpreter-free clean acceptance remains S5.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T484 S3, Ordinary Mode. |
| Admission And Approval | Owner: 准入新t任务！要求构建脚步去除python powershell javascript 仅使用cmd. |
| Original Owner Request | Admit unified build/release; CMD-only build scripts, removing Python, PowerShell and JavaScript dependencies. |
| Objective | Migrate all14 authored addon producers and test-binary production callers to CMD/native declarative targets; retire the old worker generator and interpreted producer chains while preserving actual source/flag/ABI closures. |
| Candidate Proposal | [Unified build/release](../proposals/proposal-unified-build-cmd-release-001.md) |
| Reference Baseline | T483 closure e74afba09; product P85 9e7eb19c0, package100b16d9-992d-4e2c-8d99-66a4e891dd26. |
| Files And ABI Surface | Addon/test build entrypoints, native CMake/ASM/RC generation and consumer references; no original guest or product ABI change. |
| Applicable Rules | CMD-only project build scripting, existing layers, source-owner inventories and caches, immutable original guest, truthful failures. |
| Verification | Reconcile14 addon outputs and live test producers; actual compilation and targeted tests, coherent dual publication for changed products, docs/link/diff gates. Full clean/interpreter-free acceptance remains S5. |
| Expected Markers | Canonical declarative source groups, CMD entry, generated source identity, native toolchain and cache identity, no forbidden interpreter in the migrated host path. |
| Asset Needs | Existing source/toolchains and approved assets only. |
| Non-goals | No runtime redesign, mirror audit, original guest rebuild or wholesale standalone test/audit language conversion. |
| Similar-Issue Sweep | Nested interpreters, configure hooks, generated commands, post-link steps, manifest/hash tools, addon/test producers and clean bootstrap. |
| Reporting Requirements | Separate admission, inventory, implementation and delivery; count remaining forbidden-interpreter build edges and output coverage. |
| Stop Conditions | No forbidden interpreter behind CMD or another script mode; no parallel build graph or stale output as clean-build proof. |
| Exit Criteria | All14 authored addons and current test-binary production paths use native/CMD producers; retired callers repaired, required tests/publication/commit/push complete; standalone assertion languages may remain. |

## Current Progress

[S3 P5](../etc/evidence/t484-s3-helper-retirement.md) migrates the final four
guest-memory helper consumers and removes three JavaScript generators plus
two Python helpers. Five executable cases, four original compile instances,
COMMAND167 assertions, cached full build and11 raw/22 published hashes pass.
Other test producers remain; S3 is active.

[S3 P4](../etc/evidence/t484-s3-caller-fixtures.md) replaces six manual
generators with five native groups, passing nine executable instances and
seven original compile-only units. Cached full build and11 raw/22 published
hashes pass. Four additional Python-helper consumers remain; S3 stays active.

[S3 P3](../etc/evidence/t484-s3-memory-fixtures.md) migrates two more test
producers and passes seven x86/x64 executable instances. Shared projection
bytes and repeated-build image hashes/mtimes are stable; cached full build
passes. Six manual generators still retain the old Python projection wiring.

[S3 P2](../etc/evidence/t484-s3-observation-cmake.md) resolves all five remaining
executable consumers of the old worker generator and removes it. Two static
boundaries and DEM/redirector115/14/18 assertions pass through native targets;
final cached full build passes. Other S3 test producers remain pending.

[S3 P1 addon migration](../etc/evidence/t484-s3-addon-cmake.md) builds all14
authored addons through CMD/native CMake and16 opt-in mouse probes. Three old
PowerShell producers are removed; nine utility wrappers share the producer.
Five drivers are byte-identical; nine tools differ only in verified PE
timestamps. Cached full host build passed; all11 raw inputs and22 published
host hashes remain unchanged. Nine tools are published, all14 addon hashes
verified, ABI/utility/cache/failure checks passed. S3 remains active: other
test producers and retired worker-generator consumers are not yet complete.

S1 [inventory/design](../etc/evidence/t484-s1-build-inventory.md) records817
script/metadata inputs,355 ordered migration candidates,1116 literal references
and9 existing graphs containing21 interpreter command rules. Candidate counts
are not proved live dependencies. All11 host and14 addon outputs are accounted
for. S2 P1 migrates two native data generators; the remaining candidate
dispositions are not marked complete.

The [execution plan](../etc/operations/t484-cmd-build-release-plan.md) defines
the whole task. S2 host migration is complete; S3 now owns addon/test producer migration.
Release consolidation remains S4. Existing release
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
reported five unchanged WOW units, reproduced with the prior test;
the following P4 resolves them. Three host graph generators
remain. S3-S5 are not yet admitted.

[S2 P4](../etc/evidence/t484-s2-wow-profile.md) applies the existing independent
adapter compile profile to those five WOW units. Full545-command macro audit,
cached full11-image build, dual publication and post-publication checks passed.
Current package:2588d3a4-f526-43d0-99be-2c47727b9612. Eight raw images identical,
three timestamp-only (including WOW32);22 published hashes verified. No product
source, guest or mirror code change. S2 remains active.

[S2 P5](../etc/evidence/t484-s2-wow-cmake.md) migrates WOW32 to native CMake
and removes its PowerShell generator. All228 C profiles remain equivalent;
cached11-image build, dual publication and actual Win16 message construction
passed. Current package:903798b5-7b13-4d59-b6c2-1e23e6c5babc. Four raw inputs
identical, six timestamp-only; WOW32 also changes23 diagnostic path separators.
The extra Clipboard witness failed on P4 too and is not counted as passed;
see the evidence for the reverted harness experiment. Two host graph
generators remain; S2 and the overall T remain active.

[S2 P6](../etc/evidence/t484-s2-native-family.md) migrates the five native host
components and removes their PowerShell generator and x86 reference-graph
dependency. All330 compile profiles,73 archives and41 links match the prior
composition. Final cached11-image build and dual publication passed; package
972c2293-4961-4c9e-b1eb-bd690b0ad3a4 has22 verified hashes. Four raw inputs
are identical/seven timestamp-only. Native targeted checks and the correctly
packaged private-desktop lifecycle fixtures passed. The initial incomplete
fixture invocation is recorded, not counted as passed. One host graph
generator remains; S2 is not closed, and S3-S5 remain outstanding.

[S2 P7 worker migration](../etc/evidence/t484-s2-worker-cmake.md) completes the
host chain:791 compiler profiles,67 archives,153 links and274 export lines
are reconciled, with one explicit fixture-link correction. Initial and final
cached full builds passed; package d8983d2b-3fe1-4d18-8ea9-64613c84346c is
published to both locations with22 hashes checked. COMMAND launch/capture,
C-VID and8 DPMI/CCPU cases passed. Two configurations preserve generated
input bytes/timestamps and cause no rebuild. No product implementation,
mirror or original guest changed. Detailed raw/package comparisons and the
two corrected fixture failures are recorded separately in the evidence.

## S2 Closure Record

[S2 closure](../etc/evidence/t484-s2-worker-cmake.md) completes native host composition and the CMD build entry. Its containing
P7 commit is the delivery identity. S3 is admitted under the owner's standing
continuous T484 authorization: migrate14 authored addons and test-binary
producers, repair all old generator consumers and retire superseded scripts.
No standalone test/audit language conversion or original guest rebuild is
admitted. S4 publication migration and S5 full clean acceptance remain open.

## S1 Closure Record

[S1 inventory/design](../etc/evidence/t484-s1-build-inventory.md) completes
bounded research and fixes the migration worklist. No production build,
publication or runtime pass is claimed. S1 P2 carries this evidence and S2
admission; its containing commit is the delivery identity.

## Current Technical Baseline

[T483 closure](../history/m0-t483-closure.md) indexes all S1-S21 outcomes,
retained mirror decisions and known boundaries. Final production revision is
9e7eb19c0 (S20 P85), package100b16d9-992d-4e2c-8d99-66a4e891dd26.
The current publication is T484 S2 P7, recorded above; P85 is its behavior
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

T484 S3 P5 records helper retirement and its bounded delivery under the
owner's full-task authorization. The containing commit is its delivery
identity. S3 is not closed; queue order is unchanged.
