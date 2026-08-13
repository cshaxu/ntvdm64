# T197 S2 Mantle Path Migration

## Question

Can the S1-proven native lifecycle files be physically placed under
`src/bx-mantle/` without changing their machine behavior or reintroducing VDM
meaning into Bochs?

## Inputs

- T197 S1 ownership map and exact candidate list.
- Current T196 Bochs-boundary tests.
- Current source tree after commit `477283c`.

## Procedure

1. Used `git mv` for the seven S1-authorized files.
2. Repaired only test path references; no C/C++ implementation text changed.
3. Searched non-historical tracked material for the three old minimal-source
   path families.
4. Ran the focused minimal-machine, minimal-mechanics, minimal-SIM and
   empty-port-space boundary scripts.

## Result

The following files now form the physical mantle root:

```text
src/bx-mantle/bx_ntvdm_minimal_machine.cc
src/bx-mantle/bx_ntvdm_minimal_machine.h
src/bx-mantle/bx_ntvdm_minimal_sim.cc
src/bx-mantle/bx_ntvdm_minimal_sim.h
src/bx-mantle/minimal_port_space.cc
src/bx-mantle/pc_system.cc
src/bx-mantle/pc_system.h
```

All four focused scripts pass. No live test/tool or selected minimal-lifecycle
source-vector reference remains to the former minimal-machine, minimal-SIM,
or empty-port-space paths.

## Mixed-Edge Disposition

The move makes the historical full-product metadata's reach into
`pc_system.{cc,h}` visible. `src/bochs/Makefile*`, subordinate product
Makefiles, and `vs2008/bochs.vcproj` still name the former root paths. They
are retained evidence for the rejected full `bochs.exe` product and are not a
build input for this mantle. Rewriting them would falsely make the old product
shell appear to be the new lifecycle build. The future mantle build owns its
own source vector and include roots.

This is a source-layout transition, not a claim that the legacy product recipe
continues to build. The next S must establish the selected core/mantle compile
closure; it must not repair the full product recipe as a substitute.

## Confidence and Follow-up

High confidence that the seven moved files are native lifecycle assembly and
remain selector-blind. The remaining SIM and memory initialization edges are
mixed and transfer to S3. `cpu/`, `memory/`, `fpu/` and `disasm/` remain
unmoved until their precise core closure is enumerated.
