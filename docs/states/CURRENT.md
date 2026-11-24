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
| [P1-P180](../etc/evidence/t484-s3-early-deliveries.md) | Earlier delivered producer migrations; complete results and evidence links retained in the batch index. |

| [P181 PIF fields](../etc/evidence/t484-s3-pif-memory.md) | Six callers use native PIF memory options; duplicate helper/inline patchers removed; full-byte equivalence and failure checks pass. Configuration/staging remain;315/355 disposed,40 remain. |

| [P182 matrix profiles](../etc/evidence/t484-s3-matrix-profile.md) | Two additional callers use one CMD target; eight witnesses/two paths byte-identical, cache/path checks pass. Frozen census315/355; two supplemental caller chains still need config/staging. |

| [P183 idle forwarding](../etc/evidence/t484-s3-idle-forwarding.md) | Five creation fixtures pass; real private-queue USER forwarding replaces desktop activation. Automatic foreground notification not exercised.316/355 disposed,39 remain, plus two supplemental config/staging chains. |

| [P184 profile/DDE producers](../etc/evidence/t484-s3-profile-dde.md) | Two focused regression callers reuse the existing CMD target; byte identity and no-op checks pass. Fixed316/355; four supplemental chains await staging;16 of18 newly found NASM callers remain. |

| [P185 Win95 guest-only](../etc/evidence/t484-s3-win95-guest.md) | Both COM profiles and path binding use native producers; byte equivalence and shared binder regression pass. Fixed316/355; five supplemental preparation chains pending;15 of18 newly found NASM callers remain. |

| [P186 interrupt observer](../etc/evidence/t484-s3-interrupt-observer.md) | Retired interpreted producer; native object/image bytes match, cache and missing-linker failure pass. Fixed316/355;14 of18 newly found NASM callers remain. |

| [P187 fixed-profile probes](../etc/evidence/t484-s3-fixed-probes.md) | Seven authored images share native producers, removing five NASM call sites; runtime assertions unchanged. Fixed316/355; ten supplemental preparation chains pending; nine of18 new NASM candidates remain. |

| [P188 fixed DOS profiles](../etc/evidence/t484-s3-guest-profiles.md) | All13 existing profiles of five callers share CMD production; exact bytes/cache pass. Fixed316/355;15 supplemental preparation chains pending; four of18 new NASM candidates remain. |

| [P189 transition producers](../etc/evidence/t484-s3-transition-probes.md) | FORCEDOS/memory profiles use CMD; inline CL and external C# replaced by existing native tools. Ten exact-image/cache checks and environment query pass. Fixed316/355;17 supplemental chains pending; two of18 new NASM candidates remain. |

| [P190 remote enumeration](../etc/evidence/t484-s3-remote-probe.md) | Parameterized remote witness uses checked CMD/CMake producer; existing status semantics retained. Fixed316/355;18 supplemental preparation chains pending; one of18 new NASM candidates remains. |

| [P191 generic-thunk fixtures](../etc/evidence/t484-s3-generic-native.md) | NE/native DLL producers share CMD graph; normal and cdecl real Win16 cases pass three phases and three releases. All18 new direct NASM sites handled;19 supplemental preparation chains remain. Fixed316/355. |

| [P192 keyboard isolation](../etc/evidence/t484-s3-keyboard-isolation.md) | Removed obsolete private observer and installation/global-cleanup assumptions; real PIF/MEM and four reader outcomes pass. Shared staging remains S4; fixed316/355. |

| [P193 native output preparation](../etc/evidence/t484-s3-native-output-isolation.md) | Removed dynamic BAT generation and flat-layout assumptions. Real simultaneous output loses a marker: strict test fails, recorded separately; no product change or runtime pass claimed. |

| [P194 supplemental consolidation](../etc/evidence/t484-s3-supplemental-producers.md) | Removed migration-only NASM recipe; shared native environment reader replaces duplicate C# producer. Two of13 review candidates classified;11 remain,20 supplemental staging chains remain. |

| [P195 cached tab-stop reader](../etc/evidence/t484-s3-tabstop-reader.md) | Existing read declarations use shared CMD/CMake production; read/error/cache checks pass, no new guest pass claimed.10 supplemental review candidates and21 staging chains remain. |

