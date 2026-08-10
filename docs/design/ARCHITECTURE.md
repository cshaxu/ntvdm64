# System Architecture

## Ownership

| Area | Responsibility | Boundary |
| --- | --- | --- |
| Runner | Invocation, image classification, host process handoff, diagnostics | Does not invent guest/DOS behavior. |
| OpenNT recovery | Historical CCPU/SoftPC, BIOS/BOP, DEM, and NTDOS semantics | Preserve caller, owner, layout, ordering, and failure behavior. |
| Overlay | A caller-proven missing historical semantic | Minimal, fail-closed, independently authored implementation. |
| Host compatibility seam | A retired host API reached by an original caller | Preserves the original function boundary and data flow. |
| Research fixture | Bounded compatibility observation | Default-disabled and never a product runtime substitute. |

## Control Flow

`CCPU -> BIOS[] -> MS_bop_0 -> DemDispatch -> DEM -> NTDOS` is the primary
historical control-flow model. Any deviation needs an evidence record and a
focused fixture.

## External Boundary

`ntvdm64`, Bochs, PCjs, and later Microsoft-source trees are read-only
comparative evidence. They cannot be a CMake input, linked library, generated
source, runtime component, or acceptance gate.
