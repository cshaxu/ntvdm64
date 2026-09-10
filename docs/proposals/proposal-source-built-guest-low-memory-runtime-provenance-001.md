# Source-built guest low-memory runtime provenance

## Purpose

T248 proves that the current source-built EXEC checkpoint reaches a
runtime-modified instruction at physical `0x0A87`, which executes an ordinary
far indirect call to `5859:5F04` and then runs zero RAM. The staged
`NTIO.SYS` source byte at that address is different. This package identifies
the writer and pointer provenance, or establishes the smallest exact missing
observation seam.

It is a guest-runtime provenance package, not a BOP-family implementation
task. It does not authorize a DEM/COMMAND leaf patch, an adapter PSP/PDB
representation, firmware initialization, or BOP/DOS terminology in
bx-core/bx-mantle.

## S sequence

| S | Scope | Exit evidence |
| --- | --- | --- |
| S1 | Map the NTIO source/load/relocation context for physical `0x0A87`, the exact far indirect call ABI, and every existing selector-blind RAM-write/provenance observation seam. | Source/image/ABI map says whether existing instrumentation can identify the writer and pointer without policy decoding. |
| S2 | If S1 finds a sufficient existing seam, run one bounded source-built observation and attribute the first relevant write/target publication. | Copied fixed-width evidence names the physical write range, preceding position and original/source owner, without retaining guest pointers. |
| S3 | If S1 proves no sufficient seam, admit the smallest default-off selector-blind physical-write observation, with negative coverage and any required Bochs exception registration. | The observation is mechanical, fixed-width, disabled by default, and does not change guest execution or interpret guest semantics. |
| S4 | Re-run the source-built checkpoint and reconcile `BOP-DEPENDENCY-115`. | Precise owner/disposition for the low-memory writer/transfer; no trace-led BOP patch. |

## Acceptance boundary

The package is complete when it either attributes the writer/indirect target
to an original guest runtime source path or proves a separately owned missing
machine input with concrete source evidence. A raw terminal at zero RAM is not
a reason to invent a BOP provider or a firmware/device feature.
