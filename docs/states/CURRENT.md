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

S1 inventory/design and S2 native host migration are closed. The
[execution plan](../etc/operations/t484-cmd-build-release-plan.md) retains
the complete task scope. S3 remains active; S4 publication and S5 clean,
interpreter-free end-to-end acceptance have not completed.

| S3 delivery | Actual result |
| --- | --- |
| [P1 addons](../etc/evidence/t484-s3-addon-cmake.md) | All14 authored addon outputs;5 byte-identical drivers,9 timestamp-only tools,16 opt-in mouse probes. |
| [P2 worker callers](../etc/evidence/t484-s3-observation-cmake.md) | Five old worker-generator consumers resolved; generator removed; DEM115/redirector14/18 assertions. |
| [P3 memory fixtures](../etc/evidence/t484-s3-memory-fixtures.md) | Two producer callers migrated;7 executable instances, shared projection identity and cache stability. |
| [P4 caller fixtures](../etc/evidence/t484-s3-caller-fixtures.md) | Six generators replaced;9 executable instances and7 original compile-only units. |
| [P5 helper retirement](../etc/evidence/t484-s3-helper-retirement.md) | Four remaining helper consumers migrated;3 JS and2 Python helpers removed;5 cases,4 compile instances and COMMAND167 assertions. |
| [P6 guest probes](../etc/evidence/t484-s3-guest-probes.md) | Eleven probe producers replaced,37 byte-identical outputs; one obsolete frontend-library producer removed. |
| [P7 native observers](../etc/evidence/t484-s3-native-observers.md) | Four producers replaced;4 PE timestamp-only outputs, identical DOS probe, native stream/ConPTY and6 memory checks. |
| [P8 memory profiles](../etc/evidence/t484-s3-memory-profiles.md) | Two producers replaced;65 identical EMS/XMS files over9 configurations,3 timestamp-only tools and path/failure/cache checks. |
| [P9 retired inputs](../etc/evidence/t484-s3-retired-inputs.md) | Six ineffective historical producer/test files removed; native serial and cached full build verified. |
| [P10 system fixtures](../etc/evidence/t484-s3-system-fixtures.md) | Three producers replaced; five executable instances and27 shared-link regression assertions; obsolete register fixture retired. |
| [P11 duplicate compilation](../etc/evidence/t484-s3-compile-duplicates.md) | Native suballoc lifecycle test passes; four redundant compile-only generators retired against13 current product objects. |
| [P12 OEM fixtures](../etc/evidence/t484-s3-oem-fixtures.md) | Two generators replaced by17 native targets;18 runtime cases and five equivalent source projections verified. |
| [P13 observation tools](../etc/evidence/t484-s3-observer-tools.md) | Two JS generators replaced;11 outputs, two input-milestone passes and9 argument-rejection checks. |
| [P14 original libraries](../etc/evidence/t484-s3-original-libraries.md) | Five duplicate generators retired against34 product objects; COMMAND initialization migrated and passed. |
| [P15 ABI cleanup](../etc/evidence/t484-s3-abi-cleanup.md) | Seven generator dispositions; six native DEM layout assertions and x86/x64 VDM control passed; retired SAS fixture not counted as passing. |
| [P16 redirector lifecycle](../etc/evidence/t484-s3-redirector-lifecycle.md) | Four producer migrations; five lifecycle instances,14 capacity and24 provider assertions pass; two stale matrices retired. |
| [P17 package resources](../etc/evidence/t484-s3-package-resources.md) | Two producers migrated; four native targets on x86/x64 and ten runtime cases pass. |
| [P18 Console fixtures](../etc/evidence/t484-s3-console-fixtures.md) | Three producers replaced; font x86/x64 and two private-desktop Console cases pass; expired fixture assumptions corrected. |
| [P19 thread fixtures](../etc/evidence/t484-s3-thread-fixtures.md) | Generator and extractor replaced; native thread/module outputs, alert/join and six lifecycle marker groups pass on x86/x64. |
| [P20 host boundaries](../etc/evidence/t484-s3-host-boundaries.md) | Two generators replaced; actual x86 product heartbeat/ICA objects pass unchanged suspend/resume and mutual-exclusion assertions. |
| [P21 CCPU callback](../etc/evidence/t484-s3-ccpu-callback.md) | Two generators replaced; bounded CCPU and original callback assertions pass with current initialization. |
| [P22 WOW controls](../etc/evidence/t484-s3-wow-controls.md) | One generator replaced;158 native units,46 links and38 passing default runtime cases; five identical source projections. |
| [P23 COMMAND producer](../etc/evidence/t484-s3-command-producer.md) | Last literal Ninja generator retired into current native request coverage;74 assertions pass. |
| [P24 worker diagnostics](../etc/evidence/t484-s3-worker-diagnostics.md) | Old diagnostic producer replaced;11 linked variants reuse actual source profiles and archive/link membership; map checks pass. |
| [P25 CPU contracts](../etc/evidence/t484-s3-cpu-contracts.md) | Four inline producers consolidated;NEG64631410 cases, two ABI rejection controls, notification concurrency and five native prefix cases pass. |
| [P26 DPMI contracts](../etc/evidence/t484-s3-dpmi-contracts.md) | Three producers migrated;117 port cases, selected interrupt/return contracts and65538 PM-entry cases pass. |

Each batch passed its cached full host build. P26 checks11 raw host inputs
and22 published hashes unchanged; no changed product images required
republication. Other test producers remain pending. Static candidate counts
are not live-edge counts or completion evidence. Detailed runs, limitations,
source profiles and delivery identities belong to the linked evidence.

## S2 Closure Record

[S2 P7 closure](../etc/evidence/t484-s2-worker-cmake.md), commit ce4fc0f9c,
completes the native host composition and CMD entry. All11 images are
published to both locations under package d8983d2b-3fe1-4d18-8ea9-64613c84346c.
The final worker reconciliation covers791 compiler profiles,67 archives,
153 links and274 export lines. Prior S2 batches and their actual comparison
limits are indexed in [supporting material](../etc/README.md).
S3 was admitted under the owner's continuous T484 authorization.

## S1 Closure Record

[S1 inventory/design](../etc/evidence/t484-s1-build-inventory.md) fixes the
ordered worklist:817 scanned inputs,355 migration candidates and1116 literal
references. These are lexical inventory counts, not proved live dependencies.
No production build/publication/runtime pass is attributed to S1.

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

T484 S3 P26 records DPMI contract producer migration and its delivery under the
owner's full-task authorization. The containing commit is its delivery
identity. S3 is not closed; queue order is unchanged.
