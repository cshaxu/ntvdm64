# M0 T199 S14 Debugger Source, ABI, And Failure Matrix

## Question

What is the actual OpenNT Debugger BOP ABI, and what complete disposition can
the non-invasive CLI profile provide without importing the NT debugger host?

## Inputs And Procedure

- `src/opennt/base/mvdm/inc/dbgsvc.h` and `DBGSVC.INC` define modes `0..15`.
- `softpc.new/host/src/nt_bop.c` routes selector `56h` to `MS_bop_6`, which
  calls `DBGDispatch()` without reading or advancing an inline service byte.
- `dbg/dbg.c` reads the mode and arguments from SS:SP through CCPU/SAS and
  sends `STATUS_VDM_EVENT` through `RaiseException` when debugged.
- `dpmi/dxboot.asm` and its 486 counterpart are concrete callers: they push
  `DBG_SEGLOAD`, execute `BOP BOP_DEBUGGER`, then execute `add sp,16`.

## ABI Finding

The actual BOP instruction is **three bytes**, `C4 C4 56`. The 16 values in
`dbgsvc.h` are stack-mode values, not a fourth instruction byte in the current
reachable call sites. The retained `dbgsvc` macro contains a fourth-byte form,
but repository searches find no caller of that macro. Treating byte four as a
Debugger service would consume the next guest opcode; the existing ingress
model is therefore not a valid execution ABI for selector `56h`.

## Complete Mode Map

| Modes | Original behavior and host dependency | CLI profile disposition |
| --- | --- | --- |
| `00-04` | Segment/module load, move, free notifications; `DBG_MODLOAD` is an original no-op in `DBGDispatch`. | Deferred: VDM event/debugger host absent. |
| `05-09` | Step, break, GP, divide/overflow and instruction-fault paths; result paths use guest stack frames and may write AX. | Deferred: no debugger, no stack-frame emulation. |
| `10-13` | Task/DLL lifecycle notifications. `DBG_TASKSTART` and `DBG_DLLSTOP` are not normal `DBGDispatch` cases; they arise through notifier paths. | Deferred: debugger event plane absent. |
| `14-15` | Attach and ToolHelp state; attach is an original empty dispatcher case, ToolHelp records hook state. | Deferred: no debugger state or ToolHelp bridge. |

When `fDebugged` is false, the original notification helpers generally avoid
emitting a VDM event; several result-returning helpers produce false/AX zero.
That is not a safe general substitute in this adapter because the mode is in
guest stack memory and the retained dispatcher mixes CCPU/SAS, Win32
exceptions, VDM debug context and host pointers.

## Selected Package Disposition

The CLI profile supplies no debugger host capability. Its correct whole-package
behavior is **deferred with controlled stop**, not an unavailable API result
and not a fabricated no-debug success. The subsequent provider will recognise
only the three-byte selector form, preserve the next guest byte, and return a
typed stop without reading guest stack memory or changing guest registers.

## Follow-up

T199 S15 must replace the erroneous inline-service ingress form with this
selector-only model, attach one deferred package facade, and regress both a
three-byte form and a four-byte window whose fourth byte remains unconsumed.
No Debugger feature, event delivery, stack parser, trace, or Bochs change is
admitted.
