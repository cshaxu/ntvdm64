# M0 T245 S1 — NTDOS EXEC and parent-return source map

## Question

Which original guest source owns the first local DOS child → parent-return
profile, where does it cross the already recovered COMMAND BOP plane, and
what prevents a source-built run from reaching that profile today?

## Inputs

- `dos/v86/doskrnl/dos/msdisp.asm`, `msproc.asm`, `misc.asm`, `alloc.asm`,
  `handle.asm`, `dup.asm`, `open.asm`, `file.asm`, `path.asm`, `mssw.asm`,
  `msdata.asm` and `pdb.inc`.
- `dos/v86/cmd/command/{init.asm,tcode.asm}`.
- Existing active mirrors for COMMAND `cmdexec.c` and the tracker rows
  `BOP-DEPENDENCY-080..106`.

## Original local profile

1. Source-built `NTDOS.SYS` receives `INT 21h/AH=4Bh, AL=0`; `msdisp.asm`
   dispatches to `msproc.asm:$Exec`.
2. `$Exec` classifies and loads a local DOS executable, allocates the guest
   arena, then calls `misc.asm:$Dup_PDB` / `$CREATE_PROCESS_DATA_BLOCK`.
   The latter copies the original PDB, JFN table, inherited vectors, parent
   PDB and environment; no host PDB/JFN object is permitted.
3. The local branch publishes `SVC_DEMENTRYDOSAPP (50:36)` then transfers via
   the original real-mode stack and `retf` to the child.  The unknown/Win32
   binary `SVC_CMDCHECKBINARY (54:07)` branch is excluded from this profile.
4. Child `INT 21h/AH=4Ch` reaches `$Exit/$Abort`, restores the saved vectors,
   frees the guest arena/environment, and transfers to the parent return
   address stored in the child PDB.
5. A shelled COMMAND path later emits `SVC_RETURNEXITCODE (54:0B)` from
   `tcode.asm`; imported `cmdReturnExitCode` retains its original
   carry/re-entry distinction.  It consumes guest result data but cannot
   recreate the preceding guest PSP/arena return.

## Current disposition

The guest source and image provenance are usable; the first profile is
`AL=0`, local inheritable JFNs, no HMA/A20-off, no overlay, no Redirector,
WOW, debugger or console-input expansion.  Existing `50:36` and
`54:08/0A/0B` routes are provider-local prerequisites, not proof of guest
reachability.

The present source-built startup path reaches `spcemm.asm:InitSpcEmm` after
the closed `5F` handoff.  It invokes SoftPC `BOP 66` / `emm_init`; `BX=0` is
the original unavailable outcome and causes the guest to skip `INT 67h`
installation.  This selector-blind EMS-unavailable branch is recorded as
`BOP-DEPENDENCY-107`; it belongs to the machine owner and is not a DOS EXEC
or adapter reimplementation.

## S2 boundary

S2 may create only a checked, typed observation fixture for the current
source-built NTDOS image: it must locate original `AH=4Bh/AL=0` entry and
observe guest-owned PDB/vector/parent bytes before and after transfer.  It
must not patch BOP 66, synthesize a PSP, emulate a DOS interrupt, or select a
trace-derived service repair.  If the current boot path cannot reach the
entry, the evidence transfers that predecessor to its named machine owner.
