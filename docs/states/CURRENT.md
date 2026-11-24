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
| [P1-P160](../etc/evidence/t484-s3-early-deliveries.md) | Earlier delivered producer migrations; complete results and evidence links retained in the batch index. |

| [P161 native environment reader](../etc/evidence/t484-s3-logon-reader.md) | Three CPU guest callers share native environment query; sign/transfer/ALU positive and negative cases pass.293/355 disposed,62 remain. |

| [P162 EMS environment](../etc/evidence/t484-s3-ems-environment.md) | EMS Mapping/CallReturn use shared native reader and pass actual guest checks; two forwarding callers cleaned.294/355 disposed,61 remain. |

| [P163 Win16 environment](../etc/evidence/t484-s3-win16-environment.md) | Timer/cancel,NoTimer and CBT hook use shared native reader and pass real guest checks.296/355 disposed,59 remain. |

| [P164 message environment](../etc/evidence/t484-s3-message-environment.md) | Duplicate C# reader removed; basic and structured actual Win16 messages pass; cached full and11/22 hashes unchanged.297/355 disposed,58 remain. |

| [P165 existing Win16 chains](../etc/evidence/t484-s3-win16-existing-chains.md) | Five existing CMD chains pass actual normal-user-environment contracts; inherited-agent failures retained. Cached full/11/22 hashes pass unchanged.302/355 disposed,53 remain. |

| [P166 DC/dialog chains](../etc/evidence/t484-s3-dialog-native.md) | Existing shared CMD producer and three actual guest contracts pass; cached full/11/22 hashes unchanged.304/355 disposed,51 remain. |

| [P167 external current package](../etc/evidence/t484-s3-external-current-package.md) | Removed obsolete13-image self-test; current11-image local/x86/x64 message groups and cached full/identity checks pass. Default staging still pending;304/355,51 remain. |

| [P168 direct native identity](../etc/evidence/t484-s3-direct-package-identity.md) | Removed identity wrapper/expired T228 assembler;10 call sites use native writer. Identity negatives, dual-package RPC/DOS and five RPC wait cases pass; cached full/11/22 hashes unchanged.307/355 disposed,48 remain. |

| [P169 RPC control identity](../etc/evidence/t484-s3-rpc-control-identity.md) | Separate copied roots get unique complete native identities; Full six and three substituted-worker cases pass. Cached full/11/22 hashes unchanged;308/355 disposed,47 remain. |

| [P170 hygiene consolidation](../etc/evidence/t484-s3-hygiene-consolidation.md) | Removed private compiler recipe;28 cleanup/native cache policy checks and existing two-pass CMD cache gate pass.11 raw/22 published hashes unchanged;309/355 disposed,46 remain. |

| [P171 expired diagnostics](../etc/evidence/t484-s3-expired-diagnostics.md) | Retired sealed P21 control and CONTRACE including native targets; retained current diagnostic/Win31 fixture builds pass. Cached full and11/22 identities unchanged;311/355 disposed,44 remain. |

| [P172 cached inspector](../etc/evidence/t484-s3-window-inspector.md) | Shared window inspector loads a CMD/CMake-built DLL; original C# body unchanged. Compile/load/error/cache checks and cached full11/22 hashes pass. Matrix preparation remains open;311/355,44 remain. |

| [P173 native probe paths](../etc/evidence/t484-s3-probe-paths.md) | Authored Win31 witness fields bind through native fixture; exact bytes and four no-write refusals pass. Cached full and11/22 hashes unchanged. Runtime preparation remains open;311/355,44 remain. |

| [P174 native controls](../etc/evidence/t484-s3-native-controls.md) | Two direct compiler recipes replaced; real MessageBox/ShellAbout returns and Run16 game exit/hidden carrier pass. Cached full11/22 hashes unchanged;313/355 disposed,42 remain. |

| [P175 native VT](../etc/evidence/t484-s3-vt-native.md) | Historical compiler recipe removed; x86/x64 real Console/pixel oracle each230987 checks,0 failures. Cached full and11/22 identity checks pass unchanged;314/355 disposed,41 remain. |

| [P176 DDEML environment](../etc/evidence/t484-s3-ddeml-native.md) | External C# helper replaced by shared native reader; real init/register cases and cached full11/22 identity checks pass. B0043 still pending shared staging;314/355 disposed,41 remain. |

| [P177 serial isolation](../etc/evidence/t484-s3-serial-native.md) | Native namespace oracle and owned runtime replace C# compilation, flat-layout overwrite and global process cleanup. Direct/nested guest cases pass at mask0; B0041 still pending shared staging. |

| [P178 DLL ABI](../etc/evidence/t484-s3-wow-entry-native.md) | Retired incomplete registration mocks; current return/cleanup projection passes BOOLEAN and8 OS loader cycles. Cached full11/22 hashes unchanged;315/355 disposed,40 remain. |

| [P179 controller paths](../etc/evidence/t484-s3-controller-paths.md) | Three runners share native128-byte field binding; six real controller images byte-identical, nine failure cases pass. Remaining COM/config/staging work stays open;315/355 disposed,40 remain. |

| [P180 DOS identity fields](../etc/evidence/t484-s3-identity-fields.md) | Two runners share native COM preparation; seven real templates/both roles byte-identical and18 rejection cases pass. PIF/config/staging remain;315/355 disposed,40 remain. |

| [P181 PIF fields](../etc/evidence/t484-s3-pif-memory.md) | Six callers use native PIF memory options; duplicate helper/inline patchers removed; full-byte equivalence and failure checks pass. Configuration/staging remain;315/355 disposed,40 remain. |

| [P182 matrix profiles](../etc/evidence/t484-s3-matrix-profile.md) | Two additional callers use one CMD target; eight witnesses/two paths byte-identical, cache/path checks pass. Frozen census315/355; two supplemental caller chains still need config/staging. |

| [P183 idle forwarding](../etc/evidence/t484-s3-idle-forwarding.md) | Five creation fixtures pass; real private-queue USER forwarding replaces desktop activation. Automatic foreground notification not exercised.316/355 disposed,39 remain, plus two supplemental config/staging chains. |

A fresh global scan found18 further integration scripts with direct NASM
commands; these await retention/migration reconciliation, as recorded in P183.
The frozen355 census is not the complete remaining invocation graph.

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
