# Shard 2: Win16 GDI guest version resource

Pending `wow16/gdi/gdi.rcv` matches both original trees. Original `gdi.rc` includes it; the original makefile compiles `gdi.res` and applies it to `gdi.exe` alongside the selected GDI guest objects. The accompanying observer records these file hashes and references. This is a candidate exclusion only as an immutable historical guest resource build input; it does not exclude GDI behavior or establish which resource bytes were compiled into the delivered image.

Source-tree `wow16/gdi/GDI.EXE`, ZIP `WINNT/SYSTEM32/GDI.EXE`, and installed `O:/winnt/system32/GDI.EXE` share SHA-256 `DFD5DC0236CAD81BE49070AC00CAE78891985AF395FA92755A621BD9C82BF3CE`. These identities do not establish live module selection, full guest-to-host thunk coverage, drawing or object lifetime, or that the checked-out recipe produced the binary. The original GDI assembler has a WOW GDI interface include, but this resource review is not a runtime thunk audit.

Replay `python artifacts/mvdm-audit/shard-2/win16-gdi-resource-observer.py` and compare its JSON. No guest, product, installed files, shared ledger or build tree was modified or run.
