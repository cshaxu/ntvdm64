# Win16 kernel and executable stub: retained and delivered identities

Five pending shard-2 entries cover two related historical build roles. `winstub.exe` and `retail3/krnl386.exe` match OpenNT 4.5 and are absent from the retained OpenNT edition; the other three entries match both originals. These are immutable guest inputs/artifacts: no original guest or tool was rebuilt or run.

| File | Evidence and suggested constrained disposition |
| --- | --- |
| `wow16/bin/winstub.exe` | Original module definitions for WRITE and COMM reference the stub. The 546 bytes after its 64-byte MZ header match the corresponding bytes in both published WRITE.EXE and COMM.DRV; ZIP and installed copies of each binary match in full. Candidate **partial** for the sampled DOS stub payload, not a proof of historical linker provenance, loading or overall WRITE/COMM behavior. Published KRNL386.EXE does not share that sampled payload. |
| `wow16/kernel31/KRNL386.RES` | Historical kernel makefile compiles/applies krnl386.res in its 386 path. Candidate **historical guest build resource** only; no proof this retained resource generated the shipped kernel. |
| `wow16/kernel31/ksource.txt` | Historical descriptive directory/source inventory. Candidate **documentation-only role**, preserving all referenced kernel functionality. |
| `wow16/kernel31/newexe.doc` | Historical explanatory New Executable format document. Candidate **documentation-only role**, not an exclusion of NE parsing, relocation or loader behavior. |
| `wow16/kernel31/retail3/krnl386.exe` | Original makefile selects `386P=1 DEST=RETAIL3`; retained image matches OpenNT 4.5 only, while ZIP and installed System32 KRNL386.EXE match each other but differ from this image. **Pending** provenance, actual loader selection, and behavioral equivalence. |

Stub payload equality intentionally skips the MZ header, which a linker may modify; it is a byte comparison of one region, not a complete PE/NE or source-to-binary match. Kernel loading, task dispatch, error/return handling and alternate boot scenarios remain separate obligations. The source-derived guest loader and search-path limitations remain recorded in `guest-loader-location-audit.md`.

Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/shard-2/win16-kernel-stub-owner-observer.py` and compare JSON. No guest, product, shared ledger or build tree changed.
