# Shard 2: WOW32 USER declarations and selected owners

Five pending headers match both original trees. Current `precomp.h` includes each; the companion observer captures that source relationship, a representative original-table reference, corresponding current Ninja object edge and map entry per family. These are **partial** candidates, not proof of every declaration's use or complete function behavior.

| Header | Sample selected owner | Important boundary |
| --- | --- | --- |
| `wuclip.h` | `WU32OpenClipboard`, `wuclip.obj` | Ownership and formats, delayed rendering, cross-task lifetime and failures require separate evidence. |
| `wucomm.h` | `WU32OpenComm`, `wucomm.obj` | Port configuration, async I/O, real serial partner and teardown are not proved by selection. |
| `wudlg.h` | `WU32DialogBoxParam`, `wudlg.obj` | Dialog resource identity, nested callback/reentry, parent ownership and failure cleanup remain independent. |
| `wuman.h` | `WU32MouseEvent`, `wuman.obj` | Event propagation, coordinates and input state are not proved by the declaration or map. |
| `wuser31.h` | `WU32SetWindowPlacement`, `wuser31.obj`; source also uses its `WINDOWPLACEMENT16TO32` macro | Guest structure conversion, window lifetime and placement semantics require behavioral verification. |

Each sample name appears in the current `wutbl2.h` source; this observation does not inspect the linked table relocation or prove that an actual Win16 call reaches it. The source files can have existing registered mirror adaptations; exact header identity cannot establish whole-module original identity. The captured map and Ninja graph are snapshots, not evidence that each guest API executed. Existing targeted clipboard/dialog/serial findings remain authoritative within their own test boundaries.

Replay `python artifacts/mvdm-audit/shard-2/wow32-user-header-observer.py` and compare its JSON. No guest, product, installation, shared ledger or build tree was changed.
