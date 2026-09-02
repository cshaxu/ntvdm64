# M0 T367 — Original DOS EXEC and parent return

## Purpose

Recover the original guest-owned DOS `.COM`/MZ execution lifecycle after T366
proved a declared command reaches original COMMAND and the DEM open boundary.
The guest source/artifacts remain load-only. T367 does not compile guest code
into the host or manufacture a child image.

## Source boundary

The source chain begins in selected NTDOS guest code with `$Exec` and proceeds
through JFN/SFT lookup, PSP/arena/environment construction, child entry and
ordinary `INT 21h/AH=4Ch` termination/parent restoration. Host ownership is
limited to the pre-existing DEM open/read/close contract and bounded session
guest-memory leases. COMMAND observes the final ordinary result through its
original `54:0B` contract.

## S plan

### S1 — Original owner and artifact ledger

Map original selected guest source and artifact identities, every immediate
caller/callee and all host BOP boundaries. Classify original reuse, existing
same-shaped binding, unavailable historical dependency and possible later
owner. Select one complete earliest S2 cohort; do not run the product.

### S2 — Earliest complete original EXEC cohort

Recover only the first owner-complete missing cohort established by S1. Prefer
existing original guest code and current source-shaped DEM/session binding;
preserve guest layouts/order/failure. A missing guest execution contract may
not be replaced with a host loader or a synthetic BOP result.

### S3 — Formal boundary closure

Prove selected host-side inputs are in the CPU40/x86 product and the immutable
guest source/artifact identity is retained by its manifest. Run focused
positive/negative boundary tests only; no product observation.

### S4 — One frozen guest execution observation

Make one fixed-container observation of the declared immutable `.COM` child
only after S3. Record the earliest `$Exec`, DEM read, child entry/return or
source-defined terminal. A trace can transfer the next owner but cannot select
a leaf repair.

## Completion boundary

T367 ends with a source-owned execution/return result or an exact earlier
owner transfer. It does not claim native child, MZ/WOW/DPMI/Redirector or x64
guest runtime merely because an original source route is mapped.
