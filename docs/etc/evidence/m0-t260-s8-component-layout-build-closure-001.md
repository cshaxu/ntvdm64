# M0 T260 S8 — component-layout and formal-build closure

S8/P2 completes the mechanical eight-component source-layout migration and
then purifies each component root so it contains production inputs only. It
does not change BOP provider, guest, or Bochs-machine behaviour.

## Legacy-root disposition

The historical `src/opennt` tree was first content-audited against
`refs/opennt`: 2,092 files were byte-identical original-source duplicates;
zero differed; the remaining 133 project-local files were assigned before the
duplicate tree was removed.  They now have these owners:

| Prior role | Component disposition |
| --- | --- |
| MVDM / SoftPC historical declarations and source-derived INT06 seam | `adapter-softpc` |
| Internal Windows declarations and compiler compatibility | `adapter-win32` |
| Directly composed original PIF parser | `opennt-host/command` (because `command_opennt_pif_parser.c` actually compiles it) |
| Legacy-only source, traces and tests | `docs/etc/legacy_code`, `tests/legacy/opennt` |
| Historical CMake / frozen manifests | `tools/historical` and `docs/etc/evidence` |
| Inherited XMS SoftPC overlay | `docs/etc/legacy_code/opennt-bop/overlay/xms` (evidence only) |

`src/bx-vdm` and `src/cli` were likewise fully vacated.  The engine run
contract belongs in `app`: it performs top-level BOP and host policy routing,
so leaving it in selector-blind `adapter-softpc` would violate that boundary.

## P2 production-tree purification

The former `opennt-bop/{original,mirror,overlay,route}` staging topology was
not a suitable final source layout: it mixed compiled minimal-change providers
with comparison copies and old overlays.  Active providers are now grouped by
their original owner family under `opennt-bop/{command,dem,dpmi,softpc,xms}`;
project-owned ingress is `opennt-bop/ingress`.  Non-linkable originals,
uncompiled mirror fragments, old overlays and import records moved with Git
history below `docs/etc/legacy_code/opennt-bop/`.

`opennt-host/capability` was similarly split into live
`command`, `dem`, `xms`, `redir`, `top_level` and `config` owner families.
The two adapter include surfaces are production ABI closures:
`adapter-softpc/include` retains required original relative header layout and
`adapter-win32/include` retains historical API spelling.  Unused CPU6/64
Bochs CPUID databases, MIPS/PPC SoftPC headers and non-linkable overlays moved
to `docs/etc/legacy_code/`.  This leaves the CPU5/Pentium-MMX formal closure
in `bx-core` without treating unused upstream source as product input.

Two actual production relationships were found during the purification:

- `command_opennt_pif_parser.c` directly composes `nt_pif.c`; it therefore
  remains in `opennt-host/command`, with `HOST-DIV-011` documenting the
  location-only divergence.
- DEM-to-Redirector/top-level and COMMAND-to-DEM declarations now use
  component-qualified includes.  This retains original service semantics and
  makes the formal owner edges visible to the compiler.

## Formal graph

`tools/build/t260-s8-component-manifest.json` is the single source manifest
for the active formal graph.  It declares 291 C/C++ sources across the seven
linkable components (`bx-core`, `bx-mantle`, `adapter-softpc`,
`adapter-win32`, `opennt-host`, `opennt-bop`, `app`), 78 fixtures and one
native target.  `opennt-guest` is explicitly an artifact-only eighth
component.

`tools/governance/Test-T260S8ComponentManifest.ps1` rejects legacy component
roots, duplicate/misowned inputs and an invalid library set before graph
generation.  `New-T260S8FullNinjaGraph.ps1` produces a pinned x64 `/MT`,
CPU5 configuration and keeps Bochs include roots ahead of historical SoftPC
compatibility headers to prevent `config.h` identity collision.

## Verification

- manifest gate: passed — 291 sources, 78 fixtures, one target;
- legacy-root scan: passed — no product/build input under `src/opennt`,
  `src/bx-vdm` or `src/cli`;
- formal Ninja graph: passed — `r018` ran to completion; `ninja -C
  build/M0-T260-S8/r018 -n` reports `no work to do`; all seven static
  libraries, 78 fixture targets and `bin/ntdos64-native.exe` linked;
- component manifest gate: passed again after P2 — 291 sources, 78 fixtures,
  one target;
- production-tree gate: passed — all eight component roots exist; no
  `original`, `mirror`, `overlay`, `capability`, reference/example or test
  staging directory remains under `src`; the formal manifest contains no
  `docs/etc/legacy_code` or `tests/legacy` build input;
- divergence-register gate: passed — 46 individually source-marked and
  uniquely README-registered production divergences (31 `BOP-DIV`, 11
  `HOST-DIV`, two `SOFTPC-DIV`, two `WIN32-DIV`);
- final actual incremental formal Ninja run: passed — the subsequent `r018`
  invocation and its dry run both reported `no work to do`;
- `git diff --check`: passed after mechanical EOF normalization.

The retained `refs/opennt` tree is reference-only evidence, not a product,
build, link or runtime input.
