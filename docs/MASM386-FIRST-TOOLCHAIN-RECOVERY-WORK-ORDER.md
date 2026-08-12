# MASM386-First Toolchain Recovery Work Order

Status: active toolchain recovery, 2026-08-07.

## Progress

The first two priority audits are complete: pristine MASM386 assembled all 38
NTDOS modules and all 12 NTIO modules. Both then passed unchanged response
files through historical LINK in DOSBox and their original `reloc`/`stripz`
packaging paths, reaching `historical-link-reproduced`. See
`MASM386-PRISTINE-NTIO-NTDOS-AUDIT-2026-08-07.md`.

## Decision

Every already-built or previously attempted guest artifact must be reclassified
through an original-source, per-object MASM386 audit before it is used as
current build evidence. JWasm, UASM, OpenWatcom C16, WLink, and local format
tools are retained as comparison tools or explicitly bounded compatibility
tools; they are not a replacement claim for the historical MASM + LINK16
pipeline.

This work order does not claim that a reconstructed host MASM386 executable is
the original distributed Microsoft tool. It does establish the strongest local
source-based assembler baseline available in this repository.

## P0: Historical MASM 5.10 Rebuild And Evidence Replacement

The recovered Microsoft `MASM.EXE` (version 5.10, run in DOSBox) has now
assembled an OpenNT guest module with the original `/DDEBUG=0` and
`/DCALL_DOS` command-line definitions. In contrast, the modern-host
OpenNT-source-built `masm386-managed.exe` crashes for every command-line
`/D` definition because its command-line parser invokes `definesym()` before
its line buffer is initialized.

Effective immediately:

1. Do not promote any image assembled by `masm386-managed.exe` to
   `historical-toolchain-reproduced`, even if its final link used historical
   `LINK.EXE`.
2. Rebuild every currently recorded assembly-only guest image with Microsoft
   MASM 5.10 in DOSBox, using the original makefile command-line options,
   include topology, object order, LINK invocation, and packaging semantics.
3. Preserve the existing `masm386-managed` stages as source-closure and
   diagnostic evidence only; do not delete them or treat their hashes as the
   new acceptance baseline.
4. Record one historical-MASM manifest per object and a final artifact
   manifest. A target may be promoted only after this rebuild succeeds without
   guest-source transforms.

This replaces the earlier assumption that successful `masm386-managed`
assembly was sufficient for final guest-build evidence. It is a host-tool
compatibility finding, not a defect in the OpenNT guest sources.

## Required Evidence Per Artifact

1. A pristine, manifested source stage with no guest-source transforms.
2. One MASM386 invocation and result for every makefile-selected assembly
   module, including include-path and host executable identity.
3. Separate classification of each failure as source-language, include/path,
   host ABI, generated-input, linker/OMF, reloc/packing, or resource-tool
   failure.
4. Object hash and size comparison against any legacy JWasm/UASM result only
   after the MASM386 object succeeds.
5. Link comparison only with the original object order and an admitted linker.
   A WLink result remains a comparison record, not a LINK16 substitute.

## Promotion Rules

| State | Meaning |
| --- | --- |
| `masm386-object-audited` | All selected modules assemble from pristine staged source. No link claim. |
| `masm386-link-compared` | Original object order has a recorded linker comparison. No LINK16 equivalence claim. |
| `historical-link-reproduced` | A recovered/admitted LINK16-compatible link, relocation, and packaging path exists. |
| `legacy-compatibility-only` | JWasm/UASM/OpenWatcom route or any guest-source rewrite remains necessary or is only historical evidence. |
| `blocked` | The first missing historical input or contract is recorded precisely. |

No `legacy-compatibility-only` result may be called the current build of an
artifact. Existing files and logs remain evidence and are not deleted.

## Priority Order

1. Rebuild `COMMAND.COM`, `HIMEM.SYS`, `EXE2BIN.EXE`, `NTIO.SYS`, and
   `NTDOS.SYS` with historical MASM 5.10 before retaining any final-image
   claim; repeat their original LINK and packaging phases unchanged.
2. Rebuild `FASTOPEN.EXE`, `COUNTRY.SYS`, `APPEND.EXE`, `ANSI.SYS`,
   `DEBUG.EXE`, `EDLIN.EXE`, `SHARE.EXE`, `KB16.COM`, `KEYBOARD.SYS`,
   `GRAPHICS.COM`, `LOADFIX.COM`, and `NLSFUNC.EXE` by the same route.
3. Complete `REDIR.EXE` with its original `/DDEBUG=0` and `/DCALL_DOS` options,
   then historical LINK and map generation.
4. Only after the historical-MASM rebuild queue is closed, address mixed-C16
   `SETVER.EXE` and `MEM.EXE`, source-absent artifacts, and any remaining
   generated-tool contracts.

## Toolchain Recovery Sequence

1. Rebuild and fingerprint source-built MASM386 from the managed OpenNT source
   closure on each new host/toolchain baseline.
2. Audit original modules without source overlays.
3. Repair only host ABI, path, or generated-input issues outside guest source.
4. If a pristine module still fails, preserve the exact diagnostic and seek the
   missing historical tool or macro contract before considering a narrow shim.
5. Compare successful MASM386 objects and original-order links against legacy
   compatibility records.
6. Recover or admit LINK16, `reloc`, MAPSYM, RC16, EXEPACK, and message/resource
   tools one at a time. Do not mask a missing one by declaring WLink equivalent.

## Immediate Harness

`toolchain-probe/audit-masm386-pristine-assembly.ps1` creates a new stage from
the manifested OpenNT source and audits each selected NTIO or NTDOS module with
the supplied MASM386 executable. It intentionally performs no source rewrite,
link, reloc, strip, or guest execution.

## Boundaries

- No write to `D:\home\repos.hobby\ntvdm64`.
- No removal of JWasm/OpenWatcom yet; they remain needed to compare earlier
  evidence and may still expose a missing historical-linker or C16 contract.
- A shim or overlay is admissible only after the pristine failure, its caller
  contract, and the rejected original-tool alternative are recorded.
