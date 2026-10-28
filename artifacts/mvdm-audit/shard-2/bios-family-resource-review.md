# BIOS firmware and CMOS family: selected resources and source roles

This extends `dos-bios-firmware-review.md` for all eight pending shard-2 BIOS/firmware/CMOS entries. The observer checks each against both original editions and reads the published NTVDM executable **as a data file**, without executing it. `FindResource`/`LoadResource` identifies RT_RCDATA `SOFTPC_BIOS1` and `SOFTPC_BIOS4`: their full bytes match current `roms/bios1.rom` (28,420 bytes) and `roms/bios4.rom` (8,191 bytes), respectively. The pending `data/bios1.rom` is byte-identical to the first. This establishes current published resource content, not execution, failure handling or source-to-binary provenance.

| Pending file | Evidence and suggested restricted disposition |
| --- | --- |
| `softpc.new/bios/biostruc.inc` | Prior family observer establishes original BIOS assembly include and historical NTIO build dependency. **Historical build input only** in captured recipe; no guest behavior or provenance assertion. |
| `softpc.new/bios/devmark.inc` | Same prior source/recipe evidence. **Historical build input only** in captured recipe; retain device behavior. |
| `softpc.new/bios/msgroup.inc` | Same prior source/recipe evidence. **Historical build input only** in captured recipe; retain group/segment and runtime obligations. |
| `softpc.new/bios/sysimes.asm` | Prior observer: historical NTIO makefile selects sysimes.obj, msbio.lnk includes it. **Historical build input only** for that recipe; binary binding and behavior not established. |
| `softpc.new/data/bios1.rom` | Full bytes equal published BIOS1 RCDATA via identical `roms/bios1.rom`; candidate **partial** for published content. Runtime read, reset, interrupts and memory protection remain untested. |
| `softpc.new/data/cmos.ram` | Original source `cmos.c` calls host_read_resource with CMOS_REZ_ID and writes back only after read and change. **Pending**: no evidence this particular historical data file is the selected runtime file; file location, reset, persistence and failure need separate verification. |
| `softpc.new/roms/bios2.rom` | Original `rom.c` selects bios2 in the non-CPU40 branch and bios4 for CPU40; observed current resource provider handles bios1/bios4/v7vga. **Pending** for alternate profile applicability; do not assume universal unused status. |
| `softpc.new/roms/bios4.asm` | Historical firmware assembly retained; exact assembly-to-published-bios4 binary provenance is unproven. **Pending**; matching published RCDATA to prebuilt ROM is not proof of assembly lineage. |

Original guest NTIO archive and installation identity and prior source details are in `dos-bios-firmware-review.md`; missing/invalid resource handling concerns remain in `../shard-1/rom-residency-xt-audit.md`. No guest or firmware was executed or rebuilt; no shared ledger, build tree or product changed. Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/shard-2/bios-family-resource-observer.py` and compare its JSON; replay the older `dos-bios-firmware-observer.py` for historical recipe assertions.
