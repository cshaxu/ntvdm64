# T220 S1: DEM process-resource ownership map

## Question

What is the original OpenNT owner, state layout, ordering and failure contract
for host resources associated with a guest DOS process—and what is the first
permissible modern seam after T219 disproved the proposed `50:36`-derived PDB
token?

## Inputs and procedure

This is a static source audit only.  It reviewed the local OpenNT sources:

- `base/mvdm/inc/DOSSVC.INC` and `dossvc.h`;
- `base/mvdm/dos/v86/doskrnl/dos/msproc.asm` and `msctrlc.asm`;
- `base/mvdm/dos/dem/demmisc.c`, `demsrch.c`, `demgset.c`, `demfile.c`,
  `demdata.c`, `dem.h`, and `demdisp.c`; and
- `base/mvdm/softpc.new/host/src/nt_msscs.c`.

It compared those facts with the current `bx_ntvdm_dem_file_session_v1`,
`bx_ntvdm_dem_whole_provider_v1`, and
`bx_ntvdm_dem_session_lifecycle_provider_v1` declarations.  No guest memory
was read and no runtime behavior was changed.

## Original control flow and ownership

| Phase | Original source fact | Consequence |
| --- | --- | --- |
| DOS entry | `msproc.asm` performs `SVC SVC_DEMENTRYDOSAPP` immediately before `DOCLI`, stack/segment setup and the far transfer to the user image. `DOSSVC.INC` assigns this service `36h`. | `50:36` is an entry notification.  Its `DX` PDB is useful to VDD create hooks, but is not a complete resource-owner registry. |
| Entry dispatcher | `demmisc.c::demEntryDosApp` reads `DX`, and only calls `VDDCreateUserHook(PDB)` when `!IsFirstCall`; otherwise it returns. | No file, JFT, SFT, search or generic host resource is created by this selector. |
| Current-PDB publication | `demgset.c::demSetDTALocation` maps the guest `DS:DX` address to `pusCurrentPDB`; it also maps the DTA variable, extended error record, DOS/WOW data and SFT head. | The historic DEM process model is rooted in DOS-owned, live guest structures, not an adapter-maintained child token. |
| Handle allocation | `demfile.c::VDDAllocateDosHandle` defaults a null PDB to `*pusCurrentPDB << 16`, maps the PDB and `PDB_JFN_Pointer` with `Sim32GetVDMPointer`, finds a free JFT slot, reserves an SFT entry, and only then returns the DOS handle. | Handle ownership requires the PDB→JFT→SFT relationship and its ordering; a host token alone cannot preserve the historical rule. |
| Handle lookup/release | `VDDRetrieveNtHandle` validates JFT bounds and unused (`FFh`) state, walks the SFT chain, rejects a zero reference count, and returns the SFT NT handle. `VDDReleaseDosHandle` calls it, marks the JFT entry `FFh`, then decrements `SFT_Ref_Count`. | The required failure and release order is specific.  A generic `CloseHandle` or a global token sweep is not an equivalent provider. |
| DOS process termination | `msctrlc.asm` calls `SVC SVC_PDBTERMINATE` only after it proves a real child removal (`parent != current`, `current == this`, and not TSR/keep-process); it passes the terminating PDB in `BX`, then frees the arena and performs `DOS_ABORT`. | `50:3C` is part of a guarded guest termination sequence.  It must not run for parent return, mismatch, or TSR. |
| DEM termination | `demsrch.c::demTerminatePDB` reads `BX`, conditionally calls `VDDTerminateUserHook`, calls `HostTerminatePDB`, frees that PSP's FindFirst/FindNext list, removes the PSP list record, and returns `VOID`. | Search state is only one part of the historical lifecycle.  VDD/device hooks precede host/device cleanup, which precedes search-list release. |
| Historical host cleanup | `nt_msscs.c::HostTerminatePDB` calls `FloppyTerminatePDB` then `FdiskTerminatePDB`; VDD hooks enumerate registered callbacks. | These are old product-composition/device concerns. They cannot silently become bx-vdm file-session behavior. |

## Current-source comparison

| Current state holder | Missing or divergent ownership fact | Required disposition |
| --- | --- | --- |
| `bx_ntvdm_dem_file_session_v1` | A slot has only Win32 `HANDLE`, generation and in-use state; it has no PDB, JFT/SFT identity or guest lifecycle relation. | Do not extend it with an inferred `50:36` owner.  It needs a source-derived owner contract after a separately admitted, checked guest-PDB access seam. |
| `bx_ntvdm_dem_whole_provider_v1` | It owns file/search state and a matching-path DTA continuation, but no process identity or PDB/JFT/SFT mapping. | Its resources cannot truthfully be released by the current `50:3C` provider. |
| `bx_ntvdm_dem_session_lifecycle_provider_v1` | It reads `BX` but releases only `boot_namespace_provider` slots. | Retain only as finite/fixture behavior; label it stale for direct host files/searches.  Do not broaden it before the process owner is recovered. |
| shared session host-context/mutation profile | It owns drive/CWD policy but not DOS process lifetime. | It must supply profile policy to the eventual resource owner; it must not be repurposed as a PDB/JFT/SFT substitute. |

## Source-recovery decision

| Recovery rung | Result |
| --- | --- |
| 1. Direct original source | Not composable in the modern x64 runner. `demfile.c` depends on historical CCPU/SAS address translation (`Sim32GetVDMPointer`), guest flat pointers/layouts, 32-bit `HANDLE` storage in `DOSSFT`, historical DEM globals and the old host/VDD product composition. |
| 2. Smallest contract-preserving seam | Candidate, not yet admitted for implementation: a bounded, versioned guest-DOS-process snapshot/read contract that validates the exact PDB/JFT/SFT fields needed by the proven original algorithms, returns copied values only, and lets the DEM owner decide source-shaped rejection.  It cannot expose raw guest pointers or give the adapter a generic PSP inspector. |
| 3. External-code intrusion | Not indicated. Bochs provides checked mechanics only; no Bochs change is required by this source map. |
| 4. New behavior | Not permitted at this point. A PDB-owned host token table, global cleanup sweep, or `DX`-only association would skip the original contract. |

## Profile and package boundary

The future owner is a DEM **process-resource** package.  It will consume the
shared direct/readonly/overlay/virtual policy only for the actual host resource
operation.  Process identity, guest PDB/JFT/SFT validation and termination
ordering remain DEM-owned.  COMMAND `54:0B` stays downstream: it is not an
alternative way to clean child resources.

## Result and follow-up

The unique next seam is now known, but it reaches the active packet's stop
condition: it requires a new checked guest-PDB read ABI.  T220 S1 therefore
does **not** implement it.  A later S must first specify the exact copied
read records, field bounds, lifecycle ownership, profile behavior and
positive/negative tests, then obtain admission before modifying bx-vdm.

Confidence is high for the source ownership/order facts.  Runtime reachability
and the exact first-profile guest PDB offsets remain untested intentionally.
