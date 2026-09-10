# Bochs 2.6 Backend Adoption Decision

## Decision

Bochs 2.6 is the selected third-party guest-machine backend. It replaces the
unrecoverable OpenNT NT4 x86 V86/CCPU product composition as the execution
substrate. OpenNT remains the normative guest/service source: DOS, NTDOS,
WOW16 and the historical BOP/DEM/COMMAND contracts are not replaced by Bochs.

## Hard Boundary

| Bochs owns | OpenNT owns | Adapter may do | Adapter must not do |
| --- | --- | --- | --- |
| x86 instructions, CPU state, RAM/ROM, interrupts, firmware and emulated PC devices | DOS/WOW semantics, original BOP/DEM/COMMAND service ownership and OpenNT data flow | translate a typed stop/event, validate a fixed-width request/response, and mediate checked guest-memory ranges | dispatch DOS/BOP/DEM services, encode OpenNT service policy, create BIOS/IVT/BDA state, or expose Bochs objects/pointers to OpenNT |

The runtime flow is `runner -> Bochs -> typed bridge -> original OpenNT owner -> typed bridge result -> Bochs`. No other direct Bochs/OpenNT call is allowed.

## Import Gate

The authoritative Bochs 2.6 input is the local repository
`O:\repos.external\bochs-2.6-compat\bochs-2.6`. Its `CHANGES` begins with
`Changes in 2.6 (September 2, 2012)` and its README identifies Bochs as LGPL
software. The current `refs/bochs` checkout is not that baseline: it contains
Bochs 3.0 material and must not enter the runtime. T95 must import the local
2.6 input into `refs/bochs` with a manifest and patch record.

Before any CMake/runtime integration, record: exact local source identity,
upstream revision when available, archive hash when applicable, local patch
series, enabled CPU/device feature set, reproducible build recipe, and focused
boundary tests. Keep upstream notices intact. This is internal research;
distribution and license review are deferred until a release is considered.

## First Integration Slice

1. Build a minimal Bochs configuration outside the default product target.
2. Define an adapter-owned stop record and a bounded guest-memory interface.
3. Demonstrate one non-DOS CPU/memory stop and its negative cases.
4. Only then invoke one original OpenNT service owner through a typed bridge.
5. Accept the result only when the same test proves that Bochs did not execute
   DOS service policy and OpenNT did not receive a Bochs pointer/object.

## Scope Compression

The Bochs feature manifest starts empty except for the mechanics demonstrably
needed by the first admitted profile. GUI, the internal debugger, plugin
framework, USB, networking, SMP, APIC, PCI, sound, optional storage
controllers, and all other convenience features are off by default. Each later
feature needs all of: a reached OpenNT caller, a named Bochs owner, a boundary
impact assessment, an explicit admission record, and a focused negative test.
Upstream availability, compatibility ambition, and speculative future use are
not admission evidence.

The same constraint protects OpenNT fidelity. OpenNT remains the sole owner of
its DOS/WOW/BOP/DEM/COMMAND behavior, calling conventions, data layouts,
dispatch order, and failure outcomes. Bochs and `bx-ntvdm-adapter` may transport a
documented contract only; they must not reconstruct service behavior. No Bochs
code belongs in an OpenNT overlay, and no OpenNT service implementation belongs
in Bochs.

## Non-goals

- Rebuilding the historical NT4 V86/CCPU composition;
- importing Bochs code into OpenNT overlays;
- a wholesale Bochs GUI/plugin/device build;
- WOW16, DPMI, VDD, networking, or retail compatibility;
- making `ntvdm64` a dependency. It receives only a later boundary report.
