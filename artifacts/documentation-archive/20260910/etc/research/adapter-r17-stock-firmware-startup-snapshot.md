# r17 Stock-Firmware Startup Snapshot Evidence

## Scope

This is evidence for the generic multi-range snapshot mechanism on one pinned
CPU5 Bochs ROM profile. It does not establish NTIO preload, entry-state
application, NTDOS startup, DOS filesystem operation, DEM/BOP services or
general firmware compatibility.

## Declared Inputs

- Build root: `artifacts/build/bochs-2.6-native-nogui-ucrt-r17-snapshot-cpu5`.
- Executable: custom `ntdos64-observation-fixture.exe`, not default `all` or
  `bochs.exe`.
- CPU level: 5.
- System ROM SHA-256:
  `D974DE50C94D76445123FD1AC886612351BED7CC5E3C8D07C556814DA72BEBD6`.
- VGA ROM SHA-256:
  `CD9FDD6A789DCD22B8A6B3B152788D43238DE49CCE674CFF57BDEB94580246C6`.
- Neutral floppy SHA-256:
  `492A91F45CCB245A04FE846EF39E7854CC044EC7CF8F972F036B7AA0E10A9D1C`.
- Boot order: explicitly `floppy`.
- Profile-owned ranges, in order: `0x40..0x6f` (48 bytes), `0x496` (one
  byte), `0x714..0x717` (four bytes). Total: 53 bytes.
- Stop: neutral boot-sector `UD2;HLT`, bounded at twenty million benchmark
  ticks.

## Observed Predicates

```text
Using boot sequence floppy, none, none
Starting rombios32
Booting from 0000:7c00
ntdos64 adapter snapshot copied digest=b090682dab2ebc2c
cpu loop quit, shutting down simulator
```

The logged digest is produced by the adapter over ordered opaque range IDs,
range boundaries and copied output bytes. Bochs does not decode or branch on
the bytes. Before any copying, the Bochs seam preflights every snapshot range
through the generic ordinary-RAM readability predicate; any failure leaves the
adapter snapshot incomplete and resumes nothing.

## Classification

| Claim | Status |
| --- | --- |
| r17 same-version custom target link closure | passed |
| stock firmware reaches neutral boot sector | passed |
| three-range atomic snapshot mechanics | passed |
| r16 BDA `0x496 = 0x10` on its single-range probe | remains separately proven |
| exact r17 IVT/`0x714` field bytes | not yet evidence-exposed |
| NTIO preload and entry | not implemented |

The composite digest intentionally cannot be inverted to recover the 53 bytes.
The next gate is a controlled adapter-owned evidence export format with fixed
range IDs and bytes, not a Bochs memory API or a firmware parser. Until then,
the descriptor must not treat IVT or `0x714` values as known.
