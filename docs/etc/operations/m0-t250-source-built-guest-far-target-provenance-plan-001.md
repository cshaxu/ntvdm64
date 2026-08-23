# M0 T250 — source-built guest far-target publication and control-flow provenance

## Purpose

T249 proves that original relocated NTDOS `DOSCODE` writes the runtime `FF`
at low physical `0x0A87`. The following bytes decode as `call far
[bx+0x326]`, reach `5859:5F04`, and currently execute zero RAM. This task
first determines whether those bytes are an original executable entry or
data accidentally reached by an earlier guest control-flow error; it does
not presume that a far target was legitimately published.

## Boundary

This is not a DEM/COMMAND BOP implementation package. It may not introduce a
BOP provider, adapter PSP/PDB state, machine feature, firmware device or
target-value workaround. `bx-core`/`bx-mantle` remain selector-blind. Any new
diagnostic requires an S admission and, if it touches adopted Bochs,
pre-registration in the exception ledger.

## S sequence

| S | Scope | Exit evidence |
| --- | --- | --- |
| S1 | Source/image/relocation map for `0x0A87`, the original NTDOS `DOSCODE` writer, and the logical data/code ownership of the destination bytes. | Original source and image/layout calculation distinguishes a legitimate executable target from a data/control-flow error. |
| S2 | Audit whether any existing fixed-width observation is needed to identify an unproven publisher or target. | Explicitly admit no new observer when S1 establishes that the decoded bytes are not an executable source entry. |
| S3 | Only if S1/S2 retain a mechanically observable unknown, implement the smallest registered default-off selector-blind observation and run one bounded source-built checkpoint. | Copied fact identifies publisher/target disposition without changing guest execution. |
| S4 | Reconcile `BOP-DEPENDENCY-115`, write closure, and assign the next owner. | No BOP/machine repair is inferred from a trace alone. |

## Completion condition

The task closes when the source map either attributes a legitimate original
guest publication or proves that control has reached non-executable guest
storage. It does not claim ordinary guest parent return unless that path
actually reaches `$Exit/$Abort/reset_environment`.
