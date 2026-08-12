# r16 Stock-Firmware BDA Probe Evidence

## Scope

This record concerns only the selected native CPU-level-5 Bochs fixture and
the neutral stock-firmware boot probe. It establishes one BDA input byte only.
It does not establish an NTIO preload, a DOS/NTDOS boot, a DEM/BOP service,
or general firmware compatibility with OpenNT.

## Declared Input

- Build root: `artifacts/build/bochs-2.6-native-nogui-ucrt-r16-cpu5`
- Executable: `ntdos64-observation-fixture.exe` (the declared custom target,
  not `all` and not stock `bochs.exe`)
- CPU level: 5, required solely because the selected ROM BIOS executes
  `CPUID` while initializing.
- ROM: `bios/BIOS-bochs-latest`, SHA-256
  `D974DE50C94D76445123FD1AC886612351BED7CC5E3C8D07C556814DA72BEBD6`.
- VGA ROM: `bios/VGABIOS-lgpl-latest`, SHA-256
  `CD9FDD6A789DCD22B8A6B3B152788D43238DE49CCE674CFF57BDEB94580246C6`.
- Neutral floppy sector: `UD2; HLT` plus `0x55AA`; it contains no OpenNT,
  NTIO, NTDOS, DOS, BOP or DEM content. Its full image SHA-256 is
  `492A91F45CCB245A04FE846EF39E7854CC044EC7CF8F972F036B7AA0E10A9D1C`.
- Requested observation: exactly physical `0x496`, one byte, through the
  profile-owned `UD2` trigger.
- Successful bounded execution: twenty million benchmark ticks, explicit
  `boot: floppy`.

## Result

The successful log records, in order:

```text
Using boot sequence floppy, none, none
Starting rombios32
Booting from 0000:7c00
ntdos64 adapter observation copied digest=44bd3bd473cd12c9
cpu loop quit, shutting down simulator
```

The fixed adapter completion digest is FNV-1a-64. Exhaustive decoding over the
only 256 possible one-byte inputs yields `0x10`; this is the observed value at
physical `0x496` for the declared firmware/profile run.

The earlier one- and ten-million-tick attempts were inconclusive because the
first was below the ROM BIOS delay budget and the second had an erroneous
`boot: disk` directive despite attaching a floppy. A 100-million-tick run
therefore reached `07C0:0000` but then attempted hard disk `ELDL=80` and
reported no bootable device. The fixture now declares `boot: floppy` only for
this stock-firmware probe. This is a test-config correction, not an admission
of a device or adapter semantic.

## Classification

| Item | Status |
| --- | --- |
| CPU instruction admission for this ROM BIOS | passed at CPU level 5 |
| Neutral `UD2` observation mechanics | previously passed in r15 |
| Stock firmware reaches neutral boot sector | passed: `0000:7c00` |
| Stock BDA `0x496` byte | observed: `0x10` |
| IVT/`0x714` startup inputs | unobserved; unavailable |
| NTIO preload/entry | not implemented |

## Next Constraint

The next probes must remain independent declarations for IVT and `0x714`; do
not expand this one-byte record into a generic guest-memory reader. Do not
implement BIOS, DOS filesystem, DEM or BOP semantics in the adapter.
