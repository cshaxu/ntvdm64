# Shard 2: WOW32 generic thunk, SHELL and Winsock declarations

The three pending headers match both original trees. The accompanying observer normalizes the relative paths emitted by `ninja -t deps` against the actual WOW32 build directory and confirms all three occur in the captured `wowtbl.obj` dependencies. It also records relevant source-table entries, selected object edges and map symbols. Recommend **partial** for each header, subject to the boundaries below.

| Entry | Captured role | Remaining boundary |
| --- | --- | --- |
| `wkgthunk.h` | Generic kernel thunk declarations; the table names `WK32ICallProc32`. Current map distinguishes the project wrapper from the renamed original body in `wkgthunk.obj`. | Call target validity, marshalling, reentry, failure and ownership are not established by these declarations or map symbols. |
| `wshtbl2.h` | Included in the unified thunk table; it names SHELL registry, drag and other entries, including unimplemented/local slots. `WS32RegOpenKey` has a captured `wshell.obj` map entry. | Registry identity, permissions, error propagation and lifecycle require separate evidence; no blanket SHELL completeness claim. |
| `wsocktbl.h` | Declares WinSock thunk interfaces and a legacy separate-table symbol. The unified table instead incorporates `wwstbl2.h`, which names `WWS32accept`; its function is mapped to `wsraw.obj`. | Header dependency does not prove a separate `aw32Winsock` table exists, nor live socket translation, async callbacks, cleanup or any individual guest call. |

These are representative source and map observations, not linked table-relocation verification, full API coverage, or live guest evidence. Current implementation files and the project wrapper may differ from originals even though these three headers do not. The observer records graph and map hashes as a snapshot; they may change on a later rebuild.

Replay `python artifacts/mvdm-audit/shard-2/wow32-generic-shell-socket-observer.py` and compare the JSON. No guest, product, installation, shared ledger or build tree was modified.
