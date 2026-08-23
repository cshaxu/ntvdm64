# M0 T242 S1 NTDOS EXEC And Parent-Return Source/ABI Audit

## Question

Where does the original DOS child-to-parent lifecycle actually live, and what
must be true before one source-built child can exercise it without turning
`bx-vdm` into a DOS process emulator?

## Inputs

- `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msproc.asm` (`$Exec`, `$Exit`,
  `$Abort`).
- `misc.asm`, `alloc.asm`, `dup.asm`, `handle.asm`, `pdb.inc`, `msdisp.asm`,
  `mssw.asm`, `exepatch.asm` and their direct call edges.
- `src/opennt/base/mvdm/dos/v86/cmd/command/{init.asm,tcode.asm}` and original
  host `dos/command/cmdexec.c`.
- Existing source-build/staging scripts and their recorded byte identities.
- The current `bx-vdm` COMMAND/DEM mirrors and finite selector-blind machine
  execution boundary.

## Observations

1. `msproc.asm:$Exec` is guest NTDOS code.  It validates `AL` (`0`, `1`, `3`,
   `5`), classifies the target, allocates/loads a child, calls
   `$CREATE_PROCESS_DATA_BLOCK`, saves the parent termination state, calls
   `SVC_DEMENTRYDOSAPP`, sets original segment/stack registers, and transfers
   to the child with `retf`.
2. `$CREATE_PROCESS_DATA_BLOCK` copies the fixed `Process_data_block`,
   duplicates inheritable JFNs via `DOS_DUP`, records the parent PDB and makes
   the child current. `pdb.inc` says that this public layout cannot be
   rearranged.
3. `$Exit/$Abort` record the exit type/code, restore termination, Ctrl-C and
   fatal-error vectors, free/reset source-owned process state, then transfer to
   the saved termination address. The debugger-only symbol-free SVC is an
   optional branch, not a prerequisite for a non-debug child profile.
4. COMMAND's `54:08/0A/0B` host side is a different boundary: it starts or
   completes a host child. It does not create a DOS PSP, free a DOS arena or
   restore a DOS parent. `54:0B` therefore cannot alone prove ordinary DOS
   child return.
5. The exact source-built `NTIO.SYS`, `NTDOS.SYS` and `COMMAND.COM` build and
   staging identities already exist. The current bounded native product run
   stops before any guest BOP observation, so it supplies no EXEC continuity
   claim.

## Owner Decision

The original guest bytes own DOS EXEC, PSP/PDB, arena, JFN, environment and
parent restoration. They must be recovered by running the staged original
guest image. `bx-vdm` may only serve already-defined SVC/BOP ingress and
egress; it must not allocate a second PSP, maintain a parallel JFN table, or
emulate the exit transfer. `bx-core`/`bx-mantle` retain real-mode execution,
RAM and stop/resume mechanics with no DOS vocabulary.

## Declared First Profile

One local DOS executable through `INT 21h/AH=4Bh, AL=0`, using original
NTDOS/COMMAND bytes, no remote JFN, no overlay, no HMA/A20-off branch, no
debug symbol support and no unknown-binary (`SVC_CMDCHECKBINARY`) route.
Success requires original child PDB creation, a child termination through the
original `$Exit/$Abort` path, restoration of the original parent PDB/vector
state, and return to the original parent continuation. This is deliberately
not a 32-bit host-child `cmdExec32` test.

## Ordered Dependencies

The live tracker now expands the former broad P8 bucket into
`BOP-DEPENDENCY-092..101`: staged image identity; machine execution; EXEC
classification; PDB, arena and JFN state; exit restoration; SVC ingress;
existing DEM/COMMAND endpoints; and the explicitly deferred debugger branch.
Remote JFNs transfer to Redirector, debugger symbols to VDD/debug, and
HMA/A20-off success to the selector-blind machine package.

## Confidence And Follow-Up

High confidence for ownership and source path: the code is directly present
in the OpenNT guest kernel and its local call/transfer sequence is explicit.
Runtime reachability is unproven. S2 may now create a source-to-image and
guest-state observation fixture; it may not implement DOS semantics in a host
provider.
