# m0-td historical record

Consolidated task record. Raw contemporaneous records are retained in the external documentation archive.

## m0-td-component-strata-confirmation-20260824.md

# M0 Td — Ten-Component Architecture Confirmation

## Decision

The project uses exactly ten production source components, organized into
three responsibility strata:

- **Original-source mirrors:** `bx-core`, `opennt-guest`, `opennt-bop`, and
  `opennt-host`.  Each preserves source identity where the original unit is
  available.  Every necessary edit has a local `DIVERGENCE:` marker and an
  entry in that component's `README.md` exception register.
- **Mechanical adapters:** `bx-mantle`, `adapter-bop`, `adapter-softpc`, and
  `adapter-win32`.  `bx-mantle` is Bochs-only; `adapter-bop` is selector-blind
  copied-frame transport; `adapter-softpc` preserves reached SoftPC/CCPU/SAS
  interface shape over bounded Bochs mechanics; and `adapter-win32` preserves
  reached historical Win32 interface shape over supported public Win32 APIs.
- **Project-authored composition:** `app` is the CLI entry and sole final
  composition owner.  `session` is the dependency-free, neutral lifecycle,
  identity, resource/event and teardown foundation; it is not an upward
  product layer.

The authoritative dependency direction is:

```text
app -> opennt-guest
app -> bx-mantle -> bx-core
app -> session
app -> adapter-bop -> opennt-bop -> opennt-host
opennt-bop -> adapter-win32
opennt-bop -> adapter-softpc -> bx-mantle -> bx-core
opennt-bop -> session
opennt-host -> adapter-win32 | adapter-softpc | session
adapter-bop -> session
adapter-win32 -> session
```

No dependency may reverse this diagram.  In particular, Bochs and its mantle
remain OpenNT/DOS/VDM/WOW/Win32-blind; `adapter-softpc` and `adapter-bop` do
not interpret BOP meaning; and `session` does not acquire component-specific
vocabulary or depend on a product component.

## Reconciled Authorities

- `docs/design/ARCHITECTURE.md` defines the ten-component ownership model and
  dependency graph.
- `docs/design/CODING.md` defines the production-only component roots and the
  source-identity/DIVERGENCE requirements.
- `docs/rules/ARCHITECTURE.md` and `docs/rules/CODING.md` make the boundary and
  source-first constraints enforceable.

This Td introduces no product source, build-graph behavior, numeric T
allocation, or change to active `M0 T261 S8`.  It records the owner-approved
architecture confirmation after the authorities were cross-read.

## Verification

- Cross-read the four current architecture/coding authorities above.
- Confirmed the declared source roots in `docs/design/CODING.md` enumerate
  exactly the ten production components and retire transitional `src/cli`,
  `src/bx-vdm`, and generic `src/opennt` roots.
- Run documentation-governance verification, documentation-inventory refresh,
  and `git diff --check` for this documentation-only delivery.


## m0-td-s1-p4-bop-owner-package-sequencing-20260818.md

# M0 Td S1 P4 — BOP Owner-Package Sequencing Governance

## Decision

This standalone Td delivery records the owner-directed order for the next
candidate packages.  It allocates no numeric T and admits no implementation
work.

1. The first eight candidates each close one complete BOP owner package or the
   BOP-reachable machine/BIOS selector family: DEM, COMMAND, XMS, DPMI,
   Redirector, WOW16, debugger/VDD/top-level, then bx machine/BIOS/selectors.
2. The ninth candidate is a cross-family BOP completion audit and integrated
   verification task.  It reconciles the full catalogue and runs declared
   Direct/Readonly and native integration matrices, but cannot create
   trace-selected leaf fixes.
3. The tenth candidate is Overlay mutation-capability completion.  It remains
   after the BOP audit so Overlay is not used to hide incomplete Direct or
   Readonly owner behavior.

## Rationale

The order preserves whole-owner source recovery and prevents trace-led
incremental BOP work.  It also separates implementation closure from the
later question of whether all package routes compose under real guest
execution.

## Verification

The Queue contains exactly ten unnumbered candidate packages in this order;
items 1--8 cite their owner-package proposals, item 9 cites the new
cross-family-audit proposal, and item 10 cites the Overlay proposal.  The Td
changes no product source, build output, numeric T assignment, or active
implementation scope.


## m0-td-s1-p5-no-external-bop-governance-20260822.md

# M0 Td S1 P5 — No-external-dependency BOP Governance

## Decision

