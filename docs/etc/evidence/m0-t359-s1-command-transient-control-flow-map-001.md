# M0 T359 S1 — original COMMAND transient control-flow map

## Fixed facts

T358's frozen CPU40/x86 observation reached original `54:0F`, then the
original `illegal_op_int` path reported `03F4:2171` with bytes
`63 69 66 69 65`.  The staged and mirrored `COMMAND.COM` files are byte
identical (`908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43`).
For a COM image that instruction pointer is file offset `0x2071`; the bytes
are immutable COMMAND help text.  This is not a changed guest image and it
precedes `54:01`.

## CCPU/SAS disposition

The selected formal graph contains the original CPU40 `c_main.c`, `call.c`,
`ret.c`, `iret.c`, `jmp.c`, `c_intr.c`, `c_stack.c`, `c_xfer.c`, `c_seg.c`
and `c_addr.c`.  The latter eight transfer owners are byte-identical to the
OpenNT mirror baseline.  `c_main.c` differs only in declarations/includes,
an `IU32` instruction-length declaration, and a corrected `setjmp(*ptr)`
call; its BOP path still performs `UPDATE_INTEL_IP(p)` before `bop(...)`.
The observed successful BOP return is therefore evidence for that selected
original contract, not a reason to alter it.

`illegal_op_int` in original `base/system/illegalp.c` obtains CS:IP from the
INT 6 frame, reads the five bytes via SAS, formats the error, and invokes the
original modal host error path.  It reports the consequence of the bad guest
transfer; it is not its producer.

The original `btrace.c` package is not an admissible production diagnostic:
its complete body is compiled only under `#ifndef PROD`, and its
`back_trace_flags` are set from interactive YODA.  The formal graph selects
original `trace.c`, but no production startup edge activates a per-instruction
trace.  T359 therefore does not enable YODA or invent a replacement CPU trace.

## Original COMMAND owner map

The retained original `command.map` identifies a resident `RESGROUP` and a
movable `TRANGROUP`.  Original source defines the required full lifecycle:

1. `rdata.asm::EndInit` allocates a largest temporary NTDOS block, calculates
   `TrnSeg`, reverse-copies the transient range, and frees the temporary block.
2. `command2.asm::{LodCom,LodCom1,ChkSum,LoadCom,ReadCom}` reacquires or
   relocates that block and verifies/reloads the transient through normal DOS
   open/seek/read/close semantics.
3. `command2.asm::HavCom` copies `DATARES:TranVars` to
   `TrnSeg:TRANGROUP:HeadCall`; `rdata.asm` initializes its first far pointer
   to `HeadFix_Trap`.
4. `tcode.asm` enters transient code with the `HeadCall` far call; the target
   must be a resident `THeadFix`/`HeadFix` bridge, never a help-text address.

This is corroborated by the retained source/map evidence
`t228-s1-command-transient-owner-package-map-001.md` and
`t228-s1-command-headcall-contract-map-001.md`.  Those records already prove
that a COMMAND data address cannot satisfy the original `HeadCall` contract,
and that generic reverse string copy and ordinary indirect far-call mechanics
are not themselves the permitted repair target.

## S1 decision

The first bad transfer belongs to the **complete original COMMAND transient
allocation/move/reload/TranVars lifecycle with its original NTDOS allocation
and file-service prerequisites**.  It is neither an omitted CPU40 source
file, nor a DEM/COMMAND BOP leaf, nor a SAS image-identity problem.

S2 is therefore bounded to that owner package: compare the currently selected
original NTDOS allocation and `50:12/00/16/02` file lifecycle bindings against
the original call/return/error contracts, restore any missing same-shaped
binding, and validate the package locally before a single renewed frozen run.
It must not seed a transient address, patch `HeadCall`, copy COMMAND bytes
outside original guest flow, or change a CCPU transfer instruction.
