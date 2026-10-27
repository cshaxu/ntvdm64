# Original VDM debugger provider versus external debugger extensions

## Selected worker owner

`src/mvdm/dbg/dbg.c` is the original host debugger provider, not an external
extension. Current `worker/build.ninja` compiles `obj/mvdm-debugger/dbg.obj`
from it and places that object in `debugger-bindings.lib`, which is an input to
`ntvdm.exe`. The current `ntvdm.exe.map` attributes `DBGInit`, `DBGDispatch`,
`SendVDMEvent` and module/segment notification functions to that exact object.
The frozen S10 table and T473 S1 unselected-host record instead call this path
fixture-only. That is a specific product-link misclassification; its frozen
fields are not changed by this shard. The path belongs to shard 0.

T420 S43's real-worker evidence reports original `STATUS_VDM_EVENT` exception
transport, host debugger continuation and a real original DOS module/segment
notification. This is historical evidence, not a new run or proof that all
Win16 debug entrypoints, handles or extensions work. See
`docs/etc/evidence/m0-t420-s43-debugger-capability-ledger.md` and
`src/ntvdm-exe/debugger/README.md`.

## Original independent extension owners

- `bde/sources` declares separately built `BDE` DYNLINK; `bde.def` exports
  debugger commands including k, r, heap, symbol and segment operations.
  Shard 1 has `bdeheap.c` and `bdesym.c`, which respectively inspect Win16
  heap/segment state and symbol/module names; they are not worker event
  transport implementations.
- `ieuvddex/sources` defines a separate `ntvdm.dll` debugger extension;
  `ntvdm.def` exports dr, sel, eventinfo, vdmtib and other debugger commands.
  `procdep.c` takes `PWINDBG_EXTENSION_APIS` and delegates to architecture
  routines; `pmode.c` provides selector inspection and the shard's
  `mips/helpp.c` is explicitly a MIPS source. These files are debugger
  extensions, not an Internet Explorer functionality claim.
- `vdmexts/sources` defines the distinct VDMEXTS DYNLINK and `vdmexts.def`
  exports DOS/WOW inspection, heap, profiling and debug commands. Shard 1's
  debug, misc, reflect, stack, system and wowprof files are part of that
  independent profile. `misc.c` i386 code reads a target TEB and assumes its
  `Vdm` field is an NT4 `PVDM_TIB`; equivalence to a modern worker's actual
  TEB or CCPU state is not established and must never be presumed by casting.
- `wow32/debug.c` identifies itself as debugger extensions and is not named
  in the selected original `wow32/sources`. Its independent entrypoints and
  relevant original profile still need a source-to-consumer review.

The current worker and WOW32 graphs have no direct entries for the three
extension packages and `assets/release` did not contain bde.dll, vdmexts.dll,
or ntvdm.dll at this review. This is a limited path observation, not a claim
that no external debugger installation can provide them.

## Finite remaining capability questions

Map each requested debugger command to its original target process inputs,
required `VDM_TIB`/CPU/guest structure, public debugger API availability,
original user-facing result, and teardown/failure behavior. Decide whether a
bounded compatible external provider can preserve that command without private
modern-host interpretation; where impossible, identify the exact affected
capability and stopping boundary. A separate DLL's exclusion from worker link
is necessary for ownership but does not establish that its developer-visible
feature is outside the owner's whole-MVDM recovery objective. Do not invent a
new debugger subsystem or duplicate selected `dbg.c`. No extension was built,
loaded, or functionally verified by this read-only review; no guest was changed.
