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
| Objective | Migrate all14 authored addon producers and test-binary production callers to CMD/native declarative targets; retire the old worker generator and interpreted producer chains while preserving actual source/flag/ABI closures. Reaudit already migrated test tools/probes; remove obsolete unused entries and consolidate duplicated setup/coverage instead of preserving migrations for their own sake. |
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
| [P1-P100](../etc/evidence/t484-s3-early-deliveries.md) | Earlier delivered producer migrations; complete results and evidence links retained in the batch index. |

| [P101 creation checkpoint](../etc/evidence/t484-s3-creation-native.md) | Five native bounded contracts pass; real Idle foreground chain fails before delivery. B0166 remains incomplete; no completion-count increment. |

| [P102 DDE producers](../etc/evidence/t484-s3-dde-native.md) | Current96/38 contracts and four projection comparisons pass; COPYDATA regression, full cached build/cache/publication hashes pass. P101 Idle boundary remains open. |

| [P103 dispatcher producer](../etc/evidence/t484-s3-dispatch-native.md) | Eight phases/five error scopes/debugger/map gate pass; old parser/extractor retired; full cached build/cache/publication hashes pass. |

| [P104 small WOW contracts](../etc/evidence/t484-s3-small-wow.md) | Three native producers and already-native input lifetime pass; duplicate GDI probe consolidated; full build/cache/publication hashes pass. B0169 provider mismatch remains pending. |

| [P105 capture entry](../etc/evidence/t484-s3-capture-native.md) | Existing native producer routed through CMD;28 host observations, full cached build/cache/publication hashes pass; no guest acceptance claim. |

| [P106 construction contracts](../etc/evidence/t484-s3-construction-native.md) | Current40/191 cases and shared projection regressions pass; historical builders/fixture branches retired; full cached build/cache/publication hashes pass. |

| [P107 module alias](../etc/evidence/t484-s3-module-alias-native.md) | Actual product owner/profile and alias lifetime contract pass; duplicate manual producer removed; full cached build/cache/publication hashes pass. |

| [P108 capture contracts](../etc/evidence/t484-s3-capture-contracts.md) | Three native relationship families/15 current cases pass; six historical controls retired; shared projection regressions/full cached build/cache/publication hashes pass. |

| [P109 extension/frame contracts](../etc/evidence/t484-s3-extension-native.md) | Four native groups pass, including4 frame/11 abort cases; old producers retired; full cached build/cache/publication hashes pass. |

| [P110 native references](../etc/evidence/t484-s3-native-references.md) | Query231 checks/nine imports and four queue modes pass; stale object paths/manual producers removed; full cached build/cache/publication hashes pass. |

| [P111 owned read/page control](../etc/evidence/t484-s3-owned-read.md) | Current contracts and six-body equivalence pass; historical scalar mode and duplicate producers removed; full cached build/cache/publication hashes pass. |

| [P112 parameter map](../etc/evidence/t484-s3-parameter-native.md) | Current54 cases/four projections/construction191 pass; historical producers retired; full cached build/cache/publication hashes pass. |

| [P113 print provider](../etc/evidence/t484-s3-print-provider.md) | Native PDF normal/cancel contract and11 callback identities pass; manual producer removed; full cached build/cache/publication hashes pass. |

| [P114 return unwind](../etc/evidence/t484-s3-return-unwind.md) | Two current cleanup contracts and projection checks pass after repairing stale fixture interface; full cached build/cache/publication hashes pass. |

| [P115 ABI/registry](../etc/evidence/t484-s3-abi-registry.md) | Public ABI compile/registry lifecycle pass with current owners; obsolete profile/path builders removed; full cached build/cache/publication hashes pass. |

| [P116 reply payload](../etc/evidence/t484-s3-reply-payload.md) | Three native reference cases pass with unchanged shared assertions; manual producer removed; full cached build/cache/publication hashes pass. |

Completed implementation batches through P116 passed their cached full host build. P73 checks11 raw host inputs
and22 published hashes unchanged; no changed product images required
republication. Other test producers remain pending. Static candidate counts
are not live-edge counts or completion evidence. Detailed runs, limitations,
source profiles and delivery identities belong to the linked evidence.

## S2 Closure Record

The [candidate reconciliation](../etc/evidence/t484-progress-reconciliation.md)
tracks all355 original paths and separates file changes from completed
dispositions. Remaining disposition work is explicit; batch counts are not a
completion percentage.

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

T484 S3 P39 records shared native editor composition and its delivery under the
owner's full-task authorization. The containing commit is its delivery
identity. S3 is not closed; queue order is unchanged.
