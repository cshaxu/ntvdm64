# M0 T199 S2 DEM Source, ABI and Failure Matrix

## Complete service set

The immutable DEM provider map has 74 entries: values `00` through `48` plus
the `49` sentinel. All 73 callable values have an original dispatcher row and
source definition; no original handler is unresolved. The machine-readable
per-service map is `opennt-dem-provider-map-001-20260811-002`, SHA-256
`F368D0B56420FC2A6AD3B30CA82993ACB8D969CC608861E317F234BDC795D792`.

| DEM component group | Service values | Original ABI direction and current state |
| --- | --- | --- |
| Namespace and handle lifecycle | `00-08`, `12`, `16-18`, `1E`, `20`, `22`, `27`, `2C-2F`, `31` | Historical files, paths, FCBs and guest buffers use CCPU/SAS pointers. Read-only namespace O/S/R/C and selected metadata candidates exist, but mutation/FCB/write semantics remain unimplemented. |
| Search and DTA | `09-0C`, `1B`, `44` | Original DTA/FCB search owns caller buffer and continuation state. Current copied DTA/pathname search candidates are partial; FCB and mutable-host search are not admitted. |
| Drive, media, DPB and device I/O | `0D-10`, `0E-0F`, `21`, `23-25`, `29-2A`, `46-48` | Original contracts use drive inventory, device chains, DPB, DASD/pipe state and raw guest memory. Current boot-drive, immutable drive snapshot, DPB/GSET and limited IOCTL candidates are partial. |
| DOS load, symbols and lifecycle | `11`, `1D`, `34-3D`, `45` | Original handlers load DOS, transfer DOS/app/debug lifecycle and symbols through CCPU/SAS/NT host state. Current `11` load transaction and selected non-debug/system-symbol candidates are partial; no full lifecycle provider exists. |
| Clock, current state, hard error and policy | `13-15`, `19`, `1A`, `1C`, `32`, `33`, `3B`, `3F`, `41` | Original handlers consume host date/time, current-directory/default-drive, hard-error, retry/debug/lock/computer state. Current hard-error, GSET and fixed non-debug candidates are partial; ambient host state is excluded. |
| Original no-operation slots | `1F`, `24`, `26`, `28`, `2B`, `40`, `42`, `43` | `demNotYetImplemented` is the original dispatcher target. Its exact source behavior is four-byte continuation with CF clear, not a file/device implementation. The current all-eight unavailable plane is the only whole DEM disposition already available. |
| Dispatcher boundary | `49` | `SVC_DEMLASTSVC` is not callable. Original `DemDispatch` rejects it and any larger value with CF set and FALSE return. |

Every callable non-no-op service is `source-pending-composition`: its original
handler exists, but `demdisp.c` and its modules use historical `softpc.h`,
CCPU/SAS registers, guest pointers and NT host state. Direct linking is
forbidden. A future provider must use copied CPU values, checked guest
address/length transfers and explicit result/failure records.

## Failure and continuation rules

The only uniform DEM failure/reply rules proved by the dispatcher are:

1. `1F`, `24`, `26`, `28`, `2B`, `40`, `42`, and `43` call
   `demNotYetImplemented`, which clears CF.
2. `49` and larger service values are rejected by `DemDispatch`, which sets
   CF and returns FALSE.
3. The other 65 handlers have service-specific register, CF, guest-memory and
   continuation contracts. No generic adapter failure may substitute for
   their original behavior.

## Current evidence levels

`bx-vdm` currently has partial source-derived candidates in every major DEM
group, but not a package closure. The T198 composition manifest proves only
that selected sources compile together. Focused tests prove selected copied
transactions; they do not prove all-service routing, cross-group state,
negative cases or native integration for the complete DEM package.

Consequently the next S must select one explicit disposition for all 73
callable entries as a set, preserving the eight original no-op slots and the
sentinel. It must not implement the next reached DEM service.
