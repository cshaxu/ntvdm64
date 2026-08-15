# Tool Sources

`tools/` contains tracked tool sources and admitted historical tool inputs. It
never contains a compiler working tree, generated binary, probe output or
runtime log; those belong under the ignored `build/<task-id>/<run-id>/` tree.

## Required Layout

| Directory | Responsibility |
| --- | --- |
| `build/` | Configure, compile, package and explicit publish entry points. |
| `governance/` | Documentation inventory, manifests and governance checks. |
| `import/` | Admitted source and input acquisition helpers. |
| `probe/` | Read-only or diagnostic probes; output goes only to `build/`. |
| `historical/` | Vendored historical tool closures and their manifests. |

New tools must be placed in one of these directories and state their target
inputs and outputs. The `tools/` root contains only this directory guide.

## Historical Tool Closures

| Directory | Contents | Canonical use |
| --- | --- | --- |
| `historical/msdos400/` | the complete MS-DOS 4.00 `TOOLS` directory | DOS-hosted MASM 5.10 and Link 3.65, plus companion utilities |
| `historical/opennt-4.5/` | the complete OpenNT 4.5 `tools16` directory | OpenNT-era 16-bit build, message, relocation, and packaging tools |
| `historical/msdos6-qb45/` | the MS-DOS 6 QuickBASIC 4.5 `TL` tool closure | Exact MASM/LINK510/CL/NMAKE support for QBASIC, EDIT, and MSHERC reconstruction probes |

`MANIFEST.json` in each directory records every vendored file's size and
SHA-256, along with the original local source path used for the copy. Historical
build scripts must consume these paths rather than a toolchain-probe staging
directory or an external source checkout.

These files remain subject to their original provenance and license/research
restrictions. They are not produced by this project and are not part of any
runtime or BYOB payload.

## Research Inspection

`Inspect-SoftPcCompositionCandidate.ps1` is a read-only evidence tool for a
newly acquired SoftPC archive, map, listing, build log, or generator input. It
records the candidate hash and, for COFF archives, member, machine, and selected
symbol evidence. Its output is not an admission to `src/opennt`, CMake, a
fixture, or runtime. Review it under
`docs/R1-CCPU-CVIDC-INPUT-ACQUISITION-CONTRACT.md` first.
