# DPMI guest startup, disk and ROM source-input review (shard 3)

Seven pending source paths are reviewed as one original guest build/call chain. `dpmi-guest-source-observer.py` and `dpmi-guest-source-observations.json` preserve exact SHA-256 checks: all seven plus `makefile` and `dosx.lnk` are byte-identical in both reference trees. No guest was rebuilt or modified.

Historical build evidence: `src/mvdm/dpmi/makefile:10-16,38-71,75-85,109-112` selects the `486` object set to build DOSX.EXE, explicitly lists dxboot, dxdisk and dxrom, assembles `486/dxboot.asm`, and links through platform-selected response file. `dosx.lnk` and `dosxi.lnk` list dxdisk, dxrom and dxboot objects. For x86 the makefile selects `dosxi.lnk`; do not assume the other link file is the active one. Existing `guest-input-review.md` records deployed DOSX.EXE identity with the archive manifest, but does not prove this source built that binary.

| path | proposed status | original call/input evidence and limit |
|---|---|---|
| `486/dxboot.asm` | pending | startup source explicitly selected for the historical DOSX 486 build; host `dpmi32/data.c:168` and `modesw.c:85-98` describe DOSX selector/IDT exchange. Identity does not prove the particular installed DOSX source provenance or exercise all bootstrap branches. |
| `dxdisk.asm` | pending | historical `PMIntr13`, `PMIntr25`, `PMIntr26`, `ReadWriteSectors` entrypoints (`:156,379,458,599`), included in both response files. Disk transfer directions, failure and protected-mode boundary not yet demonstrated for this binary. |
| `dxrom.asm` | pending | historical `ROMEntry`, `InvokeROMChild`, `ROMInitialization`, `ROMCleanUp` (`:139,249,407,444`); linked historically but runtime ROM path and environment are not proven. |
| `dxrom.inc` | pending | included by dxboot, dxdisk, dxrom and other DOSX units; definitions participate in the historical build, not separately observable as executable code. |
| `gendefs.inc` | pending | included throughout DOSX including dxboot, dxdisk, dxrom; guest semantic coverage follows users of the definitions. |
| `hostdata.inc` | pending | included by dxstrt, dxfunc and dxmain, including 486 variants; host data boundary requires source-to-installed-image and functional proof. |
| `woaswapi.inc` | pending | included by dxfunc and dxmmgr; historical API definitions require selected image/caller and behavior verification. |

Status is intentionally conservative: do not exclude guest functionality merely because host build does not compile assembly, or equate installed guest archive identity with source-to-image correspondence. No restored-code percentage or all-path DOS/WOW claim follows from these observations; per-route execution and failure evidence remain open. `dxrom.asm` historical ROM support in particular must not be inferred from ordinary DOSX launch alone.
