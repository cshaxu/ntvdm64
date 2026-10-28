# Part 8: Win87 floating-point emulator source/profile boundaries

The 15 owned Win87 source and metadata paths were individually identity-checked in `guest-family-inventory-review.md`. The historical `wow16/win87em/makefile:63,75-108,626-680` defines several profiles: Windows `win` target chooses `win87em`, assembles `emulator.asm` with WINFLAGS and `emoemwin.asm`, links a DLL, and builds an import/static library with `wfpsig.obj`. Shared EMASM is a dependency inventory, not proof every listed source fragment is active in that Windows assembly. `makefile:720` uses `ifstrip` and `switch.src` for a different generated OEM source; do not flatten profiles.

Original `wfpsig.asm` posts WM_QUIT on floating-point emulator error. This is guest emulator behavior, distinct from the CPU40 x87 instruction executor and its exception mechanism. `apisim.asm` identifies itself as DOS OS/2-family simulation with conditional stubs; its historical presence cannot establish it executes in the Windows profile. `key` explains naming and switches; it is not an execution selector by itself.

| Owned path | Original role / classification boundary |
| --- | --- |
| `wow16/win87em/apisim.asm` | Shared emulator source listing; DOS-family simulation, active Windows inclusion unproved. |
| `wow16/win87em/emds.asm` | Shared emulator data source included by emdos; active condition/profile unproved. |
| `wow16/win87em/emoem.src` | OEM-derived source input/output according to historical ifstrip recipe; generation history unproved. |
| `wow16/win87em/emoemqb.asm` | QuickBASIC-specific OEM source in source list; do not infer Windows selection. |
| `wow16/win87em/emoemwin.std` | Historical OEM Windows variant text; actual selected input unresolved. |
| `wow16/win87em/emoemwin.t1` | Historical alternate template; actual selected input unresolved. |
| `wow16/win87em/emoemwin.t2` | Historical alternate template; actual selected input unresolved. |
| `wow16/win87em/key` | Original naming/switch explanatory document; not a provider. |
| `wow16/win87em/switch.an` | Historical switch input; active recipe/consumer unresolved. |
| `wow16/win87em/switch.n` | Historical switch input; active recipe/consumer unresolved. |
| `wow16/win87em/switch.src` | ifstrip configuration input in historical OEM recipe. |
| `wow16/win87em/wfpsig.asm` | Windows floating-point error routine; historical WIN87EM library recipe explicitly selects it. |
| `wow16/win87em/win87em.h` | Original interface declarations; exact consumer/profile pending. |
| `wow16/win87em/win87hdr.txt` | Historical header documentation; not runtime code. |
| `wow16/win87em/win87std.def` | Original module definition for a variant; selected historical WIN87EM DLL recipe names `win87em3.def`, not this file. |

Historical `m0-t420-s21-guest-console-witness.md:27,69,74` reports actual DOS guest CPU/FPU instruction markers and an x87 layout fixture for a specific candidate. That supports its selected CPU40 code paths, not WIN87EM DLL loading, guest emulator error handling, task termination or state restore. No test is run here.

S11 should retain CPU x87 instruction/state/exception validation; S14 should first identify an immutable Win16 caller and actual WIN87EM image/profile, then validate save/restore, task reentry and emulator failure signaling against original semantics. S19 binds observations to release identity. Do not remove an emulator, switch templates or guest capability based on build-role classification, and do not invent an x87 provider based only on missing evidence.

Replay `rg -n "WINFLAGS|win87em|wfpsig|switch.src" src/mvdm/wow16/win87em/makefile`, inspect `wfpsig.asm`, `apisim.asm` and `key`, and rerun part-8 inventory observer. No guest, product, shared ledger, or build changes.
