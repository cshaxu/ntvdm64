# T188 S7 copied-state derivative closure 001

Date: 2026-08-12  
Packet: M0 T188 S7  
Disposition: r3 is build-admitted; no compiler or runtime action yet.

## Closure

The retained T187/r5 engine was linked with MSVC `/MT` adapter objects. The current copied-state API changes only `adapter/bx_ntvdm_adapter_runtime.c` and `.h`; its existing consumers and all other adapter/CLI interfaces are unchanged. The same r5 makefile already compiles that adapter object with `cl.exe /MT /W3 /DWIN32 /D_WINDOWS /D_CRT_SECURE_NO_WARNINGS /Iadapter /Icli`, matching the retained island.

`tools/New-T188S7CopiedStateTraceDerivative.ps1` copies the exact r5 root, replaces only current `cpu/exception.cc` and adapter runtime source/header, and emits two build rules: `cpu/exception.o` and `adapter/bx_ntvdm_adapter_runtime.obj`. Its final link keeps 71 other adapter/CLI objects, all retained Bochs archives and machine inputs. The manifest is `artifacts/build/current/t188-copied-state-derivative-r3/t188-s7-copied-state-derivative.json`.

## Generator attempts

r1 and r2 stopped in the generator's structural target check before any compiler or linker invocation. r1 treated `main.o` in the final link as a target; r2 counted only `.obj` targets and omitted `exception.o`. r3 corrects the check and is the sole formal root. Its key hashes are: base engine `EB55EFA8CBBB4E9B640D3AC2BFAF054AE551EB0EB793A597733E660F329CCA57`, current exception source `E953231B0ED4A6832D6505860555876103E7A126C794AC0DB24354D892EE1994`, runtime source `DCDFC3D2CE51CEDC240749882CDE1ACC5506D5459E2FF2DB1B2FE6DF46D82356`, and makefile `A5037891EAE2515A04E377825947A3D1204386336FC97C946E9FF57C76FAB03F`.

## Admitted next step

S8 may invoke only `nmake /f ntdos64-t188-copied-state.mak ntdos64-t188-copied-state.exe` in the documented MSVC x86 environment. Any additional object, archive rebuild, link-model change, source repair or guest action stops the packet.
