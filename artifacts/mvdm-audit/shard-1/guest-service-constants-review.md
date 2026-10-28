# Shard 1: pending guest service definitions and C counterparts

The observer confirms all four scoped `.INC` files are `待核验` in the current shard ledger and byte-identical to both frozen OpenNT editions. It compares their simple numeric `EQU` definitions against identically named numeric literals in the companion C headers. VALID Ninja dependency paths are normalized against each of four checked x86 worker/WOW32 and x64 frontend/service build directories; recorded counts are dependency records, not API coverage.

| File | Original purpose and checked evidence |
| --- | --- |
| `inc/DOSSVC.INC` | DOS service selectors; guest DOS kernel and COMMAND sources include it and it emits DOS BOP selector calls. All 81 shared simple numeric constants match `dossvc.h`; that C header has 17 checked VALID host dependency records. |
| `inc/DPMI.INC` | DPMI service selectors and DPMI BOP macro; guest DOSX and Win16 kernel source includes it. All 38 shared simple numeric constants match `dpmi.h`; that C header has 17 checked VALID dependency records. |
| `inc/XMSSVC.INC` | XMS service selectors and XMS BOP macro; original HIMEM and DOS BIOS source includes it. All 13 shared simple numeric constants match `xmssvc.h`; that C header has four checked VALID dependency records. |
| `inc/SOFTPC.INC` | Original error/display and related constants, included by DOS BIOS and Win16 kernel source. All 66 shared simple numeric constants match `softpc.h`, which has 144 checked VALID dependency records. One ASM-only numeric literal `EXPORT=1` has no matching simple C `#define`; it is recorded, not assumed to be a missing service. |

For DOS, XMS and DPMI the original `BOP.INC` declares 50h, 52h and 53h respectively. None of the four guest `.INC` paths occurs as an exact VALID dependency in the four host C graphs. This is expected given their assembly role and does not prove the real guest binaries were generated from these files. The C counterparts **are** used and must not be excluded as unreferenced.

Numeric equality does not validate expression-valued macros, packed structure layout, selector-to-function table mapping, service ownership, error handling or guest lifetime. Existing `shard-3/xms-service-role-review.md` separately follows one XMS path; no claim is made that these constants prove all services execute. Preserve current pending classifications until the relevant ABI and runtime evidence is reconciled.

Replay `python artifacts/mvdm-audit/shard-1/guest-service-constants-observer.py`; JSON records per-file source/catalog hashes, comparisons and normalized dependencies. No guest, compiler, provider or product was run, and no guest, product, build tree or shared ledger changed.
