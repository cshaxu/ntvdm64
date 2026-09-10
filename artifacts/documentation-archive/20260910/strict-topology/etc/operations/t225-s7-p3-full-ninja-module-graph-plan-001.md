# T225 S7 P3 Full Ninja Module Graph Plan

## Objective

Replace the pilot graph with the exact admitted MSVC x64 `/MT` build graph for
the current CPU5/P composition. Ninja is the executor; an explicit manifest is
the source-membership authority.

## Modules

- `bx-core`: the frozen source-proven CPU5/P core closure only;
- `bx-mantle`: the finite native machine assembly and mechanics only;
- `bx-vdm`: the BOP/provider composition closure only;
- `cli`: CLI-owned composition and target objects only.

Each nonempty admitted module produces one static archive. Fixtures and CLI
executables are always fresh link targets. No glob, existing object directory,
`artifacts/build`, `bochs.exe`, historical product main, GUI/plugin/device
archive, or prebuilt adapter composition can enter the graph.

## Verification

Prove a clean cold build, an unchanged no-op build, and a header dependency
rebuild under `deps = msvc`/`showIncludes`; independently run the current S6
Direct/Readonly fixture. Only after those facts are reviewed may the P1
custom-cache implementation be removed. Its documents remain historical
evidence and the removal must not erase their indexed records.

## Boundary

This is build composition only. It adds no BOP behavior, host capability,
guest trace, BIOS/device feature, or Bochs semantic change.