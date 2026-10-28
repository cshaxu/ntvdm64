# Shard 1: pending Win16 KERNEL historical variants and build inputs

The five entries below were `待核验` in the current shard ledger. Four match both frozen OpenNT editions; `retail3/kernstub.exe` matches the 4.5 edition, with no primary-edition hash recorded. The original makefile, two link response files, definition, stub source and separate SWAP makefile match both editions. Per-file hashes and normalized VALID dependency hits for four checked host graphs are in the JSON.

| File | Original role and bounded evidence |
| --- | --- |
| `wow16/kernel31/krnl286.rc` | Resource source declaring the original 286 kernel filename. The checked makefile says NT's default retail/debug target uses 386, with explicit 286 recipes retained; its selected 386 resource rule uses `krnl386.rc`, not evidence that this 286 RC contributed to the installed kernel. |
| `wow16/kernel31/log.slm` | Retained source-management history, not by itself source code or proof of the installed binary's provenance. No active build consumer was established. |
| `wow16/kernel31/retail3/kernstub.exe` | Retained guest build stub binary: the checked `kernel.def` names `KERNSTUB.EXE` and the recipe builds it from `kernstub.asm` and makes the kernel depend on it. The retained binary matches the 4.5 reference; no evidence establishes that this exact stub was embedded in the installed kernel. |
| `wow16/kernel31/swappro/swap.doc` | Historical documentation describing the separate `SWAP.EXE`/special kernel analysis workflow. `swappro/makefile` separately builds `swap.exe` from `swap.c`; the document is not a proven executable input or evidence the tool ran. |
| `wow16/kernel31/w32sys.asm` | Original Win32S-support source exposing `GetW32SysInfo`, with an explicit object rule. `w32sys.obj` occurs in neither checked 286 nor 386 link response. Separately, `i21task.asm` mentions loading `W32SYS.DLL`; that name is not evidence this `w32sys.asm` was linked into that external DLL or executed. |

No exact scoped file occurs in the normalized VALID dependencies of the four checked x86 worker/WOW32 and x64 frontend/service Ninja graphs. These observations are candidate historical guest/build/documentation roles only for the inspected recipes and host graphs; they do not exclude 286 guest variants, Win32S, SWAP, kernel bootstrap or other guest features.

The retained `retail3/krnl386.exe` SHA-256 is `BCED7650A5B5D7AC25955AB2A21BC53FF06DE1C35B25FAD0690390135B093A03`, while exact installed `O:/winnt/system32/KRNL386.EXE` and ZIP `WINNT/SYSTEM32/KRNL386.EXE` match each other at `D404576899CC11D23F368F2B30554E6108657CD37809837B341FD37E6B1A7AF4`. The mismatch is also recorded in existing DDEML evidence; it means these retained source/response observations cannot be presented as the installed binary's exact build lineage.

Replay `python artifacts/mvdm-audit/shard-1/guest-kernel-legacy-observer.py`. No guest, compiler, product or provider ran; no guest, product, build tree or shared ledger changed. Guest loader selection, stub lineage, alternate profiles and runtime behaviors remain unverified.
