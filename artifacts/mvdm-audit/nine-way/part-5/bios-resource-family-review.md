# Part 5: DOS NTIO BIOS sources and firmware resources

The 33 owned MVDM files match both frozen original editions. Each row's SHA-256 and literal NTIO recipe references are in the companion JSON. `assets/release/ntvdm.exe` is read as data only: its BIOS1, BIOS4 and V7VGA RT_RCDATA resources match both current `roms/` and `data/` ROM copies byte-for-byte. ZIP and installed NTIO.SYS match each other. Neither resource binding nor guest image identity proves runtime startup, BIOS interrupts, mouse, timer, reset, teardown or guest source-to-image provenance.

| Owned key | Captured source/build evidence | Bounded status |
| --- | --- | --- |
| `softpc.new/bios/biosbop.inc` | makefile:35:mschar.obj mschar.lst: mschar.asm biosbop.inc $(inc)\devsym.inc \; 1 recipe matches | Historical NTIO recipe input; retention/behavior and binary lineage separate. |
| `softpc.new/bios/biosseg.inc` | No direct filename reference in captured NTIO makefile/link; 0 recipe matches | Historical BIOS source/input; exact selected use pending. |
| `softpc.new/bios/biostruc.inc` | makefile:43:$(inc)\dossym.inc $(inc)\error.inc $(inc)\version.inc biostruc.inc \; 3 recipe matches | Historical NTIO recipe input; retention/behavior and binary lineage separate. |
| `softpc.new/bios/devmark.inc` | makefile:52:$(inc)\syscall.inc $(inc)\sysvar.inc $(inc)\version.inc devmark.inc \; 3 recipe matches | Historical NTIO recipe input; retention/behavior and binary lineage separate. |
| `softpc.new/bios/msbio.lnk` | makefile:78:sysinit2.obj sysimes.obj msend.obj msbio.lnk; 3 recipe matches | Historical NTIO recipe input; retention/behavior and binary lineage separate. |
| `softpc.new/bios/msbio.skl` | makefile:29:msbio.cl1: msbio.skl \; 3 recipe matches | Historical NTIO recipe input; retention/behavior and binary lineage separate. |
| `softpc.new/bios/msbio1.asm` | makefile:32:msbio1.obj msbio1.lst: msbio1.asm $(inc)\msbdata.inc \; 1 recipe matches | Historical NTIO recipe input; retention/behavior and binary lineage separate. |
| `softpc.new/bios/mschar.asm` | makefile:35:mschar.obj mschar.lst: mschar.asm biosbop.inc $(inc)\devsym.inc \; 1 recipe matches | Historical NTIO recipe input; retention/behavior and binary lineage separate. |
| `softpc.new/bios/msend.asm` | makefile:39:msend.obj msend.lst: msend.asm; 1 recipe matches | Historical NTIO recipe input; retention/behavior and binary lineage separate. |
| `softpc.new/bios/msequ.inc` | makefile:33:$(inc)\devsym.inc msequ.inc $(cinc)\vint.inc; 4 recipe matches | Historical NTIO recipe input; retention/behavior and binary lineage separate. |
| `softpc.new/bios/msgroup.inc` | makefile:36:$(inc)\ioctl.inc msequ.inc msgroup.inc $(cinc)\vint.inc; 3 recipe matches | Historical NTIO recipe input; retention/behavior and binary lineage separate. |
| `softpc.new/bios/msinit.asm` | makefile:41:msinit.obj msinit.lst: msinit.asm $(inc)\VERSIONA.inc \; 1 recipe matches | Historical NTIO recipe input; retention/behavior and binary lineage separate. |
| `softpc.new/bios/msint13.asm` | makefile:46:msint13.obj msint13.lst: msint13.asm biostruc.inc $(cinc)\vint.inc \; 1 recipe matches | Historical NTIO recipe input; retention/behavior and binary lineage separate. |
| `softpc.new/bios/spcemm.asm` | makefile:61:spcemm.obj spcemm.lst: spcemm.asm $(cinc)\vint.inc; 1 recipe matches | Historical NTIO recipe input; retention/behavior and binary lineage separate. |
| `softpc.new/bios/spckbd.asm` | makefile:57:spckbd.obj spckbd.lst: spckbd.asm $(cinc)\vint.inc; 1 recipe matches | Historical NTIO recipe input; retention/behavior and binary lineage separate. |
| `softpc.new/bios/spcmse.asm` | makefile:59:spcmse.obj spcmse.lst: spcmse.asm $(cinc)\vint.inc; 1 recipe matches | Historical NTIO recipe input; retention/behavior and binary lineage separate. |
| `softpc.new/bios/sysconf.asm` | makefile:50:sysconf.obj sysconf.lst: sysconf.asm $(inc)\doscntry.inc \; 1 recipe matches | Historical NTIO recipe input; retention/behavior and binary lineage separate. |
| `softpc.new/bios/sysimes.asm` | makefile:55:sysimes.obj sysimes.lst: sysimes.asm; 1 recipe matches | Historical NTIO recipe input; retention/behavior and binary lineage separate. |
| `softpc.new/bios/sysinit1.asm` | makefile:63:sysinit1.obj sysinit1.lst: sysinit1.asm $(inc)\VERSIONA.inc \; 1 recipe matches | Historical NTIO recipe input; retention/behavior and binary lineage separate. |
| `softpc.new/bios/sysinit2.asm` | makefile:72:sysinit2.obj sysinit2.lst: sysinit2.asm $(inc)\copyrigh.inc \; 1 recipe matches | Historical NTIO recipe input; retention/behavior and binary lineage separate. |
| `softpc.new/data/bios1.rom` | No direct filename reference in captured NTIO makefile/link; 0 recipe matches | Published NTVDM RCDATA exact bytes (duplicate data/roms copies); partial content evidence, no runtime claim. |
| `softpc.new/data/bios4.rom` | No direct filename reference in captured NTIO makefile/link; 0 recipe matches | Published NTVDM RCDATA exact bytes (duplicate data/roms copies); partial content evidence, no runtime claim. |
| `softpc.new/data/cmos.ram` | No direct filename reference in captured NTIO makefile/link; 0 recipe matches | Duplicate data/roms bytes; selected mutable file, path and lifecycle pending. |
| `softpc.new/data/profile.spc` | No direct filename reference in captured NTIO makefile/link; 0 recipe matches | Duplicate data/roms bytes; selected mutable file, path and lifecycle pending. |
| `softpc.new/data/v7vga.rom` | No direct filename reference in captured NTIO makefile/link; 0 recipe matches | Published NTVDM RCDATA exact bytes (duplicate data/roms copies); partial content evidence, no runtime claim. |
| `softpc.new/roms/bios1.asm` | No direct filename reference in captured NTIO makefile/link; 0 recipe matches | Original firmware assembly; relation to shipped prebuilt ROM pending. |
| `softpc.new/roms/bios1.rom` | No direct filename reference in captured NTIO makefile/link; 0 recipe matches | Published NTVDM RCDATA exact bytes (duplicate data/roms copies); partial content evidence, no runtime claim. |
| `softpc.new/roms/bios2.rom` | No direct filename reference in captured NTIO makefile/link; 0 recipe matches | Original non-CPU40 alternative ROM; selected runtime/profile pending. |
| `softpc.new/roms/bios4.asm` | No direct filename reference in captured NTIO makefile/link; 0 recipe matches | Original firmware assembly; relation to shipped prebuilt ROM pending. |
| `softpc.new/roms/bios4.rom` | No direct filename reference in captured NTIO makefile/link; 0 recipe matches | Published NTVDM RCDATA exact bytes (duplicate data/roms copies); partial content evidence, no runtime claim. |
| `softpc.new/roms/cmos.ram` | No direct filename reference in captured NTIO makefile/link; 0 recipe matches | Duplicate data/roms bytes; selected mutable file, path and lifecycle pending. |
| `softpc.new/roms/profile.spc` | No direct filename reference in captured NTIO makefile/link; 0 recipe matches | Duplicate data/roms bytes; selected mutable file, path and lifecycle pending. |
| `softpc.new/roms/v7vga.rom` | No direct filename reference in captured NTIO makefile/link; 0 recipe matches | Published NTVDM RCDATA exact bytes (duplicate data/roms copies); partial content evidence, no runtime claim. |

Each recommendation concerns the specific owned file only. Preserve alternate firmware builds and the real DOS/Win16 execution contract; do not rebuild or modify guest/ROM. Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/nine-way/part-5/bios-resource-family-observer.py` and compare JSON. The original shard-1 ROM residency review retains the separate failure-handling boundary. No shared ledger, guest, product or build tree was changed.
