# T103 S1 native POST / deferred-startup composition closure 001

## Result

The required lifecycle already has a closed source and binary shape.  The
missing link is not a CPU, VGA, IVT, BOP, or adapter capability.  It is that
the later current-adapter derivative intentionally inherits the r5 direct-entry
`main.o` and `cpu\\exception.o`; it therefore cannot perform the previously
validated native POST handoff.

The correct current-composition mode has exactly these two rebuilt Bochs
objects, sourced from the existing registered `BX-ABI-052` seams:

| Object | Required macro shape | Responsibility |
| --- | --- | --- |
| `main.o` | `BX_NTVDM_ENABLE_EXECUTION_PLAN=0`, `BX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN=1` | Arm the adapter session after `bx_init_hardware()` without moving CS:RIP; then enter the normal CPU loop. |
| `cpu\\exception.o` | exception intercept/listener settings retained; `BX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN=1` | At the existing generic `#UD` seam, ask the adapter once for an opaque plan; ordinary-RAM preflight/copy/restore and the existing real-mode entry setter then resume the CPU loop. |

All other Bochs objects, archives, selected reset-floor devices, ROM BIOS and
VGA ROM remain inherited inputs.  `cpu/libcpu.a` remains a link input only;
the rebuilt `exception.o` is an explicit earlier link object.  No archive is
extracted or repacked.

## Profile and composition contract

`byob_profile.c` deliberately requires a `machine_startup_handoff` to be
paired with enabled `machine_startup_plan` and `machine_startup_entry=ntio-v0`.
This is correct: the plan supplies the opaque payload, while the handoff
descriptor supplies the narrow condition under which it can be taken.  The
v1 parser fixes that descriptor to `#UD`, bytes `0f 0b`, `CA80:0003`.

The runner's deferred input creates the 512-byte optional ROM envelope at
`CA800`, with `UD2` at offset three and SHA-256
`E4CA4AE5190A15ED741D1DFED5A58533A2B6926A40302D839FB714F9FC673FFB`.
It is an evidence-time composition asset, not a Bochs or OpenNT source edit.
It is loaded through the native `optromimage1` configuration and contains no
disk, host-path, filesystem, BOP, or device contract.

The direct and deferred paths must be mutually exclusive **binary consumers**,
not mutually exclusive profile fields.  The existing
`New-T95S7RuntimeTraceBuildRoot.ps1 -DeferredStartupPlan` already expresses
the required `0/1` macro pair, and retained r49/r41 evidence proves that shape
can execute native POST and reach the real BOP path.  The current
`New-T98S1CurrentAdapterEngineDerivative.ps1` has no deferred switch and
rebuilds no Bochs object; it is therefore correctly unusable for this mode.

## Required successor

T104 S1 may make one narrow generator/test change only:

1. add an explicit deferred mode to the current-adapter derivative;
2. in that mode copy the existing `main.cc` and `cpu/exception.cc`, invalidate
   their inherited objects, and emit only their two required compile rules;
3. preserve the current adapter/CLI source list and all inherited engine
   archives; and
4. add lexical closure tests proving the `0/1` macro pair, exact two-object
   replacement, no new archive/device prerequisite, and no direct mode
   regression.

It may then build one fresh MSVC/x86 derivative and run one bounded deferred
POST trace.  It may not change Bochs source, adapter ABI, profile parser,
option-ROM bytes, device configuration, or BOP/provider semantics.  A trace
that does not reach the handoff is evidence, not permission to add a fallback.

