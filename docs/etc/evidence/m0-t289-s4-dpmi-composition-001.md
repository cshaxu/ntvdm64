# M0 T289 S4 — DPMI initialization, descriptor and xmem composition

## Result

S4 is closed. The selected original `dpmi32` initialization, extended-memory,
descriptor-normalization and memory-information bodies are locally composed on
x86 and x64. This is source-body and local-contract closure only: no `53:xx`
route, DOSX host translation unit, protected-mode executor, host LDT/IDT, or
raw guest pointer is enabled.

## Retained source shape and bindings

- `dpmi32.c` retains the original DOSX/app/table decoding order. Its persisted
  historical pointer globals are represented only as private numeric,
  session-owned state (`MVDM-HOST-DIV-015`).
- `xmem.c` retains its source allocation/list/failure flow. The historical
  `XMEM_BLOCK *` split through `SI:DI` becomes the existing session
  `host_resource` surrogate32 identity (`MVDM-HOST-DIV-012`); no second mapper
  exists and no host pointer crosses the guest ABI.
- `dpmiselr.c` keeps its descriptor normalization loop. A private checked
  protected-span buffer replaces only `Sim32GetVDMPointer` (`DIV-013`). The
  historical host-LDT install is deliberately the original `FALSE` failure
  outcome until a named Bochs-native descriptor-publication package exists.
- `dpmimemr.c` preserves its original output layout and memory-query sequence,
  with a checked read/copy/write protected buffer replacing its pointer alias
  (`DIV-014`).
- The same-shaped `Vdm*VirtualMemory` and register facades are implemented in
  `adapter-mvdm-host-out/softpc`; all actual Bochs mechanics remain behind
  `adapter-bochs`. The mirror has no direct Bochs dependency.

## Verification

`tools/build/New-T289S4DpmiCompositionNinja.ps1` creates a formal `/MT` Ninja
graph that rejects `src.old`, a DOSX host translation unit, a host-LDT
implementation and prebuilt product archives. It consumes the completed S5
Bochs-core static prerequisite, including the selector-blind real-mode frame
primitive required by the current `adapter-bochs` facade. This is build
closure only and does not select an additional DPMI path.

- `build/M0-T289/S4/x64/bin/dpmi-init-descriptor-xmem-fixture.exe`: exit `0`.
- `build/M0-T289/S4/x86/bin/dpmi-init-descriptor-xmem-fixture.exe`: exit `0`.

The fixture uses a real minimal Bochs machine and actual imported original
bodies. It proves DOSX/app/table initialization state, all five XMEM lifecycle
entries (allocate, free, reallocate, per-owner free and all-owner free), the
retired-opaque-identity carry failure, the original descriptor-install failure
direction, and memory-information writeback. Its `DpmiDispatch` declaration
stub is fixture-local static-link scaffolding; the dispatcher is not called and
no BOP ingress is selected.

## Transfer

S5 owns original stack, interrupt and fault groups. S6 owns eligible DOS
translation/VDD dispositions and the selector-family matrix. A successful
descriptor-publication path requires a separately admitted Bochs-native owner;
S4 neither designs nor emulates one.
