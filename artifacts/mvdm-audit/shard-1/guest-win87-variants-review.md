# Shard 1: pending Win16 87 emulator historical variants

The three scoped files are `待核验` in the current shard ledger and match both frozen OpenNT editions. Their hashes, original recipe context and four normalized host Ninja VALID dependency checks are recorded in the companion JSON. None of the three paths appears in those checked x86 worker/WOW32 and x64 frontend/service dependency logs.

| File | Original role and bounded build evidence |
| --- | --- |
| `wow16/win87em/emoem.src` | OEM customization source variant. The original makefile explicitly defines an `emoem.src` target generated from `emoem.asm` using `IFSTRIP` and `switch.src`; retained byte identity does not prove that transformation actually generated this file. |
| `wow16/win87em/emoemwin.std` | Historical standard-named 8087/80287 OEM source variant. The checked DLL rule compiles `emoemwin.asm`, not `.std`; their bytes differ. No direct selected consumer for `.std` is established by this recipe, and other historical variants remain possible. |
| `wow16/win87em/win87std.def` | Historical Windows 3.1 emulator module definition. The checked WIN87EM.DLL target instead links `win87em3.def` with `win87em.obj` and `emoemwin.obj`; the definitions are not byte-identical. The omission from that recipe does not prove another profile never used `win87std.def`. |

The original guest Win16 kernel `ld.asm` has a `WIN87EM.DLL` loader-name reference; the CCPU has separate x87/NPX execution paths. Do not conflate the Win16 emulator DLL, its historical source variants and the host CPU floating-point engine. The frozen `src/mvdm/bin86/i386/win87em.dll` and ZIP member `WINNT/SYSTEM32/WIN87EM.DLL` are byte-identical, SHA-256 `30D6B965D45EDDF306268C9526B01494DF4841B8B530BAB2654C379E1CF49DB2`; the exact `O:/winnt/system32/WIN87EM.DLL` path was absent at observation. No source-to-binary lineage or actual guest DLL loading follows.

These files are candidates for historical guest emulator build/variant roles only in the inspected recipe and host graphs; guest x87 computation, state save/restore, exceptions and alternate profiles remain unverified. Existing shard-2 WIN87EM report addresses other files, not these three. Replay `python artifacts/mvdm-audit/shard-1/guest-win87-variants-observer.py`. No guest, product, compiler or provider ran; no guest, product, build tree or shared ledger was modified.
