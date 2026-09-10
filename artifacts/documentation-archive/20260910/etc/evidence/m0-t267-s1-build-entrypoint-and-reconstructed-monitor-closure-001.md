# M0 T267 S1 — build-entrypoint and reconstructed-monitor closure evidence

## Question

Can the repository expose exactly one current build entrypoint without stale
component paths, and can the retained reconstructed-monitor package compile
against its own declared contract?

## Inputs

- Current component manifest and `tools/build/New-T260S8FullNinjaGraph.ps1`.
- Retired root CMake target catalogue.
- Reconstructed-monitor source, header, and two fixtures below
  `tests/legacy/reconstruction/monitor/`.

## Procedure and observations

1. Removed the obsolete root `CMakeLists.txt`. It named deleted `src/bx-vdm`
   and `src/cli` roots; it was not the formal component graph.
2. Current README and CONTRIBUTING build instructions now invoke the
   manifest-driven MSVC x64 `/MT` Ninja generator only.
3. Moved stale pre-component probes, build scripts and boundary scripts into
   `tools/historical/pre-t260/` or `tests/legacy/reconstruction/` rather than
   allowing an active script to name a deleted source root.
4. Renamed the retained monitor package from task-labelled spellings to
   `reconstructed_monitor` names. Its normal and NTIO fixtures compiled with
   MSVC x64 `/MT`; both executed successfully, the latter using owned
   `src/opennt-guest/dos-v86/doskrnl/bios/NTIO.SYS`.
5. The active source/tool/test scan, excluding indexed historical records,
   finds no `src/bx-vdm` or `src/cli` consumer. It also finds no
   `app_s4_`, `ntdos64_s4_`, or monitor filename carrying `s4`.
6. Fresh external formal Ninja generation succeeds and its 485-edge dry run
   resolves owned current component inputs. A full sequential external build
   first exposed one missing `adapter-softpc/instruction_history.cc` graph input
   at the final application link. The generator was corrected, the 81 affected
   edges were rebuilt, and both `ninja -j1` and the following no-work invocation
   pass.

## Interpretation

The stale CMake and mismatched reconstructed-monitor contract are closed.
The current formal build graph is independent of the retired roots and now has a
full external source-build and no-work verdict. The corrected input is a build-
graph closure repair; it does not recreate a CMake or old-path dependency.
