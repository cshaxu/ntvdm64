# T222 S1 P1 — DEM drive-view/CWD/GSET source, ABI and failure ledger

## Question

What one original-owner package must replace the current mix of snapshot,
boot-fixture and deferred helpers for DEM drive state, CWD, clock, DTA, DPB,
media and identity services in Direct and Readonly profiles?

## Source boundary and recovery decision

The authoritative order is `src/opennt/base/mvdm/dos/dem/demdisp.c::apfnSVC`.
The handlers below are principally in `demgset.c` and `demdir.c`.  Their
translation units cannot link directly into the modern process: register
macros, `GetVDMAddr`, DOS structures and `demClientError` bind them to the
legacy CCPU/SAS/SoftPC host composition.  The mandated recovery rung is
therefore **original+smallest typed adapter seam**: preserve each handler's
register/layout/order/failure rule while replacing raw guest pointers with
checked copied transactions and host calls with a shared drive-view owner.
No Bochs intrusion or authored replacement semantics is admitted by this
ledger.

## Complete owner/ABI/failure map

| BOP | Original handler/source | Historical ABI and result | Relevant host contract | Direct / Readonly disposition | Current route and required action |
| --- | --- | --- | --- | --- | --- |
| `50:0D` | `demGetBootDrive`, `demgset.c` | no input; `AL=1`-based boot drive; registry failure/non-fixed result falls back to C | `RegOpenKeyEx`/`RegQueryValueEx` read `BOOTDRIVE_PATH`; `GetDriveType` | selected admitted fixed drive; readonly has same identity view | `dem_boot_drive_service` has fallback/snapshot variants; migrate to shared selected-drive result, remove fixture-only decision. |
| `50:0E` | `demGetDriveFreeSpace`, `demgset.c` | `AL` drive; success `AL/BX/CX/DX/SI`, failure `CF,AX` through `demClientError` | volume geometry/free-space query, BDS media refresh | selected admitted volume read in both profiles | `dem_volume_provider_v1` is a snapshot-only leaf; migrate into provider and preserve invalid-drive/error distinction. |
| `50:0F` | `demGetDrives`, `demgset.c` | no input; `AX=nDrives`, `CF=0`; initializes physical drive types | `GetDriveType`; historical `NtOpenFile`/`NtQueryVolumeInformationFile` only classify physical media | selected-drive list in both profiles | `dem_drive_service` is reusable algorithm evidence but currently a deferred leaf; migrate under provider. |
| `50:10` | `demGSetMediaID`, `demgset.c` | `BL` drive, `AL=0` get only; writes `VOLINFO` at `DS:DX`; set is `CF=1` | `GetVolumeInformationOem` | read volume identity in both; set stays original unsupported | current media helper/snapshot must be mapped to checked `VOLINFO` write; no synthetic success. |
| `50:13` | `demQueryCurrentDir`, `demdir.c` | `AL` drive; validates/writes CDS at `DS:SI`; invalid media uses `demClientError`; invalid path resets CDS/root | directory attribute query; per-drive `=X:` environment state | Direct reconciles actual selected-host directory; Readonly maintains its session view without content mutation | `dem_cwd_service_v2` has a source-shaped checked CDS transaction but is not yet the shared profile owner; migrate it. |
| `50:14` | `demQueryDate`, `demgset.c` | returns weekday in `AL`, year `CX`, month/day `DX` | `GetLocalTime` | normal host observation in both | `dem_clock_service_v1` groups it with setters; split/retain query result contract under provider. |
| `50:15` | `demQueryTime`, `demgset.c` | returns time in `CH/CL/DH/DL` | `GetLocalTime` | normal host observation in both | same current clock leaf; retain hundredths layout. |
| `50:18` | `demSetCurrentDir`, `demdir.c` | path at `DS:DX`; `CF` and source error; updates `=X:` environment on success | `SetCurrentDirectoryOem`, `SetEnvironmentVariableOem` | Direct updates scoped CLI/VDM session CWD with host validation; Readonly updates only its selected immutable-session view | current CWD context deliberately avoids host process state; replace its synthetic scope decision with explicit session capability, not ambient fallback. |
| `50:19` | `demSetDate`, `demgset.c` | `CX,DH,DL`; `AL=0` when `SetLocalTime` succeeds **or** privilege is absent, else `AL=FF` | `SetLocalTime`, privilege-sensitive | Direct may invoke with caller permission and preserves the special privilege result; Readonly must have explicit non-mutating source-derived disposition | `dem_clock_service_v1` always rejects; replace only after an explicit clock capability decision, never fake ordinary success. |
| `50:1A` | `demSetDefaultDrive`, `demgset.c` | `DL` drive and `DS:SI` full CWD; mismatched letter gives `CF,AX=1`; otherwise current-directory source errors | `SetCurrentDirectoryOem` | Direct updates scoped session drive/CWD; Readonly validates and changes its profile session only | current CWD context has checked input but no shared drive-view admission; migrate with `18`. |
| `50:1B` | `demSetDTALocation`, `demgset.c` | stores four guest locators from `DS:AX/DX/CX/SI` (last via DOSWOW SFT offset); no result | CCPU `GetVDMAddr` only | checked guest registration, profile-independent | current `dem_dta_service` is the correct fixed-width seam; retain, attach to provider lifecycle. |
| `50:1C` | `demSetTime`, `demgset.c` | `CH/CL/DH/DL`; same `AL=0` privilege-not-held exception as date | `SetLocalTime`, privilege-sensitive | same clock policy as `19` | current clock leaf rejects; migrate with `19` as one clock setter decision. |
| `50:25` | `demGetDPB`, `demgset.c` | `AL` drive, writes `DPB` at `DS:DI`; success `AX=0,CF=0`; no disk `CF=1`; ordinary error via `demClientError` | volume geometry/BPB/BDS | selected-volume DOS DPB in both | `dem_full_dpb_service` is a fake/no-BDS slice; migrate to one full drive-view DPB constructor. |
| `50:41` | `demGetComputerName`, `demgset.c` | writes 16 bytes at `DS:DX`; success sets `CX=01FF`; failure writes NUL and clears `CH` | `GetComputerNameOem` | normal host identity observation in both, subject to profile identity policy | current `dem_computer_name_service` is fallback-only; migrate to checked 16-byte provider action. |
| `50:46` | `demGetDPBList`, `demgset.c` | list/count/result continuation uses DOS DPB layout and guest state | repeated `demGetDpbI` over drive inventory | selected-drive list in both | current adapter/runtime path is snapshot-dependent and incomplete; recover as one DPB-family implementation with `25`, not a new leaf. |

