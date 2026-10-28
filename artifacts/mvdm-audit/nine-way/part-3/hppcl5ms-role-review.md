# Part 3 external: HPPCL5MS raster mini-driver inputs

This is a bounded subfamily review, not a verdict on the 4,772 external rows of this part. `hppcl5ms-owned-files.csv` contains **all 832 frozen part-3 external keys** whose paths are under `rasprint/mini/hppcl5ms/`, with source edition, per-key SHA-256, presence and case-insensitive basename mentions in the actual historical RC file. Each key is kept separately even when audit catalogs repeat a path. The observer is replayable; it reads original sources only. No driver or guest is built or imported.

The original `sources` file declares native DLL `hppcl5ms`, builds `code.c` and `hppcl5ms.rc`; `code.c` includes common mini-driver initialization; RC labels it an HP PCL5 printer driver and names GPC tables and numerous `RC_FONT` resources. On this profile 535 rows have a literal bounded basename hit in the RC and 297 do not. A basename hit is only a **candidate** reference: duplicate names in other directories or preprocessor branches require full path/selection validation. No hit is not nonuse: PFM, conversion inputs and installation/build metadata can be consumed elsewhere. The per-key hash proves which retained original was inspected, not execution.

Current project printing calls original WOW conversion/spool thunks into a modern native GDI/WINSPOOL provider; prior `shard-1/external-printer-provider-review.md` describes this boundary and T468's limited PDF acceptance. This native mini-driver is not automatically a project build/runtime input, but fonts, character encodings, PCL device behavior and provider differences are potentially observable. Direct public-format references, actual selected printer model/provider, installation, PFM→IFI conversion and relevant lifecycle must be checked before any per-key exclusion or compatibility claim. **Keep all 832 keys pending**; this family-level evidence narrows role and dependencies only. The observed runtime provider was not checked on a physical HP device.

Replay:

```powershell
$env:PYTHONDONTWRITEBYTECODE='1'; python artifacts/mvdm-audit/nine-way/part-3/hppcl5ms-observer.py
Get-Content O:/repos.external/OpenNT-4.5/nt/private/ntos/w32/ntgdi/printers/rasprint/mini/hppcl5ms/sources
Get-Content O:/repos.external/OpenNT-4.5/nt/private/ntos/w32/ntgdi/printers/rasprint/mini/hppcl5ms/hppcl5ms.rc
Get-Content artifacts/mvdm-audit/shard-1/external-printer-provider-review.md
```
