# M0 T255 S1 — Protected-Mode Substrate Call Map

## Question

What smallest selector-blind mechanical additions, if any, can make it
possible to compose the retained OpenNT DPMI source without putting DPMI
policy into Bochs layers?

## Inputs

- Existing generic exception seam: `src/bx-core/cpu/exception.cc:842..956`,
  `src/bx-mantle/bx_ntvdm_generic_ud_bridge.h`, and
  `src/bx-vdm/bx_ntvdm_cpu_{state,delta,result_v2}_abi.*`.
- Existing CPU operations: `src/bx-core/cpu/cpu.h` public `SetCR0`,
  `load_seg_reg`, `read_virtual_checks`, `get_laddr` and the native
  protected-mode interrupt/IRET implementations.
- Existing ordinary-RAM transport: `src/bx-mantle/bx_ntvdm_mechanical_action_v1.*`.
- Original DPMI package and the preceding T254 admission map.

## Observations

1. Bochs has the required *internal* architectural operations.  `SetCR0`
   validates and changes CPU mode, `load_seg_reg` uses the native descriptor
   loader, and `exception.cc` already uses that loader for an accepted
   generic segment delta.  No adapter-side LDT, selector-cache or IRET
   implementation is justified.
2. The current generic output is deliberately insufficient for DPMI:
   it has low-16-bit GPR deltas, segment-selector deltas, CF/ZF and a resume
   RIP.  It lacks full ESP/EFLAGS restoration and a target execution-mode
   request.  Thus original `53:01` and `53:14..17` cannot be expressed even
   though their individual native CPU primitives exist.
3. `read_virtual_checks` and virtual accesses run inside CPU fault machinery;
   the existing public ordinary-RAM API deliberately cannot substitute for
   a protected selector:offset mapping.  Directly exposing Bochs pointers or
   using a raw `get_laddr` result would bypass segment/page checks and violate
   the fixed-width boundary.
4. Original `53:07..09` allocation returns host virtual addresses and host
   `XMEM_BLOCK *` values to the guest.  That historical representation cannot
   cross an x64 boundary.  The future DPMI shim must preserve allocation
   ordering/failure rules while replacing that representation with a
   guest-linear mapping identity; it is not a mantle-owned allocation policy.

## Minimum operation ledger

| Need | Existing native owner | Current boundary | Candidate addition | Owner disposition |
| --- | --- | --- | --- | --- |
| Load validated segment selectors | `BX_CPU_C::load_seg_reg` | Already accepted through generic v1 segment mask | None | Keep existing core generic behavior. |
| Switch PE state and restore 32-bit CPU context | `BX_CPU_C::SetCR0`, native register/flag setters | Missing target-mode, 32-bit GPR and full FLAGS fields | Versioned generic context-resume result; core applies only typed fields through existing native methods | Requires one registered extension of `exception.cc`; no DPMI names. |
| Read/write a bounded PM selector:offset range | Native segment validation, address translation and memory access | Ordinary physical-RAM action is insufficient | New mantle mechanical request must preflight selector/offset/length and perform an all-or-nothing copied access without leaking a pointer | Mantle design/proof required; no core change is selected by S1. |
| DPMI memory allocation identity | Bochs configured guest RAM/mapping | No current contract | A later bx-vdm DPMI compatibility shim owns the original allocation list and uses an explicitly admitted generic guest-linear mapping resource | Deferred to the DPMI owner package after substrate admission. |

## Rejected alternatives

- Calling `SetCR0` or `load_seg_reg` directly from bx-vdm: crosses a Bochs
  object and skips the declared boundary.
- Copying OpenNT's `Sim32GetVDMPointer` implementation: exposes host pointers
  and assumes NT4 process LDT state.
- Letting bx-mantle implement LDT/DPMI allocation policy: imports DOS/VDM
  semantics into the machine composition layer.
- Adding a `53:xx` special case to the core exception path: violates the
  selector-blind intrusion rule.

## S1 decision

S1 admits a single prospective, generic context-resume extension for S2.  It
must be recorded before modification as a narrow enhancement of the already
registered generic #UD seam.  The PM selector:offset action is *not* admitted
for code yet: S2 must first prove whether native checks can fail closed
without a second exception/longjmp escape.  Guest-linear allocation stays out
of the substrate implementation until the original DPMI source package is
admitted.

## Confidence and follow-up

High confidence for the context result because the current core code already
applies generic segment deltas using native APIs.  Medium confidence for the
protected-memory action pending its fault-containment audit.  The next active
step is therefore the narrowly registered generic context-result work, with
real-mode regression as a mandatory negative check.
