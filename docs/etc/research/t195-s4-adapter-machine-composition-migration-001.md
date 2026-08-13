# M0 T195 S4: Adapter Machine-Composition Migration

## Question

Can the historical `02h`/`06h` handler selection be moved out of Bochs while
retaining the original object islands and Bochs-owned checked mechanics?

## Baseline

The previous `exception.cc` seam built an event, checked the instruction bytes
through `bx_ntvdm_machine_composition_v1_probe`, and interpreted a
handler-specific outcome.  It then resumed at `prev_rip + 3`.  The external
component owned the original handler wrappers, but Bochs still selected the
path and decided the BOP-length resume.

## Repair

The machine-composition implementation now lives at
`src/bx-ntvdm-adapter/bx_ntvdm_machine_composition_v2.*`.  The adapter:

- validates copied exception/state/window facts;
- recognises the historical `C4 C4 02` and `C4 C4 06` forms;
- selects the unchanged `unexpected_int` or `illegal_op_int` object;
- supplies the synchronous original-import wrappers from one transient
  mechanics capability; and
- returns only `bx_ntvdm_cpu_result_v2` pass-through, resume, or controlled
  stop.  Its resume RIP is computed there, not in Bochs.

Bochs now constructs only `bx_ntvdm_machine_mechanics_v1`: checked byte port
read/write, checked guest-RAM byte read/write, copied stack facts and a report
callback.  It passes that opaque, non-retainable capability to the adapter and
mechanically applies the generic CPU result.  No Bochs source names the two
selectors, either historical handler, or a fixed BOP instruction length.

The superseded `src/bx-ntvdm-machine-composition` source files are removed.
The original object inputs remain unchanged and are copied into the adapter
portion of the derivative closure, not a machine-composition source domain.

## Verification

1. `Test-BochsMachineCompositionBoundary.ps1` passes.  It rejects selector,
   historical-handler, marker-byte and fixed-resume terms in Bochs; it also
   confirms those selection terms are present in adapter source.
2. The pre-existing generic #UD and adapter-observer boundary checks pass.
3. A standalone MSVC x86 unit build ran
   `bx_ntvdm_machine_composition_v2.c` with source-shaped original-handler
   stand-ins.  It proves `02h` produces generic resume at `fault_rip + 3`,
   `06h` retains its report/continue mechanics, and `50h` passes through.
4. Fresh closure
   `artifacts/build/t195-s4-closure-r1` was generated with
   `-MachineComposition`, then linked using MSVC x86 `nmake`.  The generated
   link command contains exactly the adapter component plus
   `adapter/unexp_nt.c.obj` and `adapter/illegalp.c.obj`; no machine source
   directory, archive rebuild, or device expansion is admitted.
5. Its final map resolves `_unexpected_int` and `_illegal_op_int` from the
   retained original objects and
   `_bx_ntvdm_machine_composition_v2_dispatch` plus the compatibility imports
   from `adapter/bx_ntvdm_machine_composition_v2.obj`.

The native link retains the pre-existing CRT warning pair (`LNK4098` and the
`illegalp.c.obj` `sprintf` import warning).  It has no unresolved import or
new warning attributed to the migration.

## Limits

This does not claim a runtime visit to `02h` or `06h`, nor does it reinterpret
either original handler.  PIC, RAM, CPU, exception delivery and device
ownership remain Bochs-native.  This repair only changes which component
recognises historical handler identity and translates its completion into the
generic engine result.

## Follow-up

S5 will remove/reconcile superseded register entries and generated build
surface references, leaving only approved selector-blind mechanics and
default-off diagnostics before the final S6 closure audit.
