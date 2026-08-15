# T217 S9 — SYSINIT Command-Name Realization Map

## Question

Does the source-built NTIO/NTDOS startup chain materially realize OpenNT
SYSINIT's normal `COMMAND.COM` Open contract, or does the `OTHER_ADMITTED`
classification justify changing the DEM provider?

## Direct Source And Artifact Evidence

`src/opennt/base/mvdm/dos/v86/doskrnl/bios/sysinit2.asm` declares:

```text
commnd db "\\COMMAND.COM",0
```

`sysinit1.asm` loads `DX` from that symbol before the normal INT 21h Open.
The BIOS makefile compiles `sysinit2.obj`, links it into `ntio.exe`, then
relocates the result to segment `70`. The retained source-built `NTIO.SYS`
input has SHA-256
`CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937`, matching
the retained `ntio-tools16-opennt-v1` output; it contains the command literal
at byte offset `0x3E36`. The retained linker map places `commnd` at
`009D:3466` before that normal relocation.

The BOP pathname is deliberately **not** a pointer to that raw NTIO symbol.
Original DOS `$Open` in `dos/file.asm` calls `TransPath` with the incoming
`DS:DX` pathname and `DOSDATA:OpenBuf`, changes to DOSDATA, loads `SI` from
`WFP_START`, and invokes `SVC_DEMOPEN`. `dos/macro2.asm` establishes
`WFP_START = DI` at entry, copies the selected CDS path, and, for a leading
root separator, resets canonicalization to the drive root before copying the
remaining components. The source documents a normal WFP as beginning with
`d:\\`. Thus the expected normal request after the boot-drive/default-drive
sequence is canonical C: root `COMMAND.COM` — conventionally
`C:\\COMMAND.COM` — rather than the original bare `\\COMMAND.COM` literal.

The retained NTDOS map places `OpenBuf` at `0794:03D2` and the `WFP_START`
variable at `0794:05B4`. The native observation's `DS:SI=00A7:03D2` is
consistent with relocated DOSDATA and the contents of `WFP_START`; it is not
evidence that SYSINIT's `commnd` data was lost or that the wrong NTIO image
was used.

## Current Seam Comparison

The active `bx_ntvdm_dem_path_v1_resolve` accepts a drive-qualified rooted
path with either separator, removes the root separator, and produces the
uppercase relative identity `COMMAND.COM`. The readonly namespace declares
the same C: image at `\\COMMAND.COM`, and its comparison intentionally
equates a declaration's leading separator with a DEM canonical relative path.
The source contract and the current declared-image seam therefore agree in
static analysis.

S6 nevertheless proved the live copied request as `OTHER_ADMITTED`, with C:
admitted and the declared namespace attached. This cannot justify a special
DEM Open result: the remaining unknown is the shape of the already copied
WFP request before the adapter's canonicalizer, not a missing BOP, a missing
namespace, or an NTIO provenance failure.

## Recovery Ladder And Decision

| Rung | Disposition |
| --- | --- |
| Original OpenNT source | Reused as the normative SYSINIT, `$Open`, `TransPath`, CDS, WFP and error-flow contract. |
| Smallest seam | Existing checked request gather and DEM path resolver remain the only candidate seam. No change is selected because static contracts agree. |
| External intrusion | Rejected; neither bx-core nor bx-mantle participates in pathname identity. |
| Newly authored behavior | Rejected; a fabricated `COMMAND.COM` success would mask the unresolved original-path realization discrepancy. |

The S9 outcome is a further bounded evidence step, not a repair. S10 may add
a default-off fixture record to the existing copied request which reports only
source-relevant shape predicates: byte count, recognized C-drive prefix,
root-separator class, and canonical component cardinality. It must not retain
or emit pathname bytes, host paths, guest addresses, handles, hashes, BOP
routing decisions, or a new provider outcome. Its sole purpose is to decide
whether the existing adapter canonicalizer can be source-repaired or whether
the guest-side realization/memory path needs another source map.
