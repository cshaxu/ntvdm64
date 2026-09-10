# Stock-Firmware Startup Probe v0

## Question

Before NTIO is preloaded, can the selected native Bochs firmware profile
provide the IVT/BDA inputs that NTIO reads or saves? Reset-ROM mechanics cannot
answer this: it observes RAM before the firmware has initialized it.

## Selected Probe

Use the unmodified Bochs BIOS and its normal `INT 19h` boot path. Supply a
private 1.44 MiB floppy image whose first sector contains only:

```text
0F 0B F4 ... 55 AA
UD2 ; HLT       boot signature
```

The boot sector contains no DOS, NTIO, NTDOS, BOP, DEM or OpenNT bytes. It is
only a neutral point reached after stock firmware initialization. The existing
profile-owned `UD2` trigger then runs unchanged.

## CPU-Level Admission

The initial r15 container uses `BX_CPU_LEVEL 3` (i386). Its stock BIOS cannot
reach the boot sector under that configuration: at runtime it resets at
physical `0xE236C`, and a read-only disassembly of `BIOS-bochs-latest` at
binary offset `0x236C` proves the instruction is `0F A2` (`CPUID`). The
rombios32 source likewise invokes `cpuid(1, ...)` during initialization.

Therefore the stock-firmware probe alone may use a separate CPU-level-5
container. This is a firmware-reached CPU instruction requirement, not a
device admission, OpenNT semantic change, or excuse to enable APIC, PCI,
networking, USB, sound, a storage controller, or another adapter feature. The
level-3 r15 fixture remains the minimal mechanics baseline.

## First Input: BDA 0040:0096

The first probe reads exactly one byte at physical `0x496`, the BDA keyboard
flag that NTIO reads before its first `50:11` transition. This selection is
intentional:

- its provenance is the stock firmware path, not reset RAM;
- it is a single bounded ordinary-RAM read; and
- it avoids claiming that one successful probe proves all IVT vectors.

IVT `10h`, `15h`, `19h`, `1Bh` and virtual-interrupt state at `0x714` remain
separate future declarations. The profile range limit and one-trigger rule
must not be widened merely to aggregate unrelated startup inputs.

## Evidence Sink

The adapter owns the observation buffer. After the generic Bochs copy has
succeeded, the adapter must compute and expose a fixed-width content digest
for the exact transaction. Bochs may log that opaque digest, but must not
interpret output bytes. For this one-byte probe the digest permits a separate
evidence script to derive the byte value without giving Bochs BDA knowledge.

The completion API must validate the transaction's copied boundary and range,
must be one-time, and must expose neither a Bochs pointer nor arbitrary guest
memory access. A failed completion cannot turn a failed read into resume.

## Admission Order

1. Add the adapter-only observation-completion digest record and negative
   tests; do not alter Bochs device or CPU semantics.
1. Register the one generic Bochs log call that reports the opaque adapter
   digest after a successful ordinary-RAM copy.
1. Extend the r15 fixture script with a stock-BIOS boot-sector mode and a
   profile declaring only `0x496..0x496`.
1. Run it once; record the digest, decoded byte, firmware/ROM identity and
   bounded-stop predicates.
1. Compare that evidence with NTIO's exact read site before admitting any
   startup-preload record.

No step authorizes NTIO preload, IVT replacement, BOP `12h`, a synthetic BDA
value, or a DOS boot claim.

## r16 Admission Result

With CPU level 5, explicit `boot: floppy`, and a bounded twenty-million-tick
run, the stock ROM logs `Booting from 0000:7c00`; the neutral `UD2` then
records the one-byte `0x496` observation digest. Fixed FNV-1a-64 decoding
gives `0x10`. This admits exactly that BDA byte for this ROM/configuration.

Earlier runs at one/ten million ticks were not negative compatibility
evidence: the ROM was in its normal BDA tick delay, and the later fixture had
erroneously selected `boot: disk` despite attaching the floppy. The correction
does not add a device or an adapter semantic. IVT and `0x714` remain separate
unproven declarations; the result does not authorize NTIO preload, BIOS
replacement, BOP `12h`, or a DOS boot claim.
