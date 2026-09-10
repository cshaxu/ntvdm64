# Proposal: WOW16 Guest and Host Composition

## Purpose

This is a lowest-priority candidate package proposal. It does not admit work
and does not claim that WOW16 currently builds or runs.

## Proposed ownership

| Layer | Proposed responsibility | Reuse disposition |
| --- | --- | --- |
| bx-core | x86 protected-mode execution, descriptors, exceptions, memory | Adopted Bochs mechanics; no WOW meaning. |
| bx-mantle | Minimal machine lifecycle and admitted mechanical facilities | Project-owned Bochs assembly; no WOW meaning. |
| OpenNT DOS guest | NTDOS, NTIO, COMMAND and DOS foundation | Source-built guest artifacts where closure permits. |
| OpenNT WOW16 guest | Win16 NE runtime, Kernel/User/GDI modules, loader and guest transition code | Candidate source-built guest artifacts; inventory and closure required first. |
| OpenNT DPMI guest half | DOSX, INT 31h-facing and protected-mode guest code | Candidate source-built guest artifacts; separate from host-side dependencies. |
| bx-vdm WOW composition | Fixed-width WOW ingress, routing, result copyback and provider selection | New project composition, derived from OpenNT contracts. |
| Host WOW capability | Thunks for host facilities selected by policy | Original reusable code only when independently composable; otherwise bounded source-derived capability or explicit deferment. |

## Boundary

Win16 guest modules should execute as guest NE modules after a real
protected-mode/DPMI/selector setup. They must not be rewritten as bx-vdm
services. Conversely, historical WOWEXEC/WOW32 host composition cannot be
assumed to link into the modern CLI process because it binds old NT GUI,
VDD, process, message and memory mechanisms.

In particular, the WOW32 initialization/dispatch hooks are not public Win32
extension points.  They require the historical WOWEXEC/WOW32 process and VDM
composition, including its thunk, callback and address-space contracts.  They
must be source-mapped and either composed through admitted CLI capabilities
or explicitly deferred; the project will not load a historical WOW32 DLL as a
shortcut or recreate its host installation requirements.

The intended path is:

```text
Win16 NE application
  -> OpenNT WOW16 guest runtime and NE loader
  -> DPMI guest transition code
  -> bx-core / bx-mantle mechanical execution
  -> bx-vdm fixed-width WOW boundary
  -> selected contained host capability
```

## Admission sequence

1. Inventory every required WOW16 and DPMI guest artifact as source-built,
   static, prebuilt, blocked, or unavailable.
2. Map the complete original WOW host composition, including WOWEXEC/WOW32
   dispatcher, hooks, thunk, GUI, file, input and lifecycle dependencies,
   with a separate ABI/failure disposition for every unavailable WOW32 hook.
3. Classify each host dependency as independently reusable, capability-adapted,
   source-derived, explicit unavailable, or deferred.
4. Build a minimal non-GUI NE fixture closure: loader, selector/LDT, protected
   mode entry and bounded return/stop.
5. Admit GUI, USER/GDI, input and printing only as separately evidenced
   capabilities; no current CLI profile may claim them by implication.

The package must preserve OpenNT's guest and host ownership split. It must not
implement Win16 APIs in Bochs or turn bx-vdm into a general Win16 API emulator.
