# S10 shard 3: legacy GISP SVGA and saved VGA state interfaces

Read-only review of pending `softpc.new/base/inc/gispsvga.h` and `vgastate.h`; neither product, guest, shared ledger, nor build was changed. Both hashes match both original inventories in `file-review.csv`. No runtime tests were run.

| File | role and recommendation | evidence and boundary |
|---|---|---|
| `gispsvga.h` | **selected include; conditional declaration carrier, not a restored-feature claim** | SHA-256 `27CF2B856A23D0FB84E1162959043377DADE1414D063349ACAF952CF741BF60E`. Header declares only `romMessageAddress` and `gispROMInit` under `GISP_SVGA`. Valid dependency index reports selected `bios.c` and `rom.c` includes. `bios.c:322-325` dispatches BOP 53 conditionally, `support/main.c` calls ROM init for GISP; `rom.c` has alternate ROM initialization. Build.ninja textual search has no `GISP_SVGA`, but this alone does not prove that transitive headers never define it; final macro state and extracted symbols need verification before classifying the functionality. |
| `vgastate.h` | **unselected legacy state carrier in observed dependency index; recommend file-level exclusion, retaining feature caveat** | SHA-256 `BFFF3B82A50C1AC89D1BEB0195CFFD7E4FB82A90715C25D279FD8AA8BD77580C`. Describes saved VGA registers, additional SVGA registers and four plane-data pointers. `hwvga.h` refers to its `VGAState` / `pVGAState` types, while the selected dependency index has zero exact `vgastate.h` hits. This is not evidence that current VGA state saving or Video7 itself is absent; alternate profiles, assembly and guest consumers were not tested. |

To replay from repo root in PowerShell:

```powershell
Get-FileHash src/mvdm/softpc.new/base/inc/gispsvga.h,src/mvdm/softpc.new/base/inc/vgastate.h -Algorithm SHA256
rg -n 'gispsvga\.h|vgastate\.h' artifacts/mvdm-audit/shard-3/header-inclusions.csv artifacts/mvdm-audit/shard-3/file-review.csv
rg -n '\b(VGAState|pVGAState|gispROMInit|romMessageAddress)\b|GISP_SVGA' src/mvdm/softpc.new/base
rg -n 'GISP_SVGA' build/.ninja/msvc-x86-release/build.ninja
```

The dependency index already normalizes the worker source and object paths, and evidence is from that snapshot. These are header-role conclusions only, not VGA/Video7 runtime coverage or restored behavior percentages. Confirm macro definitions and final symbols if a definitive GISP branch classification is required.