| [P196 WOW host contracts](../etc/evidence/t484-s3-wow-host-contracts.md) | Two obsolete diagnostics retired; glyph wrapper consolidated; five font cases and public metafile pass through native targets. Cached full and11/22 hashes unchanged. Five supplemental review candidates and21 staging chains remain. |

| [P197 session contracts](../etc/evidence/t484-s3-wow-session-contracts.md) | Retained rollback/independent-runtime cases use CMD; sealed reply research retired. Three supplemental compiler candidates remain; frozen316/355 and21 staging chains unchanged. |

| [P198 supplemental review complete](../etc/evidence/t484-s3-final-managed-producers.md) | Obsolete Win1.01 diagnostic chain retired; OEM/fatal interop uses native CMD. All13 supplemental candidates classified;23 preparation/staging chains remain. Frozen316/355 unchanged. |

| [P199 direct producer reconciliation](../etc/evidence/t484-s3-direct-producer-reconciliation.md) | Packet tests use native x86/x64 builds; obsolete T412 barrier builder retired. Broader scan adds30 candidates: one read-only audit,29 still to review. Earlier13-entry review was not global completion. |

| [P200 controlled boundaries](../etc/evidence/t484-s3-controlled-boundaries.md) | Two compiler/extraction wrappers consolidated;24 clipboard and7 keyboard assertions pass.27 newly reconciled candidates remain; frozen316/355 and23 staging chains unchanged. |

| [P201 redirector contracts](../etc/evidence/t484-s3-redir-contracts.md) | Three compiler recipes consolidated; DLC33/loader94 assertions pass, queue9 reproduce known failure explicitly.24 direct candidates remain; frozen316/355 and23 staging chains unchanged. |

| [P202 DIB contracts](../etc/evidence/t484-s3-dib-contracts.md) | Two producers consolidated;82 cleanup and57 retirement assertions pass with exact selected bodies.22 direct candidates remain; frozen316/355 and23 staging chains unchanged. |

| [P203 WOW output contracts](../etc/evidence/t484-s3-wow-output-contracts.md) | Four compiler/extraction runners consolidated;205 unchanged assertions pass. Cached full and11/22 hashes unchanged;18 direct candidates remain, frozen316/355 and23 staging chains unchanged. |

| [P204 WOW provider contracts](../etc/evidence/t484-s3-wow-provider-contracts.md) | Three compiler runners consolidated;497 unchanged assertions and ten source-projection comparisons pass.15 direct candidates remain; frozen316/355 and23 staging chains unchanged. |

| [P205 presentation contracts](../etc/evidence/t484-s3-presentation-contracts.md) | Two formal gates share native production; both current fixtures pass and both regression controls fail as required.13 direct candidates remain; frozen316/355 and23 staging chains unchanged. |

| [P206 observer contracts](../etc/evidence/t484-s3-observer-contracts.md) | Four diagnostic regression producers use native targets; obsolete DOS relay builder retired. Eight direct candidates remain; frozen316/355 and23 staging chains unchanged. |

| [P207 WOW file contracts](../etc/evidence/t484-s3-wow-file-contracts.md) | Four compiler runners consolidated; current file/DDE assertions and five source equivalence checks pass. Four direct candidates remain; frozen316/355 and23 staging chains unchanged. |

| [P208 netlib contract](../etc/evidence/t484-s3-netlib-contract.md) | Real product packstr object linked through CMD/native; existing boundary assertions pass. Three direct candidates remain; frozen316/355 and23 staging chains unchanged. |

| [P209 modesw guards](../etc/evidence/t484-s3-modesw-guards.md) | Native source generation/preprocessing preserves both original-line rejections and two positive controls. Two direct candidates remain; frozen316/355 and23 staging chains unchanged. |

P183 found18 additional direct NASM sites; P184-P191 migrate or retire those
producers. Their shared preparation chains remain explicitly pending.
The frozen355 census is not the complete remaining invocation graph. P193
adds13 unclassified CL/NASM/Add-Type candidates to the existing ledger;
retention and build effect are not yet decided.

Completed implementation batches through P141 passed their cached full host build. P73 checks11 raw host inputs
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
