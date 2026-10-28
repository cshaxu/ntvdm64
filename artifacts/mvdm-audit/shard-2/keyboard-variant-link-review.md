# DOS keyboard layout variants: source-to-link selection

This extends `dos-ansi-keyboard-review.md` for five shard-2 pending files. All five source hashes match both original editions. The original and current `makefile`, `keyboard.lnk`, `kdfnow.asm`, and checked-in `KEYBOARD.map` also match both originals by SHA-256 (verify separately using ordinary file hashing). The observer checks current recipe/link and captures individual symbol declarations and archive/installed image identities.

| File | Original role and selected recipe evidence | Bounded conclusion |
| --- | --- | --- |
| `kdfbr.asm` | Exports BR_LOGIC; recipe comments out KDFBR.OBJ and link selects KDFBR275.OBJ, which also defines BR_LOGIC. `kdfnow.asm` points at BR_LOGIC. | This is an alternate provider using the same external symbol, not proof identical behavior. |
| `kdfit.asm` | Exports IT_LOGIC; old `kdf.asm` points to it. Captured `keyboard.lnk` instead selects KDFIT141 and KDFIT142, while `kdfnow.asm` references IT1_LOGIC and IT2_LOGIC. | Historical alternate header/provider pair; do not equate layouts. |
| `kdfru092.asm` | Exports RU2_LOGIC; current link selects KDFRU091 and `kdfnow.asm` references RU1_LOGIC. | The 092 variant is not selected in this recipe; do not assume RU1 and RU2 are equivalent. |
| `kdfyc116.asm` | Source declares YC1_LOGIC and translation tables, described as Russia; no KDFYC116.OBJ in captured recipe/link. | Historical variant is not selected by this recipe. |
| `kdfyc117.asm` | Source declares YC2_LOGIC and translation tables, described as Russia; no KDFYC117.OBJ in captured recipe/link. | Historical variant is not selected by this recipe. |

The checked-in map lists BR_LOGIC, IT1/IT2 and RU1 and does not list RU2/YC1/YC2, consistent with the recorded recipe. This map is not proven to have produced the shipped bytes, nor does symbol presence distinguish the two BR_LOGIC providers. The current source-tree KEYBOARD.SYS, packaged ZIP entry and installed System32 copy do match by hash. These facts do not establish the assembled keyboard table contents, supported locales, actual key mappings, or that another build profile never selects these variants. Suggested file-level classification is excluded as **unselected inputs in this specific captured guest-build recipe**, preserving keyboard layout features, source, alternate profiles and runtime behavior as separate obligations; retain pending if the governing ledger cannot express that restricted scope.

Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/shard-2/keyboard-variant-link-observer.py` and compare JSON. This audit did not execute the makefile, build or run guest binaries, or modify guest, product or shared ledger.
