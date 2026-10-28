# Shard 2: DOS BIOS inputs and firmware identities

Seven pending entries match both original trees. The observer and JSON retain source hashes, original BIOS build references, current generated firmware-resource selection and package observations. No firmware or guest was rebuilt or run.

| Entry | Evidence and bounded disposition |
| --- | --- |
| `softpc.new/bios/biostruc.inc`, `devmark.inc`, `msgroup.inc` | Original BIOS assembly includes these files; the historical NTIO makefile lists the corresponding sources and dependencies. Candidate historical guest build inputs only, subject to full selected-build/profile review; no inference about deployed NTIO binary provenance. |
| `softpc.new/bios/sysimes.asm` | Historical NTIO makefile builds `sysimes.obj`, and original `msbio.lnk` includes it. Candidate historical guest build source only; no behavior or runtime linkage is inferred. |
| `softpc.new/data/bios1.rom` | Its bytes match the separate `softpc.new/roms/bios1.rom` path named in the current generated `SOFTPC_BIOS1` resource script. Candidate **partial**: equal source bytes and resource selection do not alone prove the compiled/deployed PE resource or its execution. |
| `softpc.new/roms/bios2.rom` | Original `rom.c` names this for the non-`CPU_40_STYLE` branch and `bios4.rom` for the CPU40 branch. Alternate profile applicability and actual guest behavior are unresolved; retain **pending**. |
| `softpc.new/roms/bios4.asm` | Original firmware-related assembly; an exact build rule or relationship to the selected prebuilt `bios4.rom` was not established here. Retain **pending**, without rebuilding firmware. |

The ZIP `WINNT/SYSTEM32/NTIO.SYS` and `O:/winnt/system32/NTIO.SYS` match at `CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937`; this does not establish that the historical `softpc.new/bios` recipe produced that file. Earlier `../shard-1/rom-residency-xt-audit.md` documents the current embedded ROM selection path and its independent bounds. Reset, device initialization, real-mode interrupts, alternate firmware profiles and resource lifetime remain separately verifiable behavior, not implied by identity.

Replay `python artifacts/mvdm-audit/shard-2/dos-bios-firmware-observer.py` with `PYTHONDONTWRITEBYTECODE=1` and compare the JSON. No guest, firmware, product, installation, shared ledger or build tree was modified.
