# M0 T359 S2 P2 — COMMAND transient owner/binding closure

## Question

The fixed T358 execution enters immutable `COMMAND.COM` text before `54:01`.
S2 must determine whether a missing host-side binding—not a CPU transfer change
or a fabricated BOP result—can explain the original resident-to-transient
lifecycle.

## Original normal route

1. `mvdm-guest/dos/v86/cmd/command/rdata.asm::EndInit` resizes resident
   COMMAND, allocates the largest normal DOS block, computes `TrnSeg`, copies
   the transient in reverse, frees the temporary allocation, and jumps to
   `LodCom_Trap`.
2. `command2.asm::{LodCom,LodCom1}` reacquires the transient block, calls
   `HeadFix`, checks the transient checksum and, on success, enters `HavCom`.
3. `HavCom` copies `DATARES:TranVars` to
   `TrnSeg:TRANGROUP:HeadCall` and `TJmp` enters the transient. The original
   first entry is the resident `HeadFix_Trap` continuation; a help-text address
   is not a valid outcome.
4. When the checksum fails, `LoadCom/ReadCom` use original DOS `OPEN`,
   `LSEEK`, `READ` and `CLOSE` to reload the transient from `ComSpec`.

The selected original NTDOS call sites emit their historical SVC values:
`mvdm-guest/dos/v86/doskrnl/dos/file.asm` emits `SVC_DEMOPEN (50:12)`;
`handle.asm` emits `SVC_DEMCHGFILEPTR (50:00)`, `SVC_DEMREAD (50:16)` and
`SVC_DEMCLOSE (50:02)`. `mvdm-host/inc/dossvc.h` and `DOSSVC.INC` define the
same values.

## Original host binding

The selected original host route is complete at this immediate boundary:

- `softpc.new/host/src/nt_bop.c::MS_bop_0` reads the original guest service
  byte, calls `DemDispatch`, then performs the original one-byte IP advance.
- `dos/dem/demdisp.c::DemDispatch` dispatches the value through original
  `apfnSVC` entries.
- The required entries are the original `demChgFilePtr`, `demClose`,
  `demOpen` and `demRead` bodies in `dos/dem/{demfile.c,demhndl.c}`.

The fresh CPU40/x86 formal product build selects these source sets and links
them with the original COMMAND, CCPU and host-root cohorts. No missing
same-shaped `50:00/02/12/16` dispatch or NTDOS prerequisite is present in the
selected source/link closure.

## Original failure direction

- Allocation failure in `EndInit` branches to original `Alloc_error`; failed
  reallocation or insufficient usable transient memory in `LodCom` branches to
  original `BadMemErr`.
- Failed open, seek, short read or close during reload remains on original
  `WrongCom`/`LoadCom` retry or fatal paths. It is not converted to a success
  by S2.
- `demOpen`, `demRead`, `demChgFilePtr` and `demClose` retain their original
  carry/error return ownership. S2 adds no result rewriting.

## Decision

Static recovery finds no unbound immediate NTDOS/DEM edge that can be repaired
without violating the original failure direction. The only permitted S2 code
is the generic post-store observer recorded in
`m0-t359-s2-command-transient-sas-write-binding-001.md`; it can distinguish
whether the original lifecycle writes the control destination during the
single later frozen run. The runtime question therefore transfers to S3/S4:
first prove the observer's local contract and preserve the formal link, then
perform exactly one immutable resumed observation. No COMMAND, NTDOS, DEM,
CPU or guest-media semantic replacement is admitted.