The next implementation candidate is one medium-sized T, not separate low-
dependency and internal-dependency T packages. It contains every BOP entry and
OpenNT lifecycle/helper dependency whose complete declared path has no external
machine prerequisite, legacy/private API decision, or unresolved external
owner. Purely internal BOP/session/DAG dependencies belong in that same T.

The next candidate excludes bx machine/BIOS work, legacy or private API
compatibility, DPMI, Redirector, WOW16, VDD/debugger, guest EXEC/PSP and other
external-owner work. Those transfer to candidate 2, then candidate 3 performs
the integrated completion audit.

## Reconciled records

- `M0 T237 S5 P2` is committed and pushed as `e59681c2`; Status no longer
  describes it as pending push.
- The live tracker is reconciled to 102 complete, 2 partial and 99 unfinished
  BOP entries: XMS `52:06` and top-level `59` are complete; only `52:09` and
  `FE` remain partial.
- The former internal-dependency proposal is retained as superseded evidence;
  the queue links the expanded no-external-dependency proposal instead.

## Verification

This Td changes no product code, build graph, numeric T allocation or active
implementation scope. Documentation governance and inventory verification are
required before this record is accepted.


## m0-td-s1-p6-no-external-bop-freeze-20260822.md

# M0 Td S1 P6 — No-external-dependency BOP Admission Freeze

## Question

How many tracker rows may enter the first no-external-dependency BOP
candidate, before allocating a numeric T?

## Method

The live tracker was read as the authority. A BOP or dependency entered the
local slice only when its declared minimum closure tier starts at `P0` through
`P6`; every `P7+` extension remains an explicit external transfer. The source
reference `BOP-DEPENDENCY-042` was manually excluded because it is the
uncomposable NT4 CCPU/SAS/DLL/CSR host product composition, not a local
provider merely because it retains a source-defined failure reference.

## Result

| Class | Count | Current local-pass |
| --- | ---: | ---: |
| BOP entries | 75 | 75 |
| OpenNT dependencies | 30 | 30 |
| **Frozen candidate total** | **105** | **105** |

The BOP composition is 55 DEM, 17 COMMAND, 2 XMS and one top-level `59`.
The dependency composition is 15 DEM and 15 COMMAND rows. The detailed ID
ranges, local-contract rule and named exclusion are maintained in the live
tracker, not duplicated here as a second mutable catalogue.

## Decision

This is a governance freeze, not a numeric T admission. If the owner admits
the queue-head candidate, S1 must perform source/route/fixture no-bypass
revalidation of these 105 rows. It must not assume they require new feature
implementation; a discovered local gap remains in scope, while an external
gap transfers to candidate 2 through the tracker.


## m0-td-status-governance-compaction-20260910.md

# M0 Td — Status governance compaction

## Status

Closed 2026-09-10. This is standalone documentation governance work; it does
not allocate a numeric T, admit an implementation packet, or change product
source, build, runtime, guest media, or evidence conclusions.

## Owner request

Reduce the oversized `docs/STATUS.md` safely so project sessions can resume,
while retaining every existing status record.

## Scope and non-goals

The bounded change separates the current control document from its accumulated
closed-task ledger. It does not reclassify historical claims, delete evidence,
select a next product package, or alter `QUEUE.md`, rules, design, source, or
build inputs.

## Inputs and preservation

- Pre-compaction `STATUS.md`: 1,132,143 bytes, SHA-256
  `fce40f7d9ea6f57495e806547392840cd20f94a0e7941cef0fc0c86012cb6a0b`.
- Exact retained snapshot:
  [status-ledger-snapshot-20260910.md](../../artifacts/documentation-archive/20260910/etc/evidence/status-ledgers/status-ledger-snapshot-20260910.md),
  verified with the same SHA-256 before the current control document changed.
- Current-status facts: the former leading `Current Work` record states that
  no M/T/S packet is active and that M0 T401 is closed.

## Procedure

1. Create and hash-verify an exact pre-compaction copy.
2. Retain that copy under `docs/etc/evidence/status-ledgers/` as the historical status ledger.
3. Replace `docs/states/CURRENT.md` with a compact no-active-packet record that links
   the latest product closure, the historical ledger, and the authoritative
   queue.
4. Update the document inventory hashes and run documentation governance and
   link/whitespace verification.

## Result and follow-up

