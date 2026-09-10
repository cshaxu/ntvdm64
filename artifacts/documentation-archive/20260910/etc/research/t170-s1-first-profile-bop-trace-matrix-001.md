# T170 S1 first-profile BOP trace matrix 001

## Inputs and method

The matrix is the unique `(selector, service-state, service)` set mechanically
extracted from T169's retained stderr. Identities are reconciled against
`bx_ntvdm_bop_catalog_v1.c`, pinned `dossvc.h`/`cmdsvc.h`, and T96's complete
DEM/COMMAND/system provider maps. “Effect” records the current first-profile
role, not a claim that the historical provider has been linked.

## DEM family — selector `50h`

| Service | Original identity | Current first-profile disposition/effect |
| ---: | --- | --- |
| 00 | `DEMCHGFILEPTR` | Finite read-only namespace seek; source-derived provider. |
| 02 | `DEMCLOSE` | Finite namespace close/session lifecycle; source-derived provider. |
| 0D | `DEMGETBOOTDRIVE` | Immutable profile boot-drive answer. |
| 0F | `DEMGETDRIVES` | Profile drive-inventory answer; not ambient host drives. |
| 11 | `DEMLOADDOS` | Startup session/NTDOS loading transaction; source-derived composition. |
| 12 | `DEMOPEN` | Finite namespace open; source-derived provider. |
| 1B | `DEMSETDTALOCATION` | DTA registration through copied state; source-derived provider. |
| 21 | `DEMIOCTL` | Reached contained IOCTL contract only; no general device/host-I/O provider. |
| 32 | `DEMSETHARDERRORINFO` | Bounded registration record; no host error UI. |
| 36 | `DEMENTRYDOSAPP` | Original guest transfer continuation, not a host service result. |
| 3B | `DEMGETCOMPUTERNAME` | Profile-contained host identity disposition. |
| 42 | `DEMFASTREAD` | Finite source-derived read-only fast-read into ordinary guest RAM. |
| 46 | `DEMGETDPBLIST` | Profile-backed DPB materialization. |

All thirteen are defined catalogue entries. The table is a whole finite boot
namespace/session provider family; it is not thirteen independently admitted
handlers. In particular, historical `DEMFASTREAD` is a `demNotYetImplemented`
slot, so the current contained fast-read is recorded source-derived replacement,
not an original provider link.

## COMMAND family — selector `54h`

| Service | Original identity | Current first-profile disposition/effect |
| ---: | --- | --- |
| 01 | `CMDGETNEXTCMD` | Fixed launch-envelope progression. |
| 02 | `CMDCOMSPEC` | Finite COMSPEC bootstrap data. |
| 04 | `CMDGETCURDIR` | Profile-contained current-directory answer. |
| 05 | `CMDSETINFO` | Copied/validated COMMAND information update. |
| 0C | `CMDGETCONFIGSYS` | Paired finite CONFIG materialization. |
| 0D | `CMDGETAUTOEXECBAT` | Paired finite AUTOEXEC materialization. |
| 0E | `CMDGETKBDLAYOUT` | First-profile bounded keyboard-layout disposition; no VDD. |
| 0F | `CMDGETINITENVIRONMENT` | Fixed initial-environment output. |

All eight are defined COMMAND entries. They compose as the original
COMMAND-startup/launch-envelope family. They do **not** enable original host
process execution (`54:08/10`) or arbitrary host console/redirection state.

## Non-family selectors

| Selector(s) | Catalogue/source owner | Current disposition/effect |
| --- | --- | --- |
| 12, 15, 17, 66, C8 | SoftPC BIOS table provenance only | Observed and pass-through; no adapter provider and no claim of a BIOS dispatcher. |
| 5E | OpenNT CONFIG-completion selector overlapping SoftPC table | Deferred composition notification; no result provider. |
| 5F | OpenNT unimplemented-interrupt selector | Original host diagnostic class; explicitly unavailable/pass-through. |
| FE | `BOP_UNSIMULATE`, overlapping lifecycle selector | Exact real-mode `C4 C4 FE` is the sole admitted adapter STOP contract; Bochs receives only typed STOP and no selector meaning. |

`12`, `15`, `17`, `66`, and `C8` have no catalogue service byte; the fourth
byte logged as `00` is not a service ID. The adapter listener records all of
them but applies no result. `FE` is deliberately separated because its exact
adapter contract ends this declared test target; it must not be generalized to
other selectors or programs.

## Result and next boundary

Every one of the 34 unique observed identities has a catalogue owner and a
current disposition. No observed identity is unclassified. The validated
baseline is a finite, immutable BYOB DOS/COMMAND startup namespace plus the
QUIT target's controlled lifecycle completion. It does not establish an
arbitrary-DOS-program capability.

The next capability should therefore be selected as a **whole target/COMMAND
execution composition** (launch envelope, program invocation, return/exit
state and contained namespace), not by implementing the next missing BOP seen
in this trace. Historical `54:08`, `54:10`, and `54:11` remain the named
host-process/redirection boundary and require their original-failure/CLI
replacement analysis as one component.