## Current-code consolidation finding

The current code already contains useful contracts in
`dem_cwd_service_v2`, `dem_dta_service`, `dem_volume_provider_v1`,
`dem_drive_service`, `dem_full_dpb_service_v1`, `dem_gset_plane_v1` and the
host drive/volume snapshot types.  They are not a complete provider because
they use mixed deferred selection, fixture defaults, partial synthetic CWD or
incomplete DPB state.  T222 must place the reusable typed algorithms behind
one drive-view/session authority and replace—not layer another selector
recognizer over—those routes.

## API availability and blockers

All ordinary filesystem, directory, registry-read, environment and computer
name calls above remain callable as normal user-mode Win32 capabilities.
`SetLocalTime` is callable but privilege-sensitive; its historical special
`ERROR_PRIVILEGE_NOT_HELD` result is a contract fact, not an unavailable API.
The CCPU/SAS address/register APIs and raw `GetVDMAddr` are unavailable in the
modern process; fixed-width checked guest actions are the required replacement.
No unavailable API blocks the audit of another service.

## Follow-up

S1 P2 must derive one Direct/Readonly drive-view session ABI and grouped
implementation plan from this ledger.  It must explicitly decide scoped CWD
and clock-set semantics before source changes; it may not promote an existing
helper or a trace hit into a completed service.