The historical ledger remains available for provenance lookup without being
loaded as live status. Future work must admit exactly one bounded packet to
`states/CURRENT.md`; detailed attempts and closed-task chronology belong in evidence
and history records. Candidate work remains in [QUEUE.md](../states/QUEUE.md).


## td-documentation-repository-declutter-20260910.md

# Td — Documentation repository declutter

## Status

Closed: Td S1 P3. This standalone governance delivery does not allocate or
close a numeric T and does not admit product implementation work.

## Owner requests

- "现在doc怎么还有这么多文件？"
- "你觉得哪些可以清理出去？这docs不应该这么庞杂"
- "没错，批准。请你继续Td治理提交推送。"
- "接下来所有exe构建记录、中间体和结果，都应当放在 build目录下，但是按需要复制exe到o:\\ntvdm64去实地运行；所有观察日志放在o:\\ntvdm64\\logs里面。把这个写入治理规范。"
- "好的，请继续按照你的建议实施清理。"
- "目标是最终工作区干净，并提交推送当前S任务收口。T任务保持打开。"

## Baseline

At admission, `docs` contains 4,103 files. `docs/etc/operations` contains
315 TSV ledgers totaling about 299.6 MiB; `etc/evidence`, `etc/research`, and
fragmented task records contain historical supporting material. Current
authorities and state controls are small by comparison.

## Scope

1. Move generated ledgers and raw historical material to a manifest-backed
   archive outside `docs`.
2. Retain concise task conclusions and current authorities in their governed
   locations.
3. Repair active tool and document references, enforce the product-run log
   root, preserve unrelated implementation work as local WIP, remove disposable
   workspace output, verify the result, and deliver only Td-owned changes in a
   commit and push.

## Non-goals

No product behavior, source selection, build configuration, ABI, runtime, or
guest-media change is authorized. Existing unrelated dirty-worktree changes
are preserved as WIP, but are not part of this delivery.

## Risks and stop conditions

Stop if an item is a live build/runtime input, if a preservation manifest
cannot be produced, or if staging would include unrelated user changes.

## Verification plan

Use file/hash manifests, direct-reference scans, the documentation-link and
governance checks, focused exporter regression, `git diff --check`, scoped
diff review, commit, and push.

## P2/P3 result

- Current governance now requires all locally built executables, intermediates,
  results, fixtures, and compiler/linker output under `build/`; only the formal
  `ntvdm32.exe` may be copied to `O:\ntvdm64\ntvdm32.exe` for a real-package
  run.
- Runtime observations, diagnostics, traces, reports, and stdout/stderr captures
  now belong below `O:\ntvdm64\logs\`. The startup observer rejects both its
  primary report and child diagnostic report paths when they are outside that
  root.
- 236 untracked observation outputs were moved to
  `O:\ntvdm64\logs\m0-t397-s4-archive-20260910`. 23 disposable root-level
  compiler/capture outputs were removed. The 108 tracked `artifacts/research`
  records accidentally included in the first broad move were restored exactly;
  they are not treated as disposable runtime logs.
- Existing product source, tests, and tools are preserved without push in local
  recovery branch `codex/product-wip-preserved-20260910`, commit `ef1c3cb28`.
  This leaves that product work open; it is excluded from Td.
- Verification: documentation governance and relative-link checks pass; the
  observer's path rejection/acceptance checks and scoped diff review pass. The
  Td delivery commit and push are recorded by the revision containing this
  closure.


## td-mvdm-zero-degree-root-correction-20260827.md

# Td Closure — MVDM Zero-Degree Root Correction

The owner corrected the MVDM BFS root definition on 2026-08-27. Zero-degree
contains all original `mvdm-host` definitions and their reachable,
provenance-verified call closure from the selected original OpenNT `mvdm` tree
already mirrored in a project `mvdm-*` component. Local but unreachable MVDM
support, tool, firmware, and guest definitions are excluded.

This replaces neither original source provenance nor final runtime disposition.
The detailed decision and the required rebaseline procedure are in
[the Td record](../../artifacts/documentation-archive/20260910/strict-topology/etc/operations/td-mvdm-zero-degree-root-correction-001.md).


## td-opennt-host-package-root-governance-20260827.md

# Td closure — OpenNT-host package-root governance — 2026-08-27

The owner clarified that `opennt-host` is the canonical mirror root for all
accepted non-MVDM OpenNT host packages, not only BaseSrv/BaseClient VDM. The
directory and adapter rules now preserve per-package boundaries under
`opennt-host` and `adapter-opennt-host`; no new package was imported.

