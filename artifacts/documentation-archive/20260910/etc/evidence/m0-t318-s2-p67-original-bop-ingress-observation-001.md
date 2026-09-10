# M0 T318 S2 P67 — Original BOP Ingress Observation

## Question

After P66 proved NTIO load and CCPU first fetch, does the fixed CPU40 runtime
container reach the existing original MS BOP ingress, specifically the NTIO
`0x50:0x11` `SVC_DEMLOADDOS` transition?

## Source Route

CCPU decodes the original `C4 C4..` BOP form and calls the original BIOS
`BIOS[]` entry. `bios.c` maps `0x50` to `nt_bop.c::MS_bop_0`, which reads the
following service byte and invokes the original `DemDispatch`. The selected
NTIO image contains `C4 C4 50 11` at offset 1141, and its source
`msinit.asm` names that operation `SVC_DEMLOADDOS`.

## Procedure

The existing registered P66 diagnostic seam adds an observation only after
original `MS_bop_0` has decoded the service byte and before it calls
`DemDispatch`. It reports `MVDM-BOP-DISPATCH <selector>:<service>` and has no
selector, routing, register, guest-memory, return, or failure effect.

The selected x86 CPU40 product was rebuilt and substituted as the sole mutable
input in the same fixed console-owning container for the same eight-second
observation.

## Observation

The resulting capture contains the three P66 milestones:

```text
MVDM-STARTUP-MILESTONE NTIO-LOADED
MVDM-STARTUP-MILESTONE CPU-SIMULATE
MVDM-STARTUP-MILESTONE CCPU-FIRST-FETCH
```

It contains no `MVDM-BOP-DISPATCH` line before the normal eight-second
controlled timeout (`0x53504354`). The raw fixed-container records are:

- `artifacts/research/m0-t318-s2-p67-original-bop-ingress.txt`
- `artifacts/research/m0-t318-s2-p67-original-bop-ingress.txt.console.txt`
- `artifacts/research/m0-t318-s2-p67-original-bop-ingress.txt.json`

## Interpretation

The source BOP route is selected and the product reaches original guest
execution, but the bounded observation has not yet reached its first original
MS BOP. This is a directional runtime fact, **not** evidence that DEM, BOP, or
`SVC_DEMLOADDOS` is defective. The next work remains a source-shaped audit of
the pre-`SVC_DEMLOADDOS` NTIO/SoftPC machine prerequisites; no per-service
implementation or new dispatcher is admitted.
