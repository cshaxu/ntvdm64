# M87 NTVDMx64 HAXM Adapter Boundary Audit

Status: completed source audit, 2026-08-08.

## Scope

The staged NTVDMx64 input is a fixed 13-file subset at upstream revision
`84a13d2e7bb1a55d11148971e5b9c8ec99f670bf`, as recorded by
`refs/ntvdmx64-derived/SOURCE-MANIFEST.json`. Its local CMake target is an
`EXCLUDE_FROM_ALL` object-reference gate. It has no OpenNT include path and is
not a standalone host product, a CCPU replacement, or a runtime dependency.

## Direct Source Findings

`v86/haxm/i386/monitor.c` starts a custom HAXM Windows service, opens
`\\.\HAX`, creates a HAX VM/VCPU through private IOCTLs, installs an exception
bitmap, and repeatedly transfers a HAX-owned complete VCPU register image via
`HAX_VCPU_SET_REGS`, `HAX_VCPU_IOCTL_RUN`, and `HAX_VCPU_GET_REGS`.

The run loop translates accelerator-specific `HAX_EXIT_*` outcomes into old
VDM actions such as `EventVdmIo`, `EventVdmMemAccess`, and `EventVdmBop`.
`haxmvm.h` defines the driver ABI, VMX exit enumeration, state layout and HAX
memory-protection/IOCTL flags. These are HAXM protocol details, not machine
contracts.

`int.c` stores pending interrupt flags in the historical `pNtVDMState`, calls
the legacy interrupt controller, and injects through a HAX IOCTL. `sas.c`
uses HAX page mappings together with `Sim32GetVDMPointer`, host mappings,
legacy LIM hooks, and CVIDC video synchronization. `thread.c` duplicates native
thread handles, reads TEB data with `NtQueryInformationThread`, and writes a
`VDM_TIB` pointer into the TEB.

## Disposition

| Material | Future status | Reason |
| --- | --- | --- |
| HAX service/device discovery, IOCTLs, VMX exits, HAX register layout, fast-MMIO and HAX RAM protection | prohibited | private accelerator ABI and hardware-virtualization policy |
| `VDM_TIB`, TEB mutation, old event callbacks, raw `Sim32GetVDMPointer`, raw host pointers/handles | prohibited | ambient historical host state violates copied-value and containment boundaries |
| `int.c` pending-IRQ ordering | historical behavior oracle only | injection implementation is HAXM-specific; ordering must be re-proved against core PIC/CPU semantics |
| `sas.c` A20/LIM/video mapping constraints | historical constraint oracle only | it describes why raw host mapping and MMIO shortcuts are unsuitable, not a portable mapping implementation |
| monitor lifecycle phases: create/configure, run, classify finite exit, save copied state, stop/fault | neutral design observation only | future mantle may use the phase distinction, but not these APIs or state structures |
| I/O, memory, BOP, error, IRQ as distinct observation classes | diagnostic taxonomy only | the event routes cannot be imported because their inputs/outputs are legacy mutable state |

## Architectural Result

NTVDMx64 supplies two useful negative requirements for the future core/mantle
boundary:

1. no accelerator-specific exit reason, driver state, or raw VCPU context may
   become the public mantle/core ABI; and
2. no adapter may recover compatibility by exposing direct guest-memory or
   process-global VDM state to historical code.

Its event taxonomy supports M84's redacted diagnostics, while M70 remains the
only route to executable adapter work. A configured decode observation cannot
be modeled as a generic HAX exception bitmap; a paused ordinary-RAM transaction
cannot be modeled as HAX page protection. Both must retain their independent
consumer and core-test gate.

No NTVDMx64 source, build target, or artifact was added to the normal runner.
