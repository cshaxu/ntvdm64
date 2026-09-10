# T213 S3: COMMAND Terminal Package Admission

Packet: M0 T213 S3, Ordinary Mode  
Date: 2026-08-15  
Status: implementation admission

## Correction to S2

S2 correctly rejects selector-only normalization of DEM `50:3D`, but the
complete source sweep identifies a distinct, authenticated COMMAND route:
`tcmd2b.asm` reaches `CMDEXITVDM` only after its explicit top-level
`command.com` termination branch and marks the BOP “Never returns”.
`CMDEXITVDM` is selector `54:00`, owned by the existing COMMAND lifecycle
provider alongside `54:03` and `54:0B`.

This does not make the current boot-failure `50:3D` normal.  It admits a
separate, package-level normal terminal contract for the exact accepted
COMMAND `54:00` route.

## Admitted delivery

S3 may:

1. extend the existing COMMAND lifecycle provider with one versioned copied
   terminal record for exact `54:00`, carrying `COMMAND_EXIT` and an explicit
   no-DOS-exit-code state;
2. preserve `54:03` resume and `54:0B` final-return behavior, with full
   package regression for `00/03/0B` and malformed/unrelated BOPs;
3. expose only an opaque copied terminal record through composition runtime;
4. after normal engine cleanup, map an authenticated `COMMAND_EXIT` controlled
   stop to the already-defined `ORDINARY_GUEST_COMPLETION` terminal; and
5. let the existing CLI classifier present it without a fabricated DOS exit
   code.

## Exclusions

- `50:3D` remains controlled and has no new normal mapping.
- No Bochs/mantle selector, BOP, OpenNT, DOS, or COMMAND identity is added.
- No historical `ExitVDM`, BaseSrv/CSR, `ExitProcess`, host handle, pointer,
  callback, trace heuristic, or guest-memory provenance scan is admitted.
- No interactive session or terminal output capability enters this packet.

## Required proof

A fresh MSVC x64 `/MT` package fixture must prove `54:00` creates the one
typed record and STOP; engine integration must prove reset precedes ordinary
terminal mapping; and a negative `50:3D`/unrelated-stop regression must retain
controlled terminal.  A later native trace may observe the new path but is
not an implementation selector.
