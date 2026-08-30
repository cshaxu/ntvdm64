# M0 T310 S21 P3 — storage-controller formal assembly

## Inputs

- selected source: the ten original `softpc.new/base/disks` manifest members
  and the original `base/dos/emm_mngr.c` controller-side EMS owner;
- profile: Win32/x86, `/MT`, `CPU_40_STYLE + CCPU + C_VID`;
- disposable build root:
  `build/M0-T310/S21/formal-storage-x86`.

## Procedure and result

`New-T310OriginalSoftpcNinja.ps1` generated the formal graph. Ninja rebuilt
all ten selected disk translation units and produced `original-softpc-disks.lib`.
It then completed the complete 358-edge forced-link candidate.

The first forced-link audit exposed `_host_delays` from `fla.obj` and
`fdisk.obj`. Source comparison showed that the prior CCPU collision guard in
`nt_cpu.c` had incorrectly grouped the original independent controller timing
state with the two duplicate SAS-memory globals. `host_delays` was restored
to its original every-profile definition, while only `Start_of_M_area` and
`Length_of_M_area` remain under `MVDM-HOST-DIV-030`.

The incremental formal rebuild compiled `nt_cpu.c`, rebuilt the original host
archive and completed the forced link. Its resulting log contains 121
unresolved records for later owner packages and **zero** rows from
`original-softpc-disks.lib`; `_host_delays` is absent. The one textually
floppy-named unresolved record is `original-mvdm-dem.lib(demdasd.obj)`'s
`OutputDebugStringOem`, which remains a DEM/Win32 owner edge, not a storage
controller edge.

## Interpretation

The selected original FDC, WD2010, BIOS-disk, remote-floppy client and EMS
controller sources now formally assemble under the selected CCPU40 profile
without an S21-local adapter or replacement algorithm. Existing K&R,
default-int and fixed hardware-width diagnostics remain visible as original
source evidence. Real media selection/I/O, write policy and remote transport
are intentionally not claimed here: they are S22, followed by S49 integrated
machine evidence.
