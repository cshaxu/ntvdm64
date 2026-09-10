# M0 T318 S2 P12 — Original NTIO `0x50:0x11` to NTDOS dispatch map

## Question

Does the selected CPU40 product graph retain one original, source-shaped
dispatch path from the NTDOS bootstrap guest's DEM BOP through `demLoadDos`,
rather than an app-owned loader or a replacement BOP provider?

## Method

The selected original CPU40 sources and the generated x86/x64 Ninja graphs
were read without adding instrumentation:

- `softpc.new/base/ccpu386/c_main.c`;
- `softpc.new/base/bios/bios.c` and `base/inc/bios.h`;
- `softpc.new/host/src/nt_bop.c`;
- `dos/dem/demdisp.c`, `dos/dem/demmisc.c`, and `inc/dossvc.h`; and
- `build/M0-T318/S2/cpu40-v7vga-r3*/build.ninja`.

## Source chain

1. The selected original CCPU40 decoder handles BOP opcode `0xd6` in
   `c_main.c`: it advances the Intel IP and calls `bop(ops[0].sng)`.
2. `base/inc/bios.h` defines `bop(n)` as `(*BIOS[n])()`.  The original
   NTVDM-conditioned table in `base/bios/bios.c` binds BOP `0x50` to
   `MS_bop_0`.
3. `host/src/nt_bop.c::MS_bop_0` reads the service byte at guest `CS:IP` via
   `Sim32GetVDMPointer`, calls `DemDispatch(DemCmd)`, and advances the guest
   IP by that service byte.
4. `inc/dossvc.h` defines `SVC_DEMLOADDOS` as `0x11`; the original
   `dos/dem/demdisp.c` table selects `demLoadDos` at that index.
5. `dos/dem/demmisc.c::demLoadDos` derives its guest destination with
   `GetVDMAddr(getDI(), 0)`, opens the original `ntdos.sys` beneath the
   selected DOS root, reads it in 16 KiB chunks to that guest destination,
   and retains the original controlled-termination failure disposition.

## Composition result

Both current `cpu40-v7vga-r3` formal graphs compile `bios.c`, `nt_bop.c`,
`demdisp.c`, and `demmisc.c`; their owner archives are linked into
`original-softpc-process.exe`.  Thus the selected product does not need an
app-owned NTDOS loader or a new BOP router for this chain.

## Boundary

This is source/composition evidence only.  It does not show that the running
guest actually reaches BOP `0x50` with service `0x11`, nor that the NTDOS
bytes remain present at the intended guest address, execute, or complete the
ordinary COMMAND/EXEC/parent-return profile.  The next continuity evidence
must observe that original guest transition without changing the dispatch
owners above.
