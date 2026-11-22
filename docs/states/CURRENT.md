# Project Status

## Current Work

**Active: M0 T484 S1** (Ordinary Mode). Owner admitted unified CMD-only
build and release on 2026-11-22 after T483 closure.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T484 S1, Ordinary Mode. |
| Admission And Approval | Owner: 准入新t任务！要求构建脚步去除python powershell javascript 仅使用cmd. |
| Original Owner Request | Admit unified build/release; CMD-only build scripts, removing Python, PowerShell and JavaScript dependencies. |
| Objective | Inventory actual build/generation/publication callers and producers in DIR order; define finite migration for eleven host images and authored addons. |
| Candidate Proposal | [Unified build/release](../proposals/proposal-unified-build-cmd-release-001.md) |
| Reference Baseline | T483 closure e74afba09; product P85 9e7eb19c0, package100b16d9-992d-4e2c-8d99-66a4e891dd26. |
| Files And ABI Surface | Build entrypoints, CMake/Ninja generators, generated sources, addon/test compilation and publication tools; no product ABI changes at admission. |
| Applicable Rules | CMD-only project build scripting, existing layers, source-owner inventories and caches, immutable original guest, truthful failures. |
| Verification | Admission: documentation/link/diff gates. S1: actual caller/output evidence and dependency order; later build changes require native-tool execution and applicable product gates. |
| Expected Markers | Per-file language, caller, output, owner and migration disposition; toolchain/bootstrap requirements and acceptance matrix. |
| Asset Needs | Existing source/toolchains and approved assets only. |
| Non-goals | No runtime redesign, mirror audit, original guest rebuild or wholesale standalone test/audit language conversion. |
| Similar-Issue Sweep | Nested interpreters, configure hooks, generated commands, post-link steps, manifest/hash tools, addon/test producers and clean bootstrap. |
| Reporting Requirements | Separate admission, inventory, implementation and delivery; count remaining forbidden-interpreter build edges and output coverage. |
| Stop Conditions | No forbidden interpreter behind CMD or another script mode; no parallel build graph or stale output as clean-build proof. |
| Exit Criteria | Complete finite producer/caller inventory and migration design, with bounded successor stages; no claim migration already passed. |

## Current Progress

Admission only. tools/build.cmd is absent. CMakeLists.txt requires pwsh and
node and invokes PowerShell Ninja generators and a JavaScript C-VID generator.
These are migration obligations. The [execution plan](../etc/operations/t484-cmd-build-release-plan.md)
defines the whole task; only S1 is active.

## Current Technical Baseline

[T483 closure](../history/m0-t483-closure.md) indexes all S1-S21 outcomes,
retained mirror decisions and known boundaries. Final production revision is
9e7eb19c0 (S20 P85), package100b16d9-992d-4e2c-8d99-66a4e891dd26.
All11 images remain published to assets/release and O:/winnt/system32;
S21 rechecked all22 hashes and unchanged product inputs. P85 cached full build,
targeted tests and Full20/20 evidence are reused, not rerun.

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

S21 reconciles historical plans, evidence and residual ownership. Its containing
commit/push closes T483 with documentation-only checks; queue order is unchanged.
