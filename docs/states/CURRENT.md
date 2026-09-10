# Project Status

## Current Work

**No active numeric M/T/S packet. M0 T402 is closed.**

## Latest Packet Closure

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T402 S1, Ordinary Mode; closed. |
| Admission And Approval | Owner approved x64-only residue removal, x86 product verification, and permanent commit/push authority. |
| Objective | Remove non-mapping-manager x64-only residue and deliver a verified x86 product. |
| Non-goals | No mapping-manager, native-x64, recovered-behavior, WOW/guest semantic, or preserved-WIP change. |
| Reference Baseline | T399 retired x64 product paths; T392 added the GDP width projection now under removal. |
| Files And ABI Surface | GDP overlay/generator and C-VID callers; staging, CRT/context, affected owner-path build inputs. |
| Applicable Rules | Execution, Architecture, Coding, Document, and x86 CCPU40 product rules. |
| Verification | x86 compile/archive/link, PE staging, x64 rejection, diff and governance checks. |
| Expected Markers | No GDP slot generator/state or active native-x64 product staging path. |
| Asset Needs | Existing source and local MSVC x86 toolchain only. |
| Reporting Requirements | Removed surfaces, staged path/hash, verification, limitations, and deferred historical evidence. |
| Stop Conditions | Mapping-manager dependency, x86 ABI/behavior regression, or a required new overlay. |
| Exit Criteria | Met: audited removal, passing x86 link/staging, governance/diff validation, and owner real-package test. |
| Original Owner Request | 清除所有非映射管理器负责的、x64编译产生的diff，要求通过文档、代码质量审计并通过测试。 |
| Similar-Issue Sweep | Product staging, x64 preprocessor branches, GDP consumers, and owner-path build references. |

### Admission and approval

The owner approved admission to remove native-x64-only product residue, reduce
the SoftPC overlay footprint, and provide a newly verified `ntvdm32.exe` for
testing at `O:\ntvdm64\ntvdm32.exe`.

### Objective

Restore the original x86 C-VID GDP carrier in place of the x64-only
native-width GDP projection; remove obsolete Win64 CRT/context branches; make
product staging x86-only; then build, verify, and stage the Win32/x86 CCPU40
product.

### Non-goals

No mapping-manager change, native-x64 build/repair, recovered-behavior work,
WOW/guest semantic change, or adoption of the preserved WIP branch is in
scope.

### Baseline, files, and ABI surface

T399 closed the x64 product-retirement decision, but this audit found four
residual x64-only surfaces: GDP projection (40 source/generator files), the
Win64 CRT private-name block, the x64 termination-context branch, and a
stager that still accepted x64. GDP is private SoftPC state, not a session
mapping-manager identity. Expected owned paths are the GDP overlay and C-VID
mirror set, `mvdm_crt_redirect.h`, `mvdm_softpc_termination.c`,
`StageProductExecutable.mjs`, their registers/readmes, and this packet.

### Rules, verification, and exit

Apply `docs/rules/{EXECUTION,ARCHITECTURE,CODING,DOCUMENT}.md`: one x86
CCPU40 product row; no x64-only preservation; all build results under
`build/M0-T402-S1/<run>/`; only a verified `ntvdm32.exe` may be copied to the
runtime package; logs belong in `O:\ntvdm64\logs`. Verify the source sweep,
the x86-only staging rejection/acceptance behavior, selected x86 build/link,
`git diff --check`, governance validation, and final diff. Close only after
recording the staged-file hash and any unavailable runtime limitation.

### Stop conditions and reporting

Pause for a mapping-manager dependency, an x86 behavior/ABI regression, a
required new overlay, or any scope beyond removal. Report removed surfaces,
verification, staged path/hash, limitations, and remaining deferred x64
historical evidence. Similar-issue sweep covers active product staging,
private x64 preprocessor branches, and GDP consumers.

## Latest Td Closure

[Td S2 P1 strict documentation-topology convergence](../history/m0-td-consolidated-history.md) archived 1,001 non-authority files with hashes, retained five current design authorities, consolidated and task-introduction-named history records, and reduced `etc` to explicitly owned supporting material.

## Latest Product Closure

[M0 T401 WRITE diagnostic reconciliation](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t401-s4-write-compatible-dc-non-equivalence-001.md).

## Latest Governance Closure

[M0 Td status governance compaction](../../artifacts/documentation-archive/20260910/strict-topology/history/raw/m0-td-status-governance-compaction-20260910.md).

## Current Technical Baseline

- The sole product and acceptance executable is the Win32/x86
  `ntvdm32.exe`, using the original SoftPC CCPU40 profile. Native x64 is not a
  product, build, runtime, or acceptance target.
- M0 T395 (CPU40 DPMI32/bootstrap), M0 T396 (WOW32 first-dispatch frontier),
  M0 T397 (first WOW16 UI/service boundary), M0 T400 (WRITE observation
  boundary), and M0 T401 are closed with the limits recorded in their linked
  evidence and history.
- Candidate packages are authoritative only in [QUEUE.md](QUEUE.md). No
  candidate is a numeric task or an active packet until admitted here.

## Historical Status Ledger

The complete pre-compaction status ledger is retained unchanged in
[history/status-ledger-snapshot-20260910.md](../../artifacts/documentation-archive/20260910/etc/evidence/status-ledgers/status-ledger-snapshot-20260910.md).
Use it only for historical lookup; it does not define current work.

## Status Hygiene

This page contains only the current packet state, compact baseline, and links
to retained records. Detailed run chronology belongs in `docs/etc/evidence/`;
closed-task facts belong in `docs/history/`.
