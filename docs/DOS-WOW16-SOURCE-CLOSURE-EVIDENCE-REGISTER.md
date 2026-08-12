# DOS/WOW16 Source-Closure Evidence Register

Status: authoritative local evidence register, 2026-08-09.

This register preserves the two deliverables of the DOS/WOW16 source-closure task as evidence inputs for later work. They establish artifact provenance and source-closure status only; neither is evidence of guest execution, WOW execution, an x64 `ntvdm.exe` runtime, redistribution permission, or byte-identical retail output.

| Evidence | Authority | What it proves | Regeneration / verification |
| --- | --- | --- | --- |
| [dos-wow16-source-closure-inventory.json](dos-wow16-source-closure-inventory.json) | Machine-readable per-artifact ledger | Each included candidate has exactly one classification, source/install anchor, source revision, and evidence or first blocker. Source-built records retain output hash/size, tool list, stage manifest, and historical command-manifest paths. | `powershell -NoProfile -ExecutionPolicy Bypass -File tools\Export-DosWow16SourceClosureInventory.ps1 -RepositoryRoot . -OutputDirectory docs` |
| [DOS-WOW16-SOURCE-CLOSURE-RESULT.md](DOS-WOW16-SOURCE-CLOSURE-RESULT.md) | Human-readable closure decision | The interpretation of the ledger: 27 source-built artifacts; the four `binplace` inputs remain excluded; the five WOW16 drivers do not close the WOW16 runtime; runtime admission is separate. | Review against the generated ledger; run the verification below. |

## Required verification

Run from the repository root:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tests\source-closure-inventory.ps1 -RepositoryRoot .
```

The check regenerates the ledger and rejects missing classifications/anchors, an altered exclusion set, source-built entries without retained identity and command evidence, incorrectly admitted WOW16 runtime candidates, or omission of non-DOS/WOW16 guest-side candidates.

## Preservation rules

1. Do not change a `source-built` status without replacing its retained output and command evidence.
2. Do not change `BACKUP.EXE`, `QBASIC.EXE`, `EDIT.COM`, or `MSHERC.COM` from `excluded-prebuilt` without a provenance-matching source/build rule; a `binplace` rule is insufficient.
3. Do not infer a runtime result from either evidence item. Runtime claims remain governed by `ROADMAP.md` and `SOFTPC-RECONSTRUCTION-GOVERNANCE.md`.
4. When the fixed source island changes, regenerate the inventory and review the result document against the new ledger before making a closure claim.
