# Proposal: OpenNT DPMI Host-Composition Recovery

## Purpose

Recover one complete OpenNT-derived DPMI host composition for a declared
contained guest profile. This is a future candidate T package, not an active
task, numeric T allocation, or authorization to implement any individual
`53:xx` BOP.

## Problem Statement

Bochs natively owns CPU protected-mode mechanics: CR0, segment caches,
GDTR/IDTR/LDTR/TR, descriptor checks, faults and architectural transitions.
OpenNT DPMI, however, combines those mechanics with process-LDT setup,
`VdmTib`, flat address translation, DOSX stack frames, protected-mode IRET
hooks, INT 21h crossings, DPMI application state and memory/session lifetime.

The old x86 process-LDT routes (`NtSetLdtEntries` and
`NtSetInformationProcess(ProcessLdtInformation)`) are not a supported x64
user-mode composition contract.  They cannot be directly reused merely by
declaring their native signatures.  Their OpenNT callers remain source and
failure evidence; the selected DPMI profile must instead rely on admitted
bx-core/bx-mantle protected-mode/descriptor mechanics, or retain the original
unavailable/deferred result.

The current C4 finding therefore rejects an adapter or mantle API that directly
sets CR0, edits LDT entries, or manufactures protected-mode callbacks. Such an
API would either expose Bochs internals across the architecture boundary or
gradually reproduce the DPMI server outside its original owner.

## Intended Composition

```text
guest DPMI/DOSX code
  -> BOP 53 ingress and bx-vdm provider selection
  -> OpenNT-derived DPMI host-service composition
       -> contained host-platform capabilities where source-proven
       -> versioned checked machine requests only where necessary
  -> guest executes native protected-mode transitions
  -> bx-mantle -> bx-core architectural CPU behavior
```

`bx-vdm` remains a routing, copied-ABI and capability-selection boundary. It
does not own DPMI selector allocation, LDT policy, exception-frame construction,
protected-mode stack management, DOS interrupt emulation or CPU mode changes.
Bochs and mantle remain selector-, BOP- and OpenNT-blind.

## Ordered Work Packages

1. Select one first DPMI guest profile and record its exact guest artifacts,
   launch path, expected BOP identities, and success criterion.
2. Build a source/ABI/failure map for the profile's complete DPMI session:
   initialization, descriptor/mode lifecycle, protected/real transitions,
   exception/IRET path, memory, INT 21h bridge, termination and cleanup.
3. Classify each reached OpenNT DPMI module as independently composable,
   composable with declared contained host capabilities, minimal
   source-derived rehost, or explicitly unavailable/deferred.
   This map must identify all historical LDT calls and prove that no raw LDT
   or process-descriptor operation crosses the bx-vdm boundary.
4. Define one session-scoped provider contract in `bx-vdm`; it may transport
   fixed-width copied state and checked guest-memory ranges only.
5. Admit only source-proven machine prerequisites. C1/C2 are native inputs;
   C3 UMB, C4 direct descriptor mutation and C5 INT15/IRQ are currently
   deferred, so a first profile must either avoid them or retain their original
   unavailable/deferred behavior.
6. Implement and regress the entire selected profile composition before any
   native trace. Trace then validates that composed package, not individual
   `53:xx` services.

## Non-goals

- No direct reuse of the unavailable NT4 V86/CCPU product composition.
- No generic “set CR0/LDT” API in mantle or adapter.
- No adapter-owned DPMI, DOS kernel, BIOS, PIC, keyboard or device model.
- No individual BOP success patch chosen by trace order.
- No WOW16 compatibility claim; WOW16 remains a subsequent package.

## Admission Gates

Admission requires all of the following:

1. The selected guest profile has a source-built or provenance-classified
   artifact and a bounded, non-GUI success observation.
2. The full profile session map identifies source paths, layouts, dispatch
   order and failure behavior for every reached DPMI service.
3. Each required host platform function has a contained replacement or an
   explicit profile exclusion; no ambient NT process/VDD/GUI dependency may be
   silently retained.
4. Each necessary machine capability has a named native owner, lifecycle,
   negative test and architecture-compliant ABI.
5. The planned result keeps `53:12` explicitly unavailable unless a separate
   VCD/VDD package is approved.

## Existing Evidence

- [T199 S10 DPMI source/ABI/failure matrix](t199-s10-dpmi-source-abi-failure-matrix-001.md)
- [T199 S11 shared XMS/DPMI capability plan](t199-s11-shared-xms-dpmi-machine-capability-plan-001.md)
- [T199 S27 C4 protected-mode map](t199-s27-c4-protected-mode-owner-abi-failure-map-001.md)
- [System architecture](../../design/ARCHITECTURE.md)

## Expected Outcome

Either one whole first-profile DPMI provider runs through the declared
composition with source-built regression and later one native integration
trace, or the selected profile is explicitly deferred with evidence. Neither
outcome permits a partial CPU-mode manager to become a substitute DPMI server.
