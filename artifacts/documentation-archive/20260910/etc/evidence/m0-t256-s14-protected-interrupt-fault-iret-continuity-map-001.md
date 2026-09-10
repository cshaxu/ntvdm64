# M0 T256 S14 — Protected interrupt, fault and IRET continuity map

## Decision

No generic interrupt/fault/IRET interception API is admitted below `bx-vdm`.

Bochs already owns architectural protected interrupt gates, exception frames,
stack switching, IRET validation and fault escalation.  OpenNT's DPMI source
does something different: it installs DOSX-owned handler records through
historical VDM hooks, writes custom frames using host pointers, and enters
special IRET BOP continuations to manage a locked DPMI stack.  That is a
single DPMI/DOSX owner-package lifecycle, not a selector-blind machine action.

## Source map

| Source | Historical behavior | Current owner/disposition |
| --- | --- | --- |
| `dpmi32/dpmi32.c:44..69` | `53:02`, `53:0A`, `53:14..18` are one dispatch family: protected interrupt handler, fault handler, 16/32-bit interrupt/fault IRET hooks and unhandled exception processing. | Must be recovered together, not as independently enabled BOP leaves. |
| `dpmi32/dpmiint.c:DpmiSetProtectedmodeInterrupt` | Reads a frame through `Sim32GetVDMPointer`, updates `DpmiInterruptHandlers`; on x86 additionally calls `NtVdmControl(VdmSetInt21Handler, ...)` for INT 21h. | Guest-frame ownership and handler policy belong to DPMI. `NtVdmControl` is an unavailable historical host product interface. |
| `dpmi32/dpmiint.c:DpmiSetFaultHandler` | Reads CS:EIP/SS:ESP/flags from the caller's protected stack into `DpmiFaultHandlers`. | Requires bounded protected stack reads plus table/selector identity, not an adapter pointer. |
| `dpmi32/dpmimscr.c:DpmiDpmiInUse/NoLongerInUse` | Enables/disables historical emulator IRET and interrupt hooks. The source explicitly conditions hook safety on a known protected BOP address. | A DPMI lifecycle state, not a generic Bochs enable flag. |
| `dpmi32/dpmiint.c:DpmiHwIntHandler` | Builds 16- or 32-bit custom frames on a locked PM stack, redirects CS:EIP to installed handler, and installs DOSX IRET continuation addresses. | Source-shaped DPMI stack/re-entry contract; cannot be represented by generic `resume` alone. |
| `dpmi32/dpmiint.c:DpmiIntHandlerIret16/32` and fault counterparts | Consumes the special IRET BOP, balances locked-stack nesting, restores saved context or manufactures a final IRET frame. | Must remain coupled to the corresponding guest DOSX stubs and protected frame layout. |
| `dpmi32/dpmiint.c:DpmiUnhandledExceptionHandler` | Reflects a fault to DPMI interrupt handlers after stack/frame reconstruction; it uses `Ldt[]` and pointer-returning translation. | Depends on S12/S13 descriptor/table decision and bounded protected access. |
| `bx-core/cpu/exception.cc` | Native protected interrupt delivery reads IDT gates, validates CS/SS/TSS, pushes architectural frames, dispatches faults and applies double/triple-fault rules. | Retained as exclusive CPU owner; no DPMI names or hook policy enter core. |
| `bx-core/cpu/iret*.cc` | Native IRET validates and restores architectural guest state. | Retained; a later DPMI package may compose its guest stub/continuation around it, not replace it. |

## Rejected seams

- A bx-mantle `install_protected_interrupt` or `set_fault_handler` request:
  it would carry selector, vector, handler, frame-width and DPMI stack policy.
- A raw core callback on every protected exception/IRET:
  it exposes CPU fault control flow and would turn Bochs into an OpenNT DPMI
  dispatcher.
- Treating `BX-ABI-094` context resume as an IRET implementation: it applies a
  previously validated typed context but does not own DPMI nesting, frame
  formation, IDT gates or handler lifecycle.
- Reusing `NtVdmControl`, `VdmTib`, or the historical emulator-hook functions:
  they are NT4 VDM-product interfaces, not supported CLI dependencies.

## Dependency order

1. Retain completed context (`BX-ABI-094`), copied protected access
   (`BX-MANTLE-095`) and ordinary-RAM/XMEM identity (`BX-MANTLE-096/097` plus
   bx-vdm records).
2. Recover the original DPMI/DOSX table-publication and selector package from
   S13, so the guest's own protected code/data/stack identity is valid.
3. Recover DOSX protected frame and IRET stubs together with `53:02/0A/14..18`
   under one source-shaped provider lifecycle; use native Bochs exception/IRET
   where the guest executes architectural transitions.
4. Add only a separately proven adapter bridge if a bounded host-to-guest
   handoff remains after the guest lifecycle is executable.  That bridge must
   be selector-blind and must not duplicate native exception mechanics.

## S14 closure

S14 closes the final generic-substrate classification.  The current DPMI
dependency cannot be further reduced into a Bochs/mantle primitive.  The
proper successor is one whole OpenNT DPMI/DOSX owner package using the already
closed generic substrates, not a sequence of 53:xx trace patches.
