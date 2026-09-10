# M0 T248 — Selector-blind real-mode guest execution continuity plan

## Purpose

Recover the machine-owned execution continuity exposed after original NTDOS
`$Exec:Xfer_To_User` has completed its `50:36` handoff.  This is not a BOP
family package: bx-core/bx-mantle must remain selector-, DOS- and
OpenNT-name-blind while restoring the mechanical CPU/interrupt/return facts
needed for the guest to continue.

## Source evidence baseline

T247 S3 records a source-built run that reaches original `$Exec` child entry,
with PDB `0BF2h`, then reaches accepted imported `50:1E demWrite` before the
existing `UNEXPECTED_LOOP_RETURN` terminal (`detail=6`).  `50:1E` is already
source-mirrored and resumes; it is not a candidate leaf fix.  The tracked
cross-owner seam is `BOP-DEPENDENCY-115`.

## S sequence

| S | Scope | Exit evidence |
| --- | --- | --- |
| S1 | Map exact post-`50:36` real-mode instruction, interrupt/exception, stack and return edges against original NTDOS/SoftPC and adopted Bochs sources. | One source/ABI map distinguishes guest-owned `$Exit/$Abort` from mechanical CPU/mantle work, and identifies the smallest bounded replay profile. |
| S2 | Establish a selector-blind bounded replay/observation of the mapped instruction and interrupt-return sequence. | Formal fixture proves only CPU/memory/port/interrupt mechanics; no selector, DOS, PSP or BOP label enters bx-core/bx-mantle. |
| S3 | Recover the smallest source-proven mechanical gap, if S1/S2 identify one. | Upstream/adopted source or smallest mantle composition preserves machine semantics with negative coverage; any external intrusion is registered before use. |
| S4 | Re-run the source-built T247 checkpoint and reconcile `BOP-DEPENDENCY-115`. | Native evidence either advances beyond the previous terminal or assigns the next exact owner without a trace-led BOP patch. |

## Non-goals

No DEM/COMMAND/Redirector/WOW provider change, no guest PDB or `$Exit`
reimplementation, no BOP selector interpretation in machine layers, no
device enablement without a reached source caller, and no claim that a DOS
child has returned until the original guest state proves it.
