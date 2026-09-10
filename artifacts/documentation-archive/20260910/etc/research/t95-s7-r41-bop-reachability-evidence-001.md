# T95 S7 r41 BOP Reachability Evidence

## Result

The retained observation at
`artifacts/analysis/t95-s7-native-post-handoff-034-20260811-001` is the first
native-Bochs POST-to-real-guest run in this packet.  It ended without watchdog
termination (`exitCode: 1`, `watchdogTerminated: false`) after observing 31
`C4 C4` instructions and recording seven existing bounded transaction commits.
The observation runner deliberately passes native `-benchmark 1`: Bochs's
one-shot `benchmarkTimer` sets `kill_bochs_request` after one million emulated
ticks, the CPU loop returns, and the text configuration UI assigns exit code
1.  It is therefore a bounded observation ending, not an inferred guest crash
or BOP failure.  It is evidence of control-flow reachability, not of complete
DOS-host service semantics.

The generic listener records every `#UD` whose captured bytes begin `C4 C4`.
It does not dispatch the selector, reinterpret a BOP as a Bochs operation, or
create a host capability.  A service byte is decoded only for catalogued
families.  Thus it deliberately reports both OpenNT-defined and currently
uncatalogued selectors.

## Reached Set

| Selector | Service | Count | OpenNT interpretation | Current classification |
| --- | --- | ---: | --- | --- |
| `50` | `11` | 1 | `BOP_DOS:SVC_DEMLOADDOS` | Defined; observed only. This is the first reached BOP. |
| `50` | `0D` | 1 | `SVC_DEMGETBOOTDRIVE` | Defined; observed only. |
| `50` | `0F` | 1 | `SVC_DEMGETDRIVES` | Defined; observed only. |
| `50` | `00` | 2 | `SVC_DEMCHGFILEPTR` | Defined; observed only. |
| `50` | `02` | 3 | `SVC_DEMCLOSE` | Defined; observed only. |
| `50` | `12` | 3 | `SVC_DEMOPEN` | Defined; observed only. |
| `50` | `1B` | 1 | `SVC_DEMSETDTALOCATION` | Defined; observed only. |
| `50` | `21` | 3 | `SVC_DEMIOCTL` | Defined; observed only. |
| `50` | `32` | 1 | `SVC_DEMSETHARDERRORINFO` | Defined; observed only. |
| `50` | `3B` | 1 | `SVC_ISDEBUG` | Defined; observed only. |
| `50` | `42` | 3 | `SVC_DEMFASTREAD` | Defined, but OpenNT's dispatch table itself marks it `demNotYetImplemented`; no local substitute is admitted. |
| `50` | `46` | 1 | `SVC_DEMGETDPBLIST` | Defined; observed only. |
| `50` | `36` | 1 | `SVC_DEMENTRYDOSAPP` | Defined; current final reached service. It is emitted by `msproc.asm` immediately before DOS transfers to an app. The original host handler only invokes a VDD user hook after the first call, then otherwise returns. |
| `54` | `05` | 1 | `BOP_CMD` family | Defined family; service interpretation retained in the catalog, not locally dispatched. |
| `54` | `0C` | 1 | `BOP_CMD` family | Defined family; its materialization capability remains queued. |
| `5E` | — | 1 | `BOP_NOTIFICATION` | Defined selector; observed only. |
| `5F` | — | 1 | `BOP_UNIMPINT` | Defined selector; observed only. |
| `12`, `15`, `17`, `66`, `C8` | — | 5 | No OpenNT top-level family, but explicit rows in the historical SoftPC `BIOS[]` table | SoftPC-table provenance; deliberately pass through unchanged. |

Counts are per `(selector, service)` except the final uncatalogued row, whose
five calls are one each.  The `BOP_DOS` names and values come from
`src/opennt/base/mvdm/inc/dossvc.h`; original dispatch is `DemDispatch` in
`src/opennt/base/mvdm/dos/dem/demdisp.c`; the historical `0x50` selector shim
is `MS_bop_0` in `src/opennt/base/mvdm/softpc.new/host/src/nt_bop.c`.

## What This Says About the Observation Boundary

`SVC_DEMENTRYDOSAPP` is a notification boundary before DOS transfers to the
selected application's initial stack and entry point, not a file-system
operation.  Therefore its presence is positive evidence that the current run
has progressed beyond NTIO's early setup.  It does **not** identify the later
`exitCode: 1` as a failure of that service: that code is the runner's
benchmark-window outcome, and the adapter listener has no dispatch action for
this selector.

The next admissible investigation is a separately admitted, longer bounded
observation through the guest instructions following `msproc.asm`'s
`SVC SVC_DEMENTRYDOSAPP`.  It must retain the same native benchmark mechanism
and classify newly reached calls before changing any service behavior. If a
host-facing file operation becomes a blocking edge, it belongs to the already
queued coherent read-only file/configuration capability, not to an ad hoc BOP
handler.
