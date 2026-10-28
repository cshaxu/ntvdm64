# Shard 2: Win16 kernel guest inputs and image identities

Four pending entries were compared against both original source trees and the frozen catalog. The accompanying read-only observer and JSON record the hashes and build references; no guest or product was executed or modified.

| Entry | Evidence and limited recommendation |
| --- | --- |
| `KRNL386.RES` | Matches both originals. The original 386 build recipe lists this resource and applies it to `kernel.exe` with `rc16`. Candidate exclusion as immutable guest build input only, without assuming it was used for the delivered binary. |
| `ksource.txt` | Matches both originals; the document describes the kernel source and build directories. Candidate exclusion as historical documentation. |
| `newexe.doc` | Matches both originals; historical documentation describing the new executable format. Candidate exclusion as documentation, not exclusion of the NE loader behavior it describes. |
| `retail3/krnl386.exe` | Matches OpenNT 4.5; absent from the other original tree and DOS catalog. The original makefile documents the retail3 build, but the exact binary's build lineage is unproved. Its SHA-256 differs from the current ZIP and direct installed kernel, so retain as a distinct guest image pending identity/role decision. |

ZIP `WINNT/SYSTEM32/KRNL386.EXE` and `O:/winnt/system32/KRNL386.EXE` match each other with SHA-256 `D404576899CC11D23F368F2B30554E6108657CD37809837B341FD37E6B1A7AF4`; the retained retail3 image is `BCED7650A5B5D7AC25955AB2A21BC53FF06DE1C35B25FAD0690390135B093A03`. Do not conflate these images or infer the deployed kernel's source or behavior from the retail3 identity. The prior `guest-loader-location-audit.md` gives the source-derived search chain and its limits; runtime module selection, loading, task lifecycle and guest ABI remain separate verification questions.

Replay `python artifacts/mvdm-audit/shard-2/win16-kernel-guest-observer.py` and compare the companion JSON. These recommendations concern individual source/document file roles, not deleting files or weakening Win16 kernel requirements.
