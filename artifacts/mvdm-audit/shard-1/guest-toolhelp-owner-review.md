# Shard 1: immutable Win16 TOOLHELP input group

The observer individually reconciles 15 shard-1 `wow16/toolhelp` entries against the frozen source catalog and the byte-identical original `makefile`. Nine source/resource/include files match both OpenNT editions; six prebuilt OMF objects match the T301 supplemental OpenNT-src-2 copy. The supplemental comparison establishes identical retained bytes, not how the object was produced. Each entry's SHA-256, reference edition and exact original recipe reference line is recorded in `guest-toolhelp-owner-observations.json`.

| Files | Original role and recipe relation |
| --- | --- |
| `global.c`, `local.c` | Win16 global and local heap enumeration helpers; original C compilation inputs to `global.obj`, `local.obj`. |
| `memman.asm`, `memory.asm` | Memory manager information and selector memory read/write guest routines; original assembly inputs to `memman.obj`, `memory.obj`. |
| `notify1.c` | Win16 task notification registration/unregistration source; original `notify1.obj` input. |
| `stack2.asm`, `terminat.asm` | Guest stack frame walk and task-termination helper source; original `stack2.obj`, `terminat.obj` input. Source presence does not prove current NT Monitor termination uses these guest routines. |
| `int2.OBJ`, `local.obj`, `notify1.obj`, `timer.OBJ`, `toolhelp.obj`, `usergdi2.OBJ` | Retained guest object intermediates, explicitly named in the original `toolhelp.dll` object/link list. Their byte identity is supported by the supplementary source tree, not by a source-to-object build receipt. |
| `toolhelp.rcv` | Version-resource source consumed by the original RC invocation on the DLL. |
| `toolpriv.inc` | Shared assembly include explicitly referenced by the original per-object dependency rules. |

The original makefile's `LINK16` response constructs TOOLHELP.DLL from the named guest objects and then invokes `RC16` with `toolhelp.rcv`. Four current checked x86 worker/WOW32 and x64 frontend/service Ninja graphs and dependency logs contain none of the 15 exact guest input paths. On this bounded evidence, the 15 files are **candidates for historical guest-build inputs/intermediates, excluded only from these checked host build inputs**; this does not exclude TOOLHELP as a guest feature, or establish all project targets.

The frozen `src/mvdm/wow16/toolhelp/TOOLHELP.DLL` and `WINNT/SYSTEM32/TOOLHELP.DLL` inside `assets/winnt.zip` are byte-identical, SHA-256 `DDEECCC103F7E3C105602A6F791792BAA7778A796709BE63983A8A921DF80CFE`. The exact direct path `O:/winnt/system32/TOOLHELP.DLL` was absent at observation; this says nothing about other locations or future stages. Binary equality does not prove the retained object/source group built that DLL or that the DLL is loaded and its APIs work. The current WOW32 TOOLHELP thunk table contains three `UNIMPLEMENTEDAPI` rows; this limited table must not be equated to the full 16-bit DLL API surface.

Replay `python artifacts/mvdm-audit/shard-1/guest-toolhelp-owner-observer.py`. No guest was built or changed; no product, shared ledger or build tree was modified, and no runtime was executed. Actual guest API behavior, image lineage and other product configurations remain unverified.
