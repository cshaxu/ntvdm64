# T194 S22 drive-snapshot engine bundle 001

Date: 2026-08-12  
Packet: M0 T194 S22  
Disposition: immutable bundle assembled and hash-verified; no process started.

## Bundle

`tools/New-T190S5CommandBootstrapBundle.ps1` assembled
`artifacts/build/current/t194-s22-drive-snapshot-bundle-r1` from the S21
derivative and the retained S10 ROM root. Its manifest is
`ntdos64-bochs-engine-bundle-v1` and records:

| Member | SHA-256 |
| --- | --- |
| Engine | `3F2187B9153E5DB181880BB78376B17F197774D4C73CA0A2E7B086A7088DF2F4` |
| BIOS | `D974DE50C94D76445123FD1AC886612351BED7CC5E3C8D07C556814DA72BEBD6` |
| VGA BIOS | `CD9FDD6A789DCD22B8A6B3B152788D43238DE49CCE674CFF57BDEB94580246C6` |
| Option ROM | `E4CA4AE5190A15ED741D1DFED5A58533A2B6926A40302D839FB714F9FC673FFB` |

The engine hash is exactly S21's source-built adapter derivative; the three
ROM hashes are exactly the prior fixed inputs. Assembly copied and hashed files
only. It did not execute the shim, engine, Bochs, CPU loop or guest.

## Follow-up

T194 S23 may invoke the existing one-shot v6 controller once with this bundle,
the S15 CLI pair and the S13 root. It must retain the full evidence directory,
use one 15-second watchdog and no retry, and classify `50:0F`/`54:11` only from
actual trace markers.
