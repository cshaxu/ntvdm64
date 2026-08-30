# M0 T318 S1 — NTDOS EXEC / Parent-Return Contract Freeze

## Result

The declared DOS child lifecycle is an original guest-owned cohort.  Its
primary implementation is already present as an unmodified NTDOS mirror;
this record freezes the boundary before any recovery work.  It does not claim
that a guest child has yet executed continuously under the selected SoftPC
machine.

## Mirror identity

- `src/mvdm-guest/dos/v86/doskrnl/dos/msproc.asm` has no diff against
  `O:/repos.external/OpenNT/base/mvdm/dos/v86/doskrnl/dos/msproc.asm`.
- `src/mvdm-guest/dos/v86/inc/exe.inc` has no diff against its corresponding
  OpenNT source.  It supplies the original `EXEC0`, `EXEC1`, and `EXEC3`
  request/return layouts.
- The COMMAND return caller is the original
  `src/mvdm-guest/dos/v86/cmd/command/tcode.asm`; its `CMDSVC
  SVC_RETURNEXITCODE` sites are the guest side of the existing COMMAND
  `54:0B` contract.
- The staged guest artifacts are present in the mirror: `NTDOS.SYS` is
  27,858 bytes with SHA-256
  `957662320654ad5251c3a8b228a5dadec28aa65dddbcba38c3658a6e7f93bc84`, and
  `COMMAND.COM` is 50,384 bytes with SHA-256
  `908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43`.
  These are the exact identities asserted by the retained original-toolchain
  build recipes; S1 does not substitute either artifact.

## Original lifecycle map

1. `msproc.asm::$Exec` is DOS `INT 21h/AH=4Bh`.  It validates the requested
   program, opens and classifies it, allocates a DOS arena, builds the child
   PSP, duplicates JFNs, supplies the original environment/FCB/command-line
   data described by `exe.inc`, and records the parent terminate return.
2. At `msproc.asm:1299-1312`, `$Exec` writes the parent continuation into
   `PDB_Exit` and the DOS terminate vector.  At `exec_go` it keeps the
   original register contract: `DS:SI` entry, `AX:DI` stack, `DX` child PDB
   and `BX` initial AX.
3. `msproc.asm:1371` emits `SVC_DEMENTRYDOSAPP` immediately before its native
   `SS:SP` setup and far transfer to the child.  The service is the original
   DEM selector `0x36`; `demdisp.c` routes it to original
   `demmisc.c::demEntryDosApp`.
4. The normal DOS child exit is `msproc.asm::$Exit/$Abort`; it records
   `Exit_Code`, restores terminate vectors, and transfers to
   `msctrlc.asm::reset_environment`.
5. `reset_environment` preserves the original parent-return work: it obtains
   `PDB_Parent_PID`, invokes `SVC_PDBTERMINATE` (`0x3C`) to release search
   state, frees child arenas, performs `DOS_ABORT`, restores the parent PSP
   and user stack, then returns through the saved terminate address.
6. COMMAND’s `tcode.asm:1219` and `:1377` issue the original
   `CMDSVC SVC_RETURNEXITCODE` after a DOS program return.  The host mirror
   `mvdm-host/dos/command/cmdexec.c::cmdReturnExitCode` consumes DX/AL/BX:CX,
   queries the existing Base VDM path, and returns either CF re-entry or AL
   completion to that guest caller.

## Frozen prerequisite dispositions

| Prerequisite | Owner / disposition |
| --- | --- |
| NTDOS EXEC, PSP, arena, JFN, environment and parent restoration | Original `mvdm-guest/dos/v86` sources. Recover only by selecting/repairing the original guest build/image path; no app, session or host COMMAND substitute. |
| Initial NTDOS load | Original NTIO `SVC_DEMLOADDOS` (`0x11`) invokes `mvdm-host/dos/dem/demmisc.c::demLoadDos`, which resolves the established DOS location, obtains the existing VDM address, and performs the original 16 KiB read loop. App must not invent a second guest loader. |
| Guest entry transfer and child instruction execution | Selected original x86 `CPU_40_STYLE`/CCPU40 SoftPC. It must preserve the register/stack contract from step 2; this task does not invent a second executor. |
| `SVC_DEMENTRYDOSAPP` (`0x36`) | Original DEM `demEntryDosApp`; its VDD user-hook observation remains source-shaped and belongs to the VDD owner when nontrivial behavior is required. |
| `SVC_PDBTERMINATE` (`0x3C`) | Original DEM `demTerminatePDB`; it is a required process-cleanup service, not an optional trace event. |
| COMMAND `SVC_RETURNEXITCODE` / `54:0B` | Original COMMAND `cmdReturnExitCode`, local Base VDM/session result path closed by T317. This task may consume its documented CF/AL result but may not revise the broker. |
| DOS file/program image access and device behavior | Existing DEM/Redirector and original SoftPC device owners. A missing host-drive, redirection or device condition is an explicit owner transfer, not a reason to replace `$Exec`. |

## T318 S2 entry boundary

S2 may only recover a declared COM or MZ DOS child profile once the selected
guest image actually contains this unchanged cohort and the listed service
selectors are present in the selected source-shaped composition.  Failure,
overlay, TSR, redirector, PIF, WOW and 32-bit host-child branches remain
separate owner paths.  The first integration run is evidence only; it cannot
select a new BOP implementation.

## S1 closure

The required original source, artifact identities, service selectors and
machine/host owner dispositions are now frozen.  The only permitted S2 entry
is the existing `NTIO -> SVC_DEMLOADDOS -> NTDOS` path followed by one declared
original DOS child profile; no app-owned guest loader is admissible.
