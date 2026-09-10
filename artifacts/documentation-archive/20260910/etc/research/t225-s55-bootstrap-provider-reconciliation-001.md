# M0 T225 S55 — Guest Bootstrap Provider Reconciliation 001

## Question

Before attributing the current bounded native run to `$Exec`, child teardown,
or a machine defect, does the complete original startup path through
`DosInit`, COMMAND bootstrap, and the first configuration-file open already
have an owner-qualified current route? In particular, is an unimplemented
BOP in that prefix an admissible repair candidate?

## Inputs and method

This is a source/current-code reconciliation. It uses no new observer,
native run, guest-memory inspection, or runtime change.

- Original startup flow: `src/opennt/base/mvdm/dos/v86/doskrnl/bios/sysinit1.asm`
  and `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msinit.asm`.
- Original DEM and COMMAND dispatch tables:
  `src/opennt/base/mvdm/dos/dem/demdisp.c`,
  `src/opennt/base/mvdm/inc/dossvc.h`, and
  `src/opennt/base/mvdm/dos/command/cmddisp.c`.
- Current owners: `src/bx-vdm/bx_ntvdm_dem_package_session_v1.c`,
  `bx_ntvdm_dem_drive_view_provider_v1.c`,
  `bx_ntvdm_dem_dta_service.c`,
  `bx_ntvdm_dem_error_lock_plane_v1.c`,
  `bx_ntvdm_dem_misc_plane_v1.c`,
  `bx_ntvdm_command_package_session_v1.c`, and the declared boot namespace.
- Existing bounded source-built transcript:
  `t225-s33-bootstrap-continuity-observation-admission-map-001.md`,
  `t225-s18-reached-bop-owner-package-disposition-map-001.md`, and the
  completed T225 S19/S20 package regressions.

## Original-to-current bootstrap ledger

| Stage | Original operation and required result | Bound current owner | Current disposition |
| --- | --- | --- | --- |
| `sysinit` entry | `50:11 demLoadDos`, then original far transition into relocated `sysinit` | DEM package / checked NTDOS image write | Source-derived immutable NTDOS load and checked `+4` resume. |
| Conventional memory | selector `12h` from `goinit` | machine-composition package | Fixed admitted machine-profile result; not a general BIOS implementation. |
| `DosInit` begins | `50:0F demGetDrives`; `AL` becomes `numio` | DEM GSET/drive-view provider | Captured admitted-drive inventory in both Direct and Readonly. |
| DOS/host locator registration | `50:1B demSetDTALocation` stores DMA/PDB/error/SFT locations | DEM package session | Checked fixed-width guest-read and copied registration; it does not retain a guest pointer. |
| Hard-error locator registration | `50:32 demSetHardErrorInfo` records VHE and device-chain locations | DEM error/lock plane | Checked copied registration. It does not claim an INT 24 policy. |
| Debug-only symbol calls | `50:45 demSystemSymbolOp` only under debugger branch | DEM misc plane | Original no-debug/no-op continuation. |
| Device-chain completion | `50:46 demGetDPBList` after `charinit` has completed the original chain loop | DEM GSET/DPB provider | Captured-drive DPB transaction with explicit failure behavior. |
| COMMAND/DOS coordination | `54:05 cmdSetInfo` registers SCS/DOSDATA locators | COMMAND bootstrap provider | Checked copied registration; no historical CCPU pointer is retained. |
| Config file selection | `54:0C cmdGetConfigSys` returns a DOS pathname, after which guest DOS opens it | COMMAND configuration provider plus boot namespace | Source-derived pathname write points at the declared `CONFIG.SYS`; the same namespace owns the materialized immutable bytes used by subsequent DEM open/read/close. |
| AUTOEXEC counterpart | `54:0D cmdGetAutoexecBat` | Same COMMAND/namespace package | Same paired declared-path and immutable-content contract. |

The existing 33-record source-built transcript gives bounded reach evidence for
the significant guest transitions: `50:11`, `12`, `50:0F`, `50:46`, `54:05`,
and later `54:0C`. In particular, `50:46` is after the original console and
device-chain `charinit` loop. It proves passage through `continit`, not the
dynamic value of every device-header pointer.

## Source and failure comparison

`demdisp.c` dispatches every listed DEM identity through the ordered
`apfnSVC` table. `cmddisp.c` assigns `54:05`, `54:0C`, and `54:0D` to
`cmdSetInfo`, `cmdGetConfigSys`, and `cmdGetAutoexecBat`, respectively.
Those original translation units cannot directly compose in the modern x64
CLI because they require CCPU/SAS guest pointers, historical VDM process
state, and NT host composition. The current routes are therefore
source-derived rung-2 seams, not adapter-owned DOS behavior.

The key downstream condition is preserved: original `cmdGetConfigSys`
returns a pathname and expects ordinary DOS file I/O next. Current
`bx_ntvdm_cmd_boot_file_service_v1_prepare` returns the declared
`X:\CONFIG.SYS` or `X:\AUTOEXEC.BAT` pathname only after validating the
real-mode BOP contract. `bx_ntvdm_boot_namespace_provider_v1_bind_startup_configuration`
binds the corresponding immutable generated contents into that same declared
namespace. A pathname without reachable contents would be a package defect;
the current provider explicitly rejects such an unready binding.

## Result

No bootstrap BOP in this complete prefix is an unowned fall-through or a
justified leaf repair:

- the source-built run reaches `DosInit` and reaches `continit` after the
  original device-chain loop;
- all reached registrations have a bounded owner and retained source-shaped
  role;
- the reached COMMAND configuration route has a paired namespace-content
  owner for the next ordinary DOS open; and
- Direct/Readonly select the same bootstrap semantics, so their byte-identical
  budget terminal does not point to host mutation policy.

Consequently the current `BOUND` diagnostic and later generic limit messages
cannot be assigned to an omitted startup BOP, synthetic device header, missing
CONFIG/AUTOEXEC materialization, or adapter-owned child transfer. Adding any
of those would violate the recovery ladder.

## S55 consequence

The remaining S55 work is the guest-owned lifecycle after this reconciled
bootstrap package: original NTDOS `$Exec`, child entry, teardown, and
COMMAND parent return. The next analysis must compare their source
preconditions with current package state as a whole; it must not open a new
single-service task from the retained native trace.

