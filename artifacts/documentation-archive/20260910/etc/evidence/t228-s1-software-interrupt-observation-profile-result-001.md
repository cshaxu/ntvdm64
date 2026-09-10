# T228 S1 P20 — Software-Interrupt Observation Profile Result

## Question

Can the remaining immutable COMMAND/NTDOS lifecycle be observed at one neutral
Bochs boundary without recognizing a BOP, service, guest address, or historical
component name?

## Inputs

- Adopted Bochs `src/bx-core/cpu/soft_int.cc`, existing `INT_Ib` boundary.
- Project mantle ring `src/bx-mantle/bx_ntvdm_software_interrupt_observation_v1.*`.
- P20 admission and registered exception `BX-CORE-088`.
- Fresh CPU5/Pentium-MMX, MSVC x64 `/MT` Ninja graphs.

## Procedure

The default graph `build/M0-T228-S1/p20-default-20260818c` compiled the new
mantle source with no diagnostic definition and linked/runs
`t228-s1-budget-terminal-position-fixture.exe` successfully. Its target hash is
`26d1f3d67774a261c7dccddd4cc775ca3ebbacd28ab7fd5f61c24abcfe0da57e`.

A distinct diagnostic graph `build/M0-T228-S1/p20-software-int-20260818b` was
generated with only
`BX_NTVDM_ENABLE_MANTLE_SOFTWARE_INTERRUPT_OBSERVATION=1`.
Its configuration hash is
`11eeead14242b23c3f3c7bc079d3b584876da9dbeb6973e5ab66d6b47f31b3c1`;
the focused fixture hash is
`35fd159144d66fcdc6b739b2c7c76455e75e3a90661b8f536ff98c8734791736`.

The focused fixture proves, in one finite machine lifecycle:

- configured capacity zero records nothing while normal real-mode `INT 20h`
  delivery still reaches the existing controlled `UD2` stop;
- enabled real mode records `INT 20h`, then a second `INT 21h`, in chronological
  order, with the pre-delivery CS:RIP values after their instruction fetch;
- a self-contained GDT/IDT setup enters protected mode, delivers protected
  `INT 20h` to a controlled `UD2` handler, and leaves the ring empty;
- a three-entry mantle ring retains the chronological final three records after
  five direct fixed-width record calls, and reset clears it.

The added `INT_Ib` hook is guarded solely by `real_mode() || v8086_mode()` and
copies fields already available at that delivery boundary. Its raw CPU selector
member accesses are data copies, not selector interpretation; the added core
block has no BOP, OpenNT, DOS, SoftPC, service, adapter, CLI, guest-memory, or
address/opcode branch. `INT3` and `INTO` remain deliberately unhooked.

## Observations

Both the default fixture and the diagnostic fixture returned exit code zero.
The diagnostic graph is x64, `/MT`, CPU5/Pentium-MMX, with instruction history
and provenance both disabled. The default graph carries no software-interrupt
diagnostic definition.

## Interpretation

P20 closes the standalone mechanical observation profile. It changes neither
interrupt delivery nor product behavior in default builds. It does not select a
COMMAND, NTDOS, DEM, BOP, or adapter repair. One later bounded immutable
Direct/Readonly observation may consume the copied ring only to attribute the
already mapped complete COMMAND/NTDOS owner domain.

## Follow-up

Use the profile only in the active T228 owner-domain integration bracket. Do
not turn an observed vector into a leaf BOP task or add semantic filtering.