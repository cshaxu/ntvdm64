# T473 S10: dependency-selected common headers

This extends the previous [device/video header audit](base-inc-multi-role-review.md).
From 111 still-pending common headers, the current VALID Ninja dependency
records were used to select 14 original production contexts covering 35
headers. Their exact compiler commands were run in preprocess-only mode; 19
headers emitted declarations. The [runner](base-inc-wide-preprocess.ps1),
[observer](base-inc-wide-observer.py) and [observations](base-inc-wide-observations.json)
retain the contexts, source and both catalog identities, dependency and output
hashes. The 14 cached objects and worker image were unchanged. Include
dependency or preprocessing alone is not ABI or behavior acceptance.

The [individual dispositions](base-inc-wide-role-dispositions.csv) retain
each file's observed contexts and remaining work. All 19 byte-identical
originals are **partial**, grouped here by their independent contracts:

| Area | Files | Remaining proof |
| --- | --- | --- |
| BIOS and RTC | `cmosbios.h`, `equip.h`, `rom.h`, `tape_io.h` | State/port semantics, firmware paths, reset and errors. |
| DMA and memory | `dma.h`, `emm.h`, `sasp.h`, `umb.h` | Memory and DMA ABI, mappings, transfer boundaries, ownership and failure cleanup. |
| Video | `egavideo.h`, `vga_dac.h` | Video/palette state, mode transitions, DAC semantics and frontend behavior. |
| Disk and media | `fdisk.h`, `floppy.h`, `lock.h` | Media and handle identity, synchronization, commands, reset and errors. |
| Host configuration | `get_env.h`, `spcfile.h`, `suplib.h` | Environment/configuration ownership, encoding, errors and lifetime. |
| Serial and printing | `host_com.h`, `host_lpt.h`, `printer.h` | Real COM/LPT device behavior, queueing, failures and teardown. |

All 19 retain 100% original source text; a declaration observation cannot
provide a defensible restored-behavior percentage. The other 92 headers remain
pending: absence from these particular contexts does not rule out macros,
other consumers or alternate source profiles. Product and guest images are
unchanged by this audit.
