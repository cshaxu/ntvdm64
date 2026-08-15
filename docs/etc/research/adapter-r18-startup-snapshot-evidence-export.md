# r18 Startup Snapshot Evidence Export

## Scope

This report closes the r17 evidence-export gate for one pinned stock-ROM,
CPU5 Bochs profile.  It records pre-entry firmware bytes only.  It does not
start NTIO, NTDOS or COMMAND; it does not exercise DOS filesystem, DEM, BOP,
or any host service.

## Reproducible Inputs

- Build root:
  `artifacts/build/bochs-2.6-native-nogui-ucrt-r18-evidence-cpu5`.
- Executable: the declared custom `ntdos64-observation-fixture.exe`; neither
  default `all` nor `bochs.exe` was built or used as the target.
- CPU level: 5; boot order: explicit `floppy`.
- System ROM SHA-256:
  `D974DE50C94D76445123FD1AC886612351BED7CC5E3C8D07C556814DA72BEBD6`.
- VGA ROM SHA-256:
  `CD9FDD6A789DCD22B8A6B3B152788D43238DE49CCE674CFF57BDEB94580246C6`.
- Neutral floppy SHA-256:
  `492A91F45CCB245A04FE846EF39E7854CC044EC7CF8F972F036B7AA0E10A9D1C`.
- Stop condition: neutral boot-sector `UD2; HLT`, with twenty million
  benchmark ticks.

The profile declares three opaque ranges, in order: ID 11 `0x40..0x6f` (48
bytes), ID 12 `0x496` (one byte), and ID 13 `0x714..0x717` (four bytes).

## Recorded Result

The adapter atomically wrote
`ntdos64-reset-fixture/byob/startup-snapshot-evidence.json`.  Its SHA-256 is
`168219F4C7D73ABEE06B62B1423298A2F111685E1BF36DA23B2C16A642310195`.
The fixed record has boundary `#UD`, vector 6, at `0000:7c00`, real-mode CPU
state (`CS=0`, `EIP=31744`) and this exact ordered content:

| ID | Physical range | Raw bytes (hex) |
| --- | --- | --- |
| 11 | `0x40..0x6f` | `4a0100c04df800f041f800f0fee300f039e700f059f800f02ee800f0d2ef00f0959600f0f2e600f06efe00f053ff00f0` |
| 12 | `0x496` | `10` |
| 13 | `0x714..0x717` | `000000c0` |

The record digest is `b090682dab2ebc2c`, exactly matching the independently
emitted Bochs-side generic completion log line.  The log also records:

```text
Booting from 0000:7c00
ntdos64 adapter snapshot copied digest=b090682dab2ebc2c
cpu loop quit, shutting down simulator
```

## Interpretation Limits

The physical bytes at ID 11 are evidence for the selected firmware profile;
they are not decoded by Bochs or the adapter.  The source-ledger mapping may
now decode the four required IVT entries externally, but must not infer that
they are compatible with NTIO until the whole startup descriptor admits them.
The `0x714` value is likewise an observed input, not an adapter default or a
new BIOS implementation.

For the fixed layout, the offline decoder
`tools/probe/Read-StartupSnapshotEvidence.ps1` derives these far pointers from the
raw ID 11 bytes:

| Vector | IVT location | Decoded pointer |
| --- | --- | --- |
| 10h | `0x40` | `c000:014a` |
| 15h | `0x54` | `f000:f859` |
| 19h | `0x64` | `f000:e6f2` |
| 1Bh | `0x6c` | `f000:ff53` |

This decoder is deliberately offline: it validates the fixed evidence record,
does not load a ROM, and has no adapter or Bochs dependency.

## Classification

| Claim | Status |
| --- | --- |
| r18 exporter custom-target link closure | passed |
| atomic adapter evidence file after snapshot completion | passed |
| log digest equals exported record digest | passed |
| selected-profile IVT/BDA/virtual-interrupt raw bytes | evidenced |
| NTIO preload/entry | not implemented |
| NTDOS, DOS FS, DEM/BOP and COMMAND runtime | not exercised |
