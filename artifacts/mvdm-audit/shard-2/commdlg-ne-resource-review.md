# Win16 COMMDLG family: released NE bitmap resource and inputs

This completes a grouped evidence update for seven shard-2 pending entries, extending `win16-commdlg-guest-review.md`. All seven bytes match both original trees. The original `COMMDLG.DLL`, current source-tree copy, ZIP member and installed System32 copy are byte-identical. A read-only NE resource table parse finds bitmap resource numeric ID 576 (`0x8240` encoded ID) at file offset 18,240; its content starts with the complete DIB payload of `filebmps.bmp` (BMP file header excluded). The USA resource script selects this bitmap. This establishes specific shipped resource bytes, not image build provenance, rendering, or operation of any common-dialog behavior.

| Pending entry | Evidence and bounded classification |
| --- | --- |
| `colordlg.h` | Historical color.dlg includes it; recipe compiles dialog resources. **Historical guest build input** in captured recipe, not exclusion of color dialog behavior. |
| `parse.h` | Historical parse.c includes it and recipe compiles parser. **Historical guest build input**; parsing and failure semantics untested. |
| `privcomd.h` | Historical font.c includes it. **Historical guest build input**; font dialog operation untested. |
| `STRIPPED.DEF` | Historical recipe invokes mkpublic from commdlg.def and passes stripped.def to implib. **Historical generated build role**; checked-in file's exact provenance not established. |
| `smfldr2.bmp` | Retained original bitmap; USA script selects filebmps.bmp instead and this entry is not named there. **Pending**: other resource preparation, alternate locale or embedded subimages not excluded. |
| `smfldr3.bmp` | Same limited observation. **Pending**, without asserting graphics are absent. |
| `COMMDLG.DLL` | Byte-identical original, published, and installed image. Specific NE bitmap resource now matches selected source DIB. Candidate **partial**; imports, actual loader resolution, selection, callback reentrancy and dialog lifecycle remain untested here. |

Use the earlier observer for source/recipe includes, build relationships, and original hash verification; `commdlg-ne-resource-observer.py` independently checks all seven identities, packaging and the NE resource table. Replay both with `PYTHONDONTWRITEBYTECODE=1`, comparing JSON. Historical makefile was not executed (its clean rule deletes guest files). No guest, product, build tree, or shared ledger was modified.
