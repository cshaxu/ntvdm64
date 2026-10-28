# Win16 WRITE: conditional linker response and retained inputs

This grouped update covers all ten still-pending shard-2 WRITE entries and extends `win16-write-guest-review.md`. Nine files match both original editions; `write16.exe` matches OpenNT 4.5 and is absent from OpenNT. Current WRITE.EXE and write16.exe match one another but differ from the byte-identical ZIP/installed WRITE.EXE; do not substitute or overwrite them. Source/build relationships do not prove which source built either image or that a guest executed it.

| Pending file | Evidence and restricted disposition |
| --- | --- |
| `dtabbtn.bms` | Original bitmap-style data; exact captured selected consumer unresolved. **Pending**, retain UI functionality. |
| `dtabmark.bms` | Same unresolved resource consumption. **Pending**. |
| `just.bms` | Same unresolved resource consumption. **Pending**. |
| `sp15.bms` | Same unresolved resource consumption. **Pending**. |
| `editsect.c` | Has a historical makefile object rule, but checked-in write.lnk does not list editsect.obj. **Pending** alternate link/profile use; lack of a listed object alone is insufficient to delete. |
| `form1.c` | Has object rule, while observed DBCS `format.obj` recipe uses `d_form1.c` instead. **Pending** independent or alternate profile use; don't confuse similarly named files. |
| `help.c` | DBCS branch sets `OBJ_DBCS=help.obj`; more importantly makefile's DBCS `sed` rule rewrites the generated write.lnk to insert help.obj after format.obj. This resolves the earlier mystery as a **historical DBCS guest build input**, though it does not demonstrate that a captured binary was built with DBCS or that help works. |
| `looks.h` | Exact selected consumer remains unresolved. **Pending**. |
| `WRITE.EXE` | Dual-original retained image differs from ZIP and installed image. **Pending** provenance and live loader selection; retain both. |
| `write16.exe` | OpenNT-4.5 retained image matches preceding source-tree image, absent from OpenNT; differs from release. Original recipe copies write.exe to write16.exe, but no proof that this particular file was created by that recipe. **Pending** provenance and selection. |

The checked-in write.lnk does not itself include help.obj; the conditional generation rule modifies it for DBCS. This is source-level build intent only; other rule overrides, generated inputs, resource and actual document/editor/callback behavior remain untested. Never execute the historical makefile here: its cleanup deletes guest source-tree artifacts. No guest, product, build tree, or shared ledger was modified.

Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/shard-2/write-dbcs-family-observer.py` and compare JSON; replay `win16-write-guest-observer.py` for prior package/source evidence.
