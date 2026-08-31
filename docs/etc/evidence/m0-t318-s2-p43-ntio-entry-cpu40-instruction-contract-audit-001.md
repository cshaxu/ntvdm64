# M0 T318 S2 P43 — NTIO entry CPU40 instruction-contract audit

## Purpose

Determine whether the selected original CPU40 execution body can execute the
complete source-defined instruction prefix of the selected `NTIO.SYS` entry
before its first host-service crossing.  This is a static owner audit; it does
not add a decoder hook, a guest observer, or a second runtime observation.

## Original guest prefix

`mvdm-guest/dos/v86/doskrnl/bios/msinit.asm::init` is the source mapped to the
selected `NTIO.SYS` entry `0000:0370`.  Before `SVC SVC_DEMLOADDOS`, it:

1. executes the original `FCLI`/`FSTI` macros;
2. saves ROM IVT entries with segment-register moves, `LODSW` and `STOSW`;
3. installs the original `INT 15h` and `INT 19h` vectors;
4. establishes real-mode `SS:SP = 0000:0700`; and
5. prepares BIOS/DOS data before the `0x50:0x11` BOP.

`FCLI` and `FSTI` are not simplified project instructions.  The original
`VINT.INC` macro bodies use `PUSH`/`POP`, segment moves, `LAHF`/`SAHF`,
`TEST`, conditional short branches, `LOCK AND`/`LOCK OR`, and their ordinary
`CLI`/`STI` fallback.  They therefore exercise the original NTVDM virtual
interrupt-state protocol at `40h:314h` before DEM is entered.

## CPU40 contract check

The selected original `softpc.new/base/ccpu386/c_main.c` decoder includes the
required opcode families: segment `PUSH`/`POP`, `XOR`, `AND`, `OR`, segment
`MOV`, `XCHG`, `LODS`, `STOS`, `LAHF`, `SAHF`, shifts, `LOOP`, conditional and
unconditional short branches, `LOCK`, `CLI`, `STI`, and BOP `0xD6`.

Its source-defined first fetch remains `c_cpu_simulate -> ccpu(FALSE) ->
SETUP_HOST_IP`, backed by original contiguous `CCPU_M`.  The IVT and local
stack accesses are ordinary internal SAS RAM accesses; they do not require an
external physical-page mapping, BaseSrv/CSR, console policy, COMMAND, or an
adapter-defined BOP route.  The existing mapping-manager hooks remain only
for explicitly registered external physical-page aliases.

## Disposition

No absent CPU40 opcode, virtual-interrupt provider, or first-prefix memory
binding is found.  Consequently no instruction-level patch, mapper change,
or rebuild is justified.  The first host crossing remains the already selected
original `0x50:0x11 -> demLoadDos` contract.  A future fixed-container run is
still permitted only after a genuine original-owner source correction creates
a new product EXE; this audit does not make a repeat observation admissible.
