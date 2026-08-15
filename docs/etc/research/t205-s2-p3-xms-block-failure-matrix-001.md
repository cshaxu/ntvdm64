# T205 S2 P3 — XMS block lifecycle failure matrix

## Source result map

`xmsblock.c` is the normative source. Its observable explicit outcomes are:

| Service | Success | Explicit source failure | Bounded current result |
| --- | --- | --- | --- |
| `52:02` allocate | AX base KiB | `SAAllocate` failure sets AX `0` | no free session slot, zero request, base reservation failure, mantle rejection, or >16-bit base sets AX `0` |
| `52:03` free | AX `1` | `SAFree` failure sets AX `0` | unknown/stale base-size pair or mantle rejection sets AX `0` |
| `52:0B` reallocate | CX new base KiB | `SAReallocate` failure sets CX `0` | unknown/stale pair, zero requested size, mantle rejection, or >16-bit base sets CX `0` |

The historical source has two indeterminate paths: zero-size allocate bypasses
`SAAllocate` then consumes uninitialized `BaseAddress`; equal-size reallocate
bypasses `SAReallocate` then consumes uninitialized `NewAddress`. These are
not a reproducible success contract. The source-derived provider deliberately
returns the corresponding documented failure register rather than exposing
uninitialized state.

## Evidence

The current strict MSVC x64 `/MT /W4 /WX` XMS package regression now verifies
zero allocation -> AX `0`, stale free -> AX `0`, and stale reallocate -> CX
`0`, in addition to normal allocation/free/reallocation coverage. This is an
adapter package test; the separately recorded minimal-machine link failure
still prevents a native closure claim.
