# Adapter BIOS Selector Map v1

## Purpose

This adapter-local, recovered map represents exactly one unavailable
historical `BIOS[]` selection: selector `0x50` maps to the original OpenNT
owner identity `MS_bop_0`.

It is an identity result only. It contains no C function pointer, does not
decode guest instruction bytes, does not read the following DEM service byte,
does not call OpenNT, and cannot mutate guest or CPU state.

## Source evidence

`src/opennt/base/mvdm/softpc.new/base/bios/bios.c` initializes the `NTVDM`
section with `MS_bop_0` at `BOP 50 - MS reserved`. The adjacent `0x51` and
later entries prove why this record is deliberately a table-shaped selection
record rather than a synthetic general rule.

## Admission boundary

On 2026-08-10, the project owner authorized recovering this missing selection
mapping in `src/bx-ntvdm-adapter`, using the historical BIOS-array form as
evidence. The admitted content is only:

| Selector | Returned owner identity | Status |
| --- | --- | --- |
| `0x50` | `OPENNT_MS_BOP_0` | admitted DEM family |
| `0x54` | `OPENNT_MS_BOP_4` | admitted COMMAND family |
| every other value | `NONE` | explicitly refused |

This is not a reconstruction of `BIOS[]`: no 256-entry table, fallback,
device entry, selector beyond the declared profile, service dispatcher, or
function invocation is present. The existing generic-`Video` closure remains
absent and is not claimed to be solved.

## Next prerequisite

Before this identity can be used, a separately reviewed generic Bochs-to-
adapter instruction-byte inspection contract must identify an exact admitted
guest boundary. Only then can an isolated original-owner transaction be
considered. `MS_bop_0`, its service-byte read, `DemDispatch`, IP movement,
and the full DEM/state/memory closure remain OpenNT-owned unresolved work.
