# T95 S7 r37 Bochs Firmware / IVT Lifecycle Audit

## Question

Why was the machine INT 10h vector `0000:0000` at the point that original
OpenNT `InstSpcKbd` saved it, and which owner is responsible for establishing
it?

## Local Source Evidence

The selected execution-plan path is applied in `src/bochs/main.cc` after
`bx_init_hardware()` and immediately before the first `cpu_loop()`.  Its
registered entry helper (`src/bochs/cpu/init.cc`) changes CS:RIP to the NTIO
entry.  Consequently the first executed guest instruction is NTIO, not the
processor's reset-vector firmware instruction.

The loaded system ROM is not itself initialization: `main.cc` calls
`BX_MEM(0)->load_ROM(...)`, which only maps its bytes.  The Bochs ROM BIOS
source identifies `F000:FFF0` as its power-up entry.  Its POST calls
`post_init_ivt`, which first installs a nonzero default handler into all 256
IVT slots.  It subsequently scans and calls the VGA option ROM range
`C000:C780` before boot; the system ROM's own INT 10h stub explicitly says the
VGA BIOS owns INT 10h requests.  Therefore the original native path that can
establish a usable INT 10h vector is:

```text
Bochs reset vector -> ROM BIOS POST -> IVT initialization -> VGA ROM scan/init
-> NTIO handoff (not yet defined)
```

The r37 trace independently observed original OpenNT's relocated pseudo-ROM
far jump `073B:0939 -> 0000:0000`.  `spckbd.asm` copies the then-current IVT
10h entry into `host_int10` before replacing it.  This is consistent with the
execution plan bypassing POST and option-ROM execution; it is not evidence of
a BOP, DEM, adapter, IRET, or INT 1 defect.

## Classification

`INT 10h` initialization is a **Bochs firmware/reset lifecycle** prerequisite.
It must remain owned by native Bochs firmware and its selected video-ROM
composition.  The adapter must not manufacture IVT bytes, emulate INT 10h,
or carry firmware completion state.  OpenNT must not be patched to tolerate
the absent vector.

## Consequence and Next Decision

The existing direct execution-plan entry is valid only for a profile whose
required firmware state has independently been established.  It cannot serve
as an implicit substitute for POST.  Before any code change or another guest
trace, define and evidence one native Bochs-owned firmware-to-NTIO handoff
condition.  That condition must be deterministic and narrow, must not infer
completion from arbitrary guest behavior, and must preserve the adapter as an
opaque plan provider rather than a firmware owner.

No source, configuration, device, BOP, adapter, OpenNT, or CLI behavior is
changed by this audit.
