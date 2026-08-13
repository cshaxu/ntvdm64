# T193 S1 reached machine-selector audit 001

Date: 2026-08-12  
Packet: M0 T193 S1  
Disposition: source inventory complete; no source, build, or observation change.

## Method

This audit uses the selector set in the one valid T190 observation and maps it
to the immutable SoftPC `BIOS[]` table in `base/bios/bios.c`, the original
`nt_bop.c` top-level selectors, and the already admitted machine-composition
seam.  A `C4 C4 xx` byte sequence is not sufficient to choose a `BIOS[]`
alternative: the catalogued overlap rule remains binding.

## Reached non-host-family selectors

| Selector | Original source identity | Current ownership and disposition |
| --- | --- | --- |
| `06` | `BIOS[] -> illegal_op_int` | The sole separately admitted original handler island.  `illegalp.c` reads the Bochs-created real-mode frame, reports the fault, advances its saved IP, then calls `unexpected_int`; the transient machine seam supplies only the checked RAM, CPU-frame and PIC mechanics it needs.  It is never an adapter service.  T176/T179 prove the island and a prior context-specific observation, but do not prove it was selected by the T190 engine image. |
| `12` | `BIOS[] -> memory_size` | Native BIOS/memory compatibility candidate.  There is no isolated handler island or current selector route.  Bochs memory and guest firmware remain owners; no historical BIOS table installation is admitted. |
| `15` | `BIOS[] -> cassette_io` | Native BIOS peripheral candidate.  No handler island, cassette device, or configuration admission exists; default decline/defer is required. |
| `17` | `BIOS[] -> printer_io` | Native BIOS peripheral candidate.  No handler island, printer device, or host-print bridge is admitted; default decline/defer is required. |
| `5E` | `MS_bop_E` in the NTVDM table; `illegal_bop` in alternative rows | Collision, not a machine selector.  The original top-level routine issues UMB/config-complete notification and DOS drive initialization, but needs historical composition.  Its lack of a catalogued service byte does not authorize an adapter rehost or allow the machine component to select the competing BIOS row. |
| `5F` | `MS_bop_F` in the NTVDM table; `illegal_bop` in alternative rows | Collision, not a machine selector.  The original routine installs keyboard vectors and restarts historical ICA interrupts.  It cannot be inferred from three BOP bytes or moved into adapter; the selector remains governed by its existing top-level/lifecycle composition. |
| `66` | `emm_init` only under the historical `LIM` build condition; otherwise `illegal_bop` | Explicitly unavailable in the minimal profile.  Enabling EMS/LIM would be a new machine/device admission, not a handler-island follow-up. |
| `C8` | `v_host_mouse_install1` under the NTVDM host-mouse build condition | Historical host-mouse virtualization candidate.  It is not a Bochs-native proof and cannot select a host mouse or UI bridge; unavailable/deferred. |
| `FE` | `host_unsimulate` in some historical branches; other rows differ | Existing engine-owned controlled-stop contract, not an adapter host service or historical BIOS selection.  This audit neither changes nor broadens that contract. |

## Consequences

Only `06` has a separately proven original handler island.  It is deliberately
exceptional: `src/bx-ntvdm-machine-composition/` may select the unchanged
`illegal_op_int` only through the default-off, selector-blind Bochs seam and
its fixed mechanics context.  The adapter still observes or routes only
host-service families; it owns none of the rows above.

`12/15/17/66/C8` are not a backlog of per-selector adapter handlers.  They
are Bochs/firmware or unavailable domains and require their own reached-path
evidence and feature admission before any device or firmware work.  `5E/5F`
remain overlapping historical top-level composition, so neither the machine
component nor a guessed host-service provider may claim them.

## Next S

S2 must reconcile the exact feature switches, object identities and link map
of the T190 engine bundle with the two already admitted original machine
islands (`02` and `06`).  It is a read-only provenance check: no rebuild,
runtime retry, handler activation, BIOS table import, device configuration or
adapter provider change is permitted.  Its outcome may only state which
already-proven island is actually present in that frozen image and which
selectors still take default decline.
