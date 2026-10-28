# Shard 1: pending DOS keyboard layout variants and link artifact

The observer verifies all seven paths were `待核验` in the current shard ledger. Six `.asm` files match both frozen OpenNT editions; retained `KEYBOARD.EXS` matches the OpenNT-src-2 supplemental copy. Per-file hashes, catalog identities, comparison source and checked host graphs are in the JSON. The original `makefile` and `keyboard.lnk` match both editions.

| Pending files | Bounded original role |
| --- | --- |
| `kdffr120.asm`, `kdfjp.asm`, `kdfru093.asm`, `kdfuk.asm`, `kdfur095.asm`, `kdfyc118.asm` | Historical keyboard-table definitions. Each directly includes `KEYBSHAR.INC` and `KEYBMAC.INC`. None of the six exact corresponding object names appears in this original `keyboard.lnk`; the explicit `KDFJP.OBJ` make dependency is commented. The link does name other variants such as `KDFFR189`, `KDFUK166` and `KDFRU091`. Do not infer equivalence or unsupported layouts from similar names. Several old source comments label a different locale than the filename suggests, so filename/comment alone is not a verified layout identity. |
| `KEYBOARD.EXS` | Retained historical linked guest intermediate: original `makefile` defines `KEYBOARD.exs` from the specific layout object list and uses `LINK16 @keyboard.lnk`, whose output is `KEYBOARD.EXS`; the final `keyboard.sys` target depends on it. Exact binary identity with the supplemental tree does not prove this EXS was produced by that recipe or that every selected layout works. |

The six source files are candidates for historical guest layout inputs not selected by the **inspected original link response**; they must not be excluded from all guest profiles or represented as semantically replaced. The EXS is a guest link intermediate, not a host object. None of the seven exact paths appears in the four checked x86 worker/WOW32 and x64 frontend/service Ninja graphs or dependency logs.

`src/mvdm/dos/v86/dev/keyboard/KEYBOARD.SYS`, exact installed `O:/winnt/system32/KEYBOARD.SYS` and `WINNT/SYSTEM32/KEYBOARD.SYS` in `assets/winnt.zip` are byte-identical, SHA-256 `96FCAC606683CF262DBC030713A9263731EE83F19A10207E5DD632E5EE06C2AD`. Retained EXS has a different SHA-256 (`C4BB078EC060EA3352DD307D690BE6227C6E5D699127518CAF9BD3085DF60BF1`); different bytes do not establish or disprove the original conversion lineage. No keyboard mapping, locale switching, actual input or installed profile was exercised.

Replay `python artifacts/mvdm-audit/shard-1/guest-keyboard-variants-observer.py`. No guest/product execution or changes, build mutation or shared-ledger edit occurred.
