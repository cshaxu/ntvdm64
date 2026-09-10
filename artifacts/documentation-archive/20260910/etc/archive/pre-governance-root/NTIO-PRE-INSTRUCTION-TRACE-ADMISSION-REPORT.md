# NTIO Pre-Instruction Trace Admission Report

Status: M14 read-only conclusion, 2026-08-08.

## Question

Can the fixed historical NT4 startup reach a point immediately before the
first NTIO instruction and emit the M13 register/memory snapshot without
turning any local instrumentation into a DOS runtime?

## Result

Not with the currently recovered historical build. The desired historical
observation cut is correctly defined, but there is no lawful executable path
to it yet.

The original startup-prefix fixture reaches:

```text
config -> sas_term -> sas_init -> ccpusas4 -> host_sas_init
       -> rom_init/copyROM -> InitNtCpuInfo -> bounded stop
```

The first missing dependency is `InitNtCpuInfo`'s direct CCPU-compatible
generated monitor ABI. Recovery audits establish that its required generated
global layout and `GDP_PTR`/`ConstraintR*` surface is absent from all locally
available historical sources. Continuing past that point by adding a local
adapter would reconstruct the CCPU execution path, which is expressly not
admitted by the current historical-oracle policy.

## Candidate Mechanisms Audited

| Mechanism | Can provide historic pre-NTIO state? | Admission result |
| --- | --- | --- |
| existing OpenNT config/SAS prefix fixture | no, stops before `InitNtCpuInfo` | retained bounded source trace only |
| recovered complete original-chain link | no, fails at the same monitor ABI and later historical host dependencies | no shim admitted |
| frozen self-authored startup/CCPU/BOP/DEM sources | could synthesize progress, but would define behavior rather than observe it | prohibited outside source-only instrumentation |
| current `ntvdm64` core archive probe | can expose a stopped state for an owned entry plan, but has no historical CCPU/BIOS construction path | valid generic-core evidence, not historical trace |
| existing M11 constructed firmware snapshot | exposes only self-owned IVT/BDA values after its own construction | valid profile evidence, not historical CCPU state |

The external core has `core_machine_capture_observation` at a stopped or
paused boundary and has atomic `core_machine_apply_entry_plan`, so it can host
a future controlled trace **after** a profile state is independently defined.
It cannot discover the missing historical state: the entry plan itself needs
that state as input.

## Required Contract, Not A Shim

The minimum future trace operation is still useful to name. A selected
mantle profile must be able to:

1. construct its own devices/firmware on an isolated core instance;
2. stop before its first guest instruction;
3. return a copied CPU observation plus a fixed redacted memory-field list;
4. reject any call after guest execution, provider refresh, or product session
   activation.

This is a generic constructed-profile observation capability. It is not an
`ntdos64` compatibility shim, cannot invoke a BOP/SVC handler, and cannot
substitute the historical CCPU state without a separate source/trace basis.

## Consequence For M13

M13's non-CS:IP candidates remain correctly classified as trace-required.
M12 must not apply a real core entry plan using them yet. This is an evidence
boundary, not a reason to expand self-authored DOS runtime code.

## Next Highest-ROI Work

Static first-instruction analysis of the source-built NTIO image remains
available without this trace. It can enumerate the exact opcode, memory,
interrupt, transition, and device/capability dependencies that arise after
`0070:0000`, distinguish them from the unresolved entry-state proof, and
produce the next neutral core/mantle requirements. That work does not execute
NTIO or make a normal runtime path.

## Completion

M14 is complete: no existing non-invasive mechanism can supply the historical
pre-instruction trace, and the precise missing source-recovery/contract
boundary is documented. The project remains able to progress through static
guest dependency evidence.
