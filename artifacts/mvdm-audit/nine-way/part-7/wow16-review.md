# Part 7 WOW16 initial identity and component entry points

`wow16-evidence.json` is a reproducible 120-key census of current `src/mvdm` file, three original-source tree paths, ZIP basename matches and installed `O:/winnt/system32` same basename hashes. Run `PYTHONDONTWRITEBYTECODE=1 python -B artifacts/mvdm-audit/nine-way/part-7/wow16-observer.py` and capture stdout. All 120 current paths exist. The inventory is not a build proof or a behavior test; keys remain pending until actual selected identity and behavior are established.

Original component makefiles establish some production chains: `wow16/drivers/keyboard/makefile:79,106` produces keyboard.drv; mouse `makefile:79` copies mouse.exe to mouse.drv; `mciole/makefile:60-74` links and resource-stamps MCIOLE16.DLL; `ole/server/makefile:44,74-85` produces OLESVR.DLL; kernel `makefile:209,412` binplaces and names krnl386.exe. Original builds do not prove current binary build provenance.

For owned source files, the ZIP keyboard.drv, mouse.drv and MCIOLE16.DLL match the current same-named source image byte for byte. Both debug3 and retail3 krnl386.exe and debug OLESVR.DLL differ from the ZIP same-name binaries. Exact ZIP path and hashes are in evidence; do not infer a lineage solely from the name. These observations do not demonstrate those binaries are loaded, exercise their callbacks, or prove 16-bit guest behavior. Do not modify or rebuild guest.

The other owned items cross GDI metafile code, kernel scheduling, MCI, MM driver internals, OLE and RAS. They are not blanket-excluded: review of runtime selection, W32 thunk binding, resource lifecycle and guest behavior remains a functional obligation. No execution in this census.

## MCIOLE object lifecycle: static evidence

The owned `wow16/mciole/mciole.c` contains `LibMain` at line 98, object list removal in `DelObj` at 208, `CleanObjects` at 341 and `NewObj` at 357. `NewObj` invokes cleanup before finding or adding an object; `DelObj` unlinks and LocalFree's its `OBJINFO`. The `WEP` at 131 simply returns 1, so this source by itself does not establish a comprehensive DLL-unload cleanup path. Several Ole entry wrappers invoke `ReplaceFunctions` (around 542–709), and another path invokes `DelObj` followed by `CleanObjects` (953–954). Source-level evidence does not establish that the delivered MCIOLE16 binary includes exactly that source or that these paths are exercised; binary identity alone is insufficient. Behavior during reentrancy, object invalidation and unload remains untested; do not assert a defect solely from these observations.
