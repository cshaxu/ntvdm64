# M0 T318 S2 P44 — NTIO-to-NTDOS post-load owner cohort

## Purpose

Audit the first original machine and DEM dependencies after `NTIO.SYS` returns
from `SVC_DEMLOADDOS`.  The goal is to distinguish already-composed original
providers from a real post-load binding defect before attempting another
fixed-container observation.

## Original guest handoff

`doskrnl/bios/msinit.asm::init` passes its guest-defined `dos_load_seg` in
`DI` to `SVC_DEMLOADDOS`, then immediately performs the original `jmp
sysinit`.  The selected `NTIO.MAP` places that guest `sysinit` entry at
`009D:02F9`.

`demmisc.c::demLoadDos` preserves the source-defined contract: it resolves
`DI:0000` with `GetVDMAddr`, loads `ntdos.sys` in 16 KiB reads, and returns to
the guest.  Its only modern changes are the registered native-width
`INVALID_HANDLE_VALUE` sentinel and private loader-cursor increment; neither
changes the guest load segment, file bytes, success return, or controlled
termination failure contract.

## First post-load owner cohort

The original `sysinit1.asm` route has three non-debug, startup-relevant host
crossings before COMMAND/EXEC work:

1. `BOP 12h` obtains conventional-memory size.  Original `BIOS[0x12]` selects
   `base/bios/mem_size.c::memory_size`, which reads the reset-initialized
   `MEMORY_VAR` and returns it in `AX`.
2. `BOP 15h` with `AH=88h` obtains extended-memory size.  Original
   `BIOS[0x15]` selects `base/bios/tape_io.c::cassette_io`; the selected
   original host header defines `PM`, so its `INT15_EMS_DETERMINE` branch
   reads the original CMOS upper-memory bytes populated from SoftPC SAS
   memory.  It does not take the non-PM zero-memory fallback.
3. `SVC_DEMGETBOOTDRIVE` selects original
   `dos/dem/demgset.c::demGetBootDrive` through `demdisp.c`.  It retains the
   source registry query and its original fixed-drive/C: fallback, returning
   the DOS drive number in `AL`.

The selected formal CPU40 x86 and x64 graphs compile `mem_size.c`,
`tape_io.c`, `bios.c`, `demdisp.c`, and `demgset.c` and link their owner
archives into the product.  No app-owned memory/drive provider, new BOP
router, or mapping-manager use is required for this cohort.

## Boundary

The subsequent NTDOS `DOSINIT`/segment-relocation/configuration sequence is
guest-owned and remains the next continuity cohort.  This record does not
claim that the current runtime reached any listed service, loaded NTDOS, or
executed an ordinary child profile.  It establishes only that these first
post-load dependencies already have their original owners and selected build
closure.
