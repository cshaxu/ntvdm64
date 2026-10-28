# S10 original VDD ownership and DMA boundary audit

Base `6f7ca8722`; two pending physical file roles become **partial**. No
production source, selected image, guest or installed package changes here.
The frozen 4,684-file inventory and all unrelated role rows are unchanged.

## Source, composition and responsibility

The [observer](vdd-image-observer.py) joins `nt_vdd.c` and `nt_vddp.h` to the
DOS16 catalog and both frozen WOW32 editions. Both retain **100% of primary
OpenNT bytes**. The [observations](vdd-image-observations.json) record original
paths/hashes, external catalog IDs, the original host recipe, actual compile
and archive edges, valid Ninja dependencies and exact cached-object identity.
The declaration header is a selected dependency, not an excluded header.
The public SDK `nt_vdd.h` is recorded as an external ABI boundary, outside
the 4,684 physical MVDM rows; it is not silently counted as a third row.

Nine original mapped bodies are compared byte-for-byte with the **currently
released NTVDM image**, masking only COFF relocation words: FreeVddAdapter,
VDDInstallIOHook, VDDDeInstallIOHook, VDDReserveIrqLine, VDDReleaseIrqLine,
VDDRequestDMA, VDDQueryDMA, VDDSetDMA and ms_bop. This proves retained code
identity, not correct complete execution of all branches.
Fourteen call relocations are also resolved to the selected original IOS,
DMA and ICA owners, covering all five relevant service targets. Relocation
masking alone would not establish that those calls reach the right provider.

Original VDD code owns adapter assignments, interrupt reservations, port
callback routing and DMA algorithms. Original IOS and DMA owners hold the
actual emulated port/controller state. Modern Windows supplies real callback
addresses, module identities, critical sections and LastError facilities;
these are not interpreted as NT4-private USER objects. The fixture owns only
controlled host boundaries. The private header supplies data layout and LPT
constants; consumer lifetime obligations remain at nt_vdd/nt_msscs/nt_umb.

An executable-code recovery percentage is **not defensibly measurable** from
this evidence. Source retention is 100%; nine mapped bodies plus bounded
native tests do not measure real VDD DLL load/unload, guest page faults or
all failure/retirement branches. Both files remain partial, never full.

## Actual-object controls and strict counterexamples

The [fixture](../../tests/mvdm-host/vdd_object_contract_fixture.c) links the
actual nt_vdd, IOS and CCPU in/out objects. It executes no guest, unloads no
DLL and accesses no real hardware. It supplies a guarded emulated DMA
controller block and uses actual Win32 LastError/critical-section services.
The [retained runs](vdd-object-runs/manifest.json) bind source/header/object
hashes and exact build commands to these outcomes:

- **305 normal assertions pass**, including 82 callbacks, 27 ICA lock calls
  and 16 valid-channel DMA accesses. They cover repeated BOP allocation and
  release, invalid BOP dispatch, adapter exhaustion/duplicate ownership,
  byte-port callbacks, invalid callback rejection, IRQ reservation ownership
  and exhaustion, valid channels 0–7 and rejected channels above 8.
- A deliberately wrong handler ABI fails compilation at **C2118**. Correct
  x86 handler/private-memory-hook layouts compile and execute.
- Strict uninstall fails **14 of 57 assertions**. All seven adapter slots
  still dispatch the old read/write callbacks after uninstall; ownership has
  already been released. These are contract failures, not accepted output.
- Strict DMA boundary fails **7 of 8 assertions**. Request, Query and Set
  reach the controller array for channel 8; Query/Set return success, and Set
  modifies the guard beyond the two-controller allocation.

The DMA probe resets its write payload after Query. Otherwise Query could
copy guard values into that payload and Set could write them back unchanged,
concealing the out-of-bounds write. Normal controls explicitly reset fixture
tables rather than claiming public uninstall succeeded.

## Finite S13 repair contracts

**VDD adapter uninstall:** GetVddAdapter produces an IOS adapter number
`SPARE_ADAPTER1 + index` (36–42), but FreeVddAdapter returns the raw index
0–6. Uninstall treats zero as absent and uses the other indexes as adapter
numbers. It therefore releases the owner without disconnecting its ports.
S13 must retain the original algorithm while returning the correct adapter
identity, preserve zero exclusively for not-found, and verify all seven
slots, unrelated owners, repeat uninstall/reinstall, rollback and retirement.
No special case for slot zero or delayed success is justified.

**DMA boundary:** all three APIs reject `channel > 8`, although the physical
array has only channels 0–7. S13 must reject channel 8 before obtaining or
indexing the DMA adapter, keep all valid-channel behavior, preserve the
documented invalid-address result and LastError, and rerun the strict and
normal object contracts followed by relevant product regressions.

These are proved host-emulator source defects, not guest defects or proof
that current games fail through these paths. The separate seven-slot versus
eight declared spare-adapter constants require a capacity-contract review;
it is not automatically an eighth supported VDD. Full-range port iteration,
registration failure rollback, nested callbacks, real DLL load/unload,
concurrent mutation, memory hooks, reset and worker exit remain explicit
uncovered boundaries. No runtime fix is claimed by this audit checkpoint.

## Reproduction and ledger verification

`verify-softpc-object-contract.ps1 -Contract vdd` builds/runs normal controls
against the selected objects. Its executable accepts `--deinstall-contract`
and `--dma-boundary`; both currently exit 1 with the strict failures above.
The [merge](vdd-native-role-merge.py) uses the existing shared ledger updater;
the [checkpoint verifier](guest-source-role-checkpoint.py) rechecks exact
source, image, artifact, control and negative evidence. No failed assertion
is deleted or turned into a production success stub.

Totals are **1,538 pending, 450 partial, 53 full and 2,643 excluded**. S10 and
T473 remain open. Existing current-package Full evidence is unchanged and
is not a substitute for these deliberately failing VDD contracts.
